
#ifndef GUIWINDOW_H
#define GUIWINDOW_H
#include "Robot.h"
#include<FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>

class GUIWindow : public Fl_Window{
public:
    GUIWindow(Robot&);
    GUIWindow(int, int, const char*, Robot&);
    ~GUIWindow();

    Fl_Button* drive;
    Fl_Button* turnCW;
    Fl_Button* turnCCW;
    Fl_Button* stop;
    Fl_Button* quit;
    Fl_Button* fullMode;
    Fl_Button* safeMode;

private:
    Robot* robot;

    static void cb_drive(Fl_Widget*, void*);
    inline void cb_drive_i();

    static void cb_turnCW(Fl_Widget*, void*);
    inline void cb_turnCW_i();

    static void cb_turnCCW(Fl_Widget*, void*);
    inline void cb_turnCCW_i();

    static void cb_stop(Fl_Widget*, void*);
    inline void cb_stop_i();

    static void cb_quit(Fl_Widget*, void*);
    inline void cb_quit_i();

    static void cb_fullmode(Fl_Widget*, void*);
    inline void cb_fullmode_i();

    static void cb_safemode(Fl_Widget*, void*);
    inline void cb_safemode_i();
};
#endif
