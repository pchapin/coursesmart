/*****************************************************************************
FILE          : SAXHandler.cpp
LAST REVISION : 2004-12-02
SUMMARY       : Implementation of SAX event handler and command dispatch.

It might be better to use a DOM approach to parsing. We are holding the
enter request message in memory anyway and experience with creating the
handler functions shows that random access to the information is useful.
Right now we are using some std::maps for that purpose but that is
probably, in effect, duplicating what DOM already does.

Note also that doing a validating parse might allow some error handling
to be eliminated from or improved in the various handler functions.
Right now error handling is pretty rough.
*****************************************************************************/

// C/POSIX
#include <unistd.h>

// C++
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>

// CourseSmart
#include "Course.h"
#include "SAXHandler.h"
#include "XMLtrans.h"

// =========================
// Internal Helper Functions
// =========================

// This function exists to deal with character set transcoding issues.
static bool get_attribute_value(AttributeList     &attrs,
                                const char        *name,
                                      std::string &value)
{
  XMLCh *attr_name = XMLString::transcode(name);
  const XMLCh *attr_value = attrs.getValue(attr_name);
  if (attr_value == NULL) {
    XMLString::release(&attr_name);
    return false;
  }
  char *raw_attr_value = XMLString::transcode(attr_value);
  value = raw_attr_value;

  XMLString::release(&attr_name);
  XMLString::release(&raw_attr_value);
  return true;
}

static void format_unit_list(std::stringstream &formatter)
{
  std::vector<Course::UnitInformation> list;
  list = theCourse->getUnitList();
  
  for(unsigned i = 0; i < list.size(); i++) {
    Unit* ut = theCourse->getUnit(list[i].id);

    // Indent the element to make it look pretty.
    formatter << "      <unit name=\""
	      << list[i].name
              << "\" id=\""
              << list[i].id
	      << "\" url=\""
	      << ut->getURL()
	      << "\"/>\n";
  }
}


//
// Split a string on spaces.
//
static std::vector<std::string> split(const std::string &buffer)
{
  std::vector<std::string> result;

  std::string::size_type start_pos, end_pos;

  start_pos = 0;
  while ((end_pos = buffer.find(' ', start_pos)) != std::string::npos) {
    result.push_back(buffer.substr(start_pos, end_pos - start_pos));
    start_pos = end_pos + 1;
  }
  result.push_back(buffer.substr(start_pos));

  return result;
}

//
// Join fields back together using spaces.
//
static std::string join(const std::vector<std::string> &fields)
{
  std::string result;
  std::vector<std::string>::const_iterator p;
  bool first_time = true;

  for (p = fields.begin(); p != fields.end(); ++p) {
    if (!first_time) result += " ";
    result += *p;
    first_time = false;
  }
  return result;
}

//
// Find a particular student in a vector of GradeInfos
//
static int find_student(
  const std::vector<GradeBook::GradeInfo> &g_info, const std::string &name)
{
  std::vector<GradeBook::GradeInfo>::const_iterator p;
  for (p = g_info.begin(); p != g_info.end(); ++p) {
    if (p->name == name) return p - g_info.begin();
  }
  return -1;
}

//
// Find a particular assignment in a vector of AssignmentInfos
//
static int find_assignment(
  const std::vector<GradeBook::AssignmentInfo> &a_info, const std::string &id)
{
  std::vector<GradeBook::AssignmentInfo>::const_iterator p;
  for (p = a_info.begin(); p != a_info.end(); ++p) {
    if (p->id == id) return p - a_info.begin();
  }
  return -1;
}

// ===============
// Private Methods
// ===============

void SAXHandler::write_net(std::stringstream &formatter)
{
  std::string s(formatter.str());

  std::stringstream numeric_formatter;
  numeric_formatter << s.size() << "\n";
  std::string numeric_string(numeric_formatter.str());

  write(connection_handle, numeric_string.c_str(), numeric_string.size());
  write(connection_handle, s.c_str(), s.size());
}


// This makes it a little easier to construct the start tags.
void SAXHandler::start_tag(std::stringstream &formatter, const char *status)
{
  formatter << "<response type=\""    << request_type << "\" "
            <<            "id=\""     << request_id   << "\" "
            <<            "status=\"" << status       << "\">\n";
}


// This silly function is just for symmetry with start_tag().
void SAXHandler::end_tag(std::stringstream &formatter)
{
  formatter << "</response>\n";
}


void SAXHandler::send_failure(const char *message)
{
  std::stringstream formatter;

  start_tag(formatter, "failure");
  formatter << "  <message>" << message << "</message>\n";
  formatter << "</response>\n";

  write_net(formatter);
}

// ----------------
// Request Handlers
// ----------------

void SAXHandler::process_request()
{
  if (request_map.find(request_type) == request_map.end()) {
    send_failure("Unknown request type");
  }
  else {
    (this->*request_map[request_type])();
  }
}


void SAXHandler::handle_add_assignment()
{
  send_failure("Not implemented");
}


void SAXHandler::handle_add_message()
{
  if (logged_user == "") {
    send_failure("Unauthorized");
    return;
  }

  nvmap_type::iterator p = name_value_map.find("discussion-message");
  if (p == name_value_map.end()) {
    send_failure("Malformed request: Missing 'discussion-message' element");
    return;
  }
  Discussion *dc = theCourse->getDiscussion();
  int id = dc->addMessage(p->second);

  std::stringstream formatter;
  start_tag(formatter, "success");
  formatter << "  <id>" << id << "</id>\n";
  end_tag(formatter);
  write_net(formatter);
}


void SAXHandler::handle_add_upload_file()
{
  if (logged_user == "") {
    send_failure("Unauthorized");
    return;
  }

  nvmap_type::iterator p1 = name_value_map.find("unit-id");
  if (p1 == name_value_map.end()) {
    send_failure("Malformed request: Missing 'unit-id' element");
    return;
  }
  nvmap_type::iterator p2 = name_value_map.find("file");
  if (p2 == name_value_map.end()) {
    send_failure("Malformed request: Missing 'file' element");
    return;
  }
  nvmap_type::iterator p3 = name_value_map.find("file-contents");
  if (p3 == name_value_map.end()) {
    send_failure("Malformed request: Missing 'file-contents' element");
    return;
  }
  Unit *ut = theCourse->getUnit(p1->second);
  ut->addUploadFile(logged_user, logged_role, p2->second, p3->second);

  std::stringstream formatter;
  start_tag(formatter, "success");
  end_tag(formatter);
  write_net(formatter);
}


void SAXHandler::handle_delete_assignment()
{
  send_failure("Not implemented");
}


void SAXHandler::handle_delete_message()
{
  if (logged_user == "") {
    send_failure("Unauthorized");
    return;
  }

  nvmap_type::iterator p = name_value_map.find("id");
  if (p == name_value_map.end()) {
    send_failure("Malformed request: Missing 'id' element");
    return;
  }
  int id = std::atoi((p->second).c_str());
  Discussion *dc = theCourse->getDiscussion();
  dc->deleteMessage(id);

  std::stringstream formatter;
  start_tag(formatter, "success");
  end_tag(formatter);
  write_net(formatter);
}


void SAXHandler::handle_delete_upload_file()
{
  if (logged_user == "") {
    send_failure("Unauthorized");
    return;
  }

  nvmap_type::iterator p1 = name_value_map.find("unit-id");
  if (p1 == name_value_map.end()) {
    send_failure("Malformed request: Missing 'unit-id' element");
    return;
  }
  nvmap_type::iterator p2 = name_value_map.find("file");
  if (p2 == name_value_map.end()) {
    send_failure("Malformed request: Missing 'file' element");
    return;
  }
  Unit *ut = theCourse->getUnit(p1->second);
  ut->deleteUploadFile(logged_user, logged_role, p2->second);

  std::stringstream formatter;
  start_tag(formatter, "success");
  end_tag(formatter);
  write_net(formatter);
}


void SAXHandler::handle_get_message()
{
  if (logged_user == "") {
    send_failure("Unauthorized");
    return;
  }

  nvmap_type::iterator p = name_value_map.find("id");
  if (p == name_value_map.end()) {
    send_failure("Malformed request: Missing 'id' element");
    return;
  }
  int id = std::atoi((p->second).c_str());
  Discussion *dc = theCourse->getDiscussion();
  std::string message = dc->getMessage(id);
  if (message == "") {
    send_failure("No such message");
    return;
  }

  std::stringstream formatter;
  start_tag(formatter, "success");
  formatter << "<discussion-message><![CDATA[";
  formatter << message;
  formatter << "]]></discussion-message>\n";
  end_tag(formatter);
  write_net(formatter);
}


