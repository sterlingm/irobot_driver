#ifndef ROBOT_H
#define ROBOT_H
#include "rs232.h"
#include "utility.h"
#include "path.h"

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

    int& getVelocity();
    void setVelocity(int);

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

    double adjustTurningTime(int,int);

    void drive(int, int);
    void drive(int);
    void drive_straight(int);
    void drive_straight();
    void driveXDistance(int, int);
    void driveXDistance(int);
    void turnClockwise(int);
    void turnCounterClockwise(int);
    void turnXDegrees(int, int);
    void turnXDegrees(int);
    void turnXDegreesInYSeconds(int,double);
    void stop();

    void leds(bool,bool,unsigned char,unsigned char);

    bool step(Position&, Position&, char&);
    void stepPath(Path&, char&);

private:
    int port;
    int baudrate;
    int velocity;
    int currentSensor;
    SerialConnect connection;
    bool sensorsstreaming;

    int* getHighAndLowByte(int);

};

#endif
