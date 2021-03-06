
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include "robot.h"
#include "rs232.h"

pthread_t get_sensors;
pthread_t get_real_velocity;
pthread_t get_v_over_t;


/*Callback for get real velocity thread*/
void* Robot::get_v_over_t_thread(void* threadid) {
    Robot* r = (Robot*)threadid;
    r->get_v_over_t_thread_i();
}

inline void Robot::get_v_over_t_thread_i() {
    while(1) {
        usleep(1000000);
        velocity_over_time.push_back(real_velocity);
    }   //end while
}

/*Callback for get real velocity thread*/
void* Robot::get_real_velocity_thread(void* threadid) {
    Robot* r = (Robot*)threadid;
    r->get_real_velocity_thread_i();
}

inline void Robot::get_real_velocity_thread_i() {
    usleep(1000000);
    while(1) {
        usleep(14500);

        if(sensorsstreaming) {
            sensor_packet temp = get_sensor_value(REQUESTED_VELOCITY);
            real_velocity = get_signed_value_16_bit(temp.values[0], temp.values[1]);
        }   //end if
    }   //end while
}

/*Callback for get sensors thread*/
void* Robot::get_sensors_thread(void* threadid) {
    Robot* r = (Robot*)threadid;
    r->get_sensors_thread_i();
}   //END GETSENSORS_THREAD


/*Inline for getting the robots sensors every 15ms.*/
inline void Robot::get_sensors_thread_i() {
    while(1) {

        //sleep
        #ifdef linux
        usleep(14500);
        #else
        Sleep(14.5f);
        #endif

        //if the sensors are streaming
        if(sensorsstreaming) {

            unsigned char receive;
            int read = 0;

            //read a byte
            read = connection.PollComport(port, &receive, sizeof(unsigned char));
            //std::cout << "\nBytes (" << read << "): ";
            //std::cout<<"\nreceive:"<<(int)receive;

            if((int)receive == 19) {
                //std::cout<<(int)receive;

                //next byte should be the size of the rest
                read = connection.PollComport(port, &receive, sizeof(unsigned char));
                //std::cout << "  Bytes (" << read << "): " << (int)receive;

                unsigned char rest[54];

                //get values
                read = connection.PollComport(port, rest, 54);
                //std::cout << "  Bytes (" << read << "): ";
                //for(int i=0;i<read;i++)
                    //std::cout<<"\nrest["<<i<<"]:"<<(int)rest[i];

                /* ***SET SENSOR VALUES*** */
                //bump + wheel drop
                sensor_values[0] = (int)rest[1];
                sensor_values[1] = -1;
                //wall
                sensor_values[2] = (int)rest[2];
                sensor_values[3] = -1;
                //cliff_left
                sensor_values[4] = (int)rest[3];
                sensor_values[5] = -1;
                //cliff_front_left
                sensor_values[6] = (int)rest[4];
                sensor_values[7] = -1;
                //cliff_front_right
                sensor_values[8] = (int)rest[5];
                sensor_values[9] = -1;
                //cliff_right
                sensor_values[10] = (int)rest[6];
                sensor_values[11] = -1;
                //virtual_wall
                sensor_values[12] = (int)rest[7];
                sensor_values[13] = -1;
                //low_side_driver
                sensor_values[14] = (int)rest[8];
                sensor_values[15] = -1;
                //unused_bytes
                sensor_values[16] = (int)rest[9];
                sensor_values[17] = -1;
                //unused_bytes
                sensor_values[18] = (int)rest[10];
                sensor_values[19] = -1;
                //infrared_byte
                sensor_values[20] = (int)rest[11];
                sensor_values[21] = -1;
                //buttons
                sensor_values[22] = (int)rest[12];
                sensor_values[23] = -1;
                //distance
                sensor_values[24] = (int)rest[13];
                sensor_values[25] = (int)rest[14];
                //angle
                sensor_values[26] = (int)rest[15];
                sensor_values[27] = (int)rest[16];
                //charging_state
                sensor_values[28] = (int)rest[17];
                sensor_values[29] = -1;
                //voltage
                sensor_values[30] = (int)rest[18];
                sensor_values[31] = (int)rest[19];
                //current
                sensor_values[32] = (int)rest[20];
                sensor_values[33] = (int)rest[21];
                //battery_temperature
                sensor_values[34] = (int)rest[22];
                sensor_values[35] = -1;
                //battery_charge
                sensor_values[36] = (int)rest[23];
                sensor_values[37] = (int)rest[24];
                //battery_capacity
                sensor_values[38] = (int)rest[25];
                sensor_values[39] = (int)rest[26];
                //wall_signal
                sensor_values[40] = (int)rest[27];
                sensor_values[41] = (int)rest[28];
                //cliff_left_signal
                sensor_values[42] = (int)rest[29];
                sensor_values[43] = (int)rest[30];
                //cliff_front_left_signal
                sensor_values[44] = (int)rest[31];
                sensor_values[45] = (int)rest[32];
                //cliff_front_right_signal
                sensor_values[46] = (int)rest[33];
                sensor_values[47] = (int)rest[34];
                //cliff_right_signal
                sensor_values[48] = (int)rest[35];
                sensor_values[49] = (int)rest[36];
                //cargo_bay_digital_inputs
                sensor_values[50] = (int)rest[37];
                sensor_values[51] = -1;
                //cargo_bay_analog_signal
                sensor_values[52] = (int)rest[38];
                sensor_values[53] = (int)rest[39];
                //charging_sources_available
                sensor_values[54] = (int)rest[40];
                sensor_values[55] = -1;
                //oi_mode
                sensor_values[56] = (int)rest[41];
                sensor_values[57] = -1;
                //song_number
                sensor_values[58] = (int)rest[42];
                sensor_values[59] = -1;
                //song_playing
                sensor_values[60] = (int)rest[43];
                sensor_values[61] = -1;
                //number_of_stream_packets
                sensor_values[62] = (int)rest[44];
                sensor_values[63] = -1;
                //requested_velocity
                sensor_values[64] = (int)rest[45];
                sensor_values[65] = (int)rest[46];
                //requested_radius
                sensor_values[66] = (int)rest[47];
                sensor_values[67] = (int)rest[48];
                //requested_right_velocity
                sensor_values[68] = (int)rest[49];
                sensor_values[69] = (int)rest[50];
                //requested_left_velocity
                sensor_values[70] = (int)rest[51];
                sensor_values[71] = (int)rest[52];

            }   //end if header
        }   //end if sensors streaming
    }   //end while
}   //END GET_SENSORS_THREAD_I

/*
  Constructor for a Robot
  Takes in a port number and a baudrate
  Defaults the current sensor OI_MODE
*/
Robot::Robot(int portNo, int br) : port(portNo), baudrate(br), sensorsstreaming(false), default_velocity(0), real_velocity(0) {init();}


/*
  Constructor for a Robot
  Takes in a port number and a baudrate
  Defaults the current sensor OI_MODE
*/
Robot::Robot(int portNo, int br, char ID, int cp, int cb) : port(portNo), baudrate(br), compass_port(cp), compass_baudrate(cb),
    sensorsstreaming(false), id(ID), default_velocity(0), real_velocity(0) {
        init();
}

Robot::Robot(int portNo, int br, char ID, bool create) : port(portNo), baudrate(br), sensorsstreaming(false), id(ID), default_velocity(0), real_velocity(0) {
    if(create)
        init();
}


/*
 Destructor for a Robot
 Closes the serial comport
*/
Robot::~Robot() {}

void Robot::init() {

    if(connection.OpenComport(port, baudrate))
        printf("port open did not work\n");
    else {
        //printf("port open worked\n");

        pthread_create(&get_sensors, 0, get_sensors_thread, (void*)this);
        pthread_create(&get_real_velocity, 0, get_real_velocity_thread, (void*)this);
        velocity_over_time.reserve(200);
        pthread_create(&get_v_over_t, 0, get_v_over_t_thread, (void*)this);
    }
}

void Robot::detach_threads() {
    if(pthread_detach(get_sensors) != 0)
        printf("\ndetach on get_sensors failed with error %m", errno);

    if(pthread_detach(get_real_velocity) != 0)
        printf("\ndetach on get_real_velocity failed with error %m", errno);
}

/* Closes the serial comport*/
void Robot::close() {connection.CloseComport(port);}

/*Sets the default velocity to v*/
void Robot::setDefaultVelocity(int v) {default_velocity = v;}
/*Sets connection to c*/
void Robot::setConnection(SerialConnect& c) {connection = c;}
void Robot::setRealVelocity(int v) {real_velocity = v;}

/*Returns the serial port connection*/
SerialConnect& Robot::getConnection() {return connection;}
/*Returns the port number*/
 int& Robot::getPort() {return port;}
/*Returns the baud rate*/
 int& Robot::getBaudRate() {return baudrate;}
/*Returns the default velocity*/
int& Robot::getDefaultVelocity() {return default_velocity;}
/*Returns id*/
char Robot::getID() {return id;}

int Robot::getRealVelocity() {return real_velocity;}
/*
 Sends a single byte to the robot
 Returns true if the byte was successfully sent
*/
bool Robot::sendSingleByte(unsigned char byte) {
    if(connection.SendByte(port, byte))
        return true;
    else
        return false;
}   //END SENDSINGLEBYTE

/*
 Sends a byte array to the robot
 Takes in a byte array and the length of the array
 Returns true if the byte array was successfully sent
*/
bool Robot::sendBytes(unsigned char* bytes, int length) {
    if(connection.SendBuf(port, bytes, length) == -1)
        return false;
    else
        return true;
}   //END SENDBYTES


/*
 Polls the Robot for a sensor value
 Returns the amount of bytes read back from the robot
*/
int Robot::pollSensor(unsigned char* buf, int size) {
    return connection.PollComport(port, buf, size);
}   //END POLLSENSOR


/*Sends the start command, 128, to the robot*/
void Robot::start() {
    unsigned char start[1] = {128};
    if(!sendBytes(start, 1))
        std::cout<<"\nCould not sent bytes for start\n";
}   //END START


/*Puts the robot in full mode, then sleeps for 1 second*/
void Robot::fullMode() {
    unsigned char fullmode[2] = {128, 132};
    if(!sendBytes(fullmode, 2))
        std::cout<<"\nCould not send bytes to robot for full mode\n";
    sleep(1);
}   //END FULLMODE


/*Puts the robot into safe mode, then sleeps for 1 second*/
void Robot::safeMode() {
    unsigned char safemode[2] = {128, 131};
    if(!sendBytes(safemode, 2))
        std::cout<<"\nCould not send bytes to robot for safe mode\n";
    sleep(1);
}   //END SAFEMODE


/*Makes the robot's sensors begin streaming if they are not already*/
void Robot::streamSensors() {
    if(!sensorsstreaming) {
        unsigned char stream[3] = {148, 1, 6};
        if(!sendBytes(stream, 3))
            std::cout<<"\nCould not send bytes to robot for sensor streaming\n";
        else
            sensorsstreaming = true;
    }
}   //END STREAMSENSORS

/*Pauses the sensor streaming if the sensors are streaming*/
void Robot::pauseSensorStream() {
    if(sensorsstreaming) {
        unsigned char stop[2] = {150, 0};
        if(!sendBytes(stop, 2))
            std::cout<<"\nCould not send bytes to robot for pausing the sensor stream\n";
        else
            sensorsstreaming = false;
    }
}   //END PAUSESENSORSTREAM


/*
 If the sensors are streaming, it pauses them
 If the sensors are paused, it resumes them
*/
void Robot::toggleSensorStream() {
    unsigned char a;
    if(sensorsstreaming)
        a = 0;
    else
        a = 1;
    sensorsstreaming = !sensorsstreaming;
    unsigned char command[2] = {150, a};
    if(!sendBytes(command, 2))
        std::cout<<"\nCould not send bytes to robot to toggle the sensor stream";
}   //END TOGGLESENSORSTREAM


