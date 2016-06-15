/*                                                         -*- Java -*-
 * CSGateway.java
 */

package edu.vtc.coursesmart;

import java.io.*;
import java.net.*;
import java.util.*;

import org.jdom.*;
import org.jdom.output.*;
import org.jdom.input.SAXBuilder;
import org.apache.soap.encoding.soapenc.Base64;

import javax.mail.MessagingException;

/**
 * The CSGateway is used to communicate with the CS server and encapsulates the details of the messaging protocol and
 * state of the connection.
 * <p/>
 * <p><tt> $Id: CSGateway.java,v 1.34 2004/12/03 15:08:25 dvanhorn Exp $ </tt>
 */
public class CSGateway {
    private boolean DEBUG = false;

    /**
     * Constructs a CSGateway object that will connect to the default CS server on the default port.
     * <p/>
     * <p>Note that constructing a CSGateway object does not perform a connection to the server.  You must call
     * <code>login()</code> to connect.
     */
    public CSGateway()
    {
        resetFeatures();
        host = default_host;
        port = default_port;
        try {
            course_url = new URL("http://www.cs.uvm.edu/~dvanhorn/cs205/");
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Constructs a CSGateway object that will connect to the given host on the given port number when the login method
     * is invoked.
     * <p/>
     * <p>Note that constructing a CSGateway object does not perform a connection to the server.  You must call
     * <code>login()</code> to connect.
     *
     * @param host a string represting the hostname of the CS server.
     * @param port an integer representing the port number that the CS server listens for connections on.
     */
    public CSGateway(String host, int port)
    {
        resetFeatures();
        this.host = host;
        this.port = port;
        try {
            course_url = new URL("http://www.cs.uvm.edu/~dvanhorn/cs205/");
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }


    /**********************************************************************
     * Login / Logout
     **********************************************************************/

    /**
     * This method resets the object's state to the initial state.  If the gateway is currently connected to the server,
     * the connection is closed.
     */
    public void logout()
    {
        try {
            resetFeatures();
            if (sock != null) sock.close();
            sock = null;
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * This method establishes a connection with the server using the given username and password.  An
     * AuthenticationFailure is thrown if this username and password are not authentic users in the system, otherwise
     * this gateway's capabilities are set appropriately based on the features returned by the server and the gateway is
     * marked as "logged in".
     * <p/>
     * <p>It is a fatal error (the program terminates) to call the login method if the gateway is in a connected state
     * already.
     *
     * @param username string representing the user to be logged in.
     * @param password string representing the user's password.
     */
    public void login(String username, String password)
        throws AuthenticationFailure, IOException
    {

        if (sock == null) {
            sock = new Socket(host, port);
        }
        else {
            System.out.println("Cannot login on already connected socket");
            System.exit(-1);
        }

        toServer
            ("<request type=\"login\">\n" +
                "  <username>" + username + "</username>\n" +
                "  <password>" + password + "</password>\n" +
                "</request>");

        Element response = fromServer();
        logged_in = true;
        this.username = username;

        ListIterator features
            = response.getChild("feature-list").getChildren().listIterator();

        Element feature;
        String name;
        units = new LinkedList();

        while (features.hasNext()) {
            feature = (Element) features.next();
            name = feature.getName();
            if (name.equals("content-read")) {
                caps[Cap.CONTENT_READ.ord] = true;
                ListIterator l = feature.getChildren().listIterator();
                while (l.hasNext()) {
                    Element unit = (Element) l.next();
                    if (unit.getName().equals("unit") &&
                        !(unit.getAttributeValue("name").equals("CVS"))) {
                        try {
                            units.add
                                (new Unit(unit.getAttributeValue("name"),
                                    unit.getAttributeValue("id"),
                                    (new URL(unit.getAttributeValue("url")))));

                        }
                        catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                }
            }
            else if (name.equals("discussion")) {
                try {
                    discuss_url = new URL(feature.getAttributeValue("url"));
                }
                catch (Exception e) {
                    discuss_url = null;
                }
                ListIterator l = feature.getChildren().listIterator();
                while (l.hasNext()) {
                    name = ((Element) l.next()).getName();
                    if (name.equals("read"))
                        caps[Cap.DISCUSS_READ.ord] = true;
                    else if (name.equals("write"))
                        caps[Cap.DISCUSS_WRITE.ord] = true;
                    else if (name.equals("delete"))
                        caps[Cap.DISCUSS_DELETE.ord] = true;
                }
            }
            else if (name.equals("gradebook")) {
                ListIterator l = feature.getChildren().listIterator();
                while (l.hasNext()) {
                    name = ((Element) l.next()).getName();
                    if (name.equals("read-self"))
                        caps[Cap.GRADEBOOK_STUDENT.ord] = true;
                    else if (name.equals("write-all"))
                        caps[Cap.GRADEBOOK_INSTRUCTOR.ord] = true;
                    else if (name.equals("read-all")) {
                    }
                    //to be implemented
                }
            }
            else if (name.equals("student-submit")) {
                caps[Cap.SUBMIT_STUDENT.ord] = true;
            }
            // todo: handle other features.
        }
    }

    /**********************************************************************
     * Discussion Board
     **********************************************************************/

    /**
     * Fetch a MailMessage from the server with the given message identifier.
     *
     * @param messageId string representing the message identifier.
     * @return a MailMessage constructed from the server's response.
     * @see MailMessage
     */
    public MailMessage getMessage(int messageId)
        throws IOException, AuthenticationFailure, MessagingException
    {
        toServer
            ("<request type=\"get-message\">\n" +
                "  <id>" + messageId + "</id>\n" +
                "</request>");

        return new MailMessage
            (new StringBufferInputStream
                (fromServer().getChild("discussion-message").getText()));
    }

    /**
     * Delete a discussion board message with the given message identifier.
     *
     * @param messageId string representing the message identifier.
     */
    public void deleteMessage(int messageId)
        throws IOException, AuthenticationFailure
    {
        toServer
            ("<request type=\"delete-message\">\n" +
                "  <id>" + messageId + "</id>\n" +
                "</request>");

        fromServer();
    }

    /**
     * Add the given MailMessage to the discussion board.
     *
     * @param message the message to be added.
     */
    public void addMessage(MailMessage message)
        throws IOException, AuthenticationFailure
    {
        toServer
            ("<request type=\"add-message\">\n" +
                "<discussion-message>" +
                "<![CDATA[" + message.toString() + "]]>\n" +
                "</discussion-message>\n" +
                "</request>");
        fromServer();
    }


    /**********************************************************************
     * Submitted Materials
     **********************************************************************/

    /**
     * This method uploads a file the server.  Each student can upload multiple files for each unit.  The file is
     * encoded using Base 64 before being transmitted to the server.  The server will create a file having the same name
     * as the uploaded file in the unit's student upload section of the repository.
     *
     * @param unitId string identifying the unit that contains this file.
     * @param file file object representing the local file system location where the file to be uploaded resides.
     */
    public void uploadFile(String unitId, File file)
        throws IOException, AuthenticationFailure
    {

        FileInputStream in = new FileInputStream(file);
        byte[] data = new byte[in.available()];
        in.read(data);
        String content = new String(Base64.encode(data));

        toServer
            ("<request type=\"add-upload-file\" id=\"123\">\n" +
                "<unit-id>" + unitId + "</unit-id>\n" +
                "<file>" + file.getName() + "</file>\n" +
                "<file-contents>" + content + "</file-contents>\n" +
                "</request>");

        fromServer();
    }

    /**
     * This method saves a file that has been uploaded by a student to the server as a file on the user's local file
     * system.  The file is transferred from the server in a Base 64 encoding.  This method decodes the data and saves
     * it to file.
     *
     * @param unitId string identifying the unit that contains this file.
     * @param dest file object representing the local file system location where the file should be saved.
     * @param name string representing the name of the file to be downloaded.
     */
    public void downloadFile(String unitId, File dest, String name)
        throws IOException, AuthenticationFailure
    {

        toServer
            ("<request type=\"get-upload-file\" id=\"123\">\n" +
                "<unit-id>" + unitId + "</unit-id>\n" +
                "<file>" + name + "</file>\n" +
                "</request>\n");

        //The server returns a response containing a file-contents element
        //with the base64 encoded contents of the requested file.

        Element response = fromServer();
        FileOutputStream out = new FileOutputStream(dest);
        byte[] data =
            Base64.decode(response.getChild("file-contents").getText());
        out.write(data);
        out.close();
    }

    /**
     * This method deletes a file that has been uploaded by a student to the server.  Each student can upload multiple
     * files for each unit.
     *
     * @param unitId string identifying the unit that contains this file.
     * @param name string representing the name of the file to be deleted.
     */
    public void deleteFile(String unitId, String name)
        throws IOException, AuthenticationFailure
    {

        toServer
            ("<request type=\"delete-upload-file\" id =\"123\">\n" +
                "<unit-id>" + unitId + "</unit-id>\n" +
                "<file>" + name + "</file>\n" +
                "</request>\n");

        fromServer();
    }

    /**
     * This method returns a list iterator that enumerates the names of the files that have been uploaded for the given
     * unit.
     *
     * @param unitId string identifying the unit.
     */
    public ListIterator getSubmittedFilesList(String unitId)
        throws IOException, AuthenticationFailure
    {

        toServer
            ("<request type=\"get-upload-filelist\">\n" +
                "<unit-id>" + unitId + "</unit-id>\n" +
                "</request>\n");

        Element response = fromServer();
        ListIterator files
            = response.getChildren("file").listIterator();

        List filelist = new LinkedList();
        while (files.hasNext()) {
            filelist.add(((Element) files.next()).getText());
        }
        return filelist.listIterator();
    }


    /**********************************************************************
     * Gradebook
     **********************************************************************/

    /**
     * Fetch a list of assignments.
     */
    public Object[][] getAssignmentList() throws IOException, AuthenticationFailure
    {
        toServer("<request type=\"gradebook-read\" id=\"123\">\n" +
            "<assignment-list/>" +
            "</request>");

        Element assignments = fromServer();
        Iterator iter = assignments.getChild("assignment-list").getChildren().iterator();
        LinkedList descriptions = new LinkedList();
        LinkedList ids = new LinkedList();

        while (iter.hasNext()) {
            Element assignment = (Element) iter.next();
            ids.add(assignment.getAttributeValue("id"));
            descriptions.add(assignment.getAttributeValue("description"));
        }

        Object[][] data = new Object[ids.size()][2];
        for (int i = 0; i < ids.size(); i++) {
            data[i][0] = ids.get(i);
            data[i][1] = descriptions.get(i);
        }


        return data;
    }

    /**
     * Fetch a list of students.
     */
    public Object[][] getStudentList() throws IOException, AuthenticationFailure
    {
        toServer
            ("<request type=\"gradebook-read\" id=\"123\">\n" +
                "<student-list/>" +
                "</request>");


        Element students = fromServer();
        Iterator iter = students.getChild("student-list").getChildren().iterator();
        LinkedList ids = new LinkedList();
        LinkedList lName = new LinkedList();
        LinkedList fName = new LinkedList();

        while (iter.hasNext()) {
            Element student = (Element) iter.next();
            ids.add(student.getAttributeValue("id"));
            lName.add(student.getAttributeValue("lname"));
            fName.add(student.getAttributeValue("fname"));
        }

        Object[][] data = new Object[ids.size()][3];
        for (int i = 0; i < ids.size(); i++) {
            data[i][0] = ids.get(i);
            data[i][1] = lName.get(i);
            data[i][2] = fName.get(i);
        }


        return data;
    }

    /**
     * Fetch a list of assignment grades for a given student
     *
     * @param student username for the student
     * @return ?
     */
    public Object[][] getAssignmentGrades(String student) throws IOException, AuthenticationFailure
    {
        toServer("<request type=\"gradebook-read\" id=\"123\">\n" +
            "<student-grades>" + student + "</student-grades>" +
            "</request>");

        Element assignments = fromServer();
        Iterator iter = assignments.getChild("assignment-list").getChildren().iterator();
        LinkedList grades = new LinkedList();
        LinkedList descr = new LinkedList();
        LinkedList ids = new LinkedList();

        while (iter.hasNext()) {
            Element assignment = (Element) iter.next();
            ids.add(assignment.getAttributeValue("id"));
            descr.add(assignment.getAttributeValue("description"));
            grades.add(assignment.getText());
        }

        Object[][] data = new Object[ids.size()][3];
        for (int i = 0; i < descr.size(); i++) {
            data[i][0] = ids.get(i);
            data[i][1] = descr.get(i);
            data[i][2] = grades.get(i);
        }


        return data;


    }

    public Object[][] getStudentGrades(String assignId) throws IOException, AuthenticationFailure
    {
        //to do: get names and grades for this particular assignment, then return it.

        toServer("<request type=\"gradebook-read\" id=\"123\">\n" +
            "<assignment-grades>" + assignId + "</assignment-grades>" +
            "</request>");

        Element assignments = fromServer();
        Iterator iter = assignments.getChild("student-list").getChildren().iterator();
        LinkedList grades = new LinkedList();
        LinkedList ids = new LinkedList();

        while (iter.hasNext()) {
            Element assignment = (Element) iter.next();
            ids.add(assignment.getAttributeValue("id"));
            grades.add(assignment.getText());
        }

        Object[][] data = new Object[ids.size()][3];
        for (int i = 0; i < ids.size(); i++) {
            data[i][0] = ids.get(i);
            data[i][1] = null;
            data[i][2] = grades.get(i);
        }

        return data;
    }

    //submit all assignment grades for this particular student
    public void submitStudentGrades(Object[][] data, String student) throws IOException, AuthenticationFailure
    {

        String grades = new String();
        for (int i = 0; i < data.length; i++) {
            grades += "<assignment id=\"" + data[i][0] + "\">" + data[i][2] + "</assignment>\n";
        }

        toServer
            ("<request type=\"gradebook-write\" id=\"123\">\n" +
                "<student-id>" + student + "</student-id>\n" +
                "<grades>" + grades + "</grades>\n" +
                "</request>");

        fromServer();

    }


    //submit all student's grades for this particular assignment
    public void submitAssignmentGrades(Object[][] data, String assignmentNumber) throws IOException, AuthenticationFailure
    {

        String grades = "";
        for (int i = 0; i < data.length; i++) {
            grades += "<student id=\"" + data[i][0] + "\">" + data[i][2] + "</student>\n";
        }
        toServer
            ("<request type=\"gradebook-write\" id=\"123\">\n" +
                "<assignment-id>" + assignmentNumber + "</assignment-id>\n" +
                "<grades>" + grades + "</grades>\n" +
                "</request>");

        fromServer();

    }

    /**
     * Send a request to the server.  Prepends the length (in bytes) to the first line of the request and places XML
     * declaration on second line.
     */
    private void toServer(String request) throws IOException
    {
        PrintWriter out = new PrintWriter(sock.getOutputStream(), true);
        request = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n".concat(request);
        if (DEBUG) {
            System.out.println(request);
        }
        out.println(request.getBytes().length);
        out.println(request);
    }

    /**
     * Read a response from the server and return the root element. Throws an AuthenticationFailure if the response is
     * not well-formed, does not adhere to the CS protocol, or is a "failure" response.  If the response is a success,
     * return the root element, ie <tt>&lt;response&gt;...&lt;/response&gt;</tt>.
     */
    private Element fromServer() throws AuthenticationFailure
    {
        Document doc = null;
        try {
            DataInputStream in = new DataInputStream(sock.getInputStream());

            // DataInputStream readLine is deprecated, but I don't know a work around.
            int length = Integer.valueOf(in.readLine()).intValue();
            byte data[] = new byte[length];
            in.readFully(data);

            if (DEBUG) {
                System.out.println(length);
                System.out.println(new String(data));
            }
            doc = new SAXBuilder().build(new ByteArrayInputStream(data));
        }
        catch (JDOMException e) {
            logout();
            throw new AuthenticationFailure
                ("server response is not well-formed XML:" + e.getMessage());
        }
        catch (IOException e) {
            logout();
            throw new AuthenticationFailure
                ("An IO error occured while reading the server's response:" + e.getMessage());
        }

        Element root = null;
        boolean success = false;
        try {
            root = doc.getRootElement();
            success = (root.getName().equals("response") &&
                root.getAttributeValue("status").equals("success"));

        }
        catch (Exception e) {
            // An exception occurs here only if the XML returned does not
            // conform to the CS protocol.  We treat this case as an
            // authentication failure.
            logout();
            throw new AuthenticationFailure
                ("server response is not CS message");
        }
        if (!success) {
            logout();
            throw new AuthenticationFailure("request failed");
        }
        return root;
    }

    /**
     * This method is used to test the class.  A gateway object is constructed and the login method is invoked.
     */
    public static void main(String[] argv)
    {
        try {
            CSGateway gate = new CSGateway();
            gate.login("jjones", "humble");
            gate.logout();
        }
        catch (UnknownHostException e) {
            System.out.println("Unknown Host: " + e);
        }
        catch (IOException e) {
            System.out.println("IOException: " + e);
        }
        catch (AuthenticationFailure e) {
            System.out.println("Authentication Failure: " + e);
        }
    }

    /**
     * Sets the state of the gateway to the initial state.
     */
    private void resetFeatures()
    {
        logged_in = false;
        discuss_url = null;
        username = null;
        for (int i = Cap.size() - 1; i > 0; i--) caps[i] = false;
        caps[Cap.CONTENT_GUEST.ord] = true;
    }

    /**
     * Returns true if granted the given capability.
     *
     * @param capability
     * @return true iff the gateway has been granted the given capability.
     */
    public boolean allows(Cap capability)
    {
        return caps[capability.ord];
    }

    /**
     * Returns true if the gateway is in a "logged in" state.
     *
     * @return true iff the gateway is logged in to the server.
     * @see #login(java.lang.String, java.lang.String)
     * @see #logout()
     */
    public boolean getLoggedIn()
    {
        return logged_in;
    }

    /**
     * Fetches the top-level URL for the discussion board.
     *
     * @return top-level URL for the discussion board.
     */
    public URL getDiscussionURL()
    {
        return discuss_url;
    }

    public ListIterator getUnits()
    {
        return units.listIterator();
    }

    //public String getName() { return name; }
    public String getUsername()
    {
        return username;
    }

    private Socket sock = null;
    private String host;
    private int port;
    private static String default_host = "your.ballistichelmet.org";
    private static int default_port = 8080;
    private static URL default_course_url = null;

    private boolean[] caps = new boolean[Cap.size()];
    private boolean logged_in;
    private URL discuss_url;
    private URL course_url;

    private List units = new LinkedList();

    //private string name;
    private String username;
}

class AuthenticationFailure extends Exception {
    public AuthenticationFailure()
    {
        super();
    }

    public AuthenticationFailure(String s)
    {
        super(s);
    }
}
