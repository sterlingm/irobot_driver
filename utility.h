#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <sstream>
#include <pthread.h>

#define BUMP 7
#define WHEEL_DROP 7
#define WALL 8
#define CLIFF_LEFT 9
#define CLIFF_FRONT_LEFT 10
#define CLIFF_FRONT_RIGHT 11
#define CLIFF_RIGHT 12
#define VIRTUAL_WALL 13
#define LOW_SIDE_DRIVER 14
#define WHEEL_OVERCURRENT 14
#define INFRARED_BYTE 17
#define BUTTONS 18
#define DISTANCE 19
#define ANGLE 20
#define CHARGING_STATE 21
#define VOLTAGE 22
#define CURRENT 23
#define BATTERY_TEMPERATURE 24
#define BATTERY_CHARGE 25
#define BATTERY_CAPACITY 26
#define WALL_SIGNAL 27
#define CLIFF_LEFT_SIGNAL 28
#define CLIFF_FRONT_LEFT_SIGNAL 29
#define CLIFF_FRONT_RIGHT_SIGNAL 30
#define CLIFF_RIGHT_SIGNAL 31
#define CARGO_BAY_DIGITAL_INPUTS 32
#define CARGO_BAY_ANALOG_SIGNAL 33
#define CHARGING_SOURCES_AVAILABLE 34
#define OI_MODE 35
#define SONG_NUMBER 36
#define SONG_PLAYING 37
#define NUMBER_OF_STREAM_PACKETS 38
#define REQUESTED_VELOCITY 39
#define REQUESTED_RADIUS 40
#define REQUESTED_RIGHT_VELOCITY 41
#define REQUESTED_LEFT_VELOCITY 42



#define FORWARD 1
#define BACKWARDS 2
#define TURNCW 3
#define TURNCCW 4

#define PORT "4950"


#define ROBOT_PORT 16
#define ROBOT_BAUDRATE 57600
#define VELOCITY 200



/*
 struct to hold sensor values
 index 0 is the first byte, index 1 is the second byte
 a value of -1 represents no value
*/
struct Sensor_Packet {
    int values[2];
    Sensor_Packet() {values[0] = -1; values[1] = -1;}
};

static pthread_mutex_t mutex_agent = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex_sensors = PTHREAD_MUTEX_INITIALIZER;

static int MENU_SLEEP_TIME = 850000;
static int UPDATE_PATH_TIME = 200000;

//~1ft more than 305 to account for speeding up
static int UNIT_SIZE = 320;

static int lowsv = -1;
static int highsv = -1;


#endif
