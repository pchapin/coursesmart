
package edu.vtc.coursesmart;

import java.io.*;
import java.net.*;

public class SimpleServer {

    // This method reads a request that is assumed to be a login
    // request. It then sends back fake results to the client. This
    // method is intended to test the XML handling of the client.
    //
    public static void main(String[] argv)
        throws IOException
    {
        ServerSocket listener = new ServerSocket(8080);

        // For each client...
        while (true) {
            Socket client = listener.accept();

            PrintWriter out = new PrintWriter(client.getOutputStream(), true);
            BufferedReader in = new BufferedReader(
              new InputStreamReader(client.getInputStream()));

            // Read and display the request.
            String line;
            while ((line = in.readLine()) != null) {
                System.out.println(line);
                if (line.equals("</request>")) break;
            }

            // Send fake results.
            out.println("<response type=\"login\" status=\"success\">");
            out.println("  <role-list>student</role-list>");
            out.println("</response>");

            // Clean up.
            in.close();
            out.close();
            client.close();
        }
    }
}