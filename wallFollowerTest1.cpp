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

int main (int argc, char const *argv[])
{
  cout << "in main" << endl;
  wallFollower f = wallFollower();
  //cout << "constructor finished" << endl;
  //f.goForward();
  //cout << "goForward finished" << endl;
  //f.holdTheFuckUp(10000000);
  //cout << "holdTheFuckUp" << endl;
// TODO f.startWallFollowingLeft();
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
  Mat frame;
  f.updateDists();
  //*****************
  // Capture frame-by-frame
  cap >> frame;

  // If the frame is empty, break immediately
  if (frame.empty())
    break;

  // Write the frame into the file 'outcpp.avi'
  video.write(frame);
  //*****************

  //Left Direct Avg
  double left = f.avg(87, 93);
  //General Left Avg
  double softLeft = f.avg(90, 135);
  //Front Left Avg
  double frontLeft = f.avg(125, 160);
  //Left Front Avg
  double leftFront = f.avg(145, 170);
  //Front Direct Avg
  double front = f.avg(175, 185);
  //Right Front Avg
  double rightFront = f.avg(190, 215);
  //Front Right Avg
  double frontRight = f.avg(210, 235);
  //General Left Avg
  double softRight = f.avg(225, 270);

  cout<<left<<"|Left"<<endl;
  cout<<softLeft<<"|softLeft"<<endl;
  cout<<frontLeft<<"|frontLeft"<<endl;
  cout<<leftFront<<"|leftFront"<<endl;
  cout<<front<<"|Front"<<endl;
  cout<<"--------------------------"<<endl;

  if (!f.frontHandler(front, left))
    if(!f.beSmart())
      if(!f.orientationFix(softLeft, left))
        f.leftWallFollower(left);


}
  // When everything done, release the video capture and write object
  cap.release();
  video.release();

/*//Closest thing to working
while (true){
    usleep(10000);
    f.updateDists();
    //check immediate left
    double avg = 0;
    for (int i = 89; i < 93; i++){
      avg = avg + f.dists[i];
    }
    avg = avg/3;
    while (f.dists[180] < 250){
      f.stop();
      f.updateDists();
      usleep(10000);
    }
    if (avg > 250){
      f.swayToLeft();
    }
    if (avg < 250){
      f.swayToRight();
    }
    if (avg == 250){
      f.goForward();
    }

    //check forward left
    for (int i = 93; i < 175; i++){
      if(f.dists[i] < 200){
        f.swayToRight();
        canLeft = false;
      }
      else{
        f.goForward();
      }
    }

    //check front
    for (int i = 175; i < 185; i ++){
      if(f.dists[i] < 300){
        f.stop();
        while (f.dists[i] < 300){
          //f.reverse();
          f.stop();
          f.updateDists();
        }
        if (canLeft){
          f.turnLeft(80);
        }
        else if (canRight){
        f.turnRight(80);
      }
        else
        f.turnRight(180);
      }
    }

    //check front-right
    for (int i = 185; i < 215; i++){
      if(f.dists[i] < 250){
        f.turnLeft(45);
        canRight = false;
        break;
      }
      else
      canRight = true;
    }

    //check forward right
    for (int i = 267; i < 215; i++){
      if(f.dists[i] < 200){
        f.strafeLeft();
        canRight = false;
      }
      else
      canRight = true;
    }

    //check immediate right
    for (int i = 267; i < 270; i ++){
      if(f.dists[i] < 200){
        f.strafeLeft();
        canRight = false;
      }
      else
      canRight = true;
    }
  }*/
  f.drv->stop();
  f.drv->stopMotor();

  rp::standalone::rplidar::RPlidarDriver::DisposeDriver(f.drv);

  return 0;
}
