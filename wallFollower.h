#ifndef WALLFOLLOWER_H
#define WALLFOLLOWER_H
#endif
#include "rplidar.h"
#include "rplidar_driver.h"
#include "PCA9685.h"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstddef>
#include <cmath>
#include <fstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include "point.hpp"
#include "interpolator.hpp"
#include "rptypes.h"
#include "lidar_support.hpp"

namespace WallFollower
{
  class wallFollower
  {
      public:
         //array of past canGoLeft values
         bool * pastGoLefts;

         //array of past canGoRight values
         bool * pastGoRights;

          //an 360 length array with distance from a lidar detector to the nearest detectable object at the index's angle
          double dists [360];

          //lidar controller
          rp::standalone::rplidar::RPlidarDriver * drv;

          //logfile
          std::ofstream logfile;

          //left wheel
          PCA9685 pwm1;

          //right wheel
          PCA9685 pwm2;

          int avoidance_radius;

          //maximum allowable distance between the robot and the wall
          int max_left_dist;

          //minimum allowable distance between the robot and any object in front of it
          int min_forward_dist;

          //minimum allowable distance between the robot and any object to the left of it
          int min_left_dist;

          //minimum allowable distance between the robot and any object to the right of it
          int min_right_dist;

          //constructor
          wallFollower();

          //averages distances between degree values
          double avg(int start, int end);

          //dka;hghd
          int avoidHeadOn();

          //scans front Left quadrant for obstacles
          bool avoidLeft();

          //scans front Right quadrant for obstacles
          bool avoidRight();

          //checks if the robot is at an intersection
          bool beSmart();

          //returns whether or not we are able to reverse
          bool canGoBack();

          //returns whether or not we are able to go forward
          bool canGoForward();

          //returns whether or not we are able to go left
          bool canGoLeft();

         //return whether or not we are able to go right
         bool canGoRight();

         //whether or not the majority of past canGoLeft values were true
         bool couldntGoLeft();

         //whether or not the majority of past canGoRight values were true
         bool couldntGoRight();

         //handler call for front collisions
         bool frontHandler(double fAvg, double lAvg);

         //getter for dist[i]
         double getDists(int i);

         //getter for max_left_dist
         int getMaxLeftDist();

         //getter for min_forward_dist
         int getMinForwardDist();

         //getter for min_left_dist
         int getMinLeftDist();

         //getter for min_right_dist
         int getMinRightDist();

         //makes the robot go forward hopefully
         void goForward();

         //calls pause(units)
         void holdTheFuckUp(int units);

         //majority of past canGoLeft values were false, but we now canGoLeft()
         bool leftPathAppeared();

         //takes an average distance from the wall and follows it
         bool leftWallFollower(double avg);

         //uses avg of forward-left distances to orient robot to a wall
         bool orientationFix(double leftAngleAvg, double lAvg);

          //holds for units milliseconds?
          void pause(int units);

          //reverses the robot
          void reverse();

          //majority of past canGoRight values were false, but now we canGoRight()
          bool rightPathAppeared();

          //returns whether or not the robot should strafe left to align with the left wall
          bool strafeLeftToAlignWithWall();

          //returns whether or not the robot should strafe right to align with the right wall
          bool strafeRightToAlignWithWall();

          //start wall following left
          void startWallFollowingLeft();

          //stop both wheels
          void stop();

          //arc left significantly
          void strafeLeft();

          //arc right significantly
          void strafeRight();

          //slightly sway towards left
          void swayToLeft();

          //slightly tend towards right
          void swayToRight();

          void turnRightInPlace();

          //calls reverse()
          void takeItBackNowYall();

          //returns whether or not the robot is too far from the wall on the left
          bool tooFarOnLeft();

          //turns the robot degrees to the left
          void turnLeft(int degrees);

          //turns left until can't go left
          void turnLeftUntilYouCant();

          //turns the robot degree so the right
          void turnRight(int degres);

          //updates the dist[] using drv
          void updateDists();

          //getVideoCaptureToFile
          void camToFile();

  };
}
