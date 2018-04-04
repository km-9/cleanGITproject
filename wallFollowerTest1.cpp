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
  pause(.5);
  f.setMotorSpeed(0,0);
}

void checkBumpSwitch() {
	if(!digitalRead(7)) {
		f.stop();
	}
}

int main(int argc, char const *argv[]) {
  cap >> frame;
  frame = findFire(frame);
  drawRect(frame);

  wiringPiSetup();
  pinMode(4, INPUT);

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

int mainqqqqq (int argc, char const *argv[])
{
  cout << "in main" << endl;
//***********************************************

  // Check if camera opened successfully
  if(!cap.isOpened())
  {
    cout << "Error opening video stream" << endl;
    return -1;
  }

  // Default resolution of the frame is obtained.The default resolution is system dependent.
  int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
  int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

  // Define the codec and create VideoWriter object.The output is stored in 'outcpp.avi' file.
  VideoWriter video("outcppJUNK.avi",CV_FOURCC('M','J','P','G'),10, Size(frame_width,frame_height));

  const char* source_window = "Source";
  namedWindow( source_window, WINDOW_NORMAL );
  resizeWindow("souce", 500,500);
  Mat temp;
  cap >> temp;
  imshow( source_window, temp );
  //createTrackbar( " Threshold:", "Source", &thresh, max_thresh, drawRect);
  //trackTarget(drawRect(findFire(temp)));

//***********************************************
while(true){
  Mat capFrame;
  f.updateDists();
  getAvg();
  //*****************
  // Capture frame-by-frame
  cap >> capFrame;

  // If the frame is empty, break immediately
  if (capFrame.empty())
    break;

  // Write the frame into the file 'outcpp.avi'
  video.write(capFrame);
  //*****************
  cout<<left<<"|Left"<<endl;
  cout<<softLeft<<"|softLeft"<<endl;
  cout<<frontLeft<<"|frontLeft"<<endl;
  cout<<leftFront<<"|leftFront"<<endl;
  cout<<front<<"|Front"<<endl;
  cout<<"--------------------------"<<endl;

  /*
  The chain of decision making should be as follows.
  1| Collision Avoidance (if the robot is on a collision course it must stop itself and adjust)
  2| Orientation/Turning (the robot must be oriented properly in regards to the wall)
  3| Distance (the robot must maintain a proper distance from obstacles)
  4| Movement (the robot should move forward under basic principles)
  *wildcard*| Decision Making (the robot must make decisions based upon conditions, the nature of these determines their ordering in the heirarchy)
  */

  /*if (wontCrash){
    if (wontSkew){
      if (notTooShort/Long){
        if (canMoveForward){

        }
      }
    }
  }*/



  if (front < 200 && front != 0){
    while (front < 300 && front != 0){
      f.stop();
      cout<<"cant go forward"<<endl;
      f.updateDists();
      front = f.avg(175, 185);
    }
    while (realLeft > front){
      f.updateDists();
      f.turnRightInPlace();
      front = f.avg(175, 185);
      realLeft = f.avg(87, 93);
    }
  }
  else if (realLeft > softLeft){
    f.strafeRight();
  }
  else if (softLeft > frontLeft){
    f.strafeRight();
  }
  else if (frontLeft < realLeft){
    f.strafeRight();
  }
  else if (realLeft > 200 && frontLeft-50>realLeft){
    f.strafeLeft();
  }
  //all is well. Make most basic wallfollowing decisions
  else if (realLeft < 200){
    f.swayToRight();
  }
  else if (realLeft > 200){
    f.swayToLeft();
  }
  else if (realLeft == 200){
    f.goForward();
  }


}
  // When everything done, release the video capture and write object
  cap.release();
  video.release();
  f.drv->stop();
  f.drv->stopMotor();

  rp::standalone::rplidar::RPlidarDriver::DisposeDriver(f.drv);

  return 0;
}

void getAvg(){
  //Left Direct Avg
  realLeft = f.avg(87, 93);
  //General realLeft Avg
  softLeft = f.avg(90, 135);
  //Front realLeft Avg
  frontLeft = f.avg(125, 160);
  //Left Front Avg
  leftFront = f.avg(145, 170);
  //Front Direct Avg
  front = f.avg(175, 185);
  //Right Front Avg
  rightFront = f.avg(190, 215);
  //Front Right Avg
  frontRight = f.avg(210, 235);
  //General realLeft Avg
  softRight = f.avg(225, 270);
}

//TODO this all needs to get redone into proper OOP style
Mat findFire(Mat frame){
  Size_<int> size;
  size = Size_<int>(21, 21);

  GaussianBlur(frame, blurFrame, size, 0, 0, BORDER_DEFAULT);
  return blurFrame;
}

Rect drawRect(Mat src){
 double minVal1; double maxVal1; Point minLoc1; Point maxLoc1;
 Mat plain;
 cvtColor(src, plain, CV_BGR2GRAY);
 minMaxLoc( plain, &minVal1, &maxVal1, &minLoc1, &maxLoc1, Mat() );
cout<<maxVal1<<" maxVal"<<endl;
cout<<maxLoc1.x << " max x loc"<<endl;
Rect box;
if (maxVal1 > 100){
 box.x = (maxLoc1.x - 10);
 box.y = (maxLoc1.y - 10);
 box.width = 20;
 box.height = 20;
}
 rectangle(src, box, CV_BGR2GRAY, 1, 8, 0);
 cout<<"got to imshow"<<endl;
 imshow("box", src);

 return box;
}
