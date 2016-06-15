############################################################################
# FILE        : Makefile
# LAST REVISED: 2005-12-23
# SUBJECT     : Makefile to build notes common library.
# AUTHOR      : Peter C. Chapin
############################################################################

AR     = ar
CC     = g++
CFLAGS = -c
OBJS   = config.o connect.o ntp.o str.o

#
# For some reason, the implicit rule below doesn't work as expected.
#
#.cpp.o:
#	$(CC) $(CFLAGS) $<

libcommon.a:	$(OBJS)
	$(AR) r libcommon.a $(OBJS)

config.o:	config.cpp config.h lists.h str.h
	$(CC) $(CFLAGS) config.cpp

connect.o:	connect.cpp connect.h
	$(CC) $(CFLAGS) connect.cpp

str.o:		str.cpp str.h
	$(CC) $(CFLAGS) str.cpp

ntp.o:		ntp.cpp ntp.h connect.h
	$(CC) $(CFLAGS) ntp.cpp


