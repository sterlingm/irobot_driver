

#include "Robot.h"
#include "rs232.h"
#include "agent.h"
#include <math.h>



/*
  Constructor for a Robot
  Takes in a port number and a baudrate
  Defaults the current sensor OI_MODE
*/
Robot::Robot(int portNo, int br) : port(portNo), baudrate(br), sensorsstreaming(false), currentSensor(OI_MODE), velocity(0) {
    if(connection.OpenComport(port, baudrate))
        printf("port open did not work\n");

    else
        printf("port open worked\n");
}   //END ROBOT()

/*
 Destructor for a Robot
 Closes the serial comport
*/
Robot::~Robot() {
    connection.CloseComport(port);
}   //END ~ROBOT()

/* Closes the serial comport*/
void Robot::close() {connection.CloseComport(port);}

/*Sets the port number to p*/
void Robot::setPort(int& p) {port = p;}
/*Sets the baud rate to br*/
void Robot::setBaudRate(int& br) {baudrate = br;}
/*Sets the default velocity to v*/
void Robot::setVelocity(int v) {velocity = v;}
/*Sets the current sensor to s*/
void Robot::setCurrentSensor(int& s) {currentSensor = s;}
/*Sets agent to a*/
void Robot::setAgent(Agent*& a) {agent = a;}
/*Sets connection to c*/
void Robot::setConnection(SerialConnect& c) {connection = c;}

/*Returns the serial port connection*/
SerialConnect& Robot::getConnection() {return connection;}
/*Returns the port number*/
int& Robot::getPort() {return port;}
/*Returns the baud rate*/
int& Robot::getBaudRate() {return baudrate;}
/*Returns the default velocity*/
int& Robot::getVelocity() {return velocity;}
/*Returns the current sensor*/
int& Robot::getCurrentSensor() {return currentSensor;}
/*Returns agent*/
Agent*& Robot::getAgent() {return agent;}



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
        std::cout<<"\nCould not send bytes for full mode\n";
    sleep(1);
}   //END FULLMODE


/*Puts the robot into safe mode, then sleeps for 1 second*/
void Robot::safeMode() {
    unsigned char safemode[2] = {128, 131};
    if(!sendBytes(safemode, 2))
        std::cout<<"\nCould not send bytes for safe mode\n";
    sleep(1);
}   //END SAFEMODE


/*Makes the robot's sensors begin streaming if they are not already*/
void Robot::streamSensors() {
    if(!sensorsstreaming) {
        unsigned char stream[3] = {148, 1, 6};
        if(!sendBytes(stream, 3))
            std::cout<<"\nCould not send bytes for sensor streaming\n";
        else
            sensorsstreaming = true;
    }
}   //END STREAMSENSORS

/*Pauses the sensor streaming if the sensors are streaming*/
void Robot::pauseSensorStream() {
    if(sensorsstreaming) {
        unsigned char stop[2] = {150, 0};
        if(!sendBytes(stop, 2))
            std::cout<<"\nCould not send bytes for pausing the sensor stream\n";
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
        std::cout<<"\nCould not send bytes to toggle the sensor stream";
}   //END TOGGLESENSORSTREAM


/*Returns a Sensor_Packet for which*/
Sensor_Packet Robot::getSensorValue(int which) {
    Sensor_Packet result;
    unsigned char receive;
    unsigned char* rest;

    int read = 0;

    //read a byte
    read = connection.PollComport(port, &receive, sizeof(unsigned char));
    //std::cout << "\nBytes (" << read << "): ";

    if((int)receive == 19) {
        //std::cout<<(int)receive;

        //next byte should be the size of the rest
        read = connection.PollComport(port, &receive, sizeof(unsigned char));
        //std::cout << "  Bytes (" << read << "): " << (int)receive;

        //get the size, add 1 for checksum
        int packet_size = (int)receive + 1;
        rest = new unsigned char[packet_size];

        //get values
        read = connection.PollComport(port, rest, (sizeof(unsigned char) * (packet_size)));
        //std::cout << "  Bytes (" << read << "): ";


        switch(which) {
            case BUMP:  //works for wheel drop also
                result.values[0] = (int)rest[1];
                result.values[1] = -1;
                break;
            case WALL:
                result.values[0] = (int)rest[2];
                result.values[1] = -1;
                break;
            case CLIFF_LEFT:
                result.values[0] = (int)rest[3];
                result.values[1] = -1;
                break;
            case CLIFF_FRONT_LEFT:
                result.values[0] = (int)rest[4];
                result.values[1] = -1;
                break;
            case CLIFF_FRONT_RIGHT:
                result.values[0] = (int)rest[5];
                result.values[1] = -1;
                break;
            case CLIFF_RIGHT:
                result.values[0] = (int)rest[6];
                result.values[1] = -1;
                break;
            case VIRTUAL_WALL:
                result.values[0] = (int)rest[7];
                result.values[1] = -1;
                break;
            case LOW_SIDE_DRIVER:
                result.values[0] = (int)rest[8];
                result.values[1] = -1;
                break;
            case 15:
                result.values[0] = (int)rest[9];
                result.values[1] = -1;
                break;
            case 16:
                result.values[0] = (int)rest[10];
                result.values[1] = -1;
                break;
            case INFRARED_BYTE:
                result.values[0] = (int)rest[11];
                result.values[1] = -1;
                break;
            case BUTTONS:
                result.values[0] = (int)rest[12];
                result.values[1] = -1;
                break;
            case DISTANCE:
                result.values[0] = (int)rest[13];
                result.values[1] = (int)rest[14];
                break;
            case ANGLE:
                result.values[0] = (int)rest[15];
                result.values[1] = (int)rest[16];
                break;
            case CHARGING_STATE:
                result.values[0] = (int)rest[17];
                result.values[1] = -1;
                break;
            case VOLTAGE:
                result.values[0] = (int)rest[18];
                result.values[1] = (int)rest[19];
                break;
            case CURRENT:
                result.values[0] = (int)rest[20];
                result.values[1] = (int)rest[21];
                break;
            case BATTERY_TEMPERATURE:
                result.values[0] = (int)rest[22];
                result.values[1] = -1;
                break;
            case BATTERY_CHARGE:
                result.values[0] = (int)rest[23];
                result.values[1] = (int)rest[24];
                break;
            case BATTERY_CAPACITY:
                result.values[0] = (int)rest[25];
                result.values[1] = (int)rest[26];
                break;
            case WALL_SIGNAL:
                result.values[0] = (int)rest[27];
                result.values[1] = (int)rest[28];
                break;
            case CLIFF_LEFT_SIGNAL:
                result.values[0] = (int)rest[29];
                result.values[1] = (int)rest[30];
                break;
            case CLIFF_FRONT_LEFT_SIGNAL:
                result.values[0] = (int)rest[31];
                result.values[1] = (int)rest[32];
                break;
            case CLIFF_FRONT_RIGHT_SIGNAL:
                result.values[0] = (int)rest[33];
                result.values[1] = (int)rest[34];
                break;
            case CLIFF_RIGHT_SIGNAL:
                result.values[0] = (int)rest[35];
                result.values[1] = (int)rest[36];
                break;
            case CARGO_BAY_DIGITAL_INPUTS:
                result.values[0] = (int)rest[37];
                result.values[1] = -1;
                break;
            case CARGO_BAY_ANALOG_SIGNAL:
                result.values[0] = (int)rest[38];
                result.values[1] = (int)rest[39];
                break;
            case CHARGING_SOURCES_AVAILABLE:
                result.values[0] = (int)rest[40];
                result.values[1] = -1;
                break;
            case OI_MODE:
                result.values[0] = (int)rest[41];
                result.values[1] = -1;
                break;
            case SONG_NUMBER:
                result.values[0] = (int)rest[42];
                result.values[1] = -1;
                break;
            case SONG_PLAYING:
                result.values[0] = (int)rest[43];
                result.values[1] = -1;
                break;
            case NUMBER_OF_STREAM_PACKETS:
                result.values[0] = (int)rest[44];
                result.values[1] = -1;
                break;
            case REQUESTED_VELOCITY:
                result.values[0] = (int)rest[45];
                result.values[1] = (int)rest[46];
                break;
            case REQUESTED_RADIUS:
                result.values[0] = (int)rest[47];
                result.values[1] = (int)rest[48];
                break;
            case REQUESTED_RIGHT_VELOCITY:
                result.values[0] = (int)rest[49];
                result.values[1] = (int)rest[50];
                break;
            case REQUESTED_LEFT_VELOCITY:
                result.values[0] = (int)rest[51];
                result.values[1] = (int)rest[52];
                break;
            default:
                result.values[0] = -1;
                result.values[1] = -1;
        }   //end switch
    }   //end if
    return result;
}   //END GETSENSORVALUE



/*
 Returns an int array of size two for the high and low byte for a value
 index 0 is the high byte and index 1 is the low byte
*/
int* Robot::getHighAndLowByte(int v) {
    int* result = new int[2];

    //if a low value, just use the low byte
    if(v > 0 && v < 256) {
        result[0] = 0;
        result[1] = v;
    }   //end if

    //else use both the bytes
    else {
        result[0] = (v>>8) & 0xff;
        result[1] = v & 0xff;
    }

    return result;
}   //END GETHIGHANDLOWBYTE

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
    vhigh = v[0];
    vlow = v[1];
    rhigh = r[0];
    rlow = r[1];

    unsigned char command[5] = {137, vhigh, vlow, rhigh, rlow};
    if(!sendBytes(command, 5))
        std::cout<<"\nCould not send bytes to drive";
}   //END DRIVE


