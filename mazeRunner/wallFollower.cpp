#include "wallFollower.h"
using namespace std;

namespace wallFollower
{
    wallFollower::wallFollower()
    {
        //left wheel
        PCA9685 * pwm1 = &PCA9685();
        pwm1.init(1,0x40);

        //right wheel
        PCA9685 * pwm2 = &PCA9685();
        pwm2.init(1,0x40);

        //look up the documentation on this
        pwm1->setPWMFreq(61);
        pwm2->setPWMFreq(61);

        //hold for a sec
        usleep(1000000);

        //stop both wheels 0 and 1 pins
        pwm1->setPWM(0,0,0);
        pwm2->setPWM(1,0,0);

        //to communicate with lidar
        int opt_com_baudrate = 115200;
      	//device path for lidar
        const char * opt_com_path = "/dev/ttyUSB0";

        this->drv = RPlidarDriver::CreateDriver(RPlidarDriver::DRIVER_TYPE_SERIALPORT);
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

            if (IS_FAIL(capture_and_display(drv))) {
                fprintf(stderr, "Error, cannot grab scan data.\n");
                break;
            }
      			cout << "end of lidar driver in main" << endl;
      	}
      	while(0);
        cout << "in wallFollower(drv)" << endl;
        //I incremented these because hypotenuses now
        this->min_forward_dist = 300;
        this->min_left_dist = 300;
        this->min_right_dist = 300;
        this->max_left_dist = 400;
        updateDists();
    }

    bool wallFollower::canGoForward()
    {
      cout << "in canForward" << endl;
      updateDists();

      //we should do trigonometry and enter dimensions of the robot to get this but fuck it
      if(getDists(170) > getMinForwardDist() && getDists(190) > getMinForwardDist())
      {
        cout << getDists(170)  << "  " << getDists(190) << endl;
        return true;
      }
      return false;
    }

    bool wallFollower::canGoLeft()
    {
      cout << "in canRight()" << endl;
      if(getDists(80) > getMinLeftDist() && getDists(100) > getMinLeftDist())
      {
        cout << getDists(80)  << "  " << getDists(100) << endl;
        return true;
      }
      return false;
    }

    bool wallFollower::canGoRight()
    {
      cout << "in canRight()" << endl;
      if(getDists(260) > getMinRightDist() && getDists(280) > getMinRightDist())
      {
        cout << getDists(260)  << "  " << getDists(280) << endl;
        return true;
      }
      return false;
    }

    double wallFollower::getDists(int i)
    {
      return (i < 0 || i > 360) ? 0 : dists[i];
    }

    int wallFollower::getMaxLeftDist()
    {
      return max_left_dist;
    }

    int wallFollower::getMinForwardDist()
    {
      return min_forward_dist;
    }

    int wallFollower::getMinLeftDist()
    {
      return min_left_dist;
    }

    int wallFollower::getMinRightDist()
    {
      return min_right_dist;
    }

    void wallFollower::holdTheFuckUp(int units)
    {
      pause(units);
    }

    void wallFollower::goForward()
    {
      pwm1->setPWM(0, 0, 150);
			pwm2->setPWM(1, 0, 600);
    }

    void wallFollower::pause(int units)
    {
      this->drv->usleep(units);
    }

    void wallFollower::reverse()
    {
      pwm1->setPWM(0, 0, 600);
			pwm2->setPWM(1, 0, 150);
    }

    void wallFollower::stop()
    {
      pwm1->setPWM(0,0,0);
    	pwm2->setPWM(1,0,0);
    }

    void wallFollower::takeItBackNowYall()
    {
      reverse();
    }

    bool wallFollower::tooFarOnLeft()
    {
      cout << "in canRight()" << endl;
      if(getDists(80) > getMaxLeftDist() || getDists(100) > getMaxLeftDist())
      {
        cout << getDists(80)  << "  " << getDists(100) << endl;
        return true;
      }
      return false;
    }

    void wallFollower::turnLeft(int degrees)
    {
      //idk if this works, but my idea is grab four points
      //of reference and if three agree within a range we can stop turning
      //if this doesn't work I would want to make time our parameter
      //and for (i < units) turn, then take in a bool keep_moving
      //if keep_moing == true go back to straight, else stop
      pwm1->setPWM(0,0,100);
      pwm2->setPWM(1,0,600);
    }

    void wallFollower::turnRight(int degrees)
    {
      //same idea with turnLeft()
      pwm1->setPWM(0,0,150);
      pwm2->setPWM(1,0,0);
    }

    void wallFollower::updateDists()
    {
      cout << "in updateDists" << endl;

    	rplidar_response_measurement_node_t nodes[360*2];
    	size_t count = _countof(nodes);

    	ans = drv->grabScanData(nodes, count);
    	if (IS_OK(ans) || ans == RESULT_OPERATION_TIMEOUT)
    	{
    			drv->ascendScanData(nodes, count);

    			double tmp [360];
    			for (int pos = 0; pos < (int)count ; ++pos)
    			{
    					tmp[pos] = nodes[pos].distance_q2/4.0f;
    			}
    	 else
    	 {
    			printf("error code: %x\n", ans);
    	}

      this->dists = tmp;
    }


}