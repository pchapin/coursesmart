#!/usr/bin/perl -w
###########################################################################
# FILE          : test_client.pl
# LAST REVISION : 2004-11-29
# SUBJECT       : Perl client to exercise CourseSmart server.
#
# This quick and dirty client sends XML messages to the server and
# displays the results. It is intended for test/debugging, not normal
# use. A more elegant program than this could be imagined.
#
###########################################################################

use IO::Handle;
use strict;
use Socket;

sub to_server {
  my ($message);

  $message = $_[0];
  $message = length($message) . "\n" . $message;
  print "\n";
  print "=====> BEGIN client request <=====\n";
  print $message;
  print "=====> END client request <=====\n";
  print SOCK $message
}

sub from_server {
  my ($line);

  print "\n";
  print "=====> BEGIN server response <=====\n";
  while ($line = <SOCK>) {
    print $line;
    if ($line eq "</response>\n") { last; }
  }
  print "=====> END server response <=====\n";
}

sub print_menu {
  print "\n";
  print "  0) Exit (closing connection)\n";
  print "  1) Log in                           8) Delete upload file\n";
  print "  2) Add discussion message           9) Read grade book lists\n";
  print "  3) Get discussion message          10) Read student grades\n";
  print "  4) Delete discussion message       11) Read assignment grades\n";
  print "  5) Get upload file list            12) Write student grades\n";
  print "  6) Add upload file                 13) Write assignment grades\n";
  print "  7) Get upload file";
  print "\nChoice => ";
  return <STDIN>;
}

# ===========
# Main Script
# ===========

my ($choice,
    $contents,
    $filename,
    $grade1, $grade2,
    $id, $id1, $id2, $sid, $sid1, $sid2,
    $line,
    $password,
    $remote, $port, $iaddr, $paddr, $proto,
    $request,
    $user);

$remote = shift || "localhost";
$port   = shift || 8080;

$iaddr  = inet_aton($remote);
$paddr  = sockaddr_in($port, $iaddr);
$proto  = getprotobyname("tcp");

socket(SOCK, PF_INET, SOCK_STREAM, $proto) or die "socket: $!";
connect(SOCK, $paddr) or die "connect: $!";
SOCK->autoflush(1);