void SAXHandler::handle_get_upload_file()
{
  if (logged_user == "") {
    send_failure("Unauthorized");
    return;
  }

  nvmap_type::iterator p1 = name_value_map.find("unit-id");
  if (p1 == name_value_map.end()) {
    send_failure("Malformed request: Missing 'unit-id' element");
    return;
  }
  nvmap_type::iterator p2 = name_value_map.find("file");
  if (p2 == name_value_map.end()) {
    send_failure("Malformed request: Missing 'file' element");
    return;
  }
  Unit *ut = theCourse->getUnit(p1->second);
  std::string
    file_contents(ut->getUploadFile(logged_user, logged_role, p2->second));
  if (file_contents == "") {
    send_failure("No such file");
  }
  else {
    std::stringstream formatter;

    start_tag(formatter, "success");
    formatter << "  <file-contents>" << file_contents << "</file-contents>\n";
    end_tag(formatter);
    write_net(formatter);
  }
}


void SAXHandler::handle_get_upload_filelist()
{
  if (logged_user == "") {
    send_failure("Unauthorized");
    return;
  }

  nvmap_type::iterator p = name_value_map.find("unit-id");
  if (p == name_value_map.end()) {
    send_failure("Malformed request: Missing 'unit-id' element");
    return;
  }
  Unit *ut = theCourse->getUnit(p->second);
  std::vector<std::string> list = ut->getUploadList(logged_user, logged_role);

  std::stringstream formatter;
  start_tag(formatter, "success");
  std::vector<std::string>::iterator it = list.begin();
  while (it != list.end()) {
    formatter << "  <file>" << *it << "</file>\n";
    ++it;
  }
  end_tag(formatter);
  write_net(formatter);
}


void SAXHandler::handle_gradebook_read()
{
  if (logged_user == "") {
    send_failure("Unauthorized");
    return;
  }

  nvmap_type::iterator p;
  std::stringstream formatter;

  start_tag(formatter, "success");

  p = name_value_map.find("student-list");
  if (p != name_value_map.end()) {
    UserData *ud = theCourse->getUserData();
    std::vector<UserData::UserInformation> info = ud->getUserList();
    std::vector<UserData::UserInformation>::iterator it;

    formatter << "  <student-list>\n";
    for (it = info.begin(); it != info.end(); ++it) {
      if (it->role == "student") {
        formatter << "    <student id=\"" << it->user_name
                  << "\" fname=\"" << it->first_name
                  << "\" lname=\"" << it->last_name << "\"/>\n";
      }
    }
    formatter << "  </student-list>\n";
  }

  p = name_value_map.find("assignment-list");
  if (p != name_value_map.end()) {
    GradeBook *gb = theCourse->getGradeBook();
    std::vector<GradeBook::AssignmentInfo> info = gb->getAssignments();
    std::vector<GradeBook::AssignmentInfo>::iterator it;

    formatter << "  <assignment-list>\n";
    for (it = info.begin(); it != info.end(); ++it) {
      formatter << "    <assignment id=\"" << it->id
                << "\" description=\"" << it->name << "\"/>\n";
    }
    formatter << "  </assignment-list>\n";
  }

  p = name_value_map.find("student-grades");
  if (p != name_value_map.end()) {
    if (logged_role == "student" && logged_user != p->second) {
      send_failure("Unauthorized");
      return;
    }
    GradeBook *gb = theCourse->getGradeBook();
    std::vector<GradeBook::GradeInfo> g_info = gb->getAllGrades();
    std::vector<GradeBook::GradeInfo>::iterator git;
    std::vector<GradeBook::AssignmentInfo> a_info = gb->getAssignments();
    std::vector<GradeBook::AssignmentInfo>::iterator ait;

    for (git = g_info.begin(); git != g_info.end(); ++git) {
      if (git->name == p->second) {
        std::vector<std::string> fields = split(git->grades);
        int i = 0;

        formatter << "  <assignment-list>\n";
        for (ait = a_info.begin(); ait != a_info.end(); ++ait) {
          formatter << "    <assignment id=\"" << ait->id;
          formatter << "\" description=\"" << ait->name << "\">";
          formatter << fields[i++];
          formatter << "</assignment>\n";
        }
        formatter << "  </assignment-list>\n";
      }
    }
  }

  p = name_value_map.find("assignment-grades");
  if (p != name_value_map.end()) {
    if (logged_role == "student") {
      send_failure("Unauthorized");
      return;
    }
    GradeBook *gb = theCourse->getGradeBook();
    std::vector<GradeBook::GradeInfo> g_info = gb->getAllGrades();
    std::vector<GradeBook::GradeInfo>::iterator git;
    std::vector<GradeBook::AssignmentInfo> a_info = gb->getAssignments();
    std::vector<GradeBook::AssignmentInfo>::iterator ait;

    int i = 0;
    for (ait = a_info.begin(); ait != a_info.end(); ++ait, ++i) {
      if (ait->id == p->second) {
        formatter << "  <student-list>\n";
        for (git = g_info.begin(); git != g_info.end(); ++git) {
          std::vector<std::string> fields = split(git->grades);
          formatter << "    <student id=\"" << git->name << "\">";
          formatter << fields[i];
          formatter << "</student>\n";
        }
        formatter << "  </student-list>\n";
      }
    }
  }

  end_tag(formatter);
  write_net(formatter);
}


