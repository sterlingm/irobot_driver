# This makefile compiles ....

INCLUDE = -I/usr/X11R6/include 
#INCLUDE_W32 = -Isrc 
srcdir = .
VPATH =  .

CC=g++ 
CFLAGS=-w -D LINUX -O3 -fpermissive 
OBJDIR=obj
LDFLAGS= -L/usr/X11R6/lib$(LIBSELECT) -lfltk
SOURCES_RAW=main.cpp Robot.cpp rs232.c GUIWindow.cpp

TARGET:= go
TARGETD:= go_d


OBJECTS:=$(SOURCES_RAW:.cpp=.o)
SOURCES:=$(SOURCES_RAW)

all: $(TARGET)
	
$(TARGET): $(OBJECTS)
	$(CC) -w -D LINUX $(INCLUDE) $^ -o $@ $(LDFLAGS)

debug: $(TARGETD)
	
$(TARGETD): $(OBJECTS)
	$(CC) -w -D LINUX $(INCLUDE) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CC) -c -g $< $(CFLAGS)-o $@ 


.PHONY : clean
clean:
	rm -f *.o
	rm -f $(TARGET) $(TARGETD)
