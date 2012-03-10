#include "guiwindow.h"


pthread_t update_sensor;


/*Callback for get sensor thread*/
void* GUIWindow::update_sensor_thread(void* threadid) {
    GUIWindow* w = (GUIWindow*)threadid;
    w->update_sensor_thread_i();
}   //END GET_C_SENSOR_THREAD


/*Inline for get sensor thread. Pollatthe robot for sensor values every 15ms*/
inline void GUIWindow::update_sensor_thread_i() {
    udps->communicate();
}   //END GET_C_SENSOR_THREAD_I



/*
 Callback function to loop
 Prints out the values of a given sensor
*/
void IdleCallback(void* v) {

    GUIWindow* window = (GUIWindow*)v;
    usleep(15000);
    if(window->network) {
        std::cout<<"\n---";
        std::cout<<"\n"<<window->tcps->get_clients()[0].agent->getHighSV()<<"\n"<<window->tcps->get_clients()[0].agent->getLowSV();
        std::cout<<"\n"<<"---"<<"\n";
    }
        else {
        sensor_packet temp;
        temp = window->getAgent()->getRobot()->get_sensor_value(window->getAgent()->getCurrentSensor());
        std::cout<<"\n---";
        std::cout<<"\n"<<temp.values[0]<<"\n"<<temp.values[1];
        std::cout<<"\n"<<"---"<<"\n";
    }
}   //END IDLECALLBACK

/*
 Contructor for a GUIWindow
 Takes in a specified robot
 By default, the sensors begin streaming when the window is created and the OI_MODE sensor is printed
*/
GUIWindow::GUIWindow(Agent& a, TcpServer& t, UdpServer& u) : Fl_Window(500, 400, "iRobot Create"), agent(&a), tcps(&t), udps(&u), network(true) {
init_widgets();
pthread_create(&update_sensor, 0, update_sensor_thread, (void*)this);
}   //END CONSTRUCTOR

/*
 Contructor for a GUIWindow
 Takes in a specified robot
 By default, the sensors begin streaming when the window is created and the OI_MODE sensor is printed
*/
GUIWindow::GUIWindow(Agent& a) : Fl_Window(500, 400, "iRobot Create"), agent(&a), network(false) {
init_widgets();
}   //END CONSTRUCTOR

/*Destructor*/
GUIWindow::~GUIWindow() {
    delete fullMode;
    delete safeMode;
    delete drive;
    delete turn;
    delete turnCW;
    delete turnCCW;
    delete stop;
    delete leds;
    delete toggleSensorStream;
    delete quit;

    delete playLED;
    delete advanceLED;

    delete velocity;
    delete driveRadius;
    delete turnAngle;
    delete turnSeconds;

    delete powerColor;
    delete powerIntensity;

    if(network)
        if(pthread_detach(update_sensor) != 0)
            printf("\ndetach on udp communication thread failed with error %m", errno);
}

void GUIWindow::init_widgets() {
    begin();    //make the widgets, button for each option

        fullMode = new Fl_Button(25, 300, 90, 30, "Full Mode");
        fullMode->when(FL_WHEN_RELEASE);
        fullMode->callback(cb_fullmode, this);
        fullMode->shortcut('f');

        safeMode = new Fl_Button(25, 350, 90, 30, "Safe Mode");
        safeMode->when(FL_WHEN_RELEASE);
        safeMode->callback(cb_safemode, this);
        safeMode->shortcut('s');
        velocity = new Fl_Input(230, 25, 50, 30, "Velocity");

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

        stop = new Fl_Button(25, 125, 70, 30, "Stop");
        stop->when(FL_WHEN_RELEASE);
        stop->callback(cb_stop, this);
        stop->shortcut('q');

        leds = new Fl_Button(25, 175, 70, 30, "LEDs");
        leds->when(FL_WHEN_RELEASE);
        leds->callback(cb_leds, this);
        leds->shortcut('l');

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
        toggleSensorStream->shortcut('r');

        whichSensor = new Fl_Choice(300, 315, 180, 25, "Sensor");
        setSensorItems();

        quit = new Fl_Button(425, 350, 50, 30, "Quit");
        quit->when(FL_WHEN_RELEASE);
        quit->callback(cb_quit, this);
        quit->shortcut(FL_CTRL + 'q');

    end();  //end making widgets
    agent->getRobot()->fullMode();
    agent->getRobot()->streamSensors();
    Fl::add_idle(IdleCallback, this);
    resizable(this);
    show(); //show it
}
/*Returns a reference to the robot*/
Agent*& GUIWindow::getAgent() {return agent;}

