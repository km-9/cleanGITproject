#include "wallFollower.h"
#include <stdlib.h>
#include <fstream>
#include "opencv2/opencv.hpp"

#include <iostream>

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

wallFollower f = wallFollower();
void drawRect(Mat src);
int findFire(Mat frame);
void getAvg();

int main (int argc, char const *argv[])
{
  cout << "in main" << endl;
//***********************************************
  // Create a VideoCapture object and use camera to capture the video
  VideoCapture cap(0);

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
int findFire(Mat frame){
  Size_<int> size;
  size = Size_<int>(21, 21);

  GaussianBlur(frame, blurFrame, size, 0, 0, BORDER_DEFAULT);

  cvtColor(blurFrame, postColor, COLOR_BGR2HSV, 0);

  Scalar lower = Scalar(18, 50, 50);
  Scalar upper = Scalar(35, 255, 255);
  inRange(postColor, lower, upper, mask);

  bitwise_and(frame, postColor, output, mask);

  drawRect(frame);

  return 0;
}

void drawRect(Mat src){
 Mat threshold_output;
 vector<vector<Point> > contours;
 vector<Vec4i> hierarchy;

 threshold( src, threshold_output, thresh, 255, THRESH_BINARY );
 findContours( threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

 vector<vector<Point> > contours_poly( contours.size() );
 vector<Rect> boundRect( contours.size() );
 vector<Point2f>center( contours.size() );

 vector<float>radius( contours.size() );
 for( size_t i = 0; i < contours.size(); i++ )
 {
   approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
   boundRect[i] = boundingRect( Mat(contours_poly[i]) );
   minEnclosingCircle( contours_poly[i], center[i], radius[i] );
 }

 Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );

 for( size_t i = 0; i< contours.size(); i++ )
 {
   Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
   drawContours( drawing, contours_poly, (int)i, color, 1, 8, vector<Vec4i>(), 0, Point() );
   rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
   circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
 }

 namedWindow( "Contours", WINDOW_AUTOSIZE );
 imshow( "Contours", drawing );
}
