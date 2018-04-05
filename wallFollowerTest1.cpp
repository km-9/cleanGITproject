#include "wallFollower.h"
#include "templateMatcher.h"
#include <stdlib.h>
#include <fstream>
#include "opencv2/opencv.hpp"
#include <wiringPi.h>

#include <iostream>

using namespace templateMatch;
using namespace WallFollower;
using namespace std;
using namespace cv;

bool canLeft;
bool canRight;
bool canForward;

double realLeft, softLeft, frontLeft, leftFront, front, rightFront, frontRight, softRight, realRight;
Mat frame, mask, output, blurFrame, postColor;

int thresh = 100;
int max_thresh = 255;
RNG rng(12345);


// Create a VideoCapture object and use camera to capture the video
VideoCapture cap(0);

wallFollower f = wallFollower();
Rect drawRect(Mat src);
Mat findFire(Mat frame);
int trackTarget(Rect target);
void getAvg();


// ------------------------------
// wall following parameters
// ------------------------------

// desired distance from left wall (measured at 45 degree angle from robot)
const int desiredLeftDistance = 320;
const int desiredFrontDistance = 320;

// mean speed of the motors
const int forwardSpeed = 50;

void pause(double n) {
  usleep(1000000*n);
}

void frontCaseHandler() {
  f.setMotorSpeed(0,0);
  pause(.5);
  f.setMotorSpeed(20, -20);
  pause(.6);
  f.setMotorSpeed(0,0);
}

void checkBumpSwitch() {
	if(!digitalRead(7)) {
		f.stop();
		pause(.5);
		f.setMotorSpeed(-10,20);
		pause(.5);
		f.stop();
	}
}

int main(int argc, char const *argv[]) {

  wiringPiSetup();
  pinMode(7, INPUT);

  while(true) {
    f.updateDists();
    checkBumpSwitch(); 
    int frontDistance = f.getDists(180);
   if(frontDistance > 0) {
 if(frontDistance < desiredFrontDistance) {
   frontCaseHandler();
 }
   }
    double leftDistance = f.avg(135,136);
    cout << leftDistance << endl;
    if (leftDistance > 0) {
      if (leftDistance < desiredLeftDistance)
	f.setMotorSpeed(forwardSpeed+40, forwardSpeed-40);
      else
	f.setMotorSpeed(forwardSpeed-40, forwardSpeed+40);
    }
  }
}

