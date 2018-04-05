#include "wallFollower.h"
#ifndef _countof
#define _countof(_Array) (int)(sizeof(_Array) / sizeof(_Array[0]))
#endif
using namespace std;
using namespace cv;


const int leftMotorIndex = 1;
const int rightMotorIndex = 0;

namespace WallFollower
{
    wallFollower::wallFollower()
    {

      cout << "Attaching to PCA9685" << endl;
        pwm.init(1,0x40);

        //Period of one full PWM wave (not the pulse width)
        pwm.setPWMFreq(50);

        //hold for a sec
        usleep(1000000);

	setMotorSpeed(0,0);

      cout << "Connecting to Lidar" << endl;

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

            updateDists();
      			//cout << "end of lidar driver in main" << endl;
      	}
      	while(0);
        //cout << "in wallFollower(drv)" << endl;
        //I incremented these because hypotenuses now

        updateDists();

	cout << "opening log file" << endl;

        this->logfile.open("WallFollowerRun.log");

	cout << "constructor completed" << endl;
    }

    //returns the average of all distances at angles between start and end
    double wallFollower::avg(int start, int end){
        int temp = 0;
        int divisor = end-start;
        double ans = 0;
        while (start < end){
          if (getDists(start) == 0){
            temp++;
          }
          ans = ans+getDists(start);
          start++;
        }
        divisor = divisor - temp;
	if (divisor == 0 || ans == 0) return -1;
        return ans/divisor;
    }


    double wallFollower::getDists(int i)
    {
      return (i < 0 || i > 360) ? 0 : dists[i];
    }

    void wallFollower::stop()
    {
      logfile << "stop has been called" << endl;
      //cout << "stop has been called" << endl;
      setMotorSpeed(0,0);
    }


    void wallFollower::updateDists()
    {
      vector<point> data = readLidar(drv, true);
      interpolate(data, dists, 360, 1);
      int first = dists[0];
      int second = dists[1];
      for(size_t i=2; i < 360; i++)
	dists[i-2] = dists[i];
      dists[358]=first;
      dists[359]=second;
    }


    void wallFollower::setMotorSpeed(int left, int right) {
      pwm.setPWM(leftMotorIndex,320+left*2); // left == 320 stops, <160 backwards, >160 forwards
      pwm.setPWM(rightMotorIndex,318-right*2);
    }
}
