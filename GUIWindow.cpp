#include <iostream>
#include "GUIWindow.h"
using namespace std;

//all code in here should be self explanatory


GUIWindow::GUIWindow(Robot& r) : Fl_Window(500, 350, "iRobot Create"), robot(&r) {
    begin();    //make the widgets, button for each option
        drive = new Fl_Button(50, 75, 70, 30, "Drive");
        drive->callback(cb_drive, this);
        drive->shortcut('d');

        turnCW = new Fl_Button(280, 75, 120, 30, "Turn Clockwise");
        turnCW->callback(cb_turnCW, this);
        turnCW->shortcut('c');

        turnCCW = new Fl_Button(50, 175, 180, 30, "Turn CounterClockwise");
        turnCCW->callback(cb_turnCCW, this);
        turnCCW->shortcut(FL_CTRL + 'c');

        stop = new Fl_Button(350, 175, 50, 30, "Stop");
        stop->callback(cb_stop, this);
        stop->shortcut('q');

        fullMode = new Fl_Button(50, 275, 80, 30, "Full Mode");
        fullMode->callback(cb_fullmode, this);
        fullMode->shortcut('f');

        safeMode = new Fl_Button(150, 275, 80, 30, "Safe Mode");
        safeMode->callback(cb_safemode, this);
        safeMode->shortcut('s');

        quit = new Fl_Button(350, 275, 50, 30, "Quit");
        quit->callback(cb_quit, this);
        quit->shortcut(FL_CTRL + 'q');
    end();  //end making widgets
    robot->fullMode();
    resizable(this);
    show(); //show it
}


//same as before, just able to specify dimensions
GUIWindow::GUIWindow(int w, int h, const char* title, Robot& r) : Fl_Window(w, h, title), robot(&r) {
    begin();
        drive = new Fl_Button(50, 75, 70, 30, "Drive");
        drive->callback(cb_drive, this);
        drive->shortcut('d');

        turnCW = new Fl_Button(280, 75, 120, 30, "Turn Clockwise");
        turnCW->callback(cb_turnCW, this);
        turnCW->shortcut('c');

        turnCCW = new Fl_Button(50, 175, 180, 30, "Turn CounterClockwise");
        turnCCW->callback(cb_turnCCW, this);
        turnCCW->shortcut(FL_CTRL + 'c');

        stop = new Fl_Button(310, 175, 50, 30, "Stop");
        stop->callback(cb_stop, this);
        stop->shortcut('q');

        quit = new Fl_Button(150, 275, 50, 30, "Quit");
        quit->callback(cb_quit, this);
        quit->shortcut(FL_CTRL + 'q');

        fullMode = new Fl_Button(50, 275, 80, 30, "Full Mode");
        fullMode->callback(cb_fullmode, this);
        fullMode->shortcut('f');

        safeMode = new Fl_Button(150, 275, 80, 30, "Safe Mode");
        safeMode->callback(cb_safemode, this);
        safeMode->shortcut('s');
    end();
    robot->fullMode();
    resizable(this);
    show();
}

GUIWindow::~GUIWindow() {}




//CALLBACK FUNCTIONS
//each just calls the robot's respective function


void GUIWindow::cb_drive(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_drive_i();
}

void GUIWindow::cb_drive_i() {
    robot->drive_straight(100);
}

void GUIWindow::cb_turnCW(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_turnCW_i();
}

void GUIWindow::cb_turnCW_i() {
    robot->turnClockwise(100);
}

void GUIWindow::cb_turnCCW(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_turnCCW_i();
}

void GUIWindow::cb_turnCCW_i() {
    robot->turnCounterClockwise(100);
}

void GUIWindow::cb_stop(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_stop_i();
}

void GUIWindow::cb_stop_i() {
    robot->stop();
}


void GUIWindow::cb_fullmode(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_fullmode_i();
}

void GUIWindow::cb_fullmode_i() {
    robot->fullMode();
}

void GUIWindow::cb_safemode(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_safemode_i();
}

void GUIWindow::cb_safemode_i() {
    robot->safeMode();
}


void GUIWindow::cb_quit(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_quit_i();
}

void GUIWindow::cb_quit_i() {
    robot->stop();
    hide();
}

/*
int main() {
    Robot robot(16, 57600);
    GUIWindow w(robot);
    return Fl::run();
}
*/