void SAXHandler::handle_gradebook_write()
{
  if (logged_role != "instructor" && logged_role != "grader") {
    send_failure("Unauthorized");
    return;
  }

  GradeBook *gb = theCourse->getGradeBook();
  nvmap_type::iterator p;
  std::stringstream formatter;

  start_tag(formatter, "success");

  p = name_value_map.find("student-id");
  if (p != name_value_map.end()) {
    std::vector<GradeBook::GradeInfo> g_info = gb->getAllGrades();
    std::vector<GradeBook::AssignmentInfo> a_info = gb->getAssignments();
    int g_index = find_student(g_info, p->second);
    std::vector<std::string> g_fields = split(g_info[g_index].grades);
    amap_type::iterator ait;
    for (ait = assignment_map.begin(); ait != assignment_map.end(); ++ait) {
      int a_index = find_assignment(a_info, ait->first);
      g_fields[a_index] = ait->second;
    }
    g_info[g_index].grades = join(g_fields);
    gb->putAllGrades(g_info);
  }

  p = name_value_map.find("assignment-id");
  if (p != name_value_map.end()) {
    std::vector<GradeBook::GradeInfo> g_info = gb->getAllGrades();
    std::vector<GradeBook::AssignmentInfo> a_info = gb->getAssignments();
    int a_index = find_assignment(a_info, p->second);
    std::vector<GradeBook::GradeInfo>::iterator git;
    for (git = g_info.begin(); git != g_info.end(); ++git) {
      smap_type::iterator sit = student_map.find(git->name);
      if (sit != student_map.end()) {
        std::vector<std::string> g_fields = split(git->grades);
        g_fields[a_index] = sit->second;
        git->grades = join(g_fields);
      }
    }
    gb->putAllGrades(g_info);
  }

  end_tag(formatter);
  write_net(formatter);
}


void SAXHandler::handle_login()
{
  Logger *lg = theCourse->getLogger();
  UserData *ud = theCourse->getUserData();
  std::string uname(name_value_map.find("username")->second);
  std::string pword(name_value_map.find("password")->second);
  std::string l_name;
  std::string f_name;
  std::string role;

  if ((role = ud->verifyUser(uname, pword, &l_name, &f_name))
      != "nonexistent") {

    std::stringstream log_formatter;
    log_formatter << "Login for " << uname << " successful";
    lg->write(log_formatter.str());

    // Remember this for later access control decisions.
    logged_user = uname;
    logged_role = role;

    // This is ungainly.
    std::stringstream formatter;
    start_tag(formatter, "success");
    formatter << "  <lname>" << l_name << "</lname>\n";
    formatter << "  <fname>" << f_name << "</fname>\n";
    formatter << "  <role-list>\n";
    formatter << "    <role>" << role << "</role>\n";
    formatter << "  </role-list>\n";
    formatter << "  <feature-list>\n";
    formatter << "    <content-read>\n";
    format_unit_list(formatter);
    formatter << "    </content-read>\n";
    formatter << "    <student-submit/>\n";
    formatter << "    <discussion url=\"http://your.ballistichelmet.org:8888/discussion/threads.html\">\n";
    if (role == "instructor") {
      formatter << "      <read/><write/><delete/>\n";
    }
    else {
      formatter << "      <read/><write/>\n";
    }
    formatter << "    </discussion>\n";
    formatter << "    <gradebook>\n";
    if (role == "instructor" || role == "grader") {
      formatter << "      <read-all/><write-all/>\n";
    }
    else {
      formatter << "      <read-self/>\n";
    }
    formatter << "    </gradebook>\n";
    formatter << "  </feature-list>\n";
    end_tag(formatter);
    write_net(formatter);
  }
  else {
    std::stringstream log_formatter;
    log_formatter << "Login for " << uname << " FAILED";
    lg->write(log_formatter.str());
    send_failure("Invalid username/password pair");
  }
}


