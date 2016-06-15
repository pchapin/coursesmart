/**                                                         -*- Java -*-
 * MailMessage.java
 *
 */

package edu.vtc.coursesmart;

import javax.mail.*;
import javax.mail.internet.*;
import java.io.*;
import java.util.*;
import java.text.*;

public class MailMessage {

    public MailMessage(String from)
    {
        headers = new InternetHeaders();

        headers.setHeader("Message-ID", generateID());
        headers.setHeader("From", from);
        headers.setHeader("To", MailMessage.csTo);
        DateFormat df = new SimpleDateFormat("EEE, d MMM yyyy HH:mm:ss Z");
        headers.setHeader("Date", df.format(new Date()));

        content = "";
    }

    public MailMessage(InputStream is) throws MessagingException
    {
        headers = new InternetHeaders(is);
        content = "";
        try {
            // Continue to read lines while
            // there are still some left to read
            BufferedReader in = new BufferedReader(new InputStreamReader(is));
            while (in.ready())
                content += in.readLine() + "\n";
            in.close();
        }
        catch (Exception e) {
            e.printStackTrace();
        }
        //headers.setHeader("Message-ID", generateID());
    }

    public MailMessage(InternetHeaders h, String c)
    {
        headers = h;
        content = c;
        //headers.setHeader("Message-ID", generateID());
    }

    // Constructs a new message in response to this one.
    public MailMessage reply()
    {
        return reply(headers.getHeader("From", ","));
    }

    public MailMessage reply(String from)
    {
        MailMessage m = new MailMessage(from);

        if (headers == null) System.out.println("headers null");

        InternetHeaders h = m.getHeaders();
        h.setHeader("From", from);
        h.setHeader("In-Reply-To", headers.getHeader("Message-ID", ","));
        h.addHeader("References", headers.getHeader("Message-ID", ","));
        headers.setHeader("Message-ID", generateID());
        String subj = headers.getHeader("Subject", ",");
        h.setHeader("Subject", (subj.matches("^Re:.*") ? "" : "Re: ") + subj);
        m.setHeaders(h);

        String c = "";
        c += headers.getHeader("From", ",") + " wrote:\n";
        c += "> " + content.replaceAll("\n", "\n> ");
        m.setContent(c);

        return m;
    }

    public static String generateID()
    {
        return "<" + String.valueOf(Calendar.getInstance().getTimeInMillis()) + "@coursesmart>";
    }

    // Returns a string representation of the message.
    public String toString()
    {
        String head = "";
        for (Enumeration e = headers.getAllHeaderLines();
             e.hasMoreElements(); ) {
            head += e.nextElement() + "\n";
        }
        return head + "\n" + content;
    }

    public void setHeaders(InternetHeaders h)
    {
        headers = h;
    }

    public InternetHeaders getHeaders()
    {
        return headers;
    }

    public void setContent(String c)
    {
        content = c;
    }

    public String getContent()
    {
        return content;
    }

    public String getSubject()
    {
        return headers.getHeader("Subject", ", ");
    }

    public void setSubject(String s)
    {
        headers.setHeader("Subject", s);
    }

    private InternetHeaders headers;
    private String content;

    // This is the default "To" address for all CS discussion board posts.
    public static String csTo = "CS <cs@coursesmart.org>";

    public static void main(String[] args)
    {
        // Try reading a message from a file, construct a response, and a
        // response to the response and print all messages to the std out.
        MailMessage m = null;
        try {
            m = new MailMessage(new FileInputStream("msg.txt"));
        }
        catch (Exception e) {
            e.printStackTrace();
            System.exit(-1);
        }

        System.out.println(m.toString());
        MailMessage re = m.reply();
        System.out.println(re.toString());
        System.out.println(re.reply("Bob").toString());

        // Try constructing a new message and printing it to std out.
        try {
            m = new MailMessage("John");
        }
        catch (Exception e) {
            e.printStackTrace();
            System.exit(-1);
        }

        m.setContent("Hello friend.\n");
        System.out.println(m.toString());
    }
}
