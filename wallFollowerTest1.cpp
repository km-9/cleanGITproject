#include "wallFollower.h"
#include "templateMatcher.h"
#include <stdlib.h>
#include <fstream>
#include "opencv2/opencv.hpp"

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

int main (int argc, char const *argv[])
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
 return box;
}

Mat image1;
bool backprojMode1 = false;
bool selectObject1 = false;
int trackObject = 0;
bool showHist1 = true;
Point origin1;
Rect selection1;
int vmin1 = 10, vmax1 = 256, smin1 = 30;
int trackTarget(Rect target){
  selection1 = target;
  Rect trackWindow;
  int hsize = 16;
  float hranges[] = {0,180};
  const float* phranges = hranges;
  Mat frame, hsv, hue, mask, hist, histimg = Mat::zeros(200, 320, CV_8UC3), backproj;
  bool paused = false;
  for(;;)
  {
      if( !paused )
      {
          cap >> frame;
          if( frame.empty() )
              break;
      }
      frame.copyTo(image1);
      if( !paused )
      {
          cvtColor(image1, hsv, COLOR_BGR2HSV);
          if( trackObject )
          {
              int _vmin1 = vmin1, _vmax1 = vmax1;
              inRange(hsv, Scalar(0, smin1, MIN(_vmin1,_vmax1)),
                      Scalar(180, 256, MAX(_vmin1, _vmax1)), mask);
              int ch[] = {0, 0};
              hue.create(hsv.size(), hsv.depth());
              mixChannels(&hsv, 1, &hue, 1, ch, 1);
              if( trackObject < 0 )
              {
                  // Object has been selected by user, set up CAMShift search properties once
                  Mat roi(hue, selection1), maskroi(mask, selection1);
                  calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
                  normalize(hist, hist, 0, 255, NORM_MINMAX);
                  trackWindow = selection1;
                  trackObject = 1; // Don't set up again, unless user selects new ROI
                  histimg = Scalar::all(0);
                  int binW = histimg.cols / hsize;
                  Mat buf(1, hsize, CV_8UC3);
                  for( int i = 0; i < hsize; i++ )
                      buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i*180./hsize), 255, 255);
                  cvtColor(buf, buf, COLOR_HSV2BGR);
                  for( int i = 0; i < hsize; i++ )
                  {
                      int val = saturate_cast<int>(hist.at<float>(i)*histimg.rows/255);
                      rectangle( histimg, Point(i*binW,histimg.rows),
                                 Point((i+1)*binW,histimg.rows - val),
                                 Scalar(buf.at<Vec3b>(i)), -1, 8 );
                  }
              }
              // Perform CAMShift
              calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
              backproj &= mask;
              RotatedRect trackBox = CamShift(backproj, trackWindow,
                                  TermCriteria( TermCriteria::EPS | TermCriteria::COUNT, 10, 1 ));
              if( trackWindow.area() <= 1 )
              {
                  int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5)/6;
                  trackWindow = Rect(trackWindow.x - r, trackWindow.y - r,
                                     trackWindow.x + r, trackWindow.y + r) &
                                Rect(0, 0, cols, rows);
              }
              if( backprojMode1 )
                  cvtColor( backproj, image1, COLOR_GRAY2BGR );
              ellipse( image1, trackBox, Scalar(0,0,255), 3, LINE_AA );
          }
      }
      else if( trackObject < 0 )
          paused = false;
      if( selectObject1 && selection1.width > 0 && selection1.height > 0 )
      {
          Mat roi(image1, selection1);
          bitwise_not(roi, roi);
      }
      imshow( "CamShift Demo", image1 );
      imshow( "Histogram", histimg );
      char c = (char)waitKey(10);
      if( c == 27 )
          break;
      switch(c)
      {
      case 'b':
          backprojMode1 = !backprojMode1;
          break;
      case 'c':
          trackObject = 0;
          histimg = Scalar::all(0);
          break;
      case 'h':
          showHist1 = !showHist1;
          if( !showHist1 )
              destroyWindow( "Histogram" );
          else
              namedWindow( "Histogram", 1 );
          break;
      case 'p':
          paused = !paused;
          break;
      default:
          ;
      }
  }
  return 0;
}
