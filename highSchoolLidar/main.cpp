/*
 * Copyright (C) 2015 Ben Shaffer
 */

#include <wiringPi.h>
#include <softPwm.h>
#include <mcp3004.h>

#include <signal.h>

#include <cmath>
#include <vector>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>

#include "line.h"
#include "point.h"
using namespace std;

#include "rplidar.h" //RPLIDAR standard sdk, all-in-one header

#ifndef _countof
#define _countof(_Array) (int)(sizeof(_Array) / sizeof(_Array[0]))
#endif

const int base=100;
const int spichannel=0;

const char* TTY = "/dev/ttyUSB0";

const int leftMotorSpeed=23;
const int leftMotorDirection=22;
const int rightMotorSpeed=26;
const int rightMotorDirection=21;

const int LEDpin=0;
const int buttonPin=3;
const int fanPin=24;





using namespace rp::standalone::rplidar;

typedef rplidar_response_measurement_node_t lidarNode;

RPlidarDriver *setupLidar();
void setupMotors(void);
double correctAngle(double degree);
void setSpeed( int, int);
void setupKillHandler(void);
vector<point> nodesToPoints(lidarNode nodes[], int count);
vector<point> filterSlopeNoise(vector<point> points);
vector<point> readLidar(RPlidarDriver * drv);
void rightWallFollow(vector<point> sensorData);
void leftWallFollow(vector<point> sensorData);
double flameValue();
bool frontWall(vector<point> readIn);
void spinLeft();
void spinRight();
void candleDance();
void waitToStart();

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

void setupOtherPins(){
    pinMode(LEDpin, OUTPUT);
    pinMode(buttonPin, INPUT);
    pinMode(fanPin, OUTPUT);
}



int main(int argc, const char * argv[]) {
    // something wiringPi
    if(wiringPiSetup()==-1){
	cout<<"wiringPi setup error"<<endl;
    	exit(1);
    } 
    mcp3004Setup(base, spichannel);
    ofstream fout("log");
    setupMotors();
    setupOtherPins();

    digitalWrite(LEDpin, LOW);
    
    // create the driver instance
    RPlidarDriver * drv = setupLidar();

    setupKillHandler();
    
   // waitToStart();
    
    delay(500);

    while(1){
	    vector<point> data=readLidar(drv);
        if(flameValue()<70){
	        rightWallFollow(data);    
	    }
	    else{
	        setSpeed(0,0);
	        digitalWrite(LEDpin, HIGH);
	        digitalWrite(fanPin, HIGH);
	        delay(3000);
	        //candleDance();
	        digitalWrite(LEDpin, LOW);
	        digitalWrite(fanPin, LOW);
	        delay(3000);
	    }
    
    }
}

void waitToStart(){
    while(digitalRead(buttonPin)==HIGH){
        //switch IS grounded
    }
}

void candleDance(){
    while(flameValue()>50){
        while(flameValue()>60){
            setSpeed(300, 400);
        }
        while(flameValue()<40){
            spinLeft();
        }
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


bool frontWall(vector<point> readIn){

	    double min=9999.9;
        
        for (size_t pos = 0; pos < readIn.size() ; ++pos) {
	          double angle = readIn[pos].theta;
	          double distance = readIn[pos].r;
	          if (75 < angle && 100>angle){
		            if (distance<min){
			            min=distance;
                    }	
              }
        }
        if(min<250||min>9000){
            return true;
        }
        else{
            return false;
        }
}

void spinLeft(){
	setSpeed(-250, 250);
}

void spinRight(){
	setSpeed(250, -500);
}

double flameValue(){
    return analogRead(base+0);
}


double correctAngle(double degree){
	return degree+10;
}

RPlidarDriver *setupLidar(){
    const char * opt_com_path = NULL;
    _u32         opt_com_baudrate = 115200;
    

 
 
    opt_com_path = TTY;

    

    // create the driver instance
    RPlidarDriver * drv = RPlidarDriver::CreateDriver(RPlidarDriver::DRIVER_TYPE_SERIALPORT);
    
    if (!drv) {
        fprintf(stderr, "insufficent memory, exit\n");
        exit(-2);
    }


    // make connection...
    if (IS_FAIL(drv->connect(opt_com_path, opt_com_baudrate))) {
        fprintf(stderr, "Error, cannot bind to the specified serial port %s.\n"
            , opt_com_path);
        goto on_finished;
    }




    // check health...
    if (!checkRPLIDARHealth(drv)) {
        goto on_finished;
    }


    // start scan...
    drv->startScan();

    return drv;

    on_finished:
    RPlidarDriver::DisposeDriver(drv);
    exit(0);
}

void setupMotors() {
    pinMode(leftMotorSpeed, PWM_OUTPUT);
    pinMode(rightMotorSpeed, PWM_OUTPUT);
    
    //softPwmCreate(leftMotorSpeed, 0, 100);
    //softPwmCreate(rightMotorSpeed, 0, 100);

 
    pinMode(rightMotorDirection, OUTPUT);
    pinMode(leftMotorDirection, OUTPUT);

    digitalWrite(rightMotorDirection, LOW);
    digitalWrite(leftMotorDirection, HIGH);

}

void setSpeed( int leftSpeed, int rightSpeed){
    digitalWrite(rightMotorDirection, rightSpeed<0?HIGH:LOW);
    digitalWrite(leftMotorDirection, leftSpeed<0?LOW:HIGH);
	pwmWrite(leftMotorSpeed, abs(leftSpeed)); // 0-1024
	pwmWrite(rightMotorSpeed, abs(rightSpeed));
	//cout << leftSpeed << " " << rightSpeed << endl;
}

void killHandler(int s){
        printf("Caught signal %d\n",s);
	softPwmWrite(leftMotorSpeed,0); // 0-100
	softPwmWrite(rightMotorSpeed, 0);
        exit(1); 

}

void setupKillHandler(){
	struct sigaction sigIntHandler;

   sigIntHandler.sa_handler = killHandler;
   sigemptyset(&sigIntHandler.sa_mask);
   sigIntHandler.sa_flags = 0;

   sigaction(SIGINT, &sigIntHandler, NULL);
   

}

vector<point> nodesToPoints(lidarNode nodes[], size_t count){
	vector<point> result;
	for (int pos = 0; pos < (int)count ; ++pos) {
		
		if ((nodes[pos].sync_quality >> RPLIDAR_RESP_MEASUREMENT_QUALITY_SHIFT) >0){
			double angle = (nodes[pos].angle_q6_checkbit >> RPLIDAR_RESP_MEASUREMENT_ANGLE_SHIFT)/64.0f ;
			angle = -correctAngle(angle)+90; // so x is on right
			if (angle < 0) angle += 360;
			double distance = nodes[pos].distance_q2/4.0f;


			point p;
			p.r= distance;
			p.theta= angle;
			result.push_back(p);
            	}
	}
	return result;
}

vector<point> readLidar(RPlidarDriver * drv){

    u_result op_result;
    lidarNode nodes[360*2];
    while(1){
        size_t   count = _countof(nodes);
        op_result = drv->grabScanData(nodes, count);
       	 if (IS_OK(op_result)) {
      	    drv->ascendScanData(nodes, count);
      	    return nodesToPoints(nodes, count);
      	 }
      	 else{
      	    cout<<"bad lidar"<<endl;
      	 }
	}
}



