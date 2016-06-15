/**                                                         -*- Java -*-
 * ContentPanel
 *
 */

package edu.vtc.coursesmart;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

import calpa.html.*;

import java.net.*;
import java.io.*;
import java.util.prefs.*;

public class ContentPanel extends JPanel {

    static Preferences prefs;

    public ContentPanel(URL address)
    {
        prefs = Preferences.userRoot().node("com/coursesmart/preferences");

        JLabel label = new JLabel("0");
        label.setBorder(BorderFactory.createEmptyBorder(2, 8, 2, 8));
        label.setPreferredSize(label.getPreferredSize());
        label.setText("");
        CalHTMLPane panel =
            new CalHTMLPane(null, new MyCalHTMLObserver(label), null);

        if (address != null) {
            panel.showHTMLDocument(address);
        }

        JPanel p = new JPanel();
        ButtonListener ml = new ButtonListener(panel);
        String[] s = {"Back", "Forward"};
        JButton b;
        for (int i = 0; i < 2; i++) {
            b = new JButton(s[i]);
            b.addActionListener(ml);
            p.add(b);
        }
        p.setLayout(new BoxLayout(p, BoxLayout.X_AXIS));

        setLayout(new BorderLayout());
        add(p, "North");
        add(panel, "Center");
        add(label, "South");
    }

    private static class MyCalHTMLObserver extends DefaultCalHTMLObserver {
        JLabel label;

        public MyCalHTMLObserver(JLabel label)
        {
            super();
            this.label = label;
        }

        public void statusUpdate(CalHTMLPane p, int status, java.net.URL url, int value, java.lang.String message)
        {
            //write the new current url to the prefs file.
            try {
                OutputStream stream = new FileOutputStream("prefs.xml");
                prefs.exportNode(stream);
                prefs.put("publicPage", url.toExternalForm());
            }
            catch (Exception e) {
                System.out.println("Error storing currentPage: " + e);
            }
        }
    }

    private static class ButtonListener implements ActionListener {
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
        }
    }

}
