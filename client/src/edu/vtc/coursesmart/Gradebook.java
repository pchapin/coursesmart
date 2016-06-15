/*
 * Gradebook
*/

package edu.vtc.coursesmart;

import javax.swing.*;
import javax.swing.table.AbstractTableModel;
import java.awt.Dimension;
import java.awt.GridLayout;
import javax.swing.event.*;
import javax.swing.table.TableModel;
import javax.swing.ListSelectionModel;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import java.util.ArrayList;
import java.util.Iterator;

public class Gradebook extends JPanel implements TableModelListener {

    private boolean ALLOW_ROW_SELECTION = true;
    private boolean DEBUG = false;
    private ArrayList changedList = new ArrayList();

    //record items in the table that are selected
    public class SelectionListener {
        int selectedRow = -1;

        //constructor sets up listener on table.
        public SelectionListener(JTable table)
        {
            table.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
            if (ALLOW_ROW_SELECTION) { // true by default
                ListSelectionModel rowSM = table.getSelectionModel();
                rowSM.addListSelectionListener(new ListSelectionListener() {
                    public void valueChanged(ListSelectionEvent e)
                    {
                        //Ignore extra messages.
                        if (e.getValueIsAdjusting()) return;

                        ListSelectionModel lsm = (ListSelectionModel) e.getSource();
                        if (lsm.isSelectionEmpty()) {
                            //System.out.println("No rows are selected.");
                        }
                        else {
                            selectedRow = lsm.getMinSelectionIndex();

                        }
                    }
                });
            }
        }

        public int getSelectedRow() throws NothingSelectedException
        {
            if (selectedRow > -1) return selectedRow;
            else throw new NothingSelectedException();
        }
    }


    //A new grade has been entered.
    public void tableChanged(TableModelEvent e)
    {
        int row = e.getFirstRow();
        int column = e.getColumn();
        TableModel model = (TableModel) e.getSource();
        if (row > -1 && column > -1) {
            String columnName = model.getColumnName(column);
            Object data = model.getValueAt(row, column);
            changedList.add(new Integer(row));
        }
    }


    class MyTableModel extends AbstractTableModel {
        private String[] columnNames = null;
        private Object[][] data = null;
        boolean edit = false;

        public Iterator getChangedValues()
        {
            return changedList.iterator();
        }

        public int getNumberChanged()
        {
            return changedList.size();
        }

        public MyTableModel(Object[][] a, String[] titles)
        {
            changedList.clear();
            columnNames = titles;
            this.data = a;
        }

        public MyTableModel()
        {
            changedList.clear();
            columnNames = null;
            this.data = null;
        }

        public void setEditable(boolean tf)
        {
            edit = tf;
        }

        public int getColumnCount()
        {
            if (data == null || data.length == 0) {
                return 0;
            }
            return columnNames.length;
        }

        public int getRowCount()
        {
            if (data == null) return 0;
            return data.length;
        }

        public String getColumnName(int col)
        {
            return columnNames[col];
        }

        public Object getValueAt(int row, int col)
        {
            return data[row][col];
        }

        public Class getColumnClass(int c)
        {
            return getValueAt(0, c).getClass();
        }

        public boolean isCellEditable(int row, int col)
        {
            //To do: If student set false, if instructor, return true
            /*if (   )
                return true;
            else  */

            if (edit && col == 2)//only allow grade column to be edited
                return true;
            else
                return false;

        }

        public void setData(Object[][] b)
        {
            changedList.clear();
            data = b;
            fireTableDataChanged();
        }

        public Object[][] getData()
        {
            return data;
        }

        public void setValueAt(Object value, int row, int col)
        {
            if (DEBUG) {
                System.out.println("Setting value at " + row + "," + col
                    + " to " + value
                    + " (an instance of "
                    + value.getClass() + ")");
            }
            if (!data[row][col].equals(value)) {
                data[row][col] = value;
                fireTableCellUpdated(row, col);
            }

            if (DEBUG) {
                System.out.println("New value of data:");
                printDebugData();
            }
        }

        private void printDebugData()
        {
            int numRows = getRowCount();
            int numCols = getColumnCount();

            for (int i = 0; i < numRows; i++) {
                System.out.print("    row " + i + ":");
                for (int j = 0; j < numCols; j++) {
                    System.out.print("  " + data[i][j]);
                }
                System.out.println();
            }
            System.out.println("--------------------------");
        }
    }


    class NothingSelectedException extends Exception {
        public NothingSelectedException()
        {
            super();
        }

        public NothingSelectedException(String s)
        {
            super(s);
        }
    }

}