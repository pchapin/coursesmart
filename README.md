
CourseSmart
===========

CourseSmart is a course delivery package produced in the Fall 2004 session of CS-205 (Software
Engineering) at the University of Vermont. The package is (C) Copyright 2004 by the following
authors:

    Peter C. Chapin
    Shaun Chu
    Ahmed Foco
    David Guay
    Germain Njila Mopa
    David Van Horn

We are distributing this package under the terms of the GNU Public License and the GNU Free
Documenation Licence, as appropriate.

    This program is free software; you can redistribute it and/or modify it under the terms of
    the GNU General Public License as published by the Free Software Foundation; either version
    2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
    without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with this program;
    if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307 USA

The GPL is intended to cover all the Java source code in the client directory (and its
subdirectories) and all the C++ source code in the server directory (and its subdirectories).
See the file GPL.txt in one of those directories for the details of the license.

The GFDL is intended to cover all the documentation in the doc directory (and its
subdirectories), the www directory (and its subdirectories), the http directory (and its
subdirectories), and the server/repository directory (and its subdirectories). See the file
GFDL.txt in one of those directories for the details of the license.

This distribution contains some third party libraries that are NOT necessarily covered by the
GPL and the statements made here are not intended to cover those libraries. Those libraries are
contained in JAR files in the client directory; all JAR files in that directory are from third
parties, however no other part of this distribution is third party code.

Source Organization
-------------------

The following directories are part of the source distribution.

+ client: Source code for the Java client.

+ doc: Design documentation in Microsoft Word format.

+ httpd: Sample configuration file for the CERN httpd web server (used to serve course content).

+ server: Source code for the C++ server (written to POSIX API).

  + repository: Test repository with sample course content (including user level
      documentation... currently very sketchy).

  + test: Test Perl client used to send CourseSmart protocol messages to the server. Also text
      fixture for unit tests of server components.

Building CourseSmart
--------------------

The client and server are built separately.

To build the client, load the client project into IntelliJ and build it using the IntelliJ
menus. CourseSmart is known to compile with Java 1.7. The libraries needed to build the client
are included in this source distribution.

To build the server, go into the server directory and type 'make'. You will need a reasonably
recent version of g++ in your path. CourseSmart is known to compile with the 3.3.3 version of
g++. You will also need a copy of the Xerces XML parser library installed. This library is not
provided as part of the distribution. CourseSmart is known to work with v2.5 of Xerces.

Running CourseSmart
-------------------

To run the server execute 'main' in the server directory. By default it will listen on port 8080
and use the directory './repository' for its repository. This distribution comes with a sample
repository below the server's home directory so executing 'main' in the server's home directory
will cause it to use this sample repository by default. You can use the -p and -r command line
options on the server to override the defaults. For example:

    main -p 9000 -r /path/to/repository

Currently the repository needs to exist with certain control files and directories already
established before the server will function properly. If you are interested in setting up a new
repository you should copy the sample repository in this distribution to a new location and edit
the various files as you desire. In a future release, the server may automatically create all
the necessary files so that the repository directory could be initially empty.

The CourseSmart server depends on a conventional web server for distributing course content (the
URL to each unit's index page is stored in the file 'unit_index.url' in the unit's directory).
The location of that server, and of the corresponding content, is entirely independent of the
location of the CourseSmart server. However, if the instructor intends to use CourseSmart for
managing unit content via the CourseSmart client, a web server needs to run on the same machine
as the CourseSmart server. We recommend the CERN httpd server for this purpose since it is quite
lightweight and relatively easy to configure. A sample configuration file is provided in this
distribution.

The CourseSmart server also depends on Mhonarc (http://www.mhonarc.org/) for converting the
textual discussion board messages into HTML format. It is these HTML messages that are served to
the client via HTTP.

If you wish to run the server on Windows you will need the Cygwin system. You will also need to
install the cygserver module so that System V semaphores are supported. See the Cygwin
documentation for more information on doing this.

To run the client, go to the client's home directory and type 'make run'. This executes the
client with an appropriate classpath so that the libraries it needs will be accessible.

Documentation
-------------

There is some design documentation on the CourseSmart system and on the protocol used between
the client and server in the 'doc' directory in this distribution.

It is the intention to eventually prepare user level documentation as a collection of course
units in the sample repository in this distribution. That documentation does not yet exist.

Unimplemented Features
----------------------

A number of features described in the design documentation are not yet implemented (feel free to
fix this!). In particular

+ The assignment feature is implemented in a different way than described in the design
  documents. Students are allowed to upload and manipulate their submissions on a per-unit
  basis. However, there is no special support for unit assignments in the course materials.
  While the rationale given in the design documents for special support is reasonable, it was
  felt that the instructor could simply provide the assignment for a unit using an HTML page
  linked to the other pages for a unit. In this respect the abiility to post and submit
  assignments is supported.

+ The facility for on-line quizzes is entirely unimplemented.

+ The facility for an automatic glossary of terms present in the course materials is entirely
  unimplemented.

+ There is no facility that allows the instructor to add assignments or manage the user database
  from the client. The instructor needs to edit the server's control files directly. While this
  is clearly not 100% desirable, it was felt to be relatively acceptable. CourseSmart is
  designed for small scale, "personal" use. An instructor who is running the server on his/her
  office machine, for example, should have little trouble accessing and editing the necessary
  files.

+ Too much user state information is currently maintained on the client. The original concept
  was that the server would store all per-user state (such as information about the last unit
  accessed). The client would then restore that state from the server when a user logs in. This
  facility is currently unimplemented.
