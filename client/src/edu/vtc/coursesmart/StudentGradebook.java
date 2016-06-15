/*
 * Student Gradebook
*/

package edu.vtc.coursesmart;

import javax.swing.*;
import javax.swing.table.AbstractTableModel;
import java.awt.Dimension;
import java.awt.GridLayout;
import javax.swing.table.TableModel;


public class StudentGradebook extends Gradebook {

    private JTable grades;
    private CSGateway gateway;
    private MyTableModel gradeModel;


    private String[] gradeLabel = {"Assignment Number", "Description", "Grade"};
    Object[][] gradeList = null;


    public StudentGradebook(CSGateway g)
    {
        gateway = g;

        //set the student and assignment lists
        try {
            gradeList = g.getAssignmentGrades(g.getUsername());
        }
        catch (Exception e) {
            System.out.println("Error retrieving data " + e);
        }

        //set up the table displays
        grades = new JTable(new MyTableModel(gradeList, gradeLabel));

        gradeModel = new MyTableModel(gradeList, gradeLabel);
        grades = new JTable(gradeModel);
        grades.setPreferredScrollableViewportSize(new Dimension(500, 500));

        JScrollPane gradeScrollPane = new JScrollPane(grades);


        //grade panel
        JPanel gradePanel = new JPanel();
        gradePanel.add(gradeScrollPane, "CENTER");


        add(gradePanel);
    }


}