/*Calls drive(int,int) with the robot's velocity variable as the velocity*/
void Robot::drive(int radius) {
    drive(getVelocity(), radius);
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
    vhigh = v[0];
    vlow = v[1];

    unsigned char command[5] = {137, vhigh, vlow, 127, 255};
    if(!sendBytes(command, 5))
        std::cout<<"\nCould not send bytes to drive straight";
}   //END DRIVESTRAIGHT

/*Calls drive_straight with the velocity variable as the velocity*/
void Robot::drive_straight() {
    drive_straight(getVelocity());
}   //END DRIVESTRAIGHT



/*Drives the robot the specified distance (in mm) at the specified speed*/
void Robot::driveXDistance(int distance, int velocity) {
    bool backwards = false;
    if(velocity < 0) {
        velocity = velocity * -1;
        backwards = true;
    }
    if(distance < 0) {
        distance = distance * -1;
        backwards = true;
    }

    double time = (double)distance / velocity;

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

    if(backwards)
        drive_straight((velocity*-1));
    else
        drive_straight(velocity);

    usleep(time);
    stop();

}   //END DRIVEXDISTANCE


/*Calls driveXMM with the velocity variable as the velocity value*/
void Robot::driveXDistance(int distance) {
    driveXDistance(distance, getVelocity());
}   //END DRIVEXDISTANCE



/*
 Turns the robot clockwise at a given velocity
 velocity is the specified velocity in mm/s and can range from -500-500
*/
void Robot::turnClockwise(int velocity) {
    unsigned char vhigh;
    unsigned char vlow;

    int* v = getHighAndLowByte(velocity);   //get velocity values
    vhigh = v[0];   //high byte
    vlow = v[1];    //low byte

    //create and send drive command
    unsigned char command[5] = {137, vhigh, vlow, 255, 255};
    if(!sendBytes(command, 5))
        std::cout<<"\nCould not send bytes to turn clockwise";
}   //END TURNCLOCKWISE


/*
 Turns the robot counter clockwise at a given velocity
 velocity is the specified velocity in mm/s and can range from -500-500
*/
void Robot::turnCounterClockwise(int velocity) {
    unsigned char vhigh;
    unsigned char vlow;

    int* v = getHighAndLowByte(velocity);   //get velocity values
    vhigh = v[0];   //high byte
    vlow = v[1];    //low byte

    //create and send drive command
    unsigned char command[5] = {137, vhigh, vlow, 0, 1};
    if(!sendBytes(command, 5))
        std::cout<<"\nCould not send bytes to turn counter clockwise";
}   //END TURNCOUNTERCLOCKWISE





