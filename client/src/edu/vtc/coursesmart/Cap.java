/**                                                         -*- Java -*-
 * Cap.java 
 * $Id: Cap.java,v 1.2 2004/11/19 15:53:58 schu Exp $
 *
 * This class provides an enumeration of capabilities (hence the name
 * Cap).  The idea comes from:
 * http://www.javaworld.com/javaworld/jw-07-1997/jw-07-enumerated-p1.html
 */

package edu.vtc.coursesmart;

import java.util.*;

public final class Cap {
    private String id;
    public final int ord;
    private Cap prev;
    private Cap next;

    private static int upperBound = 0;
    private static Cap first = null;
    private static Cap last = null;

    private Cap(String anID)
    {
        this.id = anID;
        this.ord = upperBound++;
        if (first == null) first = this;
        if (last != null) {
            this.prev = last;
            last.next = this;
        }
        last = this;
    }

    public static Enumeration elements()
    {
        return new Enumeration() {
            private Cap curr = first;

            public boolean hasMoreElements()
            {
                return curr != null;
            }

            public Object nextElement()
            {
                Cap c = curr;
                curr = curr.next();
                return c;
            }
        };
    }

    public String toString()
    {
        return this.id;
    }

    public static int size()
    {
        return upperBound;
    }

    public static Cap first()
    {
        return first;
    }

    public static Cap last()
    {
        return last;
    }

    public Cap prev()
    {
        return this.prev;
    }

    public Cap next()
    {
        return this.next;
    }

    public static final Cap CONTENT_GUEST = new Cap("Content-Guest");
    public static final Cap CONTENT_READ = new Cap("Content-Read");
    public static final Cap DISCUSS_READ = new Cap("Discuss-Read");
    public static final Cap DISCUSS_WRITE = new Cap("Discuss-Write");
    public static final Cap DISCUSS_DELETE = new Cap("Discuss-Delete");
    public static final Cap SUBMIT_STUDENT = new Cap("Submit-Student");
    public static final Cap GRADEBOOK_STUDENT = new Cap("Gradebook-Student");
    public static final Cap SUBMIT_INSTRUCTOR = new Cap("Instructor-Submit");
    public static final Cap GRADEBOOK_INSTRUCTOR = new Cap("Gradebook-Instructor");
} 
