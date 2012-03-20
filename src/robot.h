#ifndef ROBOT_H
#define ROBOT_H
#include <sys/time.h>
#include "rs232.h"
#include "robot_driver_utility.h"
#include "robot_driver_path.h"

class Robot
{
public:

    Robot() {}


    //! A constructor
    /*! Sets port member to first parameter passed and baudrate member to second parameter passed */
    Robot(int, int);

    //! A constructor
    /*! Sets port member to first parameter passed, baudrate member to second parameter passed, and id to third parameter */
    Robot(int, int, char);

    //! A destructor
    /*! Closes communication port */
    ~Robot();

    void init();

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

    //! Getter for baudrate member
    /*! Returns a reference to baudrate member */
    int& getBaudRate();

    //! Getter for velocity member
    /*! Returns a reference to velocity member */
    int& getVelocity();
    //! Setter for velocity member
    /*! Sets velocity member to v */
    void setVelocity(int);


    //! Getter for id member
    /*! Returns the value of id member */
    char getID();


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
    sensor_packet get_sensor_value(int);

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
    void turnXDegrees(int,int);

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

    int get_r_velocity();
    std::vector<int> velocity_over_time;
private:

    double adjustTurningTime(int,int);


    static void* get_sensors_thread(void*);
    void get_sensors_thread_i();

    static void* get_real_velocity_thread(void*);
    void get_real_velocity_thread_i();

    static void* get_v_over_t_thread(void*);
    void get_v_over_t_thread_i();


    int port;
    int baudrate;
    int velocity;
    short int real_velocity;  //robot requested velocity sensor value
    int currentSensor;
    int sensor_values[72];
    bool sensorsstreaming;
    SerialConnect connection;
    char id;
    char mode;

    struct timeval timestamp;

};

#endif
