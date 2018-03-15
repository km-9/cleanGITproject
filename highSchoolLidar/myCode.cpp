#include "PCA9685.h"
#include <iostream>
#include <stdio.h>
//#include <ncurses.h>
//#include <thread>
#include <unistd.h>
#include <stdlib.h>
#include <cstddef>
#include <cstdio>
#include <ctime>
#include "rplidar.h"
#ifndef _countof
#define _countof(_Array) (int)(sizeof(_Array) / sizeof(_Array[0]))
#endif


using namespace rp::standalone::rplidar;
using namespace std;

bool canForward: canLeft; canRight;
double leftDist; rightDist; forwardDist;
double leftDegree; rightDegree; forwardDegree;
RPlidarDriver * drv = setupLidar();
//**********************************************************************************
bool checkRPLIDARHealth(RPlidarDriver * drv)
{
    u_result     op_result;
    rplidar_response_device_health_t healthinfo;


    op_result = drv->getHealth(healthinfo);
    if (IS_OK(op_result)) { // the macro IS_OK is the preperred way to judge whether the operation is succeed.
        //cout << "RPLidar health status : " << (int)healthinfo.status << endl;
        if (healthinfo.status == RPLIDAR_STATUS_ERROR) {
	    cerr << "Error, rplidar internal error detected. Please reboot the device to retry." << endl;
            // enable the following code if you want rplidar to be reboot by software
            // drv->reset();
            return false;
        } else {
            return true;
        }

    } else {
	cerr << "Error, cannot retrieve the lidar health code:" << op_result << endl;
        return false;
    }
}

void rightWallFollow(vector<point> sensorData){

	    point min;
	    min.r=9999.9;
	    min.theta=0;
        for (size_t pos = 0; pos < sensorData.size() ; ++pos) {
	          double angle = sensorData[pos].theta;
	          double distance = sensorData[pos].r;
	          if ((35 < angle && 90>angle)){
	            if (distance<min.r){
		            min.r=distance;
		            min.theta=angle;
                }
              }
        }
        if((min.r<250 || min.r>9000) && !frontWall(sensorData)){
		    setSpeed(450, 500);
		    //cout<<"left"<<endl;
	    }
	    else if(frontWall(sensorData)){
	       spinLeft();
	      // cout<<"spin"<<endl;
	    }
	    else{
		    setSpeed(600, 350);
		    //cout<<"right"<<endl;
	    }

}
//*****************************************************************************
