/*
 * Instructor Gradebook
*/

package edu.vtc.coursesmart;

import javax.swing.*;
import javax.swing.table.AbstractTableModel;
import java.awt.event.*;
import java.awt.Dimension;
import java.awt.GridLayout;
import javax.swing.event.*;
import javax.swing.table.TableModel;
import java.util.Iterator;

public class InstructorGradebook extends Gradebook {
    private boolean DEBUG = false;
    private JButton assignmentSelectButton;
    private JButton studentSelectButton;
    private JButton submitChangesButton;
    private SelectionListener studentListener;
    private SelectionListener gradeListener;
    private SelectionListener assignmentListener;
    private JPanel gradePanel;
    private JTable students;
    private JTable assignments;
    private JTable grades;
    private int mode = 0;
    private CSGateway gateway;
    private MyTableModel gradeModel;

    private String[] studentLabel = {"UserID", "Last Name",
        "First Name"};
    private String[] assignmentLabel = {"Entry",
        "Description"};
    private String[] gradeLabel = {" ", " ", "Grade"};

    Object[][] studentList = {{" ", " "}};
    Object[][] gradeList = {{" ", " ", new Float(0)}};
    Object[][] assignmentList = {{" ", " "}};


    public InstructorGradebook(CSGateway g)
    {
        gateway = g;

        //set the student and assignment lists
        try {
            studentList = gateway.getStudentList();
            assignmentList = gateway.getAssignmentList();
        }
        catch (Exception ex) {
            System.out.println("Error retrieving data " + ex);
        }

        //set up the table displays
        students = new JTable(new MyTableModel(studentList, studentLabel));
        students.getModel().addTableModelListener(this);
        students.setPreferredScrollableViewportSize(new Dimension(250, 250));

        assignments = new JTable(new MyTableModel(assignmentList, assignmentLabel));
        assignments.getModel().addTableModelListener(this);
        assignments.setPreferredScrollableViewportSize(new Dimension(250, 250));

        gradeModel = new MyTableModel(gradeList, gradeLabel);
        gradeModel.setEditable(true);
        grades = new JTable(gradeModel);
        grades.getModel().addTableModelListener(this);
        grades.setPreferredScrollableViewportSize(new Dimension(400, 100));

        grades.getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).getParent().put(KeyStroke.getKeyStroke(KeyEvent.VK_TAB, 0, false), "tabPressed");
        grades.getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).getParent().put(KeyStroke.getKeyStroke(KeyEvent.VK_TAB, KeyEvent.SHIFT_MASK, false), "shiftTabPressed");
        grades.getActionMap().put("tabPressed", new TabAction());
        grades.getActionMap().put("shiftTabPressed", new ShiftTabAction());

        //Create the scroll pane and add the table to it.
        JScrollPane studentScrollPane = new JScrollPane(students);
        JScrollPane assignmentScrollPane = new JScrollPane(assignments);
        JScrollPane gradeScrollPane = new JScrollPane(grades);

        studentListener = new SelectionListener(students);
        assignmentListener = new SelectionListener(assignments);
        gradeListener = new SelectionListener(grades);

        //Create Buttons
        assignmentSelectButton = new JButton("Select assignment");
        assignmentSelectButton.setActionCommand("Select assignment");
        assignmentSelectButton.addActionListener(new AssignmentSelectButtonListener());

        studentSelectButton = new JButton("Select student");
        studentSelectButton.setActionCommand("Select student");
        studentSelectButton.addActionListener(new StudentSelectButtonListener());

        submitChangesButton = new JButton("Submit Changes");
        submitChangesButton.setActionCommand("Submit Changes");
        submitChangesButton.setVisible(false);
        submitChangesButton.addActionListener(new SubmitChangesButtonListener());

        //create SplitPane to display list of students and list of assignments
        JSplitPane splitPane = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT,
            studentScrollPane, assignmentScrollPane);
        JPanel listPanel = new JPanel();
        listPanel.add(splitPane);

        //create button selection panel
        JPanel buttonPanel = new JPanel();
        buttonPanel.add(studentSelectButton, "WEST");
        buttonPanel.add(assignmentSelectButton, "EAST");
        buttonPanel.setMaximumSize(new Dimension(400, 50));

        //grade panel
        gradePanel = new JPanel();
        gradePanel.setLayout(new BoxLayout(gradePanel, BoxLayout.Y_AXIS));
        gradePanel.add(gradeScrollPane);
        gradePanel.add(submitChangesButton);
        gradePanel.setPreferredSize(new Dimension(200, 200));


        //put everything together
        JPanel viewPanel = new JPanel();
        viewPanel.setLayout(new BoxLayout(viewPanel, BoxLayout.Y_AXIS));
        viewPanel.add(listPanel);
        viewPanel.add(buttonPanel);
        viewPanel.add(gradePanel);

        //and display it
        this.add(viewPanel);
    }


    //fetch and display all assignments+grades for selected student.
    class StudentSelectButtonListener implements ActionListener {
        public void actionPerformed(ActionEvent e)
        {
            try {
                Object s = students.getValueAt(studentListener.getSelectedRow(), 0);
                try {
                    gradeList = gateway.getAssignmentGrades((String) s);
                    gradeModel.setData(gradeList);
                    grades.getColumnModel().getColumn(0).setHeaderValue("Assignment Number");
                    grades.getColumnModel().getColumn(1).setHeaderValue("Description");
                    grades.getTableHeader().resizeAndRepaint();
                    mode = 1;
                    submitChangesButton.setVisible(true);

                }
                catch (Exception ex) {
                    System.out.println("Error retrieving grades " + ex);
                }
            }
            catch (NothingSelectedException ex) {
                {
                    System.out.println("No Student Selected");
                }
            }
        }
    }


    //fetch and display all students+grades for selected assignment
    class AssignmentSelectButtonListener implements ActionListener {
        public void actionPerformed(ActionEvent e)
        {
            try {
                assignmentListener.getSelectedRow();
                Object a = assignments.getValueAt(assignmentListener.getSelectedRow(), 0);
                try {
                    gradeList = gateway.getStudentGrades((String) a);

                    for (int i = 0; i < gradeList.length; i++)//set the student names from the studentList table
                    {
                        gradeList[i][1] = studentList[i][2];
                    }
                    gradeModel.setData(gradeList);
                    grades.getColumnModel().getColumn(0).setHeaderValue("UserID");
                    grades.getColumnModel().getColumn(1).setHeaderValue("Name");
                    grades.getTableHeader().resizeAndRepaint();
                    mode = 2;
                    submitChangesButton.setVisible(true);
                }
                catch (Exception ex) {
                    System.out.println("Error retrieving grades " + ex);
                }

            }
            catch (NothingSelectedException ex) {
                System.out.println("No Assignment Selected");
            }

        }
    }

    class SubmitChangesButtonListener implements ActionListener {

        public void actionPerformed(ActionEvent e)
        {
            if (gradeModel.getNumberChanged() == 0)
                grades.getCellEditor(grades.getEditingRow(), grades.getEditingColumn()).stopCellEditing();

            if (gradeModel.getNumberChanged() > 0) {
                Object[][] grades = new Object[gradeModel.getNumberChanged()][gradeModel.getColumnCount()];
                Iterator iter = gradeModel.getChangedValues();
                //create new object array grades consisting of only those rows that have been changed.
                for (int row = 0; iter.hasNext(); row++) {
                    Object changedRow = iter.next();
                    for (int col = 0; col < gradeModel.getColumnCount(); col++) {
                        grades[row][col] = gradeModel.getValueAt(((Integer) changedRow).intValue(), col);
                    }
                }


                switch (mode) {
                    case 1:
                        try {
                            gateway.submitStudentGrades(grades, ((String) students.getValueAt(studentListener.getSelectedRow(), 0)));
                        }
                        catch (Exception ex) {
                            System.out.println("Error Submitting Changes " + ex);
                        }
                        break;
                    case 2:
                        try {
                            gateway.submitAssignmentGrades(grades, ((String) assignments.getValueAt(assignmentListener.getSelectedRow(), 0)));
                        }
                        catch (Exception ex) {
                            System.out.println("Error Submitting Changes " + ex);
                        }
                        break;
                    default:
                        System.out.println("Nothing selected");
                }
            }

        }
    }


    //*****************
    //The following tab-key methods to change cell focus and editing of a JTable is taken from mar_shell@yahoo.com
    //http://forum.java.sun.com/thread.jspa?threadID=124286&messageID=963181

    private class TabAction extends AbstractAction {
        public void actionPerformed(ActionEvent ae)
        {
            int row = grades.getEditingRow();
            int col = grades.getEditingColumn();
            boolean inEditingMode = row >= 0 && col >= 0;
            //if user is not editing OR user is editing AND value did pass validation
            //send focus to next focusable component
            if ((inEditingMode && grades.getCellEditor(row, col).stopCellEditing()) || !inEditingMode) {
                grades.editCellAt(row + 1, col);
            }
        }
    }

    private class ShiftTabAction extends AbstractAction {
        public void actionPerformed(ActionEvent ae)
        {
            int row = grades.getEditingRow();
            int col = grades.getEditingColumn();
            boolean inEditingMode = row >= 0 && col >= 0;
            //if user is not editing OR user is editing AND value did pass validation
            //send focus to previous component
            if ((inEditingMode && grades.getCellEditor(row, col).stopCellEditing()) || !inEditingMode) {
                grades.editCellAt(row - 1, col);
            }
        }
    }


}
