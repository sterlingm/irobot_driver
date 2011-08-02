#ifndef ROBOT_H
#define ROBOT_H
#include "rs232.h"
#include "utility.h"
#include "path.h"

class Agent;

class Robot
{
public:

    //! A constructor
    /*! Sets port member to first parameter passed and baudrate member to second parameter passed */
    Robot(int, int);

    //! A destructor
    /*! Closes communication port */
    ~Robot();

    //! Closes communication port
    /*! Closes communication port */
    void close();

    //! Getter for connection member
    /*! Returns a reference to connection member */
    SerialConnect& getConnection();
    //! Setter for connection member
    /*! Sets connection member to c */
    void setConnection(SerialConnect&);

    //! Getter for port member
    /*! Returns a reference to port member */
    int& getPort();
    //! Setter for port member
    /*! Sets port member to p */
    void setPort(int&);

    //! Getter for baudrate member
    /*! Returns a reference to baudrate member */
    int& getBaudRate();
    //! Setter for baudrate member
    /*! Sets baudrate member to br */
    void setBaudRate(int&);

    //! Getter for velocity member
    /*! Returns a reference to velocity member */
    int& getVelocity();
    //! Setter for velocity member
    /*! Sets velocity member to v */
    void setVelocity(int);

    //! Getter for currentSensor member
    /*! Returns a reference to currentSensor member */
    int& getCurrentSensor();
    //! Setter for currentSensor member
    /*! Sets currentSensor to s */
    void setCurrentSensor(int&);


    //! Getter for agent member
    /*! Returns a reference to agent member */
    Agent*& getAgent();
    //! Setter for agent member
    /*! Sets agent to a */
    void setAgent(Agent*&);


    //! Sends a byte over the port
    /*! Sends a single byte over the communication port */
    bool sendSingleByte(unsigned char);

    //! Sends a byte array over the port
    /*! Sends an array of bytes over the communication port */
    bool sendBytes(unsigned char*, int);

    //! Polls the port for a single sensor value
    /*!
     * Polls the communication port for a sensor's value\n
     * Value is stored in unsigned char* parameter passed\n
     * Returns the amount of bytes read
     */
    int pollSensor(unsigned char*, int);


    //! Starts the robot
    /*! Sends start command, 128, over the communication port */
    void start();

    //! Puts the robot in full mode
    /*! Sends the full mode command, 132, over the communication port */
    void fullMode();

    //! Puts the robot in safe mode
    /*! Sends the safe mode command, 131, over the communication port  */
    void safeMode();

    //! Begins sensor streaming
    /*! Sends command for the robot to begin sensor streaming over the communication port */
    void streamSensors();

    //! Pauses sensor streaming
    /*! Sends command for the robot to pause sensor streaming over the communication port */
    void pauseSensorStream();

    //! Toggles sensor streaming
    /*!
     * Begins streaming robot's sensors if not already
     * Stops streaming robot's sensors if they are already streaming
     */
    void toggleSensorStream();

    //! Returns a Sensor_Packet for requested sensor id
    /*! Returns a Sensor_Packet of the sensor whose id is passed through parameter */
    Sensor_Packet getSensorValue(int);

    //! Drives robot at specified velocity and radius
    /*!
     * Drives robot at x velocity and y radius\n
     * velocity parameter specifies the velocity\n
     * radius parameter specifies the radius\n
     */
    void drive(int, int);

    //! Drives robot at specified radius
    /*!
     * Drives robot at specified radius\n
     * radius parameter specifies the radius\n
     * velocity member is used as velocity\n
     */
    void drive(int);

    //! Drives robot straight at specified velocity
    /*!
     * Drives robot straight at specified velocity\n
     * velocity parameter specifies the velocity\n
     */
    void drive_straight(int);

    //! Drives robot straight
    /*!
     * Drives robot straight\n
     * velocity member is used as velocity
     */
    void drive_straight();

    //! Drives the robot straight X distance
    /*!
     * Drives the robot straight X distance at specified velocity\n
     * distance parameter specifies distance\n
     * velocity parameter specifies velocity
     */
    void driveXDistance(int, int);

    //! Drives the robot straight X distance
    /*!
     * Drives the robot straight X distance\n
     * distance parameter specifies the distance\n
     * velocity member is used for velocity
     */
    void driveXDistance(int);

    //! Turns the robot clockwise
    /*!
     * Turns the robot clockwise at specified velocity\n
     * velocity parameter specifies the velocity
     */
    void turnClockwise(int);

    //! Turns the robot counter-clockwise
    /*!
     * Turns the robot counter-clockwise at specified velocity\n
     * velocity parameter specifies the velocity
     */
    void turnCounterClockwise(int);

    //! Turns the robot X degrees
    /*!
     * Turns the robot X degrees at specified velocity\n
     * degree parameter specifies the degree\n
     * velocity parameter specifies the velocity
     */
    void turnXDegrees(int, int);

    //! Turns the robot X degrees
    /*!
     * Turns the robot X degrees\n
     * degree parameter specifies degree\n
     * velocity member is used for velocity\n
     */
    void turnXDegrees(int);

    //! Turns robot X degrees in Y seconds
    /*!
     * Turns the robot X degrees in Y seconds\n
     * degree parameter specifies the degree\n
     * seconds parameter specifies the seconds\n
     */
    void turnXDegreesInYSeconds(int,double);

    //! Stops the robot
    /*! Sends the command to stop the robot */
    void stop();

    //! Changes LED lights on the robot
    /*!
     * Changes LED lights on the robot\n
     * Play LED will turn on if first parameter is true\n
     * Advance LED will turn on if second parameter is true\n
     * Third parameter is the color value between 0-255. Closer to 0 is more green, closer to 255 is more red\n
     * Fourth parameter is the intensity value between 0-255. 0 is no intensity (off) and 255 is the brightest intensity
     */
    void leds(bool,bool,unsigned char,unsigned char);

    //! Moves the robot from one position to another
    /*!
     * Moves the robot from Position a to Position b, if possible\n
     * First parameter is Position a\n
     * Second Parameter is Position b\n
     * Third parameter is the robot's direction\n
     */
    Position step(Position&, Position&, char&);

    //! Moves the robot through its agent's path
    /*!
     * Moves the robot through its agent's path\n
     * If it cannot complete the whole path, the robot will move as far as it can
     */
    Position stepPath();

private:

    double adjustTurningTime(int,int);
    int* getHighAndLowByte(int);

    int port;
    int baudrate;
    int velocity;
    int currentSensor;
    bool sensorsstreaming;
    SerialConnect connection;
    Agent* agent;

};

#endif
