/**                                                         -*- Java -*-
 * Client.java 
 * $Id: Client.java,v 1.26 2004/12/01 06:36:36 schu Exp $
 */

package edu.vtc.coursesmart;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.prefs.*;
import java.io.*;
import java.net.*;

public class Client extends JPanel {

    public Client()
    {
        super(new GridLayout(1, 1));

	/* load the publicPage from prefs file. Error case:
     * publicPage reverts to fallback page.
	 */
        prefs = Preferences.userRoot().node("com/coursesmart/preferences");
        publicPage = prefs.get("publicPage", fallback);

        add(tabbedPane);
        reloadTabs();
    }

    private static void reloadTabs()
    {
        tabbedPane.removeAll();

        //course guest tab
        if (gateway.allows(Cap.CONTENT_GUEST)) {

            try {
                courseAddress = new URL(publicPage);
            }
            //revert to fallback if publicPage is malformed URL.
            catch (MalformedURLException g) {
                try {
                    courseAddress = new URL(fallback);
                }
                catch (MalformedURLException h) {
                    System.out.println("Invalid Course Page");
                    System.exit(1);
                }
            }
            ContentPanel panel1 = new ContentPanel(courseAddress);
            panel1.setPreferredSize(new Dimension(600, 600));
            tabbedPane.addTab("Public Course Page", null, panel1, "View Course Content");
        }

        if (gateway.allows(Cap.CONTENT_READ)) {
            JComponent panel2 = new CourseContentPanel(gateway);
            panel2.setPreferredSize(new Dimension(600, 600));
            tabbedPane.addTab("Course Content", null, panel2, "View Course Content");
        }

        if (gateway.allows(Cap.DISCUSS_READ)) {
            // discussion board tab
            JComponent panel3 = new MessageContentPanel(gateway);
            tabbedPane.addTab("Discussion Board", null, panel3, "Course discussion board");
        }

        // gradebook tab
        if (gateway.allows(Cap.GRADEBOOK_STUDENT)) {
            JComponent panel5 = new StudentGradebook(gateway);
            tabbedPane.addTab("Grades", null, panel5, "View Grades");
        }

        if (gateway.allows(Cap.GRADEBOOK_INSTRUCTOR)) {
            JComponent panel5 = new InstructorGradebook(gateway);
            tabbedPane.addTab("Gradebook", null, panel5, "View Grades");
        }

        // submission tab
        if (gateway.allows(Cap.SUBMIT_STUDENT)) {
            JComponent panel7 = new StudentSubmitPanel(gateway);
            tabbedPane.addTab("Submit Files", null, panel7, "Submit Files");
        }

        if (gateway.allows(Cap.SUBMIT_INSTRUCTOR)) {
            tabbedPane.addTab("Submit Files", null, makeTextPanel("Manage Course Files"), "to be implemented");
        }
    }


    private static void createAndShowGUI()
    {
        //Make sure we have nice window decorations.
        JFrame.setDefaultLookAndFeelDecorated(true);

        //Create and set up the window.
        frame = new JFrame("CourseSmart Client");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        //Create and set up the content pane.
        JComponent newContentPane = new Client();
        newContentPane.setOpaque(true); //content panes must be opaque

        frame.getContentPane().add(new Client(),
            BorderLayout.CENTER);

        menuBar = new JMenuBar();
        menuBar.add(new FileMenu());
        frame.setJMenuBar(menuBar);

        //Display the window.
        frame.pack();
        frame.setVisible(true);
    }


    public static void main(String[] args)
    {
        //Schedule a job for the event-dispatching thread:
        //creating and showing this application's GUI.
        javax.swing.SwingUtilities.invokeLater(new Runnable() {
            public void run()
            {
                createAndShowGUI();
            }
        });
    }


    //create a simple text panel.
    protected static JComponent makeTextPanel(String text)
    {
        JPanel panel = new JPanel(false);
        JLabel filler = new JLabel(text);
        filler.setHorizontalAlignment(JLabel.CENTER);
        panel.setLayout(new GridLayout(1, 1));
        panel.add(filler);
        return panel;
    }


    // Private fields.
    private static JFrame frame;
    private static JMenuBar menuBar;
    private static CSGateway gateway = new CSGateway();
    private static JTabbedPane tabbedPane = new JTabbedPane();
    private static Preferences prefs;
    private static String publicPage = null;
    private static String fallback = "http://www.cs.uvm.edu/~dvanhorn/cs205/";
    private static URL courseAddress = null;

    private static class FileMenu
        extends JMenu
        implements ActionListener {

        public FileMenu()
        {
            super("File");
            setCourse = new JMenuItem("Set/Change Course Address");
            loginItem = new JMenuItem("Login");
            logoutItem = new JMenuItem("Logout");
            exitItem = new JMenuItem("Exit");

            logoutItem.setEnabled(false);

            setCourse.addActionListener(this);
            loginItem.addActionListener(this);
            logoutItem.addActionListener(this);
            exitItem.addActionListener(this);
            add(setCourse);
            addSeparator();
            add(loginItem);
            add(logoutItem);
            addSeparator();
            add(exitItem);
        }

        public void actionPerformed(ActionEvent evt)
        {
            Object source = evt.getSource();
            if (source == exitItem) {
                System.exit(1);
            }
            else if (source == loginItem) {
                JDialog d = new LoginPanel(frame, gateway);
                if (gateway.getLoggedIn()) {
                    reloadTabs();
                    logoutItem.setEnabled(true);
                    loginItem.setEnabled(false);
                }
            }
            else if (source == logoutItem) {
                gateway.logout();
                reloadTabs();
                logoutItem.setEnabled(false);
                loginItem.setEnabled(true);
            }
            else if (source == setCourse) {
                try {
                    // allows user to specify the course (public) page.
                    String p = JOptionPane.showInputDialog(null, "Enter Address\nFormat: http://www.uvm.edu", publicPage);
                    if (p != null) {
                        OutputStream stream = new FileOutputStream("prefs.xml");
                        prefs.exportNode(stream);
                        prefs.put("publicPage", p);
                        publicPage = p;
                        reloadTabs();
                    }
                }

                catch (Exception e) {
                    System.out.println("Error writing Preferences: " + e);
                }
            }
        }

        private JMenuItem setCourse, loginItem, logoutItem, exitItem;
    }

}