// ==============
// Public Methods
// ==============

// ----------------------------------------
// SAXHandler: Constructors and Destructor
// ----------------------------------------
SAXHandler::SAXHandler(int handle) : connection_handle(handle)
{
  // Initialize the request map.
  request_map["add-assignment"     ] = &SAXHandler::handle_add_assignment;
  request_map["add-message"        ] = &SAXHandler::handle_add_message;
  request_map["add-upload-file"    ] = &SAXHandler::handle_add_upload_file;
  request_map["delete-assignment"  ] = &SAXHandler::handle_delete_assignment;
  request_map["delete-message"     ] = &SAXHandler::handle_delete_message;
  request_map["delete-upload-file" ] = &SAXHandler::handle_delete_upload_file;
  request_map["get-message"        ] = &SAXHandler::handle_get_message;
  request_map["get-upload-file"    ] = &SAXHandler::handle_get_upload_file;
  request_map["get-upload-filelist"] = &SAXHandler::handle_get_upload_filelist;
  request_map["gradebook-read"     ] = &SAXHandler::handle_gradebook_read;
  request_map["gradebook-write"    ] = &SAXHandler::handle_gradebook_write;
  request_map["login"              ] = &SAXHandler::handle_login;
}


SAXHandler::~SAXHandler()
{
}


// ----------------------------------------------------------------
// SAXHandler: Implementation of the SAX DocumentHandler interface
// ----------------------------------------------------------------

void SAXHandler::startElement(
   const XMLCh* const name,
   AttributeList& attrs)
{
  StrX element_name(name);

  current_id.erase();
  content.erase();

  if (std::strcmp(element_name.localForm(), "request") == 0) {
    name_value_map.clear();
    assignment_map.clear();
    student_map.clear();
    request_type.erase();
    request_id.erase();

    get_attribute_value(attrs, "type", request_type);
    get_attribute_value(attrs, "id", request_id);
  }
  else if (std::strcmp(element_name.localForm(), "assignment") == 0) {
    get_attribute_value(attrs, "id", current_id);
  }
  else if (std::strcmp(element_name.localForm(), "student") == 0) {
    get_attribute_value(attrs, "id", current_id);
  }
}


void SAXHandler::characters(
  const XMLCh* const chars,
  const unsigned int length)
{
  char *buffer = new char[length + 1];
  XMLString::transcode(chars, buffer, length);
  buffer[length] = '\0';
  content += buffer;
  delete [] buffer;
}


void SAXHandler::endElement(
    const XMLCh *const name)
{
  StrX element_name(name);

  if (std::strcmp(element_name.localForm(), "request") == 0) {
    process_request();
  }
  else if (std::strcmp(element_name.localForm(), "assignment") == 0) {
    assignment_map[current_id] = content;
  }
  else if (std::strcmp(element_name.localForm(), "student") == 0) {
    student_map[current_id] = content;
  }
  else {
    name_value_map[element_name.localForm()] = content;
  }
}


// --------------------------------------------------------
// SAXHandler: Overrides of the SAX ErrorHandler interface
// --------------------------------------------------------

void SAXHandler::error(const SAXParseException& e)
{
  std::stringstream formatter;

  formatter << "CourseSmart protocol parsing error: "
            << StrX(e.getMessage());
  theCourse->getLogger()->write(formatter.str());
}


void SAXHandler::fatalError(const SAXParseException& e)
{
  std::stringstream formatter;

  formatter << "CourseSmart protocol parsing fatal error: "
            << StrX(e.getMessage());
  theCourse->getLogger()->write(formatter.str());
}


void SAXHandler::warning(const SAXParseException& e)
{
  std::stringstream formatter;

  formatter << "CourseSmart protocol parsing warning: "
            << StrX(e.getMessage());
  theCourse->getLogger()->write(formatter.str());
}
