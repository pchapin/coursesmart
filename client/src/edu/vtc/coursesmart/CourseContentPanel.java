/**                                                         -*- Java -*-
 * CourseContentPanel
 *
 */

package edu.vtc.coursesmart;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

import calpa.html.*;

import java.net.*;
import java.io.*;
import java.util.*;
import java.util.prefs.*;

public class CourseContentPanel extends JPanel {

    private static Preferences prefs;
    private CSGateway gateway;
    private static String savedPage = null;
    private static URL savedURL = null;
    private boolean loadSaved = false;

    public CourseContentPanel(CSGateway g)
    {
        prefs = Preferences.userRoot().node("com/coursesmart/preferences");
        savedPage = prefs.get("savedPage", null);
        try {
            savedURL = new URL(savedPage);
        }
        catch (MalformedURLException m) {
            savedURL = null;
            savedPage = null;
        }
        ;
        gateway = g;

        CalHTMLPane panel =
            new CalHTMLPane(null, new MyCalHTMLObserver(), null);

        setLayout(new BorderLayout());
        add(panel, "Center");

        //create a panel, add buttons, and add a listener to the buttons
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

        ListIterator units = g.getUnits();
        Unit u;
        if (units.hasNext()) {
            u = (Unit) units.next();
            URL firstURL = (u.getURL());
            JComboBox box = new JComboBox();
            box.addActionListener(new DropListListener(panel, box));
            box.addItem(u.getName());
            while (units.hasNext()) {
                u = (Unit) units.next();
                box.addItem(u.getName());
                if (savedURL != null && u.getURL().equals(savedURL)) {
                    loadSaved = true;
                    box.setSelectedItem(u.getName());
                }
            }
            p.add(box);
            if (loadSaved) {
                panel.showHTMLDocument(savedURL);
            }
            else {
                panel.showHTMLDocument(firstURL);
            }
        }
        add(p, "North");
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

    private class DropListListener implements ActionListener {
        CalHTMLPane pane;
        JComboBox box;

        public DropListListener(CalHTMLPane pane, JComboBox box)
        {
            this.pane = pane;
            this.box = box;
        }

        public void actionPerformed(ActionEvent e)
        {
            ListIterator units = gateway.getUnits();
            Unit u;
            while (units.hasNext()) {
                u = (Unit) units.next();
                if (u.getName().equals(box.getSelectedItem())) {
                    //System.out.println(url);
                    pane.showHTMLDocument(u.getURL());
                    break;
                }
            }
        }
    }

    private static class MyCalHTMLObserver extends DefaultCalHTMLObserver {
        JLabel label;

        public MyCalHTMLObserver()
        {
            super();
            //this.label = label;
        }

        public void statusUpdate(CalHTMLPane p, int status, java.net.URL url, int value, java.lang.String message)
        {
            //write the new current url to the prefs file.
            try {
                OutputStream stream = new FileOutputStream("prefs.xml");
                prefs.exportNode(stream);
                prefs.put("savedPage", url.toExternalForm());
            }
            catch (Exception e) {
                System.out.println("Error storing currentPage: " + e);
            }
        }
    }
}
