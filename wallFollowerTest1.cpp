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

double left, softLeft, frontLeft, leftFront, front, rightFront, frontRight, softRight, right;

wallFollower f = wallFollower();

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
  Mat frame;
  f.updateDists();
  getAvg();
  //*****************
  // Capture frame-by-frame
  cap >> frame;

  // If the frame is empty, break immediately
  if (frame.empty())
    break;

  // Write the frame into the file 'outcpp.avi'
  video.write(frame);
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
    while (left > front){
      f.updateDists();
      f.turnRightInPlace();
      front = f.avg(175, 185);
      left = f.avg(87, 93);
    }
  }
  else if (left > softLeft){
    f.strafeRight();
  }
  else if (softLeft > frontLeft){
    f.strafeRight();
  }
  else if (frontLeft < left){
    f.strafeRight();
  }
  else if (left > 200 && frontLeft-50>left){
    f.strafeLeft();
  }
  //all is well. Make most basic wallfollowing decisions
  else if (left < 200){
    f.swayToRight();
  }
  else if (left > 200){
    f.swayToLeft();
  }
  else if (left == 200){
    f.goForward();
  }


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

void getAvg(){
  //Left Direct Avg
  left = f.avg(87, 93);
  //General Left Avg
  softLeft = f.avg(90, 135);
  //Front Left Avg
  frontLeft = f.avg(125, 160);
  //Left Front Avg
  leftFront = f.avg(145, 170);
  //Front Direct Avg
  front = f.avg(175, 185);
  //Right Front Avg
  rightFront = f.avg(190, 215);
  //Front Right Avg
  frontRight = f.avg(210, 235);
  //General Left Avg
  softRight = f.avg(225, 270);
}