/*Returns a Sensor_Packet for which*/
/*All sensors that use 2 bytes RETURN HIGH BYTE FIRST so result[0] is high, result[1] is low*/
sensor_packet Robot::get_sensor_value(int which) {
    sensor_packet result;
    //if the sensors are streaming
    if(sensorsstreaming) {

            //set the values for the specified sensor
            switch(which) {
                case BUMP:  //works for wheel drop also
                    result.values[0] = sensor_values[0];
                    result.values[1] = sensor_values[1];
                    break;
                case WALL:
                    result.values[0] = sensor_values[2];
                    result.values[1] = sensor_values[3];
                    break;
                case CLIFF_LEFT:
                    result.values[0] = sensor_values[4];
                    result.values[1] = sensor_values[5];
                    break;
                case CLIFF_FRONT_LEFT:
                    result.values[0] = sensor_values[6];
                    result.values[1] = sensor_values[7];
                    break;
                case CLIFF_FRONT_RIGHT:
                    result.values[0] = sensor_values[8];
                    result.values[1] = sensor_values[9];
                    break;
                case CLIFF_RIGHT:
                    result.values[0] = sensor_values[10];
                    result.values[1] = sensor_values[11];
                    break;
                case VIRTUAL_WALL:
                    result.values[0] = sensor_values[12];
                    result.values[1] = sensor_values[13];
                    break;
                case LOW_SIDE_DRIVER:
                    result.values[0] = sensor_values[14];
                    result.values[1] = sensor_values[15];
                    break;
                case 15:
                    result.values[0] = sensor_values[16];
                    result.values[1] = sensor_values[17];
                    break;
                case 16:
                    result.values[0] = sensor_values[18];
                    result.values[1] = sensor_values[19];
                    break;
                case INFRARED_BYTE:
                    result.values[0] = sensor_values[20];
                    result.values[1] = sensor_values[21];
                    break;
                case BUTTONS:
                    result.values[0] = sensor_values[22];
                    result.values[1] = sensor_values[23];
                    break;
                case DISTANCE:
                    result.values[0] = sensor_values[24];
                    result.values[1] = sensor_values[25];
                    break;
                case ANGLE:
                    result.values[0] = sensor_values[26];
                    result.values[1] = sensor_values[27];
                    break;
                case CHARGING_STATE:
                    result.values[0] = sensor_values[28];
                    result.values[1] = sensor_values[29];
                    break;
                case VOLTAGE:
                    result.values[0] = sensor_values[30];
                    result.values[1] = sensor_values[31];
                    break;
                case CURRENT:
                    result.values[0] = sensor_values[32];
                    result.values[1] = sensor_values[33];
                    break;
                case BATTERY_TEMPERATURE:
                    result.values[0] = sensor_values[34];
                    result.values[1] = sensor_values[35];
                    break;
                case BATTERY_CHARGE:
                    result.values[0] = sensor_values[36];
                    result.values[1] = sensor_values[37];
                    break;
                case BATTERY_CAPACITY:
                    result.values[0] = sensor_values[38];
                    result.values[1] = sensor_values[39];
                    break;
                case WALL_SIGNAL:
                    result.values[0] = sensor_values[40];
                    result.values[1] = sensor_values[41];
                    break;
                case CLIFF_LEFT_SIGNAL:
                    result.values[0] = sensor_values[42];
                    result.values[1] = sensor_values[43];
                    break;
                case CLIFF_FRONT_LEFT_SIGNAL:
                    result.values[0] = sensor_values[44];
                    result.values[1] = sensor_values[45];
                    break;
                case CLIFF_FRONT_RIGHT_SIGNAL:
                    result.values[0] = sensor_values[46];
                    result.values[1] = sensor_values[47];
                    break;
                case CLIFF_RIGHT_SIGNAL:
                    result.values[0] = sensor_values[48];
                    result.values[1] = sensor_values[49];
                    break;
                case CARGO_BAY_DIGITAL_INPUTS:
                    result.values[0] = sensor_values[50];
                    result.values[1] = sensor_values[51];
                    break;
                case CARGO_BAY_ANALOG_SIGNAL:
                    result.values[0] = sensor_values[52];
                    result.values[1] = sensor_values[53];
                    break;
                case CHARGING_SOURCES_AVAILABLE:
                    result.values[0] = sensor_values[54];
                    result.values[1] = sensor_values[55];
                    break;
                case OI_MODE:
                    result.values[0] = sensor_values[56];
                    result.values[1] = sensor_values[57];
                    break;
                case SONG_NUMBER:
                    result.values[0] = sensor_values[58];
                    result.values[1] = sensor_values[59];
                    break;
                case SONG_PLAYING:
                    result.values[0] = sensor_values[60];
                    result.values[1] = sensor_values[61];
                    break;
                case NUMBER_OF_STREAM_PACKETS:
                    result.values[0] = sensor_values[62];
                    result.values[1] = sensor_values[63];
                    break;
                case REQUESTED_VELOCITY:
                    result.values[0] = sensor_values[64];
                    result.values[1] = sensor_values[65];
                    break;
                case REQUESTED_RADIUS:
                    result.values[0] = sensor_values[66];
                    result.values[1] = sensor_values[67];
                    break;
                case REQUESTED_RIGHT_VELOCITY:
                    result.values[0] = sensor_values[68];
                    result.values[1] = sensor_values[69];
                    break;
                case REQUESTED_LEFT_VELOCITY:
                    result.values[0] = sensor_values[70];
                    result.values[1] = sensor_values[71];
                    break;
                default:
                    result.values[0] = -1;
                    result.values[1] = -1;
            }   //end switch
    }   //end if sensors streaming
    else {
        result.values[0] = -1;
        result.values[1] = -1;
    }   //end else
    return result;
}   //END GETSENSORVALUE


/*
 Drives the robot at a given velocity and radius
 velocity is the specified velocity in mm/s and can range from -500-500
 radius is the specified radius, measured from the center of the turning circle to the center of the create, and can range from -2000-2000
*/
void Robot::drive(int velocity, int radius) {
    unsigned char vhigh;
    unsigned char vlow;
    unsigned char rhigh;
    unsigned char rlow;

    //get velocity and radius values
    int* v = getHighAndLowByte(velocity);
    int* r = getHighAndLowByte(radius);
    vhigh = v[1];
    vlow = v[0];
    rhigh = r[1];
    rlow = r[0];

    unsigned char command[5] = {137, vhigh, vlow, rhigh, rlow};
    if(!sendBytes(command, 5))
        std::cout<<"\nCould not send bytes to robot to drive";
}   //END DRIVE


