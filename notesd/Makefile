############################################################################
# FILE        : Makefile
# LAST REVISED: 2005-12-23
# SUBJECT     : Root level makefile to build notes client/server
# AUTHOR      : Peter C. Chapin
############################################################################

all:	common/libcommon.a client/client server/server

common/libcommon.a:
	cd common; $(MAKE) libcommon.a

client/client:
	cd client; $(MAKE) client

server/server:
	cd server; $(MAKE) server

clean:
	-cd common; rm -f *.o libcommon.a
	-cd client; rm -f *.o
	-cd server; rm -f *.o

