#include "wallFollower.h"
#ifndef _countof
#define _countof(_Array) (int)(sizeof(_Array) / sizeof(_Array[0]))
#endif
using namespace std;

namespace WallFollower
{
    wallFollower::wallFollower()
    {
        //left wheel
        PCA9685 pwm1;
        //PCA9685 * pwm1 = &PCA9685();
        pwm1.init(1,0x40);

        //right wheel
        PCA9685 pwm2;
        //PCA9685 * pwm2 = &PCA9685();
        pwm2.init(1,0x40);

        //look up the documentation on this
        pwm1.setPWMFreq(50);
        pwm2.setPWMFreq(50);

        //hold for a sec
        usleep(1000000);

        //stop both wheels 0 and 1 pins
        pwm1.setPWM(0,0,0);
        pwm2.setPWM(1,0,0);

        this->pwm1 =  pwm1;
        this->pwm2 =  pwm2;

        //to communicate with lidar
        int opt_com_baudrate = 115200;
      	//device path for lidar
        const char * opt_com_path = "/dev/ttyUSB0";

        this->drv = rp::standalone::rplidar::RPlidarDriver::CreateDriver(rp::standalone::rplidar::RPlidarDriver::DRIVER_TYPE_SERIALPORT);
      	do
        {
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

            if (IS_FAIL(updateDists()))
            {
                fprintf(stderr, "Error, cannot grab scan data.\n");
                break;
            }
      			cout << "end of lidar driver in main" << endl;
      	}
      	while(0);
        cout << "in wallFollower(drv)" << endl;
        //I incremented these because hypotenuses now
        this->min_forward_dist = 140.5;
        this->min_left_dist = 140.5;
        this->max_left_dist = 175;
        this->min_right_dist = 140.5;
        this->max_left_dist = 175;
        updateDists();

        this->logfile.open("WallFollowerRun.log");
    }

    bool wallFollower::canGoForward()
    {
      updateDists();

      //we should do trigonometry and enter dimensions of the robot to get this but fuck it
      for (int i = -30; i < 31; i++)
      {
        if(getDists(180 + i) < getMinForwardDist() + (22 * abs(i))/30)
        {
          logfile << "canGoForward has registered an object at " << 180 + i << " degrees " << endl;
          return false;
        }
      }
      logfile << "canGoForward has not registered any objects in front of it" << endl;
      return true;
    }

    bool wallFollower::canGoLeft()
    {
      for (int i = -30; i < 31; i++)
      {
        if(getDists(90 + i) < getMinLeftDist() + (22 * abs(i))/30)
        {
          logfile << "canGoLeft has registered an object at " << 90 + i << " degrees " << endl;
          return false;
        }
      }
      logfile << "canGoLeft has not registered any objects in front of it" << endl;
      return true;
    }

    bool wallFollower::canGoRight()
    {
      cout << "in canRight()" << endl;
      for (int i = -30; i < 31; i++)
      {
        if(getDists(270 + i) < getMinRightDist() + (22 * abs(i))/30)
        {
          logfile << "canGoRight has registered an object at " << 270 + i << " degrees " << endl;
          return false;
        }
      }
      logfile << "canGoLeft has not registered any objects in front of it" << endl;
      return true;
    }

    double wallFollower::getDists(int i)
    {
      return (i < 0 || i > 360) ? 0 : dists[i];
    }

    int wallFollower::getMaxLeftDist()
    {
      return this->max_left_dist;
    }

    int wallFollower::getMinForwardDist()
    {
      return this->min_forward_dist;
    }

    int wallFollower::getMinLeftDist()
    {
      return this->min_left_dist;
    }

    int wallFollower::getMinRightDist()
    {
      return this->min_right_dist;
    }

    void wallFollower::holdTheFuckUp(int units)
    {
      pause(units);
    }

    void wallFollower::goForward()
    {
      pwm1.setPWM(0, 0, 250);
			pwm2.setPWM(1, 0, 500);
    }

    void wallFollower::pause(int units)
    {
      logfile << "pause(" << units << ") has been called" << endl;
      usleep(units);
    }

    void wallFollower::reverse()
    {
      pwm1.setPWM(0, 0, 600);
			pwm2.setPWM(1, 0, 150);
    }

    bool wallFollower::shouldStrafeLeft()
    {
      int goLeft = 0;
      for (int i = 10; i < 30; i++)
      {
          (getDists(90 - i) < getDists(90 + i))?goLeft++:goLeft--;
      }
      logfile << "shouldStrafeLeft has determined "<< (goLeft>0)?"true":"false" << endl;
      (goLeft>0)?true:false;
    }

