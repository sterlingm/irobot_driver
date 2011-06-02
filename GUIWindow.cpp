#include <iostream>
#include "GUIWindow.h"
using namespace std;

/*
 Callback function to loop
 Prints out the values of a given sensor
*/
void IdleCallback(void* pData) {

    GUIWindow* window = (GUIWindow*)(pData);
    //sleep for x milliseconds
    usleep(15000);
    Sensor_Packet temp;
    temp = window->getRobot()->getSensorValue(window->getRobot()->getCurrentSensor());
    cout<<endl<<"---";
    cout<<endl<<temp.values[0]<<endl<<temp.values[1];
    cout<<endl<<"---"<<endl;
}   //END IDLECALLBACK


/*
 Contructor for a GUIWindow
 Takes in a specified robot
*/
GUIWindow::GUIWindow(Robot& r) : Fl_Window(500, 400, "iRobot Create"), robot(&r) {
    begin();    //make the widgets, button for each option
        drive = new Fl_Button(25, 75, 70, 30, "Drive");
        drive->when(FL_WHEN_RELEASE);
        drive->callback(cb_drive, this);
        drive->shortcut('d');

        driveRadius = new Fl_Input(150, 75, 50, 30, "Radius");

        turn = new Fl_Button(300, 75, 60, 30, "Turn");
        turn->when(FL_WHEN_RELEASE);
        turn->callback(cb_turn, this);
        turn->shortcut('t');

        turnAngle = new Fl_Input(425, 75, 50, 30, "Angle");
        turnSeconds = new Fl_Input(425, 125, 50, 30, "Seconds");

        turnCW = new Fl_Button(300, 175, 120, 30, "Turn Clockwise");
        turnCW->when(FL_WHEN_RELEASE);
        turnCW->callback(cb_turnCW, this);
        turnCW->shortcut(FL_SHIFT + 't');

        turnCCW = new Fl_Button(300, 225, 180, 30, "Turn CounterClockwise");
        turnCCW->when(FL_WHEN_RELEASE);
        turnCCW->callback(cb_turnCCW, this);
        turnCCW->shortcut(FL_SHIFT + FL_CTRL + 't');

        velocity = new Fl_Input(230, 25, 50, 30, "Velocity");

        stop = new Fl_Button(25, 125, 70, 30, "Stop");
        stop->when(FL_WHEN_RELEASE);
        stop->callback(cb_stop, this);
        stop->shortcut('q');

        leds = new Fl_Button(25, 175, 70, 30, "LEDs");
        leds->when(FL_WHEN_RELEASE);
        leds->callback(cb_leds, this);
        leds->shortcut('l');

        fullMode = new Fl_Button(25, 300, 90, 30, "Full Mode");
        fullMode->when(FL_WHEN_RELEASE);
        fullMode->callback(cb_fullmode, this);
        fullMode->shortcut('f');

        safeMode = new Fl_Button(25, 350, 90, 30, "Safe Mode");
        safeMode->when(FL_WHEN_RELEASE);
        safeMode->callback(cb_safemode, this);
        safeMode->shortcut('s');

        quit = new Fl_Button(425, 350, 50, 30, "Quit");
        quit->when(FL_WHEN_RELEASE);
        quit->callback(cb_quit, this);
        quit->shortcut(FL_CTRL + 'q');


        playLED = new Fl_Check_Button(25, 200, 50, 30, "Play");
        playLED->type(FL_TOGGLE_BUTTON);

        advanceLED = new Fl_Check_Button(25, 225, 50, 30, "Advance");
        playLED->type(FL_TOGGLE_BUTTON);

        powerColor = new Fl_Slider(125, 175, 150, 30, "PowerLED Color");
        powerColor->type(FL_HOR_NICE_SLIDER);
        powerColor->minimum(0);
        powerColor->maximum(255);

        powerIntensity = new Fl_Slider(125, 225, 150, 30, "PowerLED Intensity");
        powerIntensity->type(FL_HOR_NICE_SLIDER);
        powerIntensity->minimum(0);
        powerIntensity->maximum(255);

        toggleSensorStream = new Fl_Button(300, 275, 180, 30, "Toggle Sensor Stream");
        toggleSensorStream->when(FL_WHEN_RELEASE);
        toggleSensorStream->callback(cb_toggleSensorStream, this);

        whichSensor = new Fl_Menu_Button(300, 315, 180, 25, "Sensor");
        cout<<endl<<"IN CONSTRUCTOR whichSensor: "<<whichSensor;
        whichSensor->when(FL_WHEN_RELEASE);
        whichSensor->callback(cb_choice);
        setSensorItems();


    end();  //end making widgets
    robot->fullMode();
    robot->streamSensors();
    Fl::add_idle(IdleCallback, this);
    resizable(this);
    show(); //show it
    cout<<endl<<"END OF CONSTRUCTOR whichsensor: "<<whichSensor;
}   //END CONSTRUCTOR

/*
 Destructor
*/
GUIWindow::~GUIWindow() {}

/*
 Returns a reference to the robot
*/
Robot*& GUIWindow::getRobot() {return robot;}

/*
 Adds items to the sensor Fl_Choice
*/
void GUIWindow::setSensorItems() {


    whichSensor->add("Bump", 0, 0, (void*)1);
    whichSensor->add("Wheel_Drop", 0, 0, (void*)1);
    whichSensor->add("Wall", 0, 0, (void*)1);
    whichSensor->add("Cliff_Left", 0, 0, (void*)1);
    whichSensor->add("Cliff_Front_Left", 0, 0, (void*)1);
    whichSensor->add("Cliff_Front_Right", 0, 0, (void*)1);
    whichSensor->add("Cliff_Right", 0, 0, (void*)1);
    whichSensor->add("Virtual_Wall", 0, 0, (void*)1);
    whichSensor->add("Low_Side_Driver", 0, 0, (void*)1);


}   //END ADDSENSORITEMS


