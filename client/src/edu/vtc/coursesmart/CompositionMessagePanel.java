/**
 ** Window for message composition
 **/

package edu.vtc.coursesmart;

import javax.swing.*;
import javax.swing.text.*;
import java.awt.*;
import java.awt.event.*;
import java.util.prefs.*;
import java.io.*;
import java.net.*;

public class CompositionMessagePanel extends JDialog
    implements ActionListener {

    public CompositionMessagePanel(CSGateway gateway, MailMessage msg)
    {
        super();
        this.gateway = gateway;
        this.msg = msg;

        sendButton = new JButton("Send Message");
        cancelButton = new JButton("Cancel Message");
        defaultLabel = new JLabel("Message Composition - ");
        fromLabel = new JLabel("From      : ");
        fromField = new JLabel(gateway.getUsername());
        subjectLabel = new JLabel("Subject: ");
        subjectField = new JTextField(" ", 20);
        messageBody = new JTextArea(10, 30);
        messageBody.setText(msg.getContent());
        subjectField.setText(msg.getSubject());

        /***********************************************************/
        JPanel row1 = new JPanel();
        row1.add(defaultLabel);

        /***********************************************************/

        JPanel row2 = new JPanel();
        row2.setMaximumSize(new Dimension(500, 25));
        row2.add(fromLabel);
        row2.add(fromField);
        row2.setLayout(new BoxLayout(row2, BoxLayout.X_AXIS));

        /***********************************************************/

        JPanel row3 = new JPanel();
        row3.setMaximumSize(new Dimension(500, 25));
        row3.add(subjectLabel);
        row3.add(subjectField);
        row3.setLayout(new BoxLayout(row3, BoxLayout.X_AXIS));

        /***********************************************************/

        JTextArea row4 = messageBody;
        JScrollPane scrollpane = new JScrollPane
            (row4,
                JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
                JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
        //row4.setLineWrap(true);
        //row4.add(scrollpane);


        /**********************************************************/

        JPanel row5 = new JPanel();
        row5.add(sendButton);
        row5.add(cancelButton);
        row5.setLayout(new BoxLayout(row5, BoxLayout.X_AXIS));

        /**********************************************************/

        JPanel p = new JPanel();
        p.setLayout(new BoxLayout(p, BoxLayout.Y_AXIS));
        p.add(row1);
        p.add(row2);
        row3.setMaximumSize(new Dimension(500, 25));
        p.add(row3);
        p.add(row4);
        p.add(row5);

        sendButton.addActionListener(this);
        cancelButton.addActionListener(this);

        getContentPane().add(p);
        pack();
        setVisible(true);
    }


    /**
     * ******************************************************
     */

    public CompositionMessagePanel(CSGateway gateway)
    {
        this(gateway, new MailMessage(gateway.getUsername()));
    }

    public void actionPerformed(ActionEvent evt)
    {
        Object source = evt.getSource();
        if (source == sendButton) {

            msg.setContent(messageBody.getText());
            msg.setSubject(subjectField.getText());

            try {
                gateway.addMessage(msg);
                dispose();
            }
            catch (Exception e) {
                e.printStackTrace();
            }
        }
        else if (source == cancelButton) {
            dispose();
        }
    }

    //private fields
    private JFrame frame;
    private JButton sendButton, cancelButton;
    private JLabel fromField, defaultLabel, subjectLabel, fromLabel;
    private JTextField subjectField, messageField;
    private JTextArea messageBody;

    private CSGateway gateway;
    private MailMessage msg;
}
