/**
 ** MessageContentPanel
 **/

package edu.vtc.coursesmart;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

import calpa.html.*;

import java.net.*;
import java.io.*;
import java.util.prefs.*;
import java.util.regex.*;

public class MessageContentPanel extends JPanel {

    public MessageContentPanel(CSGateway gateway)
    {

        this.gateway = gateway;
        CalHTMLPane htmlPane = new CalHTMLPane(null, new MyObserver(), null);
        current_url = gateway.getDiscussionURL();
        htmlPane.showHTMLDocument(gateway.getDiscussionURL());

        JPanel p = new JPanel();
        ButtonListener ml = new ButtonListener(htmlPane);
        String[] s = {"Back", "Forward"};
        JButton b;
        for (int i = 0; i < 2; i++) {
            b = new JButton(s[i]);
            b.addActionListener(ml);
            p.add(b);
        }

        /** Adds buttons to the Discussion board panel **/
        composeButton = new JButton("Compose");
        replyButton = new JButton("Reply");
        deleteButton = new JButton("Delete");
        reloadButton = new JButton("Reload");

        JPanel row1 = new JPanel();
        if (gateway.allows(Cap.DISCUSS_WRITE)) {
            p.add(composeButton);
            p.add(replyButton);
            p.add(reloadButton);
        }

        if (gateway.allows(Cap.DISCUSS_DELETE))
            p.add(deleteButton);

        p.setLayout(new BoxLayout(p, BoxLayout.X_AXIS));

        setLayout(new BorderLayout());
        add(p, "North");
        add(htmlPane, "Center");

        composeButton.addActionListener(ml);
        replyButton.addActionListener(ml);
        deleteButton.addActionListener(ml);
        reloadButton.addActionListener(ml);
    }

    private void msgFail(String msg)
    {
        JOptionPane.showMessageDialog
            (this, msg, "Operation failed", JOptionPane.OK_OPTION);
    }

    /**
     * This method takes a URL and extracts an integer message identifier from it, or returns NULL if it is not a
     * message URL.
     */
    private static Integer extractID(URL url)
    {
        Pattern p = Pattern.compile(".*/msg([0-9]+).html");
        Matcher m = p.matcher(url.toString());
        if (m.matches())
            return Integer.valueOf(m.group(1));
        else return null;
    }

    private class ButtonListener implements ActionListener {
        CalHTMLPane pane;

        public ButtonListener(CalHTMLPane pane)
        {
            this.pane = pane;
        }

        public void actionPerformed(ActionEvent e)
        {
            String s = e.getActionCommand();
            if (("Reload").equals(s)) {
                pane.reloadDocument();
            }
            else if (("Back").equals(s)) {
                pane.goBack();
            }
            else if (("Forward").equals(s)) {
                pane.goForward();
            }
            else if (("Stop").equals(s)) {
                pane.stopAll();
            }

            Object source = e.getSource();
            if (source == composeButton) {
                new CompositionMessagePanel(gateway);
            }
            else if (source == replyButton) {
                // Check the current url to see if it is a message url
                // if so, extract the message ID and fetch the message.
                // if not, display message to user.
                Integer id = extractID(current_url);
                if (id == null) msgFail("You cannot reply to a non-message.");
                else {
                    try {
                        MailMessage msg = gateway.getMessage(id.intValue());
                        new CompositionMessagePanel
                            (gateway, msg.reply(gateway.getUsername()));
                    }
                    catch (Exception exn) {
                        exn.printStackTrace();
                    }
                }
            }
            else if (source == deleteButton) {
                Integer id = extractID(current_url);
                if (id == null) msgFail("You cannot delete a non-message");
                else {
                    try {
                        gateway.deleteMessage(id.intValue());
                    }
                    catch (Exception exn) {
                        exn.printStackTrace();
                    }
                }
            }
        }
    }

    /**
     * This class extends the DefaultCalHTMLObserver and updates the private field <code>current_url</code> each time a
     * page is loaded.
     */
    private class MyObserver extends DefaultCalHTMLObserver {
        public void statusUpdate(CalHTMLPane pane, int status, URL url,
                                 int value, String message)
        {
            if (status == CalCons.DOC_LOADED) {
                current_url = url;
            }
        }
    }

    //private fields
    private static JFrame frame;
    private JButton composeButton, replyButton, deleteButton, reloadButton;
    private CSGateway gateway;
    private URL current_url;
}

		
	
	