# Handle menu selections until the user has had enough.
while (1) {
  $choice = print_menu();
  last if $choice == 0;

  # Log in.
  if ($choice == 1) {
    print "What user? ";
    chomp ($user = <STDIN>);
    print "What password? ";
    chomp ($password = <STDIN>);

    $request = "<request type=\"login\" id=\"123\">\n";
    $request = $request . "  <username>" . $user . "</username>\n";
    $request = $request . "  <password>" . $password . "</password>\n";
    $request = $request . "</request>\n";
  }

  # Add discussion message.
  elsif ($choice == 2) {
    $request = "<request type=\"add-message\" id=\"123\">\n";
    $request = $request . "<discussion-message><![CDATA[";
    $request = $request . "From: Jill Jones <jjones\@cs.uvm.edu>\n";
    $request = $request . "Subject: Introduction\n\n";
    $request = $request . "Hi guys! What's happening?\n";
    $request = $request . "]]></discussion-message>\n";
    $request = $request . "</request>\n";
  }

  # Get discussion message.
  elsif ($choice == 3) {
    print "What message ID? ";
    chomp ($id = <STDIN>);

    $request = "<request type=\"get-message\" id=\"123\">\n";
    $request = $request . "  <id>" . $id . "</id>\n";
    $request = $request . "</request>\n";
  }

  # Delete discussion message.
  elsif ($choice == 4) {
    print "What message ID? ";
    chomp ($id = <STDIN>);

    $request = "<request type=\"delete-message\" id=\"123\">\n";
    $request = $request . "  <id>" . $id . "</id>\n";
    $request = $request . "</request>\n";
  }

  # Get upload file list.
  elsif ($choice == 5) {
    print "What unit ID? ";
    chomp ($id = <STDIN>);

    $request = "<request type=\"get-upload-filelist\" id=\"123\">\n";
    $request = $request . "  <unit-id>" . $id . "</unit-id>\n";
    $request = $request . "</request>\n";
  }

  # Add upload file.
  elsif ($choice == 6) {
    print "What unit ID? ";
    chomp ($id = <STDIN>);
    print "What file? ";
    chomp ($filename = <STDIN>);
    print "File contents (base64 encoded): ";
    chomp ($contents = <STDIN>);

    $request = "<request type=\"add-upload-file\" id=\"123\">\n";
    $request = $request . "  <unit-id>" . $id . "</unit-id>\n";
    $request = $request . "  <file>" . $filename. "</file>\n";
    $request = $request . "  <file-contents>" . $contents . "</file-contents>\n";
    $request = $request . "</request>\n";
  }

  # Get upload file.
  elsif ($choice == 7) {
    print "What unit ID? ";
    chomp ($id = <STDIN>);
    print "What file? ";
    chomp ($filename = <STDIN>);

    $request = "<request type=\"get-upload-file\" id=\"123\">\n";
    $request = $request . "  <unit-id>" . $id . "</unit-id>\n";
    $request = $request . "  <file>" . $filename . "</file>\n";
    $request = $request . "</request>\n";
  }

  # Delete upload file.
  elsif ($choice == 8) {
    print "What unit ID? ";
    chomp ($id = <STDIN>);
    print "What file? ";
    chomp ($filename = <STDIN>);

    $request = "<request type=\"delete-upload-file\" id=\"123\">\n";
    $request = $request . "  <unit-id>" . $id . "</unit-id>\n";
    $request = $request . "  <file>" . $filename . "</file>\n";
    $request = $request . "</request>\n";
  }

  # Read grade book lists.
  elsif ($choice == 9) {
    $request = "<request type=\"gradebook-read\" id=\"123\">\n";
    $request = $request . "  <student-list/>\n";
    $request = $request . "  <assignment-list/>\n";
    $request = $request . "</request>\n";
  }

  # Read student grades.
  elsif ($choice == 10) {
    print "What student? ";
    chomp ($sid = <STDIN>);

    $request = "<request type=\"gradebook-read\" id=\"123\">\n";
    $request = $request . "  <student-grades>" . $sid . "</student-grades>\n";
    $request = $request . "</request>\n";
  }

  # Read assignment grades.
  elsif ($choice == 11) {
    print "What assignment id? ";
    chomp ($id = <STDIN>);

    $request = "<request type=\"gradebook-read\" id=\"123\">\n";
    $request = $request . "  <assignment-grades>" . $id . "</assignment-grades>\n";
    $request = $request . "</request>\n";
  }

  # Write student grades.
  elsif ($choice == 12) {
    print "What student? ";
    chomp ($sid = <STDIN>);
    print "First assignment? ";
    chomp ($id1 = <STDIN>);
    print "Grade? ";
    chomp ($grade1 = <STDIN>);
    print "Second assignment? ";
    chomp ($id2 = <STDIN>);
    print "Grade? ";
    chomp ($grade2 = <STDIN>);

    $request = "<request type=\"gradebook-write\" id=\"123\">\n";
    $request = $request . "  <student-id>" . $sid . "</student-id>\n";
    $request = $request . "  <grades>\n";
    $request = $request . "    <assignment id=\"" . $id1 . "\">";
    $request = $request . $grade1 . "</assignment>\n";
    $request = $request . "    <assignment id=\"" . $id2 . "\">";
    $request = $request . $grade2 . "</assignment>\n";
    $request = $request . "  </grades>\n";
    $request = $request . "</request>\n";
  }

  # Write assignment grades.
  elsif ($choice == 13) {
    print "What assignment? ";
    chomp ($id = <STDIN>);
    print "First student? ";
    chomp ($sid1 = <STDIN>);
    print "Grade? ";
    chomp ($grade1 = <STDIN>);
    print "Second student? ";
    chomp ($sid2 = <STDIN>);
    print "Grade? ";
    chomp ($grade2 = <STDIN>);

    $request = "<request type=\"gradebook-write\" id=\"123\">\n";
    $request = $request . "  <assignment-id>" . $id . "</assignment-id>\n";
    $request = $request . "  <grades>\n";
    $request = $request . "    <student id=\"" . $sid1 . "\">";
    $request = $request . $grade1 . "</student>\n";
    $request = $request . "    <student id=\"" . $sid2 . "\">";
    $request = $request . $grade2 . "</student>\n";
    $request = $request . "  </grades>\n";
    $request = $request . "</request>\n";
  }

  to_server($request);
  from_server();
}

close(SOCK);
