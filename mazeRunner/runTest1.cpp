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

int leftCount; int rightCount;
bool leftFollow = true;
double frontVal;
double leftVal;
double rightVal;

//gets 360 degree data set from lidar
u_result capture_and_display(RPlidarDriver* drv)
{
	u_result ans;
	cout << "inside cap_and_disp" << endl;

	rplidar_response_measurement_node_t nodes[360*2];
	size_t count = _countof(nodes);

	// fetch extactly one 0-360 degrees' scan
	ans = drv->grabScanData(nodes, count);
	if (IS_OK(ans) || ans == RESULT_OPERATION_TIMEOUT)
	{
			drv->ascendScanData(nodes, count);
			frontVal = 1;
			leftVal = 1;
			rightVal = 1;

			//0 is behind
			for (int pos = 0; pos < (int)count ; ++pos)
			{
					if (pos == 270)
					{
							rightVal = nodes[pos].distance_q2/4.0f;
							cout << "Angle: "<< pos << " Dist: " << nodes[pos].distance_q2/4.0f;
					}
					if (pos == 90)
					{
							leftVal = nodes[pos].distance_q2/4.0f;
							cout << "Angle: "<< pos << " Dist: " << nodes[pos].distance_q2/4.0f;
					}
					if (pos == 180)
					{
							frontVal = nodes[pos].distance_q2/4.0f;
							cout << "Angle: "<< pos << " Dist: " << nodes[pos].distance_q2/4.0f;
					}
				}
			}
	 else
	 {
			printf("error code: %x\n", ans);
	}
	return ans;
}

bool canLeft()
{
	cout << "leftCall" << endl;
	cout << leftVal << endl;
  if(leftVal > 200)
	{
//  if(leftCount < 1){
		cout << "canLeft" << endl;
    return true;
//  }
	}
  return false;
}

bool canRight()
{
	cout << "rightCall" << endl;
	cout << rightVal << endl;
	//this logic is misplaced. You should simply ask canLeft() first and if not canLeft ask canRight, but canRight has nothing to do with canRight
	if(rightVal > 200 && !canLeft()){
 //if(rightCount < 1){
	 cout << "canRight" << endl;
   return true;
	 // }
	}
 return false;
}

bool canForward()
{
	cout << "forwardCall" << endl;
	cout << frontVal << endl;
	if(frontVal > 175 || frontVal == 0)
	{
		cout << "canForward" << endl;
	 	return true;
	}
 return false;
}

int main (int argc, char const *argv[])
{
	cout << "inside main" << endl;
	//left wheel
	PCA9685 pwm1;
	pwm1.init(1,0x40);

	//right wheel
  PCA9685 pwm2;
	pwm2.init(1,0x40);

	//look up the documentation on this
	pwm1.setPWMFreq(50);
  pwm2.setPWMFreq(50);

	//hold for a sec
	usleep(1000000);

	//stop both wheels 0 and 1 pins
  pwm1.setPWM(0,0,0);
	pwm2.setPWM(1,0,0);

	//to communicate with lidar
  int opt_com_baudrate = 115200;
	//device path for lidar
  const char * opt_com_path = "/dev/ttyUSB0";

  RPlidarDriver * drv = RPlidarDriver::CreateDriver(RPlidarDriver::DRIVER_TYPE_SERIALPORT);
	do {
    	// try to connect
      if (IS_FAIL(drv->connect(opt_com_path, opt_com_baudrate)))
			{
          fprintf(stderr, "Error, cannot bind to the specified serial port %s.\n", opt_com_path);
          break;
      }
			//spin lidar
      drv->startMotor();

      // take only one 360 deg scan and display the result as a histogram (we don't know wtf this is?)
			//check doc
      if (IS_FAIL(drv->startScan( /* true */ ))) // you can force rplidar to perform scan operation regardless whether the motor is rotating
      {
          fprintf(stderr, "Error, cannot start the scan operation.\n");
          break;
      }

      if (IS_FAIL(capture_and_display(drv))) {
          fprintf(stderr, "Error, cannot grab scan data.\n");
          break;
      }
			cout << "end of lidar driver in main" << endl;
		}
		while(0);
  bool stop = false;
	while(!stop)
	{
		//read lidar if initiziatization didn't error out
		capture_and_display(drv);

		//while we can go forward
		while (0)
		{
			//read again
			capture_and_display(drv);

			//check to see if 150/600 gets straight line
			pwm1.setPWM(0, 0, 150);
			pwm2.setPWM(1, 0, 600);

			//hold a sec
			usleep(1000000);

				//too far from wall
				//leftVal = d(this, object @ pi/4)
				if (leftVal > 300)
				{
					//turn left
					pwm1.setPWM(0,0,100);
			    pwm2.setPWM(1,0,600);
					usleep(500000);
					//capture_and_display(drv);
				}

				//too close to wall
				//why are we using this instead of canLeft()?
				if (leftVal < 275)
				{
					//turn right
					pwm1.setPWM(0, 0, 150);
					pwm2.setPWM(1, 0, 0);
					usleep(500000);
					//capture_and_display(drv);
				}
			}

		if (!canForward && false)
		{
			//why isn't this stopping?
			pwm1.setPWM(0, 0, 0);
			pwm2.setPWM(1, 0, 0);
			usleep(1000000);


			//reverses
			pwm1.setPWM(0, 0, 600);
			pwm2.setPWM(1, 0, 150);
			usleep(1000000);

			//turn left (left wall following)
			pwm1.setPWM(0, 0, 150);
			pwm2.setPWM(1, 0, 0);
			usleep(1000000);
		}
		//****************plug-and-play testing********************************
    //4/10 if both wheels make the turn
		//1 second if one wheel makes the turns
		//hardly anything close to accurate, is short at least 5 degrees each turns

    //*********************************************************************
		stop = true;
	}
    drv->stop();
    drv->stopMotor();

    RPlidarDriver::DisposeDriver(drv);
    return 0;
  }
