# Makefile for RainlendarServer

OUTPUT      := rainlendarserver
OUTPUT_DBG  := rainlendarserver_dbg

#
#-----------------------------------------------------
#

SRCS := main.cpp RainlendarServer.cpp EventCombiner.cpp

#
# Convert the source files into object files
#
OBJS     := $(SRCS:.cpp=.o)
OBJS_DBG := $(SRCS:.cpp=_dbg.o)

#
# Include and library paths used in source
#
INCLUDES := -I../.. -I. -I../../ssobjects
LIBS_DBG := -L../../ssobjects -lssobjects_dbg -lpthread 
LIBS     := -L../../ssobjects -lssobjects     -lpthread 

#
# Build options
#
WARNINGS = -W -Wall
CCFLAGS_DBG := $(INCLUDES) $(WARNINGS) -g -fPIC  -D_REENTRANT -DDEBUG  -DUSE_NCURSES -DSERVER
CCFLAGS     := $(INCLUDES) $(WARNINGS) -fPIC -O2 -D_REENTRANT -DNDEBUG -DUSE_NCURSES -DSERVER

#
# Compiler used
#
CC := g++

#
# Targets
#
none         : release
all          : debug release
release      : tags $(OUTPUT)
debug        : tags $(OUTPUT_DBG)
makefile.dep : depend

#
# Build commands
#
%.o : %.cpp
	@echo "Compiling release $<"
	@$(CC) -c $(CCFLAGS) $< -o $@

%_dbg.o : %.cpp
	@echo "Compiling debug $<"
	@$(CC) -c $(CCFLAGS_DBG) $< -o $@

$(OUTPUT): $(OBJS)
	@echo "*** Building release $(OUTPUT)"
	@$(CC) -static -o $(OUTPUT) $(OBJS) $(LIBS)

$(OUTPUT_DBG): $(OBJS_DBG)
	@echo "*** Building debug $(OUTPUT_DBG)"
	@$(CC) -static -o $(OUTPUT_DBG) $(OBJS_DBG) $(LIBS_DBG)

tidy :
	@echo "Removing object files"
	@rm -f *.o

clean : 
	@echo "Removing all intermediate files"
	@rm -f  $(OUTPUT) $(OBJS) $(OUTPUT_DBG) $(OBJS_DBG) \
		core makefile.dep TAGS *.log *~

tags : 
	@echo Started build at:
	@date
	@echo "Building tags file"
	@etags -t *.cpp

depend: 
	@if [ ! -f makefile.dep ]; then \
	echo "Building dependancies"; \
	rm -f makefile.dep; \
	for i in $(SRCS) ; \
	do \
	$(CC) -DUSE_NCURSES $(INCLUDES) -MM -c $$i >> makefile.dep ; \
	$(CC) -DUSE_NCURSES $(INCLUDES) -MM -c $$i | sed 's/\.o/_dbg\.o/' >> makefile.dep ; \
	done; \
	fi

-include makefile.dep
