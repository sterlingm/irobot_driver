#ifndef ROBOT_H
#define ROBOT_H
#include "rs232.h"
#include "utility.h"

class Robot
{
public:
    Robot(int, int);
    ~Robot();

    void close();

    SerialConnect& getConnection();

    int& getPort();
    void setPort(int&);

    int& getBaudRate();
    void setBaudRate(int&);

    int& getDefaultVelocity();
    void setDefaultVelocity(int);

    int& getCurrentSensor();
    void setCurrentSensor(int);

    bool sensorsStreaming();

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
    void turnClockwise(int);
    void turnCounterClockwise(int);
    void turnXDegrees(int, int);
    void turnXDegreesInYSeconds(int,double);
    void stop();

    void leds(bool,bool,unsigned char,unsigned char);

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