/*
 Callback function for the drive button
*/
void GUIWindow::cb_drive(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_drive_i();
}   //END CBDRIVE

/*
 Inline function for the drive button's callback
 Drives the robot at the velocity specified in the Velocity text field
 If there is a value in the Radius text field, the robot drives at that radius. Otherwise, the robot drives straight
*/
void GUIWindow::cb_drive_i() {
    int v = atoi(velocity->value());
    if(v > 500)
        v = 500;

    if(driveRadius->size() > 0) {
        int r = atoi(driveRadius->value());
        if( (r > 2000) && (r != 32768) && (r != 32767) && (r != 65535) )
            r = 2000;
        else if(r < -2000)
            r = -2000;

        robot->drive(v, r);
    }
    else
        robot->drive_straight(v);
}   //END CBDRIVE_I


/*
 Callback function for the turn button
*/
void GUIWindow::cb_turn(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_turn_i();
}   //END CBTURN


/*
 Inline function for the turn button's callback
 Turns the robot the specified angle in the Angle text field and at the velocity specified in the Velocity text field
 If there is a value in the Seconds text field, the value in the Velocity text field is ignored and the robot turns that specified angle in the specified time
*/
void GUIWindow::cb_turn_i() {
    if(atoi(turnSeconds->value()) == 0) {
        int v = atoi(velocity->value());
        robot->turnXDegrees(atoi(turnAngle->value()), v);
    }
    else
        robot->turnXDegreesInYSeconds(atoi(turnAngle->value()), atoi(turnSeconds->value()));
}   //END CBTURN_I


/*
 Callback Function for the turn clockwise button
*/
void GUIWindow::cb_turnCW(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_turnCW_i();
}   //END CBTURNCLOCKWISE

/*
 Inline function for the turn clockwise button's callback
 Turns the robot clockwise at the velocity specified in the Velocity text field indefinitely
*/
void GUIWindow::cb_turnCW_i() {
    int v = atoi(velocity->value());
    robot->turnClockwise(v);
}   //END CBTURNCLOCKWISE_I

/*
 Callback function for the turn counter clockwise button
*/
void GUIWindow::cb_turnCCW(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_turnCCW_i();
}   //END CBTURNCOUNTERCLOCKWISE

/*
 Inline function for the turn counter clockwise button's callback
 Turns the robot counter clockwise at the velocity specified in the Velocity text field indefinitely
*/
void GUIWindow::cb_turnCCW_i() {
    int v = atoi(velocity->value());
    robot->turnCounterClockwise(v);
}   //END CBTURNCOUNTERCLOCKWISE_I

/*
 Callback function for the stop button
*/
void GUIWindow::cb_stop(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_stop_i();
}   //END CBSTOP

/*
 Inline function for the stop button's callback
 Stops the robot
*/
void GUIWindow::cb_stop_i() {
    robot->stop();
}   //END CBSTOP_I

/*
 Callback function for the full mode button
*/
void GUIWindow::cb_fullmode(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_fullmode_i();
}   //END CBFULLMODE

/*
 Inline function for the stop button's callback
 Puts the robot into full mode
*/
void GUIWindow::cb_fullmode_i() {
    robot->fullMode();
}   //END CBFULLMODE_I

/*
 Callback function for the safe mode button
*/
void GUIWindow::cb_safemode(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_safemode_i();
}   //END CBSAFEMODE

/*
 Inline function for the safe mode button's callback
 Puts the robot in safe mode
*/
void GUIWindow::cb_safemode_i() {
    robot->safeMode();
}   //END CBSAFEMODE_I

/*
 Callback function for the quit button
*/
void GUIWindow::cb_quit(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_quit_i();
}   //END CBQUIT

/*
 Inline function for the quit button's callback
 Stops the robot, pausing sensor streaming, turns off all leds, and hides the window
*/
void GUIWindow::cb_quit_i() {
    robot->stop();
    robot->pauseSensorStream();
    robot->leds(false, false, 0, 0);
    hide();
}   //END CBQUIT_I

/*
 Callback function for the led button
*/
void GUIWindow::cb_leds(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_leds_i();
}   //END CBLEDS

/*
 Inline function for the led button's callback
 Turns the leds to given values
 If the play checkbox is checked, the play led comes on
 If the advance checkbox is checked, the advance led comes on
 Sliding the color slider to the right makes the power led more red and sliding to the left makes it more green
 Sliding the intensity slider to the right makes the power led light brighter and sliding to the left makes it darker
*/
void GUIWindow::cb_leds_i() {
    robot->leds(playLED->value(),advanceLED->value(),powerColor->value(),powerIntensity->value());
}   //END CBLEDS_I

/*
 Callback function for the toggle sensor stream button
*/
void GUIWindow::cb_toggleSensorStream(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_toggleSensorStream_i();
}   //END CBTOGGLESENSORSTREAM

/*
 Inline function for the toggle sensor stream button's callback
 Toggles sensor streaming on and off
*/
void GUIWindow::cb_toggleSensorStream_i() {
    robot->toggleSensorStream();
}   //END CBTOGGLESENSORSTREAM_I




void GUIWindow::cb_choice(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_choice_i();
}

void GUIWindow::cb_choice_i() {
    robot->setCurrentSensor(BUMP);
}


