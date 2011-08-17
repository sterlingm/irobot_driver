# This makefile compiles ....

INCLUDE = -I/usr/X11R6/include -lpthread 
INCLUDE_GUI = -I/usr/local/include -I/usr/local/include/FL/images -I/usr/include/freetype2 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -D_THREAD_SAFE -D_REENTRANT
#INCLUDE_W32 = -Isrc 
srcdir = .
VPATH =  .

CC=g++ 
CFLAGS=-w -D LINUX -O3 -fpermissive 
OBJDIR=obj
LDFLAGS= -L/usr/X11R6/lib$(LIBSELECT) -lfltk -lXext -lXft -lfontconfig -lXinerama -lpthread -ldl -lm -lX11
SOURCES_RAW= agent.cpp position.cpp rs232.c priorityqueue.cpp main.cpp tree.cpp grid.cpp path.cpp tcpserver.cpp tcpclient.cpp servercontrol.cpp clientcontrol.cpp Robot.cpp udpserver.cpp udpclient.cpp 

TARGET:= go
TARGETD:= go_d


OBJECTS:=$(SOURCES_RAW:.cpp=.o)
SOURCES:=$(SOURCES_RAW)



all: $(TARGET)

gui: $(TARGET)
	$(CC) $(INCLUDE_GUI) -o go GUIWindow.cpp maingui.cpp Robot.cpp rs232.c position.cpp agent.cpp path.cpp grid.cpp tree.cpp priorityqueue.cpp $(LDFLAGS)
	
$(TARGET): $(OBJECTS)
	$(CC) -w -D LINUX $(INCLUDE) $^ -o $@ 

debug: $(TARGETD)
	
$(TARGETD): $(OBJECTS)
	$(CC) -w -D LINUX $(INCLUDE) $^ -o $@ 

%.o: %.cpp
	$(CC) -c -g $< $(CFLAGS)-o $@ 


.PHONY : clean
clean:
	rm -f *.o
	rm -f $(TARGET) $(TARGETD)
