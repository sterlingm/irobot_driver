#ifndef ROBOT_DRIVER_UTILITY_H
#define ROBOT_DRIVER_UTILITY_H

#include <iostream>
#include <sstream>
#include <pthread.h>
#include <math.h>
#include <vector>
using namespace std;

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



#define NORTH 1
#define EAST 2
#define SOUTH 3
#define WEST 4
#define NORTHEAST 5
#define NORTHWEST 6
#define SOUTHEAST 7
#define SOUTHWEST 8


#define ASTAR 1
#define RRT 2

#define PORT "4950"


#define ROBOT_PORT 16
#define ROBOT_BAUDRATE 57600
#define COMPASS_PORT 17
#define COMPASS_BAUDRATE 19200


#define MENU_SLEEP_TIME 1000000
#define ASTAR_UPDATE_PATH_TIME 200000
#define RRT_UPDATE_PATH_TIME 2000000
#define UPDATE_SERVER_TIME 150000


//~1ft. more than 305 to account for speeding up
#define UNIT_SIZE_STRAIGHT 320
#define UNIT_SIZE_DIAGONAL 450

#define SAMPLE_CLOSE_DISTANCE 0.3

#define TURNING_DEGREE_VARIANCE 2.5

#define ROBOT_CIRCUMFERENCE 1037

class Agent;
struct client_info {
    int fd;
    char id;
    Agent* agent;
};

/*
 struct to hold sensor values
 index 0 is the low byte, index 1 is the high byte
 a value of -1 represents no value
*/
struct sensor_packet {
    int values[2];
    sensor_packet() {values[0] = -1; values[1] = -1;}
};

static int result_high_low_byte[2];
/*
 Returns an int array of size two for the high and low byte for a value
 index 0 is the low byte and index 1 is the high byte
*/
static int* getHighAndLowByte(int v) {

    //if a low value, just use the low byte
    if(v > 0 && v < 256) {
        result_high_low_byte[1] = 0;
        result_high_low_byte[0] = v;
    }   //end if

    //else use both the bytes
    else {
        result_high_low_byte[1] = (v>>8) & 0xff;
        result_high_low_byte[0] = v & 0xff;
    }

    usleep(1000);
    return result_high_low_byte;
}   //END GETHIGHANDLOWBYTE


static float get_signed_value_16_bit(unsigned char high, unsigned char low) {
    int result = low;

    //if high byte is used, get that
    if(high > 0)
        result += pow(2, 8);

    //if high byte > 1, negative number - shift and add
    if(high > 1)
        result = (high<<8) + low;

    return result;
}

/*result[0] is min, result[1] is max*/
static std::vector<float> get_min_max_angle(float target, float variance) {
    cout<<"\ntarget to get_min_max_angle:"<<target<<" ; variance:"<<variance;
    std::vector<float> result;
    result.push_back(target-variance);
    result.push_back(target+variance);

    if(result[0] > 360)
        result[0] -= 360;
    else if(result[0] < 0)
        result[0] += 360;

    if(result[1] > 360)
        result[1] -= 360;
    else if(result[1] < 0)
        result[1] += 360;

    for(int i=0;i<result.size();i++)
        cout<<"\n"<<result[i];
    return result;
}


static bool in_range(float num, float min, float max) {
    if(min > max)
        return ( (num >= min && num <= 360) || (num <= max && num >= 0) );

    return (num >= min && num <= max);
}

static pthread_mutex_t mutex_agent;


#endif
