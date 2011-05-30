#include "Robot.h"
#include "utility.h"
#include <time.h>
#include <iostream>
#include <map>

#ifndef __linux__
#define sleep Sleep
#endif

using namespace std;



Robot robot(16, 57600);


void testModes() {
    int input;

    do {
    cout<<"\n1 for safe mode, 2 for full mode, 0 for quit\n";
    cin>>input;

    if(input == 1)
        robot.safeMode();
    else if(input == 2)
        robot.fullMode();
    } while(input > 0);
}

int main(int argc, char* args[]) {
    //16 for /dev/ttyUSB0
    //  Robot robot(16, 19200);

    unsigned char start[1] = {128};
    unsigned char safe_mode[2]  = {128, 131};
    unsigned char full_mode[2]  = {128, 132};
    unsigned char pause_stream[3] = {128, 150, 0};
    unsigned char resume_stream[3] = {128, 150, 1};
    unsigned char play_led_on[4] = {139,2,0,0};
    unsigned char drive_forward[5] = {137, 0, 100, 128, 0};


    unsigned char leds[4] = {139,10,255,128};

    //testModes();
    //Stream sensor
    //robot.sendBytes(safe_mode);
    //robot.sendBytes(two_leds_on_half);
    //robot.safeMode();
    //robot.LED(10, 0, 0);


    robot.sendBytes(full_mode);
    sleep(1);
    robot.leds(true, true, 128, 128);



    //robot.sendBytes(full_mode);
    //sleep(2);
    //robot.sendBytes(test);
    //robot.sendBytes(two_leds_on_half);
    //robot.sendBytes(drive_forward);
    //	 return 1;
    //cout << "\nDriving straight.";
    //robot.drive_straight(20);

    //unsigned char tosend[3] = {148, 1, 6};
    //if(!robot.sendBytes(tosend))
        //cout<<"\ncouldn't send bytes for sensor streaming\n";

/*
    while(1) {

        //sleep for x milliseconds
        usleep(15000);
        Sensor_Packet temp;
        temp = robot.getSensorValue(BUMP);

        cout<<endl<<temp.values[0];
        cout<<endl<<temp.values[1];

    }   //end while

    robot.sendBytes(pause_stream);
*/
    return 0;
}   //END MAIN