/*Adds items to the sensor Fl_Choice*/
void GUIWindow::setSensorItems() {

    whichSensor->add("Bump_Wheel_Drop", 0, cb_choice, (void*)this);
    whichSensor->add("Wall", 0, cb_choice, (void*)this);
    whichSensor->add("Cliff_Left", 0, cb_choice, (void*)this);
    whichSensor->add("Cliff_Front_Left", 0, cb_choice, (void*)this);
    whichSensor->add("Cliff_Front_Right", 0, cb_choice, (void*)this);
    whichSensor->add("Cliff_Right", 0, cb_choice, (void*)this);
    whichSensor->add("Virtual_Wall", 0, cb_choice, (void*)this);
    whichSensor->add("Low_Side_Driver", 0, cb_choice, (void*)this);
    whichSensor->add("Wheel_Overcurrent", 0, cb_choice, (void*)this);
    whichSensor->add("Infrared_Byte", 0, cb_choice, (void*)this);
    whichSensor->add("Buttons", 0, cb_choice, (void*)this);
    whichSensor->add("Distance", 0, cb_choice, (void*)this);
    whichSensor->add("Angle", 0, cb_choice, (void*)this);
    whichSensor->add("Charging_State", 0, cb_choice, (void*)this);
    whichSensor->add("Voltage", 0, cb_choice, (void*)this);
    whichSensor->add("Current", 0, cb_choice, (void*)this);
    whichSensor->add("Battery_Temperature", 0, cb_choice, (void*)this);
    whichSensor->add("Battery_Charge", 0, cb_choice, (void*)this);
    whichSensor->add("Battery_Capacity", 0, cb_choice, (void*)this);
    whichSensor->add("Wall_Signal", 0, cb_choice, (void*)this);
    whichSensor->add("Cliff_Left_Signal", 0, cb_choice, (void*)this);
    whichSensor->add("Cliff_Front_Left_Signal", 0, cb_choice, (void*)this);
    whichSensor->add("Cliff_Front_Right_Signal", 0, cb_choice, (void*)this);
    whichSensor->add("Cliff_Right_Signal", 0, cb_choice, (void*)this);
    whichSensor->add("Cargo_Bay_Digital_Inputs", 0, cb_choice, (void*)this);
    whichSensor->add("Cargo_Bay_Analog_Signal", 0, cb_choice, (void*)this);
    whichSensor->add("Charging_Sources_Available", 0, cb_choice, (void*)this);
    whichSensor->add("OI_Mode", 0, cb_choice, (void*)this);
    whichSensor->add("Song_Number", 0, cb_choice, (void*)this);
    whichSensor->add("Song_Playing", 0, cb_choice, (void*)this);
    whichSensor->add("Number_Of_Stream_Packets", 0, cb_choice, (void*)this);
    whichSensor->add("Requested_Velocity", 0, cb_choice, (void*)this);
    whichSensor->add("Requested_Radius", 0, cb_choice, (void*)this);
    whichSensor->add("Requested_Right_Velocity", 0, cb_choice, (void*)this);
    whichSensor->add("Requested_Left_Velocity", 0, cb_choice, (void*)this);

}   //END ADDSENSORITEMS


