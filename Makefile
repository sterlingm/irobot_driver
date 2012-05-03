# This makefile compiles ....

INCLUDE = -I/usr/include/X11 -I/usr/local/include -I/usr/local/include/FL/images -I/usr/include/freetype2 


#INCLUDE_W32 = -Isrc 

CC=g++ 
CFLAGS=-g -w -D LINUX -O3 -fpermissive 
OBJDIR=objects
SRCDIR=src

LDFLAGS= -L/usr/X11R6/lib$(LIBSELECT) -lpthread -lfltk -lXext -lXft -lfontconfig -lXinerama -lpthread -ldl -lm -lX11

SOURCES_RAW= robot_driver_agent.cpp robot_driver_position.cpp robot_driver_priorityqueue.cpp main.cpp robot_driver_tree.cpp robot_driver_stack.cpp robot_driver_grid.cpp robot_driver_path.cpp grid_analyzer.cpp tcpserver.cpp tcpclient.cpp servercontrol.cpp clientcontrol.cpp robot.cpp udpserver.cpp udpclient.cpp owncontrol.cpp guiwindow.cpp rs232.cpp

TARGET:= go

OBJECTS:=$(SOURCES_RAW:.cpp=.o)
OBJECTS:=$(patsubst %.o, $(OBJDIR)/%.o, $(OBJECTS))

SOURCES:=$(SOURCES_RAW)
SOURCES:=$(patsubst %.cpp, $(SRCDIR)/%.cpp, $(SOURCES))

all: $(TARGET)

$(OBJDIR)/%.o: src/%.cpp
	$(CC) -c $< $(CFLAGS) -o $@ 

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $@ $(LDFLAGS)

#| makes it so that OBJDIR is only created if it does not exist
$(OBJECTS): | $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)



.PHONY : clean
clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(TARGET)
