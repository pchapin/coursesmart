
tests:	FileSystem01
	./run_tests.sh

FileSystem01:	FileSystem01.o FileSystem.o
	g++ -Wall -g -o FileSystem01 FileSystem01.o FileSystem.o

FileSystem01.o:	FileSystem01.cpp ../FileSystem.h
	g++ -Wall -g -c -I.. FileSystem01.cpp

FileSystem.o:	../FileSystem.cpp ../FileSystem.h
	g++ -Wall -g -c -I.. -o FileSystem.o ../FileSystem.cpp