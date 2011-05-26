#ifndef ROBOT_H
#define ROBOT_H
#include "rs232.h"
#include "utility.h"
#include <vector>
#include <map>

class Robot
{
public:
    Robot(int, int);
    ~Robot();

    void close();

    int& getPort();
    void setPort(int&);

    int& getBaudRate();
    void setBaudRate(int&);

    SerialConnect& getConnection();
    bool sendSingleByte(unsigned char);
    bool sendBytes(unsigned char*);
    int pollSensor(unsigned char*, int);

    Sensor_Packet getSensorValue(int);

    void drive(int, int);
    void drive_straight(int);

    void turnCounterClockwise(int);
    void turnClockwise(int);

    void stop();
private:
    int port;
    int baudrate;
    SerialConnect connection;

    int* getHighAndLowByte(int);
};

#endif
