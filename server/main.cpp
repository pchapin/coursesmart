/*****************************************************************************
FILE          : main.cpp
LAST REVISION : 2004-11-18
SUMMARY       : Main loop of CourseSmart server.

This file initializes the network, deals with incoming connections by
forking child processes, and creates an XML parser for each connection.

*****************************************************************************/

// C/POSIX
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

// C++
#include <iostream>
#include <memory>

// Xerces
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/PlatformUtils.hpp>

// CourseSmart
#include "Course.h"
#include "SAXHandler.h"

XERCES_CPP_NAMESPACE_USE

Course *theCourse; // Global variable points at singleton Course object.

// -----
// General signal and network setup
// -----

//
// This function handles the SIGCHLD signal by removing zombies.
// 
static void SIGCHLD_handler(int)
{
  int status;

  while (waitpid(-1, &status, WNOHANG) > 0) ;
  return;
}


//
// This function configures signal handling.
//
static void setup_signal_handling()
{
  struct sigaction    action;
  struct sigaction    old_action;

  action.sa_handler = SIGCHLD_handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  sigaction(SIGCHLD, &action, &old_action);
}

//
// This function starts the server listening to the network.
//
static int initialize_network(int port)
{
  int                 listen_handle;     // Socket handle to listen on.
  struct sockaddr_in  server_address;    // Our address.

  // Create the server socket.
  if ((listen_handle = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Problem creating socket");
    return -1;
  }

  // Prepare the server socket address structure.
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family      = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port        = htons(port);

  // Bind the server socket.
  if (bind(listen_handle,
        (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
    perror("Problem binding");
    return -1;
  }

  // Allow incoming connections.
  if (listen(listen_handle, 32) < 0) {
    perror("Problem listening");
    return -1;
  }

  return listen_handle;
}

// -----
// Create an XML parser for each connection
// -----

// The following function returns a single character from the network.
static int get_networkCh(int connection_handle)
{
  static char  buffer[1024];
  static int   count = 0;
  static char *p;

  if (count == 0) {
    count = read(connection_handle, buffer, 1024);
    if (count == 0) return -1;
    p = buffer;
  }
  --count;
  return *p++;
}


static std::string get_request(int connection_handle)
{
  std::string result;
  int Ch;

  // Skip non-digits (makes server more robust).
  while ((Ch = get_networkCh(connection_handle)) != -1 && !isdigit(Ch)) ;
  if (Ch == -1) return "";
  result += Ch;

  // Get the first line.
  while ((Ch = get_networkCh(connection_handle)) != -1 && Ch != '\n') {
    result += Ch;
  }
  if (Ch == -1) return "";

  // How big is the request?
  int size = atoi(result.c_str());

  // Get the request.
  result.erase();
  while (size-- && (Ch = get_networkCh(connection_handle)) != -1) {
    result += Ch;
  }
  if (Ch == -1) return "";

  return result;
}


static void process_connection(int connection_handle)
{
  try {
    XMLPlatformUtils::Initialize();
  }
  catch(const XMLException &e) {
    theCourse->getLogger()->
      write("XMLException during initialization of Xerces library");
    return;
  }

  try {
    SAXHandler handler(connection_handle);
    while (1) {
      std::string raw_data(get_request(connection_handle));
      if (raw_data == "") break;

      MemBufInputSource memory_buffer(
        (const XMLByte *)raw_data.c_str(),
        raw_data.size(),
        "http://www.memory.buffer/"
      );

      //  Create a SAX parser object.
      std::auto_ptr<SAXParser> parser(new SAXParser);
      parser->setDocumentHandler(&handler);
      parser->setErrorHandler(&handler);

      // Parse the request
      parser->parse(memory_buffer);
    }
  }
  catch (const XMLException &e) {
    theCourse->getLogger()->
      write("XMLException during parse of client request");
  }
  catch (const SAXParseException &e) {
    theCourse->getLogger()->
      write("SAXParseException during parse of client request");
  }
  catch (...) {
    theCourse->getLogger()->
      write("Unknown exception during parse of client request");
  }

  XMLPlatformUtils::Terminate();
}

// -----
// Main loop forks children.
// -----

static int mainloop(int listen_handle)
{
  int                 connection_handle; // Socket handle for each connection.
  struct sockaddr_in  client_address;    // Remote address.
  socklen_t           client_length;     // Size of remote address.
  pid_t               child_ID;          // Process ID of child.

  // Accept connections forever; create a child process to handle each.
  while (1) {
    memset(&client_address, 0, sizeof(client_address));
    client_length = sizeof(client_address);

    // Block until a client comes along. If accept() returns due to a
    // signal just call it again. The only signal we should be getting
    // is SIGCHLD.
    // 
    while ((connection_handle = accept(listen_handle,
              (struct sockaddr *) &client_address, &client_length)) < 0) {

      // If this is an interruption due to a signal, just loop back.
      if (errno != EINTR) {
        perror("Accept interrupted");
        return -1;
      }
    }

    // Create the child process to handle this connection.
    if ((child_ID = fork()) == -1) {
      perror("Problem creating child server");
      return -1;
    }

    // This is the child code.
    else if (child_ID == 0) {
      close(listen_handle);
      process_connection(connection_handle);
      close(connection_handle);
      exit(EXIT_SUCCESS);
    }

    // This is the parent code.
    close(connection_handle);
  }
}


// -----
// Main Program
// -----

int main(int argc, char **argv)
{
  int                 listen_handle;     // Socket handle to listen on.
  int                 option;
  unsigned short      port = 8080;
  std::string         repository_location("./repository");

  // Process command line options.
  while ((option = getopt(argc, argv, "p:r:")) != -1) {
    switch (option) {
    case 'p':
      port = std::atoi(optarg);
      break;
    case 'r':
      repository_location = optarg;
      break;
    case '?':
      std::exit(1);
    }
  }

  theCourse = new Course(repository_location);

  setup_signal_handling();
  if ((listen_handle = initialize_network(port)) == -1) return EXIT_FAILURE;
  mainloop(listen_handle);

  delete theCourse;

  // Why are we here?
  return EXIT_FAILURE;
}
