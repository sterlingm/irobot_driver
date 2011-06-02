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

    int& getDefaultVelocity();
    void setDefaultVelocity(int);

    int& getCurrentSensor();
    void setCurrentSensor(int);

    bool sensorsStreaming();

    SerialConnect& getConnection();
    bool sendSingleByte(unsigned char);
    bool sendBytes(unsigned char*, int);
    int pollSensor(unsigned char*, int);

    void start();

    void fullMode();
    void safeMode();

    void streamSensors();
    void pauseSensorStream();
    void toggleSensorStream();
    Sensor_Packet getSensorValue(int);

    void drive(int, int);
    void drive_straight(int);
    void turnCounterClockwise(int);
    void turnClockwise(int);
    void stop();

    void leds(bool,bool,unsigned char,unsigned char);

    void turnXDegrees(int, int);
    void turnXDegreesInYSeconds(int,double);
private:
    int port;
    int baudrate;
    int dVelocity;
    int currentSensor;
    SerialConnect connection;
    bool sensorsstreaming;

    int* getHighAndLowByte(int);
};

#endif
