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
        this->min_forward_dist = 175;
        this->min_left_dist = 150;
        this->max_left_dist = 175;
        this->min_right_dist = 150;
        this->max_left_dist = 175;
        updateDists();
    }

    bool wallFollower::canGoForward()
    {
      cout << "in canForward" << endl;
      updateDists();

      //we should do trigonometry and enter dimensions of the robot to get this but fuck it
      if(getDists(179) > getMinForwardDist() || getDists(181) > getMinForwardDist() && getDists(180) > getMinForwardDist())
      {
        cout << "can" << endl;
        cout << getDists(179)  << "  " << getDists(181) << endl;
        return true;
      }
      return false;
    }

    bool wallFollower::canGoLeft()
    {
      cout << "in canLeft()" << endl;
      if(getDists(89) > getMinLeftDist() || getDists(91) > getMinLeftDist() && getDists(90) > getMinLeftDist())
      {
        cout << "can" << endl;
        cout << getDists(90)  << "  " << getDists(90) << endl;
        return true;
      }
      return false;
    }

    bool wallFollower::canGoRight()
    {
      cout << "in canRight()" << endl;
      if(getDists(269) > getMinRightDist() && getDists(271) > getMinRightDist() && getDists(270) > getMinRightDist())
      {
        cout << "can" << endl;
        cout << getDists(270)  << "  " << getDists(270) << endl;
        return true;
      }
      return false;
    }

    double wallFollower::getDists(int i)
    {
      return (i < 0 || i > 360) ? 0 : dists[i];
    }

    rp::standalone::rplidar::RPlidarDriver * wallFollower::getLidar()
    {
      return this->drv;
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
      usleep(units);
    }

    void wallFollower::reverse()
    {
      pwm1.setPWM(0, 0, 600);
			pwm2.setPWM(1, 0, 150);
    }

    void wallFollower::stop()
    {
      pwm1.setPWM(0,0,0);
    	pwm2.setPWM(1,0,0);
    }

    void wallFollower::takeItBackNowYall()
    {
      reverse();
    }

    bool wallFollower::tooFarOnLeft()
    {
      cout << "in canRight()" << endl;
      if(getDists(90) > getMaxLeftDist() || getDists(91) > getMaxLeftDist())
      {
        cout << getDists(80)  << "  " << getDists(100) << endl;
        return true;
      }
      return false;
    }

    bool wallFollower::tooCloseOnLeft()
    {
      cout << "in canRight()" << endl;
      if(getDists(90) < getMaxLeftDist()-1 || getDists(91) < getMaxLeftDist()-1)
      {
        cout << getDists(80)  << "  " << getDists(100) << endl;
        return true;
      }
      return false;
    }

    void wallFollower::strafeLeft()
    {
      cout << "strafeLeft" << endl;
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
      cout << "strafeRight" << endl;
      //same idea with turnLeft()
      pwm1.setPWM(0,0,275);
      pwm2.setPWM(1,0,400);
    }

    int wallFollower::updateDists()
    {
      int ans;
      cout << "in updateDists" << endl;

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

    void wallFollower::turnLeft(int degrees){
      cout << "turnLeft" << endl;
      long time = degrees*11111;
      pwm1.setPWM(0,0,150);
      pwm2.setPWM(1,0,0);
      usleep(time);
    }

    void wallFollower::turnRight(int degrees){
      cout << "turnRight" << endl;
      long time = degrees*11111;
      pwm1.setPWM(0,0,0);
      pwm2.setPWM(1,0,600);
      usleep(time);
    }
}
