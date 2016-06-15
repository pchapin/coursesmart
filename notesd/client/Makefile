############################################################################
# FILE        : Makefile
# LAST REVISED: 2005-12-23
# SUBJECT     : Makefile to build notes client
# AUTHOR      : Peter C. Chapin
############################################################################

CC     = g++
CFLAGS = -c
OBJS   = client.o container.o message.o project.o

#
# For some reason, the implicit rule below doesn't work as expected.
#
#.cpp.o:
#	$(CC) $(CFLAGS) $<

client:	$(OBJS) ../common/libcommon.a
	$(CC) -o client $(OBJS) -l../common/libcommon.a

client.o:	client.cpp container.h message.h note.h project.h \
 ../common/global.h ../common/ntp.h ../common/stack.h
	$(CC) $(CFLAGS) client.cpp

container.o:	container.cpp container.h note.h
	$(CC) $(CFLAGS) container.cpp

message.o:	message.cpp message.h note.h
	$(CC) $(CFLAGS) message.cpp

project.o:	project.cpp project.h note.h
	$(CC) $(CFLAGS) project.cpp