/*Callback function for the full mode button*/
void GUIWindow::cb_fullmode(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_fullmode_i();
}   //END CBFULLMODE

/*
 Inline function for the stop button's callback
 Puts the robot into full mode
*/
inline void GUIWindow::cb_fullmode_i() {
    if(network) {
        std::string cmd = "@ 1";
        int num_sent = send(tcps->get_clients()[0].fd, cmd.c_str(), cmd.length(), 0);
        if(num_sent < 0)
            printf("\nError sending command: %m", errno);
    }   //end if network
    else
        agent->getRobot()->fullMode();
}   //END CBFULLMODE_I

/*Callback function for the safe mode button*/
void GUIWindow::cb_safemode(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_safemode_i();
}   //END CBSAFEMODE

/*
 Inline function for the safe mode button's callback
 Puts the robot in safe mode
*/
inline void GUIWindow::cb_safemode_i() {
    if(network) {
        std::string cmd = "@ 2";
        int num_sent = send(tcps->get_clients()[0].fd, cmd.c_str(), cmd.length(), 0);
        if(num_sent < 0)
            printf("\nError sending command: %m", errno);
    }   //end if network
    else
        agent->getRobot()->safeMode();
}   //END CBSAFEMODE_I


/*Callback function for the stop button*/
void GUIWindow::cb_stop(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_stop_i();
}   //END CBSTOP

/*
 Inline function for the stop button's callback
 Stops the robot
*/
inline void GUIWindow::cb_stop_i() {
    if(network) {
        std::string cmd = "@ 3";
        int num_sent = send(tcps->get_clients()[0].fd, cmd.c_str(), cmd.length(), 0);
        if(num_sent < 0)
            printf("\nError sending command: %m", errno);
    }   //end if network

    else
        agent->getRobot()->stop();
}   //END CBSTOP_I


/*Callback function for the drive button*/
void GUIWindow::cb_drive(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_drive_i();
}   //END CBDRIVE

/*
 Inline function for the drive button's callback
 Drives the robot at the velocity specified in the Velocity text field
 If there is a value in the Radius text field, the robot drives at that radius. Otherwise, the robot drives straight
*/
inline void GUIWindow::cb_drive_i() {
    if(network) {
        std::stringstream cmd;
        cmd<<"@ 4 "<<atoi(velocity->value())<<" "<<atoi(driveRadius->value());

        int num_sent = send(tcps->get_clients()[0].fd, cmd.str().c_str(), cmd.str().length(), 0);
        if(num_sent < 0)
            printf("\nError sending command: %m", errno);
    }   //end if network

    else {
        //if the radius has a value, drive at a radius
        if(driveRadius->size() > 0)
            agent->getRobot()->drive(atoi(velocity->value()), atoi(driveRadius->value()));
        //if not, drive straight
        else
            agent->getRobot()->drive_straight(atoi(velocity->value()));
    }   //end else
}   //END CBDRIVE_I


/*Callback function for the turn button*/
void GUIWindow::cb_turn(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_turn_i();
}   //END CBTURN


/*
 Inline function for the turn button's callback
 Turns the robot the specified angle in the Angle text field and at the velocity specified in the Velocity text field
 If there is a value in the Seconds text field, the value in the Velocity text field is ignored and the robot turns that specified angle in the specified time
*/
inline void GUIWindow::cb_turn_i() {
    if(network) {
        std::stringstream cmd;
        cmd<<"@ 5 "<<atoi(turnAngle->value())<<" "<<atoi(velocity->value())<<" "<<atoi(turnSeconds->value());

        int num_sent = send(tcps->get_clients()[0].fd, cmd.str().c_str(), cmd.str().length(), 0);
        if(num_sent < 0)
            printf("\nError sending command: %m", errno);
    }

    else {
        //if seconds has a value, turn x degrees in y seconds
        if(atoi(turnSeconds->value()) == 0)
            agent->getRobot()->turnXDegrees(atoi(turnAngle->value()), atoi(velocity->value()));
        //if not, turn x degrees
        else
            agent->getRobot()->turnXDegreesInYSeconds(atoi(turnAngle->value()), atoi(turnSeconds->value()));
    }   //end else
}   //END CBTURN_I


/*Callback Function for the turn clockwise button*/
void GUIWindow::cb_turnCW(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_turnCW_i();
}   //END CBTURNCLOCKWISE

/*
 Inline function for the turn clockwise button's callback
 Turns the robot clockwise at the velocity specified in the Velocity text field indefinitely
*/
inline void GUIWindow::cb_turnCW_i() {
    int v = atoi(velocity->value());

    if(network) {
        std::stringstream cmd;
        cmd<<"@ 6 "<<v;

        int num_sent = send(tcps->get_clients()[0].fd, cmd.str().c_str(), cmd.str().length(), 0);
        if(num_sent < 0)
            printf("\nError sending command: %m", errno);
    }   //end if network

    else
        agent->getRobot()->turnClockwise(v);
}   //END CBTURNCLOCKWISE_I

/*Callback function for the turn counter clockwise button*/
void GUIWindow::cb_turnCCW(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_turnCCW_i();
}   //END CBTURNCOUNTERCLOCKWISE

/*
 Inline function for the turn counter clockwise button's callback
 Turns the robot counter clockwise at the velocity specified in the Velocity text field indefinitely
*/
inline void GUIWindow::cb_turnCCW_i() {
    int v = atoi(velocity->value());

    if(network) {
        std::stringstream cmd;
        cmd<<"@ 7 "<<v;

        int num_sent = send(tcps->get_clients()[0].fd, cmd.str().c_str(), cmd.str().length(), 0);
        if(num_sent < 0)
            printf("\nError sending command: %m", errno);
    }   //end if network

    else
        agent->getRobot()->turnCounterClockwise(v);
}   //END CBTURNCOUNTERCLOCKWISE_I


/*Callback function for the led button*/
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
inline void GUIWindow::cb_leds_i() {
    if(network) {
        std::stringstream cmd;
        cmd<<"@ 8";

        if(playLED->value())
            cmd<<" 1";
        else
            cmd<<" 0";
        if(advanceLED->value())
            cmd<<" 1";
        else
            cmd<<" 0";

        cmd<<" "<<powerColor->value()<<" "<<powerIntensity->value();

        int num_sent = send(tcps->get_clients()[0].fd, cmd.str().c_str(), cmd.str().length(), 0);
        if(num_sent < 0)
            printf("\nError sending command: %m", errno);
    }   //end if network
    else
        agent->getRobot()->leds(playLED->value(), advanceLED->value(), powerColor->value(), powerIntensity->value());
}   //END CBLEDS_I

/*Callback function for the toggle sensor stream button*/
void GUIWindow::cb_toggleSensorStream(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_toggleSensorStream_i();
}   //END CBTOGGLESENSORSTREAM

/*
 Inline function for the toggle sensor stream button's callback
 Toggles sensor streaming on and off
*/
inline void GUIWindow::cb_toggleSensorStream_i() {
    if(network) {
        std::string cmd = "@ 9";

        int num_sent = send(tcps->get_clients()[0].fd, cmd.c_str(), cmd.length(), 0);
        if(num_sent < 0)
            printf("\nError sending command: %m", errno);
    }   //end if network

    else
        agent->getRobot()->toggleSensorStream();
}   //END CBTOGGLESENSORSTREAM_I


/*Callback function or the choice menu of sensors*/
void GUIWindow::cb_choice(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_choice_i();
}   //END CBCHOICE


