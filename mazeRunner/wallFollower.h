#ifndef WALLFOLLOWER_H
#define WALLFOLLOWER_H
#include "rplidar.h"
#include "PCA9685.h"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstddef>
#include <cstdio>
#include <ctime>

namespace wallFollower
{
  class wallFollower
  {
      public:
          //constructor
          wallFollower();
      private:

          //an 360 length array with distance from a lidar detector to the nearest detectable object at the index's angle
          double [] * dists;

          //lidar controller
          RPlidarDriver * drv;

          //left wheel
          PCA9685 * pwm1;

          //right wheel
          PCA9685 * pwm2;

          //maximum allowable distance between the robot and the wall
          int max_left_dist;

          //minimum allowable distance between the robot and any object in front of it
          int min_forward_dist;

          //minimum allowable distance between the robot and any object to the left of it
          int min_left_dist;

          //minimum allowable distance between the robot and any object to the right of it
          int min_right_dist;

          //returns whether or not we are able to go forward
          bool canGoForward();

          //returns whether or not we are able to go left
          bool canGoLeft();

          //return whether or not we are able to go right
          bool canGoRight();

          //getter for dist[i]
          double getDists(int i);

          //getter for max_left_dist
          int getMaxLeftDist();

          //getter for min_forward_dist
          int getMinForwardDist();

          //getter for min_left_dist
          int getMinLeftDist()

          //getter for min_right_dist
          int getMinRightDist();

          //makes the robot go forward hopefully
          void goForward();

          //calls pause(units)
          void holdTheFuckUp(int units);

          //holds for units milliseconds?
          void pause(int units);

          //reverses the robot
          void reverse();

          //stop both wheels
          void stop();

          //calls reverse()
          void takeItBackNowYall();

          //returns whether or not the robot is too far from the wall on the left
          bool tooFarOnLeft();

          //turns the robot degrees to the left
          void turnLeft(int degrees);

          //turns the robot degree so the right
          void turnRight(int degres);

          //updates the dist[] using drv
          void updateDists();
  }
}
