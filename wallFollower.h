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
#include "opencv2/imgproc.hpp"
#include <opencv2/core/utility.hpp>//TODONE add to makefile
#include "opencv2/video/tracking.hpp"//TODONE add to makefile
#include "point.hpp"
#include "interpolator.hpp"
#include "rptypes.h"
#include "lidar_support.hpp"

namespace WallFollower
{
  class wallFollower
  {
      public:
          //an 360 length array with distance from a lidar detector to the nearest detectable object at the index's angle
          double dists [360];

          //lidar controller
          rp::standalone::rplidar::RPlidarDriver * drv;

          //logfile
          std::ofstream logfile;

          //left wheel
          PCA9685 pwm;

          //constructor
          wallFollower();

          //averages distances between degree values, returns -1 if all values in the range are zero
          double avg(int start, int end);

         //getter for dist[i]
         double getDists(int i);

          //stop both wheels
          void stop();

          //updates the dist[] using drv
          void updateDists();

	  // set the left and right motor speeds speeds range from
	  // -128 to 128 where negative speeds are backwards.  The
	  // larger the number, the higher the motor speed.
	  void setMotorSpeed(int left, int right);

  };
}
