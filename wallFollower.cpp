#include "wallFollower.h"
#ifndef _countof
#define _countof(_Array) (int)(sizeof(_Array) / sizeof(_Array[0]))
#endif
using namespace std;
using namespace cv;

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
        this->min_left_dist = 175;
        this->max_left_dist = 300;
        this->min_right_dist = 175;
        this->pastGoLefts = new bool[10];
        this->pastGoRights = new bool[10];
        this->avoidance_radius = 150;
        for (int i = 0; i < 10; i++)
        {
          pastGoLefts[i] = false;
          pastGoRights[i] = false;
        }

        updateDists();

        this->logfile.open("WallFollowerRun.log");
    }

    int wallFollower::avoidHeadOn()
    {
      int direction = 0;
      for (int i = 1; i < 11; i++)
      {
        if(getDists(180 + i) > 400 + (8 * abs(i))/10)
        {

        }
        else if (getDists(180 + i) > getDists(180 - i))
        {
          direction++;
        }
        else
        {
          direction--;
        }
      }
      logfile << "aviodHeadOn has determined direction = " << direction << endl;
      cout << "avoidHeadOn has determined object = " << direction << endl;
      return direction;
    }

    bool wallFollower::avoidLeft()
    {
      for (int i = 90; i < 181; i++)
      {
        if(getDists(i) < avoidance_radius)
        {
          logfile << "avoidLeft has registered an object " << getDists(i) << " units away at " << i << " degrees " << endl;
          cout <<  "avoidLeft has registered an object " << getDists(i) << " units away at " << i << " degrees " << endl;
          return true;
        }
      }
      logfile << "avoidLeft has not registered any objects in front of it" << endl;
      cout << "avoidLeft has not registered any objects in front of it" << endl;
      return false;
    }

    bool wallFollower::avoidRight()
    {
      for (int i = 180; i < 271; i++)
      {
        if(getDists(i) < avoidance_radius)
        {
          logfile << "avoidRight has registered an object " << getDists(i) << " units away at " << i << " degrees " << endl;
          cout << "avoidRight has registered an object " << getDists(i) << " units away at " << i << " degrees " << endl;
          return true;
        }
      }
      logfile << "avoidRight has not registered any objects in front of it" << endl;
      cout <<  "avoidRight has not registered any objects in front of it" << endl;
      return false;
    }

    bool wallFollower::canGoBack()
    {
      for (int i = -30; i < 31; i++)
      {
        if(getDists(180 + i) < getMinForwardDist() + (25 * abs(i))/30)
        {
          logfile << "canGoBack has registered an object " << getDists( i) << " units away at " <<  i << " degrees " << endl;
          cout <<  "canGoBack has registered an object " << getDists( i) << " units away at " <<  i << " degrees " << endl;
          return false;
        }
      }
      logfile << "canGoBack has not registered any objects in front of it" << endl;
      cout <<  "canGoBack has not registered any objects in front of it" << endl;
      return true;
    }

    bool wallFollower::canGoForward()
    {
      for (int i = -20; i < 21; i++)
      {
        if(getDists(180 + i) < getMinForwardDist() + abs(i))
        {
          logfile << "canGoForward has registered an object " << getDists(180 + i) << " units away at " << 180 + i << " degrees " << endl;
          cout << "canGoForward has registered an object " << getDists(180 + i) << " units away at " << 180 + i << " degrees " << endl;
          return false;
        }
      }
      logfile << "canGoForward has not registered any objects in front of it" << endl;
      cout << "canGoForward has not registered any objects in front of it" << endl;
      return true;
    }

    bool wallFollower::canGoLeft()
    {
      for (int i = 0; i < 16; i++)
      {
        if(getDists(90 + i) < getMinLeftDist() + (15 * abs(i))/15)
        {
          logfile << "canGoLeft has registered an object " << getDists(90 + i) << " units away at " << 90 + i << " degrees " << endl;
          cout << "canGoLeft has registered an object " << getDists(90 + i) << " units away at " << 90 + i << " degrees " << endl;
          for (int i = 0; i < 10; i++)
          {
            pastGoLefts[i] = pastGoLefts[i+1];
          }
          pastGoLefts[0] = false;
          return false;
        }
      }
      logfile << "canGoLeft has not registered any objects in front of it" << endl;
      cout << "canGoLeft has not registered any objects in front of it" << endl;
      for (int i = 0; i < 10; i++)
      {
        pastGoLefts[i] = pastGoLefts[i+1];
      }
      pastGoLefts[0] = true;
      return true;
    }

    bool wallFollower::canGoRight()
    {
      for (int i = 0; i < 16; i++)
      {
        if(getDists(270 + i) < getMinRightDist() + (15 * abs(i))/15)
        {
          logfile << "canGoRight has registered an object " << getDists(270 + i) << " units away at " << 270 + i << " degrees " << endl;
          cout << "canGoRight has registered an object " << getDists(270 + i) << " units away at " << 270 + i << " degrees " << endl;
          for (int i = 0; i < 10; i++)
          {
            pastGoRights[i] = pastGoRights[i+1];
          }
          pastGoRights[0] = false;
          return false;
        }
      }
      logfile << "canGoRight has not registered any objects in front of it" << endl;
      cout << "canGoRight has not registered any objects in front of it" << endl;
      for (int i = 0; i < 10; i++)
      {
        pastGoRights[i] = pastGoRights[i+1];
      }
      pastGoRights[0] = true;
      return true;
    }

    bool wallFollower::couldntGoLeft()
    {
      int numLefts;
      for (int i = 0; i < 10; i++)
      {
        pastGoLefts[i]?numLefts--:numLefts++;
      }
      return (numLefts>4)?true:false;
    }

    bool wallFollower::couldntGoRight()
    {
      int numRights;
      for (int i = 0; i < 10; i++)
      {
        pastGoRights[i]?numRights--:numRights++;
      }
      return (numRights>4)?true:false;
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

    void wallFollower::goForward()
    {
      pwm1.setPWM(0, 0, 255);
			pwm2.setPWM(1, 0, 500);
    }

    void wallFollower::holdTheFuckUp(int units)
    {
      pause(units);
    }

    bool wallFollower::leftPathAppeared()
    {
      return (canGoLeft() && couldntGoLeft())?true:false;
    }

    void wallFollower::pause(int units)
    {
      logfile << "pause(" << units << ") has been called" << endl;
      cout << "pause(" << units << ") has been called" << endl;
      usleep(units);
    }

    void wallFollower::reverse()
    {
      pwm1.setPWM(0, 0, 600);
			pwm2.setPWM(1, 0, 300);
    }

    bool wallFollower::rightPathAppeared()
    {
      return (canGoRight() && couldntGoRight())?true:false;
    }

    bool wallFollower::strafeLeftToAlignWithWall()
    {
      int goLeft = 0;
      for (int i = 10; i < 30; i++)
      {
          (getDists(90 - i) < getDists(90 + i))?goLeft++:goLeft--;
      }
      string answer = (goLeft>0)?"true":"false";
      logfile << "strafeLeftToAlignWithWall has determined "<< answer << endl;
      cout << "strafeLeftToAlignWithWall has determined "<< answer << endl;
      (goLeft>0)?true:false;
    }

    bool wallFollower::strafeRightToAlignWithWall()
    {
      int goRight = 0;
      for (int i = 10; i < 30; i++)
      {
          (getDists(270 - i) < getDists(270 + i))?goRight++:goRight--;
      }
      string answer = (goRight>0)?"true":"false";
      logfile << "strafeRightToAlignWithWall has determined "<< answer << endl;
      cout << "strafeRightToAlignWithWall has determined "<< answer << endl;
      (goRight>0)?true:false;
    }

    void wallFollower::startWallFollowingLeft()
    {
      int loopCounter = 0;
      while(true)
      {
        UPDATE: updateDists();
        if(loopCounter > 3)
        {
          //unstick
          logfile << "RIP LMAO" << endl;
          reverse();
          pause(500000);
          turnLeft(180);
        };

        if (avoidHeadOn() != 0)
        {
          if (avoidHeadOn() < 0)
          {
            turnLeft(30);
          }
          else
          {
            turnRight(30);
          }
          goto UPDATE;
        }
        else if (avoidLeft())
        {
          loopCounter = 0;
          strafeRight();
          while (avoidLeft())
          {
            updateDists();
          }
          goto UPDATE;
        }
        else if (avoidRight())
        {
          loopCounter = 0;
          strafeLeft();
          while (avoidRight() && !avoidLeft())
          {
            updateDists();
          }
          goto UPDATE;
        }
        else if(canGoForward())
        {
          loopCounter = 0;
          if(strafeLeftToAlignWithWall() || tooFarOnLeft())
          {
              logfile << "canGoForward() && (strafeLeftToAlignWithWall() || tooFarOnLeft())" << endl;
              logfile << "strafeLeft called" << endl;
              cout << "canGoForward() && (strafeLeftToAlignWithWall() || tooFarOnLeft())" << endl;
              cout << "strafeLeft called" << endl;
              strafeLeft();
              while (!avoidRight() && !avoidLeft() && (avoidHeadOn() != 0))
              {
                updateDists();
              }
              goto UPDATE;
          }
          else if(strafeRightToAlignWithWall() || !canGoLeft())
          {
            logfile << "canGoForward() && (strafeRightToAlignWithWall() || !canGoLeft())" << endl;
            logfile << "strafeRight called" << endl;
            cout << "canGoForward() && (strafeRightToAlignWithWall() || !canGoLeft())" << endl;
            cout << "strafeRight called" << endl;
            strafeRight();
            while (!avoidRight() && !avoidLeft() && (avoidHeadOn() != 0))
            {
              updateDists();
            }
            goto UPDATE;
          }
          else if (!tooFarOnLeft() && !canGoLeft())
          {
            logfile << "canGoForward && !toofarOnLeft && !canGoLeft" << endl;
            logfile << "goForward called" << endl;
            cout << "canGoForward && !toofarOnLeft && !canGoLeft" << endl;
            cout << "goForward called" << endl;
            goForward();
            while (!avoidRight() && !avoidLeft() && (avoidHeadOn() != 0))
            {
              updateDists();

            }
            goto UPDATE;
          }
        }
        else
        {
          loopCounter = 0;
          if(canGoLeft())
          {
            logfile << "!canGoForward && canGoLeft" << endl;
            cout << "!canGoForward && canGoLeft" << endl;
            //might need to be looked at
            turnLeft(90);
            goto UPDATE;
          }
          else if(canGoRight())
          {
            logfile << "!canGoForward && canGoRight" << endl;
            cout << "!canGoForward && canGoRight" << endl;
            turnRight(90);
            goto UPDATE;
          }
        }
      }
      loopCounter++;
    }

    void wallFollower::stop()
    {
      logfile << "stop has been called" << endl;
      cout << "stop has been called" << endl;
      pwm1.setPWM(0,0,0);
    	pwm2.setPWM(1,0,0);
    }

    void wallFollower::strafeLeft()
    {
      logfile << "strafeLeft" << endl;
      cout <<  "strafeLeft" << endl;
      //idk if this works, but my idea is grab four points
      //of reference and if three agree within a range we can stop turning
      //if this doesn't work I would want to make time our parameter
      //and for (i < units) turn, then take in a bool keep_moving
      //if keep_moing == true go back to straight, else stop
      pwm1.setPWM(0,0,150);
      pwm2.setPWM(1,0,325);
    }

    void wallFollower::strafeRight()
    {
      logfile << "in strafeRight" << endl;
      cout << "in strafeRight" << endl;
      //same idea with turnLeft()
      pwm1.setPWM(0,0,290);
      pwm2.setPWM(1,0,400);
    }

    void wallFollower::takeItBackNowYall()
    {
      reverse();
    }

    bool wallFollower::tooFarOnLeft()
    {
      for (int i = -15; i < 16; i++)
      {
        if(getDists(90 + i) > getMaxLeftDist() + (25 * abs(i))/30)
        {
          logfile << "tooFarOnLeft has decided that theta " << 90 + i << " is " << getDists(90 + i) << " from the wall, so we are too far" << endl;
          cout << "tooFarOnLeft has decided that theta " << 90 + i << " is " << getDists(90 + i) << " from the wall, so we are too far" << endl;
          return true;
        }
      }
      logfile << "tooFarOnLeft has decided we are not too far from the left wall" << endl;
      cout << "tooFarOnLeft has decided we are not too far from the left wall" << endl;
      return false;
    }

    void wallFollower::turnLeftUntilYouCant()
    {
      stop();
      logfile << "turnLeftUntilYouCant() called" << endl;
      long time = 1000;
      do {
        pwm1.setPWM(0,0,250);
        pwm2.setPWM(1,0,0);
        holdTheFuckUp(time);
      } while(canGoLeft() && avoidLeft());
    }

    int wallFollower::updateDists()
    {
      int ans;

    	rplidar_response_measurement_node_t nodes[360*2];
    	int count = _countof(nodes);

      //double tmp [360];
      ans = drv->grabScanData(nodes, count);
    	if (IS_OK(ans) || ans == RESULT_OPERATION_TIMEOUT)
    	{
    			drv->ascendScanData(nodes, count);

    			for (int pos = 0; pos < 360 ; ++pos)
    			{
            dists[pos] = (nodes[pos].distance_q2/4.0f == 0)?1000:nodes[pos].distance_q2/4.0f;
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
      stop();
      logfile << "turnLeft(" << degrees << ") called" << endl;
      cout << "turnLeft(" << degrees << ") called" << endl;
      long time = degrees*12700;
      pwm1.setPWM(0,0,250);
      pwm2.setPWM(1,0,0);
      usleep(time);
    }

    void wallFollower::turnRight(int degrees)
    {
      stop();
      logfile << "turnRight(" << degrees << ") called" << endl;
      cout << "turnRight(" << degrees << ") called" << endl;
      long time = degrees*12700;
      pwm1.setPWM(0,0,0);
      pwm2.setPWM(1,0,500);
      usleep(time);
    }

    void wallFollower::turnRightInPlace()
    {
      pwm1.setPWM(0,0,335);
      pwm2.setPWM(1,0,335);
    }

    //This is a bit off-the-cuff and I am just expirementing with the LiDAR conditions. Feel free to ignore this, but DO NOT DELETE
    /*
    The concept here is to use "sectors" of lidar data to make decisions. With only 10 sectors we have over 1000 options (1024).
    So obviously the less complex we make sectors the better. Some situations can be ignored (anytime sRFront5 or sLFront5 are short)
    which helps cut back on the massive number of cases to deal with. If we can get the relevant cases distilled out and implimented
    we should be able to use sectors to determine the robots relative position to the closest object. Which means smarter decisions
    and smoother/faster/better navigation.
    One nice thing is that we are really only mirroring the left and right conditions. Which means we cut even our relevant cases in
    half and just change "left" to "right". (512 is still too many, so learning to filter and ignore bad cases is crucial)
    */
    double sLeft3, sLeft37, sLeft30, sLFront15, sLFront5, sRFront5, sRFront15, sRight30, sRight37, sRight3;
    /*
    the doubles will hold the average distance of the sector. we can make partial dicisions with this data
    */
    bool sLeft3LR, sLeft37LR, sLeft30LR, sLFront15LR, sLFront5LR, sRFront5LR, sRFront15LR, sRight30LR, sRight37LR, sRight3LR;
    /*
    the bools allow you to know which general orientation the robot has. If the bool for a sector is true, that means that the robot is angled TOWARDS the side of the sector.
    ie: sLeft3LR == true, means that the robot is oriented at least slightly to the left. sRight3LR means that the robot is angle to the right. This can be used to easily
    orient the robot, but also to decide between going straight, arcing, turning, or backing up. If the robot is angled right and has a sRFront5 value that is too low we know
    that the robot is slightly skewed to the right and facing a wall in front of it. Turning/Spinning left is the best option.
    */
/*
    void wallFollower::lidarSectorAssignment(){
      updateDists();
      for (int i = 89; i < 93; i++){
        ///////////////////////////////////
        if (dists[89] > dists[92]){
          sLeft3LR = true;
        }
        sLeft3 = sLeft3 + dists[i];
      }
      sLeft3 = sLeft3/3;
      /////////////////////////////////////
      for (int i = 93; i < 130; i++){
        if (dists[93] > dists[129]){
          sLeft37LR = true;
        }
        sLeft37 = sLeft37 + dists[i];
      }
      sLeft37 = sLeft37/37;
      /////////////////////////////////////
      for (int i = 130; i < 160: i++){
        if (dists[130] > dists[159]){
          sLeft30LR = true;
        }
        sLeft30 = sLeft30 + dists[i];
      }
      sLeft30 = sLeft30/30;
      /////////////////////////////////////
      for (int i = 160; i < 175; i++){
        if (dists[160] > dists[174]){
          sLFront15LR = true;
        }
        sLFront15 = sLFront15 + dists[i];
      }
      sLFront15 = sLFront15/15;
      /////////////////////////////////////
      for (int i = 175; i < 180){
        if (dists[175] > dists[179]){
          sLFront5LR = true;
        }
        sLFront5 = sLFront5 + dists[i];
      }
      sLFront5 = sLFront5/5;
      /////////////////////////////////////
      for (int i = 180; i < 185; i++){
        if (dists[180] < dists[185]){
          sRFront5LR = true;
        }
        sRFront5 = sRFront5 + dists[i];
      }
      sRFront5 = sRFront5/5;
      /////////////////////////////////////
      for (int i = 185; i < 200; i++){
        if (dists[185] < dists[200]){
          sRFront15LR = true;
        }
        sRFront15 = sRFront15 + dists[i];
      }
      sRFront15 = sRFront15/15;
      //////////////////////////////////////
      for (int i = 200; i < 230; i++){
        if (dists[200] < dists[230]){
          sRight30LR = true;
        }
        sRight30 = sRight30 + dists[i];
      }
      sRight30 = sRight30/30;
      /////////////////////////////////////
      for (int i = 230; i < 267; i++){
        if (dists[230] < dists[267]){
          sRight37LR = true;
        }
        sRight37 = sRight37 + dists[i];
      }
      sRight37 = sRight37/37;
      /////////////////////////////////////
      for (int i = 267; i < 270; i++){
        if (dists[267] < dists[270]){
          sRight3LR = true;
        }
        sRight3 = sRight3 + dists[i];
      }
      sRight3 = sRight3/3;
      /////////////////////////////////////
    }

    //*******************************************************end of expirement******************************************************/
    void wallFollower::swayToLeft(){
      pwm1.setPWM(0,0,150);
      pwm2.setPWM(1,0,360);
    }

    void wallFollower::swayToRight(){
      pwm1.setPWM(0,0,255);
      pwm2.setPWM(1,0,550);
    }
    //Left Wall Follower
    void wallFollower::leftWallFollower(double avg){
      if (avg > 200){
        swayToLeft();
        return;
      }
      if (avg < 200){
        swayToRight();
        return;
      }
      if (avg == 200){
        goForward();
        return;
      }
    }
    //Front Wall Handler
    void wallFollower::frontHandler(double fAvg, double lAvg){
      if(fAvg < 450) {
        while(getDists(180) < 275) {
          turnRightInPlace();
          updateDists();
        }
        while(getDists(90) < 275) {
          turnRightInPlace();
          updateDists();
        }
        stop();
      }
      //if (fAvg < 450){
      // stop();
      //turnRight(35);
    }

    void camToFile(string fileName)
    {
  // Create a VideoCapture object and use camera to capture the video
  VideoCapture cap(0);

  // Check if camera opened successfully
  if(!cap.isOpened())
  {
    cout << "Error opening video stream" << endl;
    return;
  }

  // Default resolution of the frame is obtained.The default resolution is system dependent.
  int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
  int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

  // Define the codec and create VideoWriter object.The output is stored in '~/Desktop/Capstone/robotGITcode/sampleVideo.avi' file.
  VideoWriter video("~/Desktop/Capstone/robotGITcode/sampleVideo.avi",CV_FOURCC('M','J','P','G'),10, Size(frame_width,frame_height));
    Mat frame;

    // Capture frame-by-frame
    cap >> frame;

    // If the frame is empty, break immediately
    if (frame.empty())
    cout<<"empty frame"<<endl;
      return;

    // Write the frame into the file 'outcpp.avi'
    video.write(frame);

    // Display the resulting frame
    imshow( "Frame", frame );

    // Press  ESC on keyboard to  exit
    char c = (char)waitKey(1);
    if( c == 27 ){
      // When everything done, release the video capture and write object
      cap.release();
      video.release();

      // Closes all the windows
      destroyAllWindows();
      return;
    }
 }
}
