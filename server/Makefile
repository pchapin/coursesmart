
#
# Main project dependencies
# Update when adding/removing translation units in the project.
#
OBJS = 	main.o \
	Course.o \
	Discussion.o \
	FileSystem.o \
	GradeBook.o \
	Logger.o \
	MaterialsManager.o \
	SAXHandler.o \
	UserData.o \
	UploadManager.o \
	Unit.o

CXXFLAGS += -Wall -g

#
# Header file interdependencies.
# Update when adding/removing #includes in headers.
#
FILESYSTEM_H_DEPS = FileSystem.h

LOGGER_H_DEPS = Logger.h \
	$(FILESYSTEM_H_DEPS)

UPLOADMANAGER_H_DEPS = UploadManager.h \
	$(FILESYSTEM_H_DEPS)

MATERIALSMANAGER_H_DEPS = MaterialsManager.h \
	$(FILESYSTEM_H_DEPS)

UNIT_H_DEPS = Unit.h \
	$(FILESYSTEM_H_DEPS) \
	$(LOGGER_H_DEPS) \
	$(MATERIALSMANAGER_H_DEPS) \
	$(UPLOADMANAGER_H_DEPS)

DISCUSSION_H_DEPS = Discussion.h \
	$(FILESYSTEM_H_DEPS) \
	$(LOGGER_H_DEPS)

USERDATA_H_DEPS = UserData.h \
	$(FILESYSTEM_H_DEPS) \
	$(LOGGER_H_DEPS)

GRADEBOOK_H_DEPS = GradeBook.h \
	$(FILESYSTEM_H_DEPS) \
	$(LOGGER_H_DEPS) \
	$(USERDATA_H_DEPS)

COURSE_H_DEPS = Course.h \
	$(DISCUSSION_H_DEPS) \
	$(FILESYSTEM_H_DEPS) \
	$(GRADEBOOK_H_DEPS) \
	$(LOGGER_H_DEPS) \
	$(UNIT_H_DEPS) \
	$(USERDATA_H_DEPS)

SAXHANDLER_H_DEPS = SAXHandler.h

XMLTRANS_H_DEPS = XMLtrans.h


main:	$(OBJS)
	g++ -o main $(OBJS) -lxerces-c

#
# Object file dependencies.
# Update when adding/removing #includes in .cpp files.
#

main.o:			main.cpp $(COURSE_H_DEPS) $(SAXHANDLER_H_DEPS)

Course.o:		Course.cpp $(COURSE_H_DEPS)

Discussion.o:		Discussion.cpp $(DISCUSSION_H_DEPS)

FileSystem.o:		FileSystem.cpp $(FILESYSTEM_H_DEPS)

GradeBook.o:		GradeBook.cpp $(GRADEBOOK_H_DEPS)

Logger.o:		Logger.cpp $(LOGGER_H_DEPS)

MaterialsManager.o:	MaterialsManager.cpp $(MATERIALSMANAGER_H_DEPS)

SAXHandler.o:		SAXHandler.cpp $(COURSE_H_DEPS) $(SAXHANDLER_H_DEPS) \
	$(XMLTRANS_H_DEPS)

UploadManager.o:	UploadManager.cpp $(UPLOADMANAGER_H_DEPS)

UserData.o:		UserData.cpp $(USERDATA_H_DEPS)

Unit.o:			Unit.cpp $(UNIT_H_DEPS)

#
# The usual targets
#
clean:
	rm -f core *~ *.o *.obj *.exe

#
# A 'make distclean' target should probably go through the test
# repository and remove any extra files it finds there as well. That way
# if one has been testing the server and adding files to the repository,
# distclean can put the repository back to its initial condition. This
# feature would probably entail maintaining backup copies of server
# control files somewhere and copying them back into position when
# necessary.
#
#distclean:

#
# Install/uninstall targets are nice. Maybe someday they will get added.
#
#install:
#
#uninstall:
