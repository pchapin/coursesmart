/*****************************************************************************
FILE          : SAXHandler.h
LAST REVISION : 2004-11-28
SUMMARY       : Definition of SAX event handler class.

This class processes SAX events from the XML parser. These events are
generated in response to requests from the client. This class also
contains the functions for handling each request type.
*****************************************************************************/

#ifndef SAXHANDLER_H
#define SAXHANDLER_H

#include <map>
#include <sstream>
#include <string>

#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/HandlerBase.hpp>

XERCES_CPP_NAMESPACE_USE

class SAXHandler : public HandlerBase
{
public:
  SAXHandler(int handle);
 ~SAXHandler();
  
  // ----------------------------------------------
  // Handlers for the SAX ContentHandler interface
  // ----------------------------------------------

  void startElement(
    const XMLCh *const name,
    AttributeList& attrs);

  void characters(
    const XMLCh *const chars,
    const unsigned int length);

  void endElement(
    const XMLCh *const name);

  // --------------------------------------------
  // Handlers for the SAX ErrorHandler interface
  // --------------------------------------------

  void warning(const SAXParseException& exc);
  void error(const SAXParseException& exc);
  void fatalError(const SAXParseException& exc);

private:
  typedef std::map<std::string, std::string> nvmap_type;
  typedef std::map<std::string, std::string> amap_type;
  typedef std::map<std::string, std::string> smap_type;
  typedef std::map<std::string, void (SAXHandler::*)()> rmap_type;

  int connection_handle;      // Handle of open connection to client.
  std::string request_type;   // Type of current request.
  std::string request_id;     // ID number of current request.
  std::string current_id;     // ID string on "current" element (if any).
  std::string content;        // Content of "current" element.
  nvmap_type  name_value_map; // (name, value) pairs for current request.
  amap_type   assignment_map; // Used when collecting student grades.
  smap_type   student_map;    // Used when collecting assignment grades.
  rmap_type   request_map;    // (request, handler) pairs.

  std::string logged_user;    // User ID of logged in user.
  std::string logged_role;    // Role for logged in user (only one supported).

  // Various helper methods to facilitate creating and sending responses.
  void start_tag(std::stringstream &formatter, const char *status);
  void end_tag(std::stringstream &formatter);
  void send_failure(const char *);
  void write_net(std::stringstream &formatter);

  // This function processes the current request.
  void process_request();

  // These functions are request handlers.
  void handle_add_assignment();
  void handle_add_message();
  void handle_add_upload_file();
  void handle_delete_assignment();
  void handle_delete_message();
  void handle_delete_upload_file();
  void handle_get_message();
  void handle_get_upload_file();
  void handle_get_upload_filelist();
  void handle_gradebook_read();
  void handle_gradebook_write();
  void handle_login();
};

#endif