/*
 Inline function for the choice menu's callback function
 The selected value will change which sensor value is printing
*/
inline void GUIWindow::cb_choice_i() {
    int sensor;
    //switch for the value selected and assign to sensor
    switch(whichSensor->value()) {

        case 0:
            sensor = BUMP;
            break;
        case 1:
            sensor = WALL;
            break;
        case 2:
            sensor = CLIFF_LEFT;
            break;
        case 3:
            sensor = CLIFF_FRONT_LEFT;
            break;
        case 4:
            sensor = CLIFF_FRONT_RIGHT;
            break;
        case 5:
            sensor = CLIFF_RIGHT;
            break;
        case 6:
            sensor = VIRTUAL_WALL;
            break;
        case 7:
            sensor = LOW_SIDE_DRIVER;
            break;
        case 8:
            sensor = WHEEL_OVERCURRENT;
            break;
        case 9:
            sensor = INFRARED_BYTE;
            break;
        case 10:
            sensor = BUTTONS;
            break;
        case 11:
            sensor = DISTANCE;
            break;
        case 12:
            sensor = ANGLE;
            break;
        case 13:
            sensor = CHARGING_STATE;
            break;
        case 14:
            sensor = VOLTAGE;
            break;
        case 15:
            sensor = CURRENT;
            break;
        case 16:
            sensor = BATTERY_TEMPERATURE;
            break;
        case 17:
            sensor = BATTERY_CHARGE;
            break;
        case 18:
            sensor = BATTERY_CAPACITY;
            break;
        case 19:
            sensor = WALL_SIGNAL;
            break;
        case 20:
            sensor = CLIFF_LEFT_SIGNAL;
            break;
        case 21:
            sensor = CLIFF_FRONT_LEFT_SIGNAL;
            break;
        case 22:
            sensor = CLIFF_FRONT_RIGHT_SIGNAL;
            break;
        case 23:
            sensor = CLIFF_RIGHT_SIGNAL;
        case 24:
            sensor = CARGO_BAY_DIGITAL_INPUTS;
            break;
        case 25:
            sensor = CARGO_BAY_ANALOG_SIGNAL;
            break;
        case 26:
            sensor = CHARGING_SOURCES_AVAILABLE;
            break;
        case 27:
            sensor = OI_MODE;
            break;
        case 28:
            sensor = SONG_NUMBER;
            break;
        case 29:
            sensor = SONG_PLAYING;
            break;
        case 30:
            sensor = NUMBER_OF_STREAM_PACKETS;
            break;
        case 31:
            sensor = REQUESTED_VELOCITY;
            break;
        case 32:
            sensor = REQUESTED_RADIUS;
            break;
        case 33:
            sensor = REQUESTED_RIGHT_VELOCITY;
            break;
        case 34:
            sensor = REQUESTED_LEFT_VELOCITY;
            break;
        default: break;
    }   //END SWITCH

    if(network) {
        std::stringstream cmd;
        cmd<<"@ 0 "<<sensor;

        int num_sent = send(tcps->get_clients()[0].fd, cmd.str().c_str(), cmd.str().length(), 0);
        if(num_sent < 0)
            printf("\nError sending command: %m", errno);
    }   //end if network
    else
        agent->setCurrentSensor(sensor);
}   //END CBCHOICE_I


/*Callback function for the quit button*/
void GUIWindow::cb_quit(Fl_Widget* o, void* v) {
    GUIWindow* w = (GUIWindow*)v;
    w->cb_quit_i();
}   //END CBQUIT


/*
 Inline function for the quit button's callback
 Stops the robot, pausing sensor streaming, turns off all leds, and hides the window
*/
inline void GUIWindow::cb_quit_i() {
    if(network) {
        std::string cmd = "@ q";

        int num_sent = send(tcps->get_clients()[0].fd, cmd.c_str(), cmd.length(), 0);
        if(num_sent < 0)
            printf("\nError sending command: %m", errno);
        udps->setDone(true);
    }   //end if network

    else {
        agent->getRobot()->stop();
        agent->getRobot()->pauseSensorStream();
        agent->getRobot()->leds(false, false, 0, 0);
    }   //end else

    hide();
}   //END CBQUIT_I
