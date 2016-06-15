
package edu.vtc.coursesmart;

import java.net.*;

public class Unit {

    public Unit(String name, String id, URL url)
    {
        this.name = name;
        this.id = id;
        this.url = url;
    }

    public String getName()
    {
        return name;
    }

    public String getId()
    {
        return id;
    }

    public URL getURL()
    {
        return url;
    }

    private String name;
    private String id;
    private URL url;
}