/*Calls drive(int,int) with the robot's velocity variable as the velocity*/
void Robot::drive(int radius) {
    drive(getDefaultVelocity(), radius);
}   //END DRIVE


/*
 Drives the robot straight at a given velocity
 velocity is the specified velocity in mm/s and can rage from -500-500
*/
void Robot::drive_straight(int velocity) {
    unsigned char vhigh;
    unsigned char vlow;

    //get velocity values
    int* v = getHighAndLowByte(velocity);
    vhigh = v[1];
    vlow = v[0];

    unsigned char command[5] = {137, vhigh, vlow, 127, 255};
    if(!sendBytes(command, 5))
        std::cout<<"\nCould not send bytes to robot to drive straight";
}   //END DRIVESTRAIGHT

/*Calls drive_straight with the velocity variable as the velocity*/
void Robot::drive_straight() {
    drive_straight(getDefaultVelocity());
}   //END DRIVESTRAIGHT



/*Drives the robot the specified distance (in mm) at the specified speed*/
void Robot::driveXDistance(int distance, int velocity) {

    bool backwards = false;
    int tVelocity = velocity;
    int tDistance = distance;
    if(velocity < 0) {
        backwards = backwards ^ true;
        tVelocity = tVelocity * -1;
    }
    if(distance < 0) {
        backwards = backwards ^ true;
        tDistance = tDistance * -1;
    }

    double time = (double)tDistance / tVelocity;

    //ADJUST TIME
    if(velocity<= 125)
        time += 0.05;
    else if(velocity <= 275)
        {}
    else if (velocity <= 350)
        time -= 0.01;
    else if(velocity <= 450)
        {}
    else if(velocity <= 475)
        time += 0.05;
    else if(velocity <= 485)
        time += 0.05;
    else
        time += 0.125;

    time = time * 1000000;

    //std::cout<<"\ndriving at velocity:"<<velocity<<" time:"<<time<<" distance: "<<distance<<"\n";

    if(velocity < 0 && backwards)
        drive_straight(velocity);
    else if(backwards)
        drive_straight((velocity*-1));
    else
        drive_straight(velocity);

    usleep(time);
    stop();

}   //END DRIVEXDISTANCE


/*Calls driveXMM with the velocity variable as the velocity value*/
void Robot::driveXDistance(int distance) {
    driveXDistance(distance, getDefaultVelocity());
}   //END DRIVEXDISTANCE



/*
 Turns the robot clockwise at a given velocity
 velocity is the specified velocity in mm/s and can range from -500-500
*/
void Robot::turnClockwise(int velocity) {
    unsigned char vhigh;
    unsigned char vlow;

    int* v = getHighAndLowByte(velocity);   //get velocity values
    vhigh = v[1];   //high byte
    vlow = v[0];    //low byte

    //create and send drive command
    unsigned char command[5] = {137, vhigh, vlow, 255, 255};
    if(!sendBytes(command, 5))
        std::cout<<"\nCould not send bytes to robot to turn clockwise";
}   //END TURNCLOCKWISE


/*
 Turns the robot counter clockwise at a given velocity
 velocity is the specified velocity in mm/s and can range from -500-500
*/
void Robot::turnCounterClockwise(int velocity) {
    unsigned char vhigh;
    unsigned char vlow;

    int* v = getHighAndLowByte(velocity);   //get velocity values
    vhigh = v[1];   //high byte
    vlow = v[0];    //low byte

    //create and send drive command
    unsigned char command[5] = {137, vhigh, vlow, 0, 1};
    if(!sendBytes(command, 5))
        std::cout<<"\nCould not send bytes to robot to turn counter clockwise";
}   //END TURNCOUNTERCLOCKWISE



