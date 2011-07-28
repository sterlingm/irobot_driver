
#ifndef GUIWINDOW_H
#define GUIWINDOW_H
#include "Robot.h"
#include<FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Choice.H>


class GUIWindow : public Fl_Window{
public:

    //! A constructor
    /*!
     * Sets robot to r
     */
    GUIWindow(Robot&);

    //! Destructor
    ~GUIWindow();

    //! Getter function for robot member
    /*!
     * Returns reference of robot member
     */
    Robot*& getRobot();

private:
    Robot* robot;

    Fl_Button* fullMode;
    Fl_Button* safeMode;
    Fl_Button* drive;
    Fl_Button* turn;
    Fl_Button* turnCW;
    Fl_Button* turnCCW;
    Fl_Button* stop;
    Fl_Button* leds;
    Fl_Button* toggleSensorStream;
    Fl_Button* quit;

    Fl_Check_Button* playLED;
    Fl_Check_Button* advanceLED;

    Fl_Input* velocity;
    Fl_Input* driveRadius;
    Fl_Input* turnAngle;
    Fl_Input* turnSeconds;

    Fl_Slider* powerColor;
    Fl_Slider* powerIntensity;

    Fl_Choice* whichSensor;

    void setSensorItems();

    static void cb_fullmode(Fl_Widget*, void*);
    void cb_fullmode_i();

    static void cb_safemode(Fl_Widget*, void*);
    void cb_safemode_i();

    static void cb_drive(Fl_Widget*, void*);
    void cb_drive_i();

    static void cb_turn(Fl_Widget*, void*);
    void cb_turn_i();

    static void cb_turnCW(Fl_Widget*, void*);
    void cb_turnCW_i();

    static void cb_turnCCW(Fl_Widget*, void*);
    void cb_turnCCW_i();

    static void cb_stop(Fl_Widget*, void*);
    void cb_stop_i();

    static void cb_leds(Fl_Widget*, void*);
    void cb_leds_i();

    static void cb_toggleSensorStream(Fl_Widget*, void*);
    void cb_toggleSensorStream_i();

    static void cb_choice(Fl_Widget*, void*);
    void cb_choice_i();

    static void cb_quit(Fl_Widget*, void*);
    void cb_quit_i();

};
#endif
