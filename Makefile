# This makefile compiles ....

INCLUDE = -I/usr/include/X11 -I/usr/local/include -I/usr/local/include/FL/images -I/usr/include/freetype2


#INCLUDE_W32 = -Isrc 

srcdir = .
VPATH =  .

CC=g++ 
CFLAGS=-w -D LINUX -O3 -fpermissive 
OBJDIR=obj

LDFLAGS= -L/usr/X11R6/lib$(LIBSELECT) -lpthread -lfltk -lXext -lXft -lfontconfig -lXinerama -lpthread -ldl -lm -lX11

SOURCES_RAW= robot_driver_agent.cpp robot_driver_position.cpp robot_driver_priorityqueue.cpp main.cpp robot_driver_tree.cpp robot_driver_grid.cpp robot_driver_path.cpp tcpserver.cpp tcpclient.cpp servercontrol.cpp clientcontrol.cpp robot.cpp udpserver.cpp udpclient.cpp owncontrol.cpp guiwindow.cpp rs232.cpp

TARGET:= go
TARGETD:= go_d


OBJECTS:=$(SOURCES_RAW:.cpp=.o)
SOURCES:=$(SOURCES_RAW)



all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -w -D LINUX $(INCLUDE) $^ -o $@ $(LDFLAGS)

debug: $(TARGETD)
	
$(TARGETD): $(OBJECTS)
	$(CC) -w -D LINUX $(INCLUDE) $^ -o $@ (LDFLAGS)

%.o: %.cpp
	$(CC) -c -g $< $(CFLAGS)-o $@ 


.PHONY : clean
clean:
	rm -f *.o
	rm -f $(TARGET) $(TARGETD)
