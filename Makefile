# This makefile compiles ....

INCLUDE = -I/usr/X11R6/include -lpthread 
#INCLUDE_W32 = -Isrc 
srcdir = .
VPATH =  .

CC=g++ 
CFLAGS=-w -D LINUX -O3 -fpermissive 
OBJDIR=obj
LDFLAGS= #-L/usr/X11R6/lib$(LIBSELECT) -lfltk -lXext -lXft -lfontconfig -lXinerama -lpthread -ldl -lm -lX11
SOURCES_RAW= agent.cpp position.cpp rs232.c priorityqueue.cpp main.cpp tree.cpp grid.cpp path.cpp tcpserver.cpp tcpclient.cpp servercontrol.cpp clientcontrol.cpp udpclient.cpp udpserver.cpp Robot.cpp 

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
