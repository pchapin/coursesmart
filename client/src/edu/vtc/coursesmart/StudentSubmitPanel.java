/*                                                         -*- Java -*-
 * StudentSubmitPanel.java
 */

package edu.vtc.coursesmart;

import javax.swing.*;
import javax.swing.table.*;
import java.awt.*;
import java.awt.event.*;

import calpa.html.*;

import java.net.*;
import java.io.*;
import java.util.*;

/**
 * The StudentSubmitPanel is the graphical interface for the student to manage files uploaded for each unit.  Students
 * may upload and download files from and to the server, respectively.
 * <p/>
 * <p><tt> $Id: StudentSubmitPanel.java,v 1.2 2004/11/30 05:49:08 dvanhorn Exp $ </tt>
 */
public class StudentSubmitPanel extends JPanel
    implements ActionListener {

    public StudentSubmitPanel(CSGateway gateway)
    {
        this.gateway = gateway;

        JPanel p = new JPanel();
        JButton b;
        String[] s = {"Upload", "Download", "Delete"};
        for (int i = 0; i < 3; i++) {
            b = new JButton(s[i]);
            b.addActionListener(this);
            p.add(b);
        }

        ListIterator units = gateway.getUnits();
        Unit u;
        if (units.hasNext()) {
            u = (Unit) units.next();
            box = new JComboBox();
            box.addActionListener(this);
            box.addItem(u.getName());
            while (units.hasNext()) {
                u = (Unit) units.next();
                box.addItem(u.getName());
            }
            p.add(box);
        }
        p.setLayout(new BoxLayout(p, BoxLayout.X_AXIS));

        setLayout(new BorderLayout());
        add(p, "North");

        fc = new JFileChooser();

        loadFileList();
    }

    /**
     * This method returns the unit that is currently selected in the drop down list of units.
     */
    private Unit getCurrentUnit()
    {
        ListIterator units = gateway.getUnits();
        Unit unit = null;
        for (int i = 0; i <= box.getSelectedIndex(); i++) {
            unit = (Unit) units.next();
        }
        return unit;
    }

    /**
     * This method populates the table listing the files available on the server for the currently selected unit.  This
     * method should be called each time the list of files is changed (via upload/download), and whenever the user
     * selects a different unit.
     */
    private void loadFileList()
    {
        // File table
        ListIterator fileIterator = null;
        try {
            fileIterator =
                gateway.getSubmittedFilesList(getCurrentUnit().getId());
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        Vector fileData = new Vector();
        while (fileIterator.hasNext()) {
            Vector v = new Vector();
            v.add(new Boolean(false));
            v.add(fileIterator.next());
            fileData.add(v);
        }

        Vector fileLabel = new Vector();
        fileLabel.add("Select");
        fileLabel.add("File name");

        if (sp != null) remove(sp);

        table = new JTable(fileData, fileLabel);
        sp = new JScrollPane(table);

        TableColumn tc = table.getColumnModel().getColumn(0);
        tc.setCellEditor(table.getDefaultEditor(Boolean.class));
        tc.setCellRenderer(table.getDefaultRenderer(Boolean.class));
        tc.setPreferredWidth(50);

        add(sp);
        updateUI();
    }

    /**
     * This method returns a list iterator that enumerates the file names of the currently selected files (those that
     * are "checked").
     */
    private ListIterator getSelectedFiles()
    {
        TableModel model = table.getModel();
        LinkedList files = new LinkedList();
        for (int i = 0; i < table.getRowCount(); i++) {
            Boolean selected = (Boolean) model.getValueAt(i, 0);
            if (selected.booleanValue()) {
                files.add(model.getValueAt(i, 1));
            }
        }
        return files.listIterator();
    }

    public void actionPerformed(ActionEvent e)
    {
        String s = e.getActionCommand();
        if (("Upload").equals(s)) {
            fc.setMultiSelectionEnabled(true);
            if (fc.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
                File[] file = fc.getSelectedFiles();
                for (int i = 0; i < file.length; i++) {
                    try {
                        gateway.uploadFile(getCurrentUnit().getId(), file[i]);
                        loadFileList();
                    }
                    catch (Exception exn) {
                        exn.printStackTrace();
                    }
                }
            }
        }
        else if (("Download").equals(s)) {
            ListIterator files = getSelectedFiles();
            if (!files.hasNext()) {
                JOptionPane.showMessageDialog(this,
                    "No files selected",
                    "Notice",
                    JOptionPane.OK_OPTION);

            }
            else {
        /* For each selected file, open a file dialog to select a
         * destination for the file.  Save the contents of the select
		 * file to that destination.
		 */
                fc.setMultiSelectionEnabled(false);
                while (files.hasNext()) {
                    String next = (String) files.next();
                    // Dialog defaults to using same file name as on sever.
                    fc.setSelectedFile(new File(next));
                    if (fc.showOpenDialog(this) ==
                        JFileChooser.APPROVE_OPTION) {
                        File file = fc.getSelectedFile();
                        try {
                            gateway.downloadFile(getCurrentUnit().getId(),
                                file,
                                next);
                            loadFileList();
                        }
                        catch (Exception exn) {
                            exn.printStackTrace();
                        }
                    }
                }
            }
        }
        else if (("Delete").equals(s)) {
            ListIterator files = getSelectedFiles();
            if (!files.hasNext()) {
                JOptionPane.showMessageDialog(this,
                    "No files selected",
                    "Notice",
                    JOptionPane.OK_OPTION);
            }
            else {
                while (files.hasNext()) {
                    try {
                        gateway.deleteFile(getCurrentUnit().getId(),
                            (String) files.next());
                        loadFileList();
                    }
                    catch (Exception exn) {
                        exn.printStackTrace();
                    }
                }
            }
        }
        else if (("comboBoxChanged").equals(s)) {
            loadFileList();
        }
    }

    private CSGateway gateway;
    private JFileChooser fc;
    private JComboBox box;
    private JTable files;
    private JScrollPane sp = null;
    private JTable table;
}
