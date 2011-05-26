#include "Robot.h"
#include "utility.h"
#include <time.h>
#include <iostream>
#include <map>

#ifndef __linux__
#define sleep Sleep
#endif

using namespace std;



int main(int argc, char* args[]) {
    //16 for /dev/ttyUSB0
	 //  Robot robot(16, 19200);
		  Robot robot(16, 57600);

    char safe_mode[2]  = {128, 131};
    char full_mode[2]  = {128, 132};
    char pause_stream[3] = {128, 150, 0};
    char resume_stream[3] = {128, 150, 1};
	 char play_led_on[4] = {139,2,0,0};
	 char drive_forward[5] = {137, 0, 100, 128, 0};
	 char two_leds_on_half[4] = {139,8,0,128};

    //Stream sensor
    //robot.sendBytes(safe_mode);
	 char start[1] = {128};
	 char full[1] = {132};
	 robot.sendBytes(start);
	 robot.sendBytes(full);

    //robot.sendBytes(full_mode);
	//sleep(2);
	 //robot.sendBytes(test);
	 //robot.sendBytes(two_leds_on_half);
	 robot.sendBytes(drive_forward);
//	 return 1;
	 //cout << "\nDriving straight.";
   //robot.drive_straight(20);
	 return 0;
	 int i=0;
    while(1) {

        //sleep for x milliseconds
        usleep(15000);
        Sensor_Packet temp;

        temp = robot.getSensorValue(REQUESTED_VELOCITY);

		  if(i % 100 == 0){
        	cout<<endl<<temp.values[0];
        	cout<<endl<<temp.values[1];
		  }
		  i++;
    }   //end while

    robot.sendBytes(pause_stream);
    return 0;
}   //END MAIN
