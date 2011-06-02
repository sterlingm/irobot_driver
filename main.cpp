
#include "Robot.h"
#include "utility.h"
#include "GUIWindow.h"

#ifndef __linux__
#define sleep Sleep
#endif

int main(int argc, char* args[]) {


    Robot robot(16, 57600);

    GUIWindow window(robot);

    return Fl::run();

}   //END MAIN