/*Returns the amount of adjusting the time needs based on the angle and velocity*/
double Robot::adjustTurningTime(int velocity, int angle) {

    if( (velocity <= 25 && velocity > 0) || (velocity >= -25 && velocity < 0)) {
        if(angle == 90 || angle == -90)
            return -0.8;
        else if(angle == 180)
            return -1.6;
        else if(angle == -180)
            return -1.25;
        else if(angle == 270)
            return -2.37;
        else if(angle == -270)
            return -2.3;
        else if(angle == 360)
            return -3.46;
        else if(angle == -360)
            return -3.25;
    }   //end velocity<=25

    else if( (velocity <=50 && velocity > 0) || (velocity >= -50 && velocity < -25)) {
        if(angle == 90)
            return -0.4;
        else if(angle == -90)
            return -0.23;
        else if(angle == 180)
            return -0.8;
        else if(angle == -180)
            return -0.72;
        else if(angle == 270 || angle == -270)
            return -1.15;
        else if(angle == 360)
            return -1.65;
        else if(angle == -360)
            return -1.6;
        else if(angle == -225)
            return -0.935;
    }   //end velocity<=50

    else if( (velocity <=75 && velocity > 0) || (velocity >= -75 && velocity < -50)) {
        if(angle == 90)
            return -0.0975;
        else if(angle == 45)
            return 0;
        else if(angle == -90)
            return -0.01;
        else if(angle == 180 || angle == -180)
            return -0.195;
        else if(angle == 270)
            return -0.25;
        else if(angle == -270)
            return -0.3;
        else if(angle == 360)
            return -0.45;
        else if(angle == -360)
            return -0.46;
    }   //end velocity<=75

    else if( (velocity <=100 && velocity > 0) || (velocity >= -100 && velocity < -75)) {
        if(angle == 90 || angle == -90)
            return -0.1;
        else if(angle == 180)
            return -0.19;
        else if(angle == -180)
            return -0.185;
        else if(angle == 270)
            return -0.35;
        else if(angle == -270)
            return -0.3175;
        else if(angle == 360)
            return -0.45999;
        else if(angle == -360)
            return -0.485;
    }   //end velocity<=100

    else if( (velocity <=125 && velocity > 0) || (velocity >= -125 && velocity < -100)) {
        if(angle == 90)
            return -0.0875;
        else if(angle == -90)
            return -0.035;
        else if(angle == 180)
            return -0.18;
        else if(angle == -180)
            return -0.15;
        else if(angle == 270)
            return -0.2925;
        else if(angle == -270)
            return -0.3175;
        else if(angle == 360 || -360)
            return -0.4;
    }   //end velocity<=125

    else if( (velocity <=150 && velocity > 0) || (velocity >= -150 && velocity < -125)) {
        if(angle == 90)
            return -0.08;
        else if(angle == -90)
            return -0.0315;
        else if(angle == 180 || angle == -180)
            return -0.155;
        else if(angle == 270)
            return -0.2625;
        else if(angle == -270)
            return -0.255;
        else if(angle == 360)
            return -0.41;
        else if(angle == -360)
            return -0.375;
    }   //end velocity<=150

    else if( (velocity <=175 && velocity > 0) || (velocity >= -175 && velocity < -150)) {
        if(angle == 90)
            return -0.0675;
        else if(angle == -90)
            return -0.0275;
        else if(angle == 180 || angle == -180)
            return -0.135;
        else if(angle == 270)
            return -0.25;
        else if(angle == -270)
            return -0.235;
        else if(angle == 360)
            return -0.345;
        else if(angle == -360)
            return -0.3275;
    }   //end velocity<=175

    else if( (velocity <=200 && velocity > 0) || (velocity >= -200 && velocity < -175)) {
        if(angle == 90)
            return -0.0685;
        else if(angle == -90)
            return -0.0275;
        else if(angle == 180)
            return -0.125;
        else if(angle == -180)
            return -0.12;
        else if(angle == 270 || angle == -270)
            return -0.215;
        else if(angle == 360 || angle == -360)
            return -0.3;
    }   //end velocity<=200

    else if( (velocity <=225 && velocity > 0) || (velocity >= -225 && velocity < -200)) {
        if(angle == 90 || angle == -90)
            return -0.0585;
        else if(angle == 180)
            return -0.11;
        else if(angle == -180)
            return -0.1075;
        else if(angle == 270 || angle == -270)
            return -0.2;
        else if(angle == 360)
            return -0.295;
        else if(angle == -360)
            return -0.285;
    }   //end velocity<=225

    else if( (velocity <=250 && velocity > 0) || (velocity >= -250 && velocity < -225)) {
        if(angle == 90)
            return -0.05;
        else if(angle == -90)
            return -0.027;
        else if(angle == 180 || angle == -180)
            return -0.99;
        else if(angle == 270)
            return -0.175;
        else if(angle == -270)
            return -0.18;
        else if(angle == 360)
            return -0.2525;
        else if(angle == -360)
            return -0.25;
    }   //end velocity<=250

    else if( (velocity <=275 && velocity > 0) || (velocity >= -275 && velocity < -250)) {
        if(angle == 90 || angle == -90)
            return -0.0475;
        else if(angle == 180)
            return -0.1025;
        else if(angle == -180)
            return -0.099;
        else if(angle == 270 || angle == -270)
            return -0.16;
        else if(angle == 360)
            return -0.25;
        else if(angle == -360)
            return -0.225;
    }   //end velocity<=275

    else if( (velocity <=300 && velocity > 0) || (velocity >= -300 && velocity < -275)) {
        if(angle == 90 || angle == -90)
            return -0.04;
        else if(angle == 180)
            return -0.095;
        else if(angle == -180)
            return -0.095;
        else if(angle == 270 || angle == -270)
            return -0.15;
        else if(angle == 360)
            return -0.1975;
        else if(angle == -360)
            return -0.225;
    }   //end velocity<=300

    else if( (velocity <=325 && velocity > 0) || (velocity >= -325 && velocity < -300)) {
        if(angle == 90)
            return -0.04;
        else if(angle == -90)
            return -0.01875;
        else if(angle == 180)
            return -0.0884;
        else if(angle == -180)
            return -0.08;
        else if(angle == 270 || angle == -270)
            return -0.14;
        else if(angle == 360)
            return -0.1975;
        else if(angle == -360)
            return -0.2;
    }   //end velocity<=325

    else if( (velocity <=350 && velocity > 0) || (velocity >= -350 && velocity < -325)) {
        if(angle == 90)
            return -0.0375;
        else if(angle == -90)
            return -0.017;
        else if(angle == 180)
            return -0.0770;
        else if(angle == -180)
            return -0.08;
        else if(angle == 270 || angle == -270)
            return -0.13;
        else if(angle == 360)
            return -0.19;
        else if(angle == -360)
            return -0.18;
    }   //end velocity<=350

    else if( (velocity <=375 && velocity > 0) || (velocity >= -375 && velocity < -350)) {
        if(angle == 90)
            return -0.035;
        else if(angle == -90)
            return -0.017;
        else if(angle == 180)
            return -0.07;
        else if(angle == -180)
            return -0.07;
        else if(angle == 270 || angle == -270)
            return -0.125;
        else if(angle == 360 || angle == -360)
            return -0.18;
    }   //end velocity<=375

    else if( (velocity <=400 && velocity > 0) || (velocity >= -400 && velocity < -375)) {
        if(angle == 90)
            return -0.02;
        else if(angle == -90)
            return -0.016;
        else if(angle == 180)
            return -0.052;
        else if(angle == -180)
            return -0.05;
        else if(angle == 270)
            return -0.1;
        else if(angle == -270)
            return -0.1;
        else if(angle == 360)
            return -0.15;
        else if(angle == -360)
            return -0.15;
    }   //end velocity<=400

    else if( (velocity <=425 && velocity > 0) || (velocity >= -425 && velocity < -400)) {
        if(angle == 90)
            return -0.015;
        else if(angle == -90)
            return -0.015;
        else if(angle == 180)
            return -0.04;
        else if(angle == -180)
            return -0.05;
        else if(angle == 270)
            return -0.0855;
        else if(angle == -270)
            return -0.085;
        else if(angle == 360)
            return -0.13;
        else if(angle == -360)
            return -0.13;
    }   //end velocity<=425

    else if( (velocity <=450 && velocity > 0) || (velocity >= -450 && velocity < -425)) {
        if(angle == 90)
            return -0.015;
        else if(angle == -90)
            return -0.0025;
        else if(angle == 180)
            return -0.00925;
        else if(angle == -180)
            return -0.04;
        else if(angle == 270)
            return -0.085;
        else if(angle == -270)
            return -0.085;
        else if(angle == 360)
            return -0.125;
        else if(angle == -360)
            return -0.12;
    }   //end velocity<=450

    else if( (velocity <=475 && velocity > 0) || (velocity >= -475 && velocity < -450)) {
        if(angle == 90)
            return -0.015;
        else if(angle == -90)
            return 0.015;
        else if(angle == 180)
            return -0.0025;
        else if(angle == -180)
            return 0;
        else if(angle == 270)
            return -0.055;
        else if(angle == -270)
            return -0.02;
        else if(angle == 360)
            return -0.089;
        else if(angle == -360)
            return -0.06;
    }   //end velocity<=475

    else if( (velocity <=500 && velocity > 0) || (velocity >= -500 && velocity < -475)) {
        if(angle == 90)
            return 0.05;
        else if(angle == 45)
            return 0.02;
        else if(angle == -90)
            return 0.05;
        else if(angle == 180)
            return 0.075;
        else if(angle == -180)
            return 0.075;
        else if(angle == 270)
            return 0.0425;
        else if(angle == -270)
            return 0.03;
        else if(angle == 360)
            return 0.05;
        else if(angle == -360)
            return 0.025;
    }   //end velocity<=500

    return 0;
}   //END ADJUSTTIME


