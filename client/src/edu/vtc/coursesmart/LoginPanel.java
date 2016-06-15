/**                                                         -*- Java -*-
 * LoginPanel.java
 *
 */

package edu.vtc.coursesmart;

import javax.swing.*;
import java.awt.event.*;
import java.awt.*;

public class LoginPanel extends JDialog
    implements ActionListener, KeyListener {

    public LoginPanel(JFrame owner, CSGateway g)
    {
        super(owner, true);

        setLocationRelativeTo(owner);

        gateway = g;
        submitButton = new JButton("Submit");
        clearButton = new JButton("Clear");
        cancelButton = new JButton("Cancel");
        usernameField = new JTextField(20);
        usernameLabel = new JLabel("Username");
        passwordField = new JPasswordField(20);
        passwordLabel = new JLabel("Password");

        JPanel row1 = new JPanel();
        row1.add(usernameLabel);
        row1.add(usernameField);
        row1.setLayout(new BoxLayout(row1, BoxLayout.X_AXIS));

        JPanel row2 = new JPanel();
        row2.add(passwordLabel);
        row2.add(passwordField);
        row2.setLayout(new BoxLayout(row2, BoxLayout.X_AXIS));

        JPanel row3 = new JPanel();
        row3.add(submitButton);
        row3.add(clearButton);
        row3.add(cancelButton);
        row3.setLayout(new BoxLayout(row3, BoxLayout.X_AXIS));

        JPanel p = new JPanel();
        p.setLayout(new BoxLayout(p, BoxLayout.Y_AXIS));
        p.add(row1);
        p.add(row2);
        p.add(row3);

        usernameField.addKeyListener(this);
        passwordField.addKeyListener(this);

        submitButton.setMnemonic(KeyEvent.VK_S);
        clearButton.setMnemonic(KeyEvent.VK_E);
        cancelButton.setMnemonic(KeyEvent.VK_C);

        submitButton.addActionListener(this);
        clearButton.addActionListener(this);
        cancelButton.addActionListener(this);

        getContentPane().add(p);
        pack();
        setVisible(true);
    }

    public void keyReleased(KeyEvent e)
    {
    }

    public void keyTyped(KeyEvent e)
    {
    }

    public void keyPressed(KeyEvent e)
    {
        if (e.getKeyCode() == KeyEvent.VK_ENTER) {
            submitButton.doClick();
        }
    }

    public void actionPerformed(ActionEvent evt)
    {
        Object source = evt.getSource();
        if (source == submitButton) {
            try {
                gateway.login(usernameField.getText(),
                    passwordField.getText());
                dispose();
            }
            catch (Exception exn) {
                //communicate failure to user.
                JOptionPane.showMessageDialog(this,
                    exn.getMessage(),
                    "Failed",
                    JOptionPane.OK_OPTION);
                gateway.logout();
            }
        }
        else if (source == clearButton) {
            usernameField.setText(null);
            passwordField.setText(null);
        }
        else if (source == cancelButton) {
            dispose();
        }

    }

    // private fields
    private CSGateway gateway;
    private JButton submitButton, clearButton, cancelButton;
    private JLabel usernameLabel, passwordLabel;
    private JTextField usernameField, passwordField;
}
