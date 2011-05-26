#include "Robot.h"
#include "rs232.h"
#include <iostream>
using std::cout;
using std::endl;

unsigned char SAFEMODE[2] = {128, 131};
unsigned char FULLMODE[2] = {128, 132};
unsigned char PLAYLEDONLY[6] = {128, 132, 139, 2, 0, 0};
unsigned char DRIVE[5] = {137, 127, 255, 0, 0};

Robot::Robot(int portNo, int br) : port(portNo), baudrate(br) {
    if(connection.OpenComport(port, baudrate))
        printf("port open did not work\n");
    else
        printf("port open worked\n");
}   //END ROBOT()

Robot::~Robot() {
    connection.CloseComport(port);
}   //END ~ROBOT()

void Robot::close() {connection.CloseComport(port);}
SerialConnect& Robot::getConnection() {return connection;}

int& Robot::getPort() {return port;}
void Robot::setPort(int& p) {port = p;}

int& Robot::getBaudRate() {return baudrate;}
void Robot::setBaudRate(int& br) {baudrate = br;}


bool Robot::sendSingleByte(unsigned char byte) {
    if(connection.SendByte(port, byte))
        return true;
    else
        return false;
}   //END SENDSINGLEBYTE

bool Robot::sendBytes(unsigned char* bytes) {
    if(connection.SendBuf(port, bytes, sizeof(bytes)) == -1){
        return false;
	 }else{
		usleep(15000);
        return true;
	 }
}   //END SENDBYTES

int Robot::pollSensor(unsigned char* buf, int size) {
    int read = connection.PollComport(port, buf, size);
    return read;
}


Sensor_Packet Robot::getSensorValue(int which) {
    Sensor_Packet result;
    //sendBytes(FULLMODE);
    //request all sensor data and send bytes
    unsigned char tosend[3] = {148, 1, 6};
    sendBytes(tosend);

    unsigned char receive;
    unsigned char* rest;

    int read = 0;

    //read a byte
    read = connection.PollComport(port, &receive, sizeof(unsigned char));
    cout << "\nBytes (" << read << "): ";

    if((int)receive == 19) {
        cout<<(int)receive;

        //next byte should be the size of the rest
        read = connection.PollComport(port, &receive, sizeof(unsigned char));
        cout << "  Bytes (" << read << "): " << (int)receive;


        //get the size, add 1 for checksum
        int packet_size = (int)receive + 1;
        rest = new unsigned char[packet_size];
        //memset(rest,0,sizeof(rest));

        //get values
        read = connection.PollComport(port, rest, (sizeof(unsigned char) * (packet_size)));
        cout << "  Bytes (" << read << "): ";


        switch(which) {
            case BUMP:
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
}   //END GETSENSORVALUES


/*
 returns an int array of size two for the high and low byte for a value
 index 0 is the high byte and index 1 is the low byte
*/
int* Robot::getHighAndLowByte(int v) {
    int* result = new int[2];
    if(v > 0 && v < 256) {
        result[0] = 0;
        result[1] = v;
    }   //end if

    else {
        if(v > 500)
            v = 500;
        if(v < -500)
            v = -500;
        result[0] = (v>>8) & 0xff;
        result[1] = v & 0xff;
    }
    return result;
}   //END GETHIGHANDLOWBYTE


void Robot::drive(int velocity, int radius) {
    unsigned char vhigh;
    unsigned char vlow;
    unsigned char rhigh;
    unsigned char rlow;
    int* v = getHighAndLowByte(velocity);
    int* r = getHighAndLowByte(radius);
    vhigh = v[0];
    vlow = v[1];
    rhigh = r[0];
    rlow = r[1];

    unsigned char command[5] = {137, vhigh, vlow, rhigh, rlow};
    sendBytes(command);
}   //END DRIVE



//velocity is in mm/s
void Robot::drive_straight(int velocity) {
    unsigned char vhigh;
    unsigned char vlow;

    //get values for creating byte array
    int* v = getHighAndLowByte(velocity);
    vhigh = v[0];
    vlow = v[1];

    unsigned char command[5] = {137, vhigh, vlow, 128, 0};
    //unsigned char command[5] = {137, vhigh, vlow, 127, 255};
    sendBytes(command);

}   //END DRIVESTRAIGHT


//velocity is in mm/s
void Robot::turnClockwise(int velocity) {
    unsigned char rhigh;
    unsigned char rlow;
    unsigned char lhigh;
    unsigned char llow;

    //if negative, call turnLeft
    if(velocity < 0) {
        //printf("calling turnLeft with %d\n", (velocity*-1));
        turnCounterClockwise(velocity*-1);
    }

    else {
        int* v = getHighAndLowByte(velocity);
        int* vneg = getHighAndLowByte((velocity*-1));
        lhigh = v[0];
        llow = v[1];
        rhigh = vneg[0];
        rlow = vneg[1];

        //create drive command and send bytes
        unsigned char command[6] = {128, 145, rhigh, rlow, lhigh, llow};
        sendBytes(command);
    }

}   //END TURNCLOCKWISE



//velocity is in mm/s.
void Robot::turnCounterClockwise(int velocity) {
    unsigned char rhigh;
    unsigned char rlow;
    unsigned char lhigh;
    unsigned char llow;


    if(velocity < 0) {
        //printf("calling turnLeft with %d\n", (velocity*-1));
        turnClockwise(velocity*-1);
    }

    else {
        int* v = getHighAndLowByte(velocity);
        int* vneg = getHighAndLowByte((velocity*-1));
        rhigh = v[0];
        rlow = v[1];
        lhigh = vneg[0];
        llow = vneg[1];

        //create drive command and send bytes
        unsigned char command[5] = {145, rhigh, rlow, lhigh, llow};
        sendBytes(command);
    }

}   //END TURNCOUNTERCLOCKWISE


//function to stop the robot
void Robot::stop() {
    unsigned char command[5] = {137, 0, 0, 0, 0};
    sendBytes(SAFEMODE);
    sendBytes(command);
}   //END STOP