float Robot::findDegreeAdjustment(int d) {

    int degree = abs(d);
    int result;
    //if(degree < 21) {
        //result = -2;
    //}
    if(degree <= 17) {
        result = ((degree / 5) * -1)+2;
    }

    else if(degree <= 52) {
        result = ((degree / 5) * -1)+2.5;
    }
    else if(degree <= 72) {
        result = ((degree / 5) * -1) + 2;
    }
    else if(degree <= 77) {
        result = ((degree / 5) * -1) + 1.25;
    }
    else if(degree <= 87) {
        result = ((degree / 5) * -1) + 1;
    }
    else if(degree <= 90) {
        result = ((degree / 5) * -1) + 2;
    }
    else if(degree <= 102) {
        result = ((degree / 5) * -1) + 1;
    }
    else if(degree <= 137) {
        result = ((degree / 5) * -1) + 0.75;
    }
    else if(degree <= 167) {
        result = ((degree / 5) * -1);
    }
    else if(degree <= 172) {
        result = ((degree / 5) * -1) + 0.1;
    }
    else if(degree <= 177) {
        result = ((degree / 5) * -1);
    }
    else if(degree <= 182) {
        result = ((degree / 5) * -1) + 0.25;
    }

    else if(degree <= 227) {
        result = ((degree / 5) * -1);
    }

    else if(degree <= 242) {
        result = ((degree / 5) * -1) - 0.25;
    }
    else if(degree <= 322) {
        result = ((degree / 5) * -1);
    }
    else if(degree <= 348) {
        result = ((degree / 5) * -1) - 0.25;
    }
    else if(degree <= 352) {
        result = ((degree / 5) * -1) - 0.50;
    }
    else if(degree <= 360) {
        result = ((degree / 5) * -1) - 1.0;
    }
    else {

        result = (degree / 5);
    }

    return result;
}