    bool wallFollower::shouldStrafeRight()
    {
      int goRight = 0;
      for (int i = 10; i < 30; i++)
      {
          (getDists(270 - i) < getDists(270 + i))?goRight++:goRight--;
      }
      logfile << "shouldStrafeRight has determined "<< (goRight>0)?"true":"false" << endl;
      (goRight>0)?true:false;
    }

    void wallFollower::startWallFollowingLeft()
    {
      while(true)
      {
        updateDists();
        if(canGoForward() && !tooFarOnLeft() && !canGoLeft())
        {
          logfile << "canGoForward && !toofarOnLeft && !canGoLeft" << endl;
          logfile << "goForward called" << endl;
          while(canGoForward() && !tooFarOnLeft() && !canGoLeft())
          {
            updateDists();
            goForward();
          }
        }
        else if(canGoForward() && (shouldStrafeLeft() || tooFarOnLeft()))
        {
          logfile << "canGoForward() && (shouldStrafeLeft() || tooFarOnLeft())" << endl;
          logfile << "strafeLeft called" << endl;
          while(canGoForward() && (shouldStrafeLeft() || tooFarOnLeft()))
          {
            updateDists();
            strafeLeft();
          }
        }
        else if(canGoForward() && (shouldStrafeRight() || !canGoLeft()))
        {
          logfile << "canGoForward() && (shouldStrafeRight() || !canGoLeft())" << endl;
          logfile << "strafeRight called" << endl;
          while (canGoForward() && (shouldStrafeRight() || !canGoLeft()))
          {
            updateDists();
            strafeRight();
          }
        }
        else if(!canGoForward() && canGoLeft())
        {
          logfile << "canGoForward && canGoLeft" << endl;
          //might need to be looked at
          turnLeft(90);
        }
        else if(!canGoForward() && !canGoLeft() && canGoRight())
        {
          logfile << "canGoForward && !canGoLeft && canGoRight" << endl;
          turnRight(90);
        }
        else
        {
          logfile << "180" << endl;
          //let's mke it a 180 turn
          turnLeft(180);
        }
      }
    }

    void wallFollower::stop()
    {
      logfile << "stop has been called" << endl;
      pwm1.setPWM(0,0,0);
    	pwm2.setPWM(1,0,0);
    }

    void wallFollower::strafeLeft()
    {
      logfile << "strafeLeft" << endl;
      //idk if this works, but my idea is grab four points
      //of reference and if three agree within a range we can stop turning
      //if this doesn't work I would want to make time our parameter
      //and for (i < units) turn, then take in a bool keep_moving
      //if keep_moing == true go back to straight, else stop
      pwm1.setPWM(0,0,150);
      pwm2.setPWM(1,0,350);
    }

    void wallFollower::strafeRight()
    {
      logfile << "in strafeRight" << endl;
      //same idea with turnLeft()
      pwm1.setPWM(0,0,275);
      pwm2.setPWM(1,0,400);
    }

    void wallFollower::takeItBackNowYall()
    {
      reverse();
    }

    bool wallFollower::tooFarOnLeft()
    {
      for (int i = -30; i < 31; i++)
      {
        if(getDists(90 + i) > getMaxLeftDist() + (22 * abs(i))/30)
        {
          logfile << "tooFarOnLeft has decided that theta" << 90 + i << " is " << getDists(90 + i) << " from the wall, so we are too far" << endl;
          return true;
        }
      }
      logfile << "tooFarOnLeft has decided we are not too far from the left wall" << endl;
      return false;
    }

    int wallFollower::updateDists()
    {
      int ans;
      logfile << "in updateDists" << endl;

    	rplidar_response_measurement_node_t nodes[360*2];
    	int count = _countof(nodes);

      //double tmp [360];
      ans = drv->grabScanData(nodes, count);
    	if (IS_OK(ans) || ans == RESULT_OPERATION_TIMEOUT)
    	{
    			drv->ascendScanData(nodes, count);

    			//0 is behind
    			for (int pos = 0; pos < 360 ; ++pos)
    			{
            dists[pos] = nodes[pos].distance_q2/4.0f;
            /*
            for (int pos = 0; pos < (int)count ; ++pos)
            {

                //tmp[pos] = nodes[pos].distance_q2/4.0f;
            }*/
    			}
    	}
    	else
    	{
    		printf("error code: %x\n", ans);
    	}

      //this->dists = tmp;
      return 1;
    }

    void wallFollower::turnLeft(int degrees)
    {
      logfile << "turnLeft(" << degrees << ") called" << endl;
      long time = degrees*11111;
      pwm1.setPWM(0,0,150);
      pwm2.setPWM(1,0,0);
      usleep(time);
    }

    void wallFollower::turnRight(int degrees)
    {
      logfile << "turnRight(" << degrees << ") called" << endl;
      long time = degrees*11111;
      pwm1.setPWM(0,0,0);
      pwm2.setPWM(1,0,600);
      usleep(time);
    }
}