double Robot::adjustTurningTime(int velocity, int angle) {

    if(velocity <= 25) {
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

    else if(velocity <=50) {
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

    else if(velocity <=75) {
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

    else if(velocity <=100) {
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

    else if(velocity <=125) {
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

    else if(velocity <=150) {
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

    else if(velocity <=175) {
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

    else if(velocity <=200) {
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

    else if(velocity <=225) {
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

    else if(velocity <=250) {
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

    else if(velocity <=275) {
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

    else if(velocity <=300) {
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

    else if(velocity <=325) {
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

    else if(velocity <=350) {
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

    else if(velocity <=375) {
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

    else if(velocity <=400) {
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

    else if(velocity <=425) {
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

    else if(velocity <=450) {
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

    else if(velocity <=475) {
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

    else if(velocity <=500) {
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
}



/*
 Turns the robot for a given degree at a given velocity
 degree is the specified degrees to turn
 velocity is the specified velocity in mm/s and can range from -500-500
*/
void Robot::turnXDegrees(int degree, int velocity) {
    bool negative = false;  //true for moving clockwise, false for counter clockwise
    double tDegree = degree;
    double tVelocity = velocity;

    //only turn if both the degree and velocity have a value > 0
    if( (degree != 0) && (velocity != 0) ) {


        //if the degree is negative, get absolute value and set negative
        if(degree < 0) {
            tDegree = degree * -1;
            negative = negative ^ true;
        }
        //if the velocity is negative, get abolsute value and set negative
        if(velocity < 0) {
            tVelocity = velocity * -1;
            negative = negative ^ true;
        }

        double p = (tDegree * (double)100 / 360) / 100;  //percent of 180
        double distance = p * 900; //how far to move (mm)

        double time = distance / tVelocity; //how long to turn

        time += adjustTurningTime(velocity,degree);

        //set time to microseconds
        time = time * 1000000;

        if(!negative)
            turnCounterClockwise(velocity);
        else
            turnClockwise(velocity);

        usleep(time);
        stop();
    }   //end if
}   //END TURNXDEGREES

/*Turns the robot x degrees*/
void Robot::turnXDegrees(int degree) {
    turnXDegrees(degree, getVelocity());
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
        std::cout<<"\nCould not send bytes to stop";
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
        std::cout<<"\nCould not send bytes to change the leds";
}   //END LEDS



/*Robot moves from one position to another*/
Position Robot::step(Position& a, Position& b, char& d) {

    //std::cout<<"\nStepping from "<<a.toString()<<" to "<<b.toString()<<" with direction "<<d;

    if(a.getRow() == b.getRow()) {
        if( (a.getCol() - b.getCol() == -1) ) {
            if( (d == 'n') || (d == 'N') ) {
                d = 'e';
                turnXDegrees(-90);
                driveXDistance(UTILITY_H::UNIT_SIZE);
            }
            else if( (d == 's') || (d == 'S') ) {
                d = 'e';
                turnXDegrees(90);
                driveXDistance(UTILITY_H::UNIT_SIZE);
            }
            else if( (d == 'w') || (d == 'W') ) {
                d = 'e';
                turnXDegrees(180);
                driveXDistance(UTILITY_H::UNIT_SIZE);
            }
            else if( (d == 'e') || (d == 'E') )
                driveXDistance(UTILITY_H::UNIT_SIZE);
        }   //end if east

        else if( (a.getCol() - b.getCol() == 1) ) {
            if( (d == 'n') || (d == 'N') ) {
                d = 'w';
                turnXDegrees(90);
                driveXDistance(UTILITY_H::UNIT_SIZE);
            }
            else if( (d == 's') || (d == 'S') ) {
                d = 'w';
                turnXDegrees(-90);
                driveXDistance(UTILITY_H::UNIT_SIZE);
            }
            else if( (d == 'e') || (d == 'E') ) {
                d = 'w';
                turnXDegrees(180);
                driveXDistance(UTILITY_H::UNIT_SIZE);
            }
            else if( (d == 'w') || (d == 'W') )
                driveXDistance(UTILITY_H::UNIT_SIZE);
        }   //end if west

        return b;
    }   //end same row


    else if(a.getCol() == b.getCol()) {

        if( (a.getRow() - b.getRow() == -1) ) {

            if( (d == 'w') || (d == 'W') ) {
                d = 's';
                turnXDegrees(90);
                driveXDistance(UTILITY_H::UNIT_SIZE);
            }
            else if( (d == 'e') || (d == 'E') ) {
                d = 's';
                turnXDegrees(-90);
                driveXDistance(UTILITY_H::UNIT_SIZE);
            }
            else if( (d == 'n') || (d == 'N') ) {
                d = 's';
                turnXDegrees(180);
                driveXDistance(UTILITY_H::UNIT_SIZE);
            }
            else if( (d == 's') || (d == 'S') )
                driveXDistance(UTILITY_H::UNIT_SIZE);

        }   //end if south

        else if( (a.getRow() - b.getRow() == 1) ) {

            if( (d == 'w') || (d == 'W') ) {
                d = 'n';
                turnXDegrees(-90);
                driveXDistance(UTILITY_H::UNIT_SIZE);
            }
            else if( (d == 'e') || (d == 'E') ) {
                d = 'n';
                turnXDegrees(90);
                driveXDistance(UTILITY_H::UNIT_SIZE);
            }
            else if( (d == 's') || (d == 'S') ) {
                d = 'n';
                turnXDegrees(180);
                driveXDistance(UTILITY_H::UNIT_SIZE);
            }
            else if( (d == 'n') || (d == 'N') )
                driveXDistance(UTILITY_H::UNIT_SIZE);

        }   //end if north

        return b;
    }   //end same column
    else {
        std::cout<<"\nCannot step from "<<a.toString()<<" to "<<b.toString();
        return a;
    }   //end else
}   //END STEP



/*Robot steps through a whole path*/
Position Robot::stepPath() {


    pthread_mutex_lock(&UTILITY_H::mutex_agent);

    Path& temp = agent->getPath();
    Position result = temp.getPath().at(0);

    pthread_mutex_unlock(&UTILITY_H::mutex_agent);




    while(!agent->getPosition().equals(agent->getGoal()) && agent->getPath().getSize() > 1) {
        Position p = step(temp.getPath().at(0), temp.getPath().at(1), agent->getDirection());
        agent->setPosition(p);
        result = p;

        //update the path
        agent->getGrid()->clear();
        Path newpath = agent->traverse(agent->getGoal());
        agent->setPath(newpath);
    }   //end while

    //stop and return
    stop();
    return result;
}