/*
 Turns the robot for a given degree at a given velocity
 degree is the specified degrees to turn
 velocity is the specified velocity in mm/s and can range from -500-500
*/
void Robot::turnXDegrees(int degree, int velocity) {
    bool clockwise = false;  //true for moving clockwise, false for counter clockwise
    int tDegree = degree;
    int tVelocity = velocity;

    //only turn if both the degree and velocity have a value > 0
    if( (degree != 0) && (velocity != 0) ) {


        //if the degree is negative, get absolute value and set negative
        if(degree < 0) {
            tDegree = degree * -1;
            clockwise = clockwise ^ true;
        }
        //if the velocity is negative, get abolsute value and set negative
        if(velocity < 0) {
            tVelocity = velocity * -1;
            clockwise = clockwise ^ true;
        }

        tDegree += findDegreeAdjustment(tDegree);
        cout<<"\ntDegree:"<<tDegree;

        //percent of 360 degrees
        double p = ((tDegree * (double)100) / 360) / 100;


        //std::cout<<"\np:"<<p;
        double distance = p * ROBOT_CIRCUMFERENCE; //how far to move (mm)
        //std::cout<<"\ndistance:"<<distance;

        double time = distance / tVelocity; //how long to turn
        //std::cout<<"\ntime:"<<time;
        //std::cout<<"\nadjustment:"<<adjustTurningTime(velocity,degree);

        //set time to microseconds
        time *= 1000000;

        if(!clockwise)
            turnCounterClockwise(velocity);
        else
            turnClockwise(velocity);

        usleep(time);
        stop();
    }   //end if
}   //END TURNXDEGREES

/*Turns the robot x degrees*/

void Robot::turnXDegrees(int degree) {
    turnXDegrees(degree, getDefaultVelocity());
}

/*Turn x degrees in y seconds*/
void Robot::turnXDegreesInYSeconds(int degree, double seconds) {

    //only turn if both seconds and degree have a value > 0
    if(seconds > 0 && degree > 0) {

        //get distance
        double p = (degree * (double)100 / 180) / 100;  //percent of 180
        double distance = p * 440;  //distance to move (mm)

        int velocity = distance / seconds;  //how fast to turn

        turnCounterClockwise(velocity);

        int time = seconds * 1000000;
        usleep(time);
        stop();
    }
}   //END TURNXDEGREESINYSECONDS


/*Stops the robot*/
void Robot::stop() {
    unsigned char command[5] = {137, 0, 0, 0, 0};
    if(!sendBytes(command, 5))
        std::cout<<"\nCould not send bytes to robot to stop";
}   //END STOP


/*
 Changes the led lights
 play is for the play led. value of 1 makes play led turn on, 0 turns it off
 advance is for the advance led. value of 1 makes advance led turn on, 0 turns it off
 value is 0-255 to specify the color of the power led. 0 for green, 255 for red.
 intensity is 0-255 to specify the how bright the power led should be. 255 is the brightest, 0 is off
*/
void Robot::leds(bool play, bool advance, unsigned char value, unsigned char intensity) {
    int which = 0;
    if(play)
        which = which | 0x02;
    if(advance)
        which = which | 0x08;
    unsigned char command[4] = {139, which, value, intensity};
    if(!sendBytes(command, 4))
        std::cout<<"\nCould not send bytes to robot to change the leds";
}   //END LEDS

