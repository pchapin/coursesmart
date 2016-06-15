############################################################################
# FILE        : Makefile
# LAST REVISED: 2005-12-23
# SUBJECT     : Makefile to build notes common library.
# AUTHOR      : Peter C. Chapin
############################################################################

CC     = g++
CFLAGS = -c
OBJS   = server.o log.o

#
# For some reason, the implicit rule below doesn't work as expected.
#
#.cpp.o:
#	$(CC) $(CFLAGS) $<

server:	$(OBJS) ../common/libcommon.a
	$(CC) -o server -L../common $(OBJS) -lcommon

server.o:	server.cpp
	$(CC) $(CFLAGS) server.cpp

log.o:		log.cpp
	$(CC) $(CFLAGS) log.cpp

