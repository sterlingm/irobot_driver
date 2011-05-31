#include "Robot.h"
#include "rs232.h"
#include <iostream>
using std::cout;
using std::endl;

Robot::Robot(int portNo, int br) : port(portNo), baudrate(br), sensorsstreaming(false) {
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

int& Robot::getDefaultVelocity() {return dvelocity;}
void Robot::setDefaultVelocity(int v) {dvelocity = v;}

bool Robot::sensorsStreaming() {return sensorsstreaming;}

bool Robot::sendSingleByte(unsigned char byte) {
    if(connection.SendByte(port, byte))
        return true;
    else
        return false;
}   //END SENDSINGLEBYTE

bool Robot::sendBytes(unsigned char* bytes, int length) {
    int sent = connection.SendBuf(port, bytes, length);
    if(sent == -1)
        return false;
    else {
        /*
        int i=0;
        cout<<endl<<sent<<" bytes were sent - ";
        while((int)bytes[i] != 0) {
            cout<<(int)bytes[i]<<" ";
            i++;
        }
        */
        return true;
    }   //end else
}   //END SENDBYTES

int Robot::pollSensor(unsigned char* buf, int size) {
    int read = connection.PollComport(port, buf, size);
    return read;
}

void Robot::start() {
    unsigned char start[1] = {128};
    if(!sendBytes(start, 1))
        cout<<"\nCould not sent bytes for start\n";
}

void Robot::fullMode() {
    unsigned char fullmode[2] = {128, 132};
    if(!sendBytes(fullmode, 2))
        cout<<"\nCould not send bytes for full mode\n";
    sleep(1);
}

void Robot::safeMode() {
    unsigned char safemode[2] = {128, 131};
    if(!sendBytes(safemode, 2))
        cout<<"\nCould not send bytes for safe mode\n";
    sleep(1);
}


void Robot::streamSensors() {
    unsigned char stream[3] = {148, 1, 6};
    if(!sendBytes(stream, 3))
        cout<<"\nCould not send bytes for sensor streaming\n";
    else
        sensorsstreaming = true;
}

Sensor_Packet Robot::getSensorValue(int which) {
    Sensor_Packet result;


    //request sensor data and send bytes
    //unsigned char tosend[3] = {148, 1, 6};
    //sendBytes(tosend);

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
}



/*
 returns an int array of size two for the high and low byte for a value
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
        //here for velocity, will change if needed and once robot is consistently driving
        //if(v > 500)
          //  v = 500;
        //if(v < -500)
          //  v = -500;
        result[0] = (v>>8) & 0xff;
        result[1] = v & 0xff;
    }

    cout<<"\nhigh: "<<(int)result[0]<<" low: "<<(int)result[1];

    return result;
}   //END GETHIGHANDLOWBYTE


void Robot::drive(int velocity, int radius) {
    cout<<"\nindrive";
    unsigned char vhigh;
    unsigned char vlow;
    unsigned char rhigh;
    unsigned char rlow;

    if(velocity > 500)
        velocity = 500;
    else if(velocity < -500)
        velocity = -500;

    if(radius < -2000)
        radius = -2000;
    else if( (radius > 2000) && (radius != 32768) && (radius != 32767) && (radius != 65535) )
        radius = 2000;

    //get velocity and radius values
    int* v = getHighAndLowByte(velocity);
    int* r = getHighAndLowByte(radius);
    vhigh = v[0];
    vlow = v[1];
    rhigh = r[0];
    rlow = r[1];

    unsigned char command[5] = {137, vhigh, vlow, rhigh, rlow};
    sendBytes(command, 5);
}   //END DRIVE


//velocity is in mm/s
void Robot::drive_straight(int velocity) {
    cout<<"\nindrivedtraight";
    unsigned char vhigh;
    unsigned char vlow;

    if(velocity > 500)
        velocity = 500;
    else if(velocity < -500)
        velocity = -500;

    //get velocity values
    int* v = getHighAndLowByte(velocity);
    vhigh = v[0];
    vlow = v[1];


    unsigned char command[5] = {137, vhigh, vlow, 128, 0};
    sendBytes(command, 5);
}   //END DRIVE


//velocity is in mm/s
void Robot::turnClockwise(int velocity) {
    cout<<"\ninturncw";
    unsigned char vhigh;
    unsigned char vlow;

    //if negative, call turnLeft
    if(velocity < 0)
        turnCounterClockwise(velocity*-1);

    else {
        int* v = getHighAndLowByte(velocity);   //get velocity values
        vhigh = v[0];
        vlow = v[1];

        //create drive command and send bytes
        unsigned char command[5] = {137, vhigh, vlow, 255, 255};
        sendBytes(command, 5);
    }   //end else

}   //END TURNCLOCKWISE



//velocity is in mm/s.
void Robot::turnCounterClockwise(int velocity) {
    unsigned char vhigh;
    unsigned char vlow;


    if(velocity < 0)
        turnClockwise(velocity*-1);

    else {
        int* v = getHighAndLowByte(velocity);
        vhigh = v[0];
        vlow = v[1];

        //create drive command and send bytes
        unsigned char command[5] = {137, vhigh, vlow, 0, 1};
        sendBytes(command, 5);
    }   //end else

}   //END TURNCOUNTERCLOCKWISE


//function to stop the robot
void Robot::stop() {
    unsigned char command[5] = {137, 0, 0, 0, 0};
    sendBytes(command, 5);
}   //END STOP


/*
 function to change the led lights
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
    sendBytes(command, 4);
}



void Robot::turnDegrees(int x, int v) {

    bool negative = false;  //true for moving clockwise
    if(x > 180) {
        x = 360 - x;
        negative = true;
    }
    if(x < 0) {
        x = x * -1;
        negative = true;
    }

    double p = (x * (double)100 / 180) / 100;  //percent of 180
    cout<<endl<<"p: "<<p;

    double distance = p * 430; //how far to actually move (mm)

    cout<<endl<<"distance: "<<distance;
    double time = distance / v; //how long to turn

    cout<<endl<<"time: "<<time;

    //ADJUST TIME
    if(v<= 125)
        time += 0.05;
    else if(v <= 200)
        {}
    else if(v <= 275)
        time += 0.007;
    else if (v <= 350)
        time -= 0.01;
    else if(v <= 450)
        {}
    else if(v <= 475)
        time += 0.05;
    else if(v <= 485)
        time += 0.05;
    else
        time += 0.125;
    //set time to microseconds
    time = time * 1000000;

    if(!negative)
        turnCounterClockwise(v);
    else
        turnClockwise(v);

    usleep(time);
    stop();
}

//turn x degrees in y seconds
void Robot::turnDegreesInSeconds(int x, double y) {
    //get distance
    double p = (x * (double)100 / 180) / 100;  //percent of 180
    double distance = p * 430;

    int velocity = distance / y;

    cout<<"\nvelocity: "<<velocity;

    turnCounterClockwise(velocity);

    int time = y * 1000000;
    usleep(time);
    stop();
}
