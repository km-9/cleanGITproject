#include "wallFollower.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>

using namespace WallFollower;
using namespace std;

bool currCanLeft;
bool currCanRight;
bool currCanForward;

int main (int argc, char const *argv[])
{
  cout << "in main" << endl;
  wallFollower f = wallFollower();
  //cout << "constructor finished" << endl;
  //f.goForward();
  //cout << "goForward finished" << endl;
  //f.holdTheFuckUp(10000000);
  //cout << "holdTheFuckUp" << endl;
//TODO  f.startWallFollowingLeft();
  /*while (true){
    f.strafeRight();
  }*/

  while (true){
    f.updateDists();
    for (int i = 85; i < 275; i++){
      if (f.getDists(i) < 250){
        cout << "a distance is less than 250. avoidance call is needed" << endl;

        for (int x = 85; x < 125; x++){
          if (f.getDists(x) < 250){
            cout << "close object detected to left of robot at " << f.getDists(x) << endl;
            f.turnRight(45);
            f.updateDists();
            currCanRight = false;
            break;
          }
          else
          currCanRight = true;
        }

        for (int x = 125; x < 155; x++){
          if (f.getDists(x) < 250){
            cout << "close object detected to left-front of robot at " << f.getDists(x) << endl;
            f.strafeRight();
            f.updateDists();
            currCanRight = false;
            break;
          }
          else
          currCanRight = true;
        }

        for (int x = 155; x < 195; x++){
          if (f.getDists(x) < 250){
            cout << "close object detected to front of robot at " << f.getDists(x) << endl;
            f.stop();
            cout << "wheels stopped" << endl;
            cout << "additional protocol is needed here" << endl;
            f.updateDists();
            currCanForward = false;
            break;
          }
          else
          currCanForward = true;
        }

        for (int x = 195; x < 230; x++){
          if (f.getDists(x) < 250){
            cout << "close object detected to right-front of robot at " << f.getDists(x) << endl;
            f.strafeLeft();
            f.updateDists();
            break;
            currCanLeft = false;
          }
          else
          currCanRight = true;
        }

        for (int x = 230; x < 275; x++){
          if (f.getDists(x) < 250){
            cout << "close object detected to right of robot at " << f.getDists(x) << endl;
            f.turnLeft(45);
            f.updateDists();
            currCanLeft = false;
            break;
          }
          else
          currCanLeft = true;
        }

      }
    }
  }
/*
  while (true)
  {
    f.updateDists();
    if (f.avoidLeft())
    {
      f.strafeRight();
      while (f.avoidLeft())
      {
        f.updateDists();
      }
    }
    else if (f.avoidRight())
    {
      f.strafeLeft();
      while (f.avoidRight() && !f.avoidLeft())
      {
        f.updateDists();
      }
    }
    else if(f.canGoForward())
    {
      if(f.strafeLeftToAlignWithWall() || f.tooFarOnLeft())
      {
          f.logfile << "canGoForward() && (strafeLeftToAlignWithWall() || tooFarOnLeft())" << endl;
          f.logfile << "strafeLeft called" << endl;
          f.strafeLeft();
          while (!f.avoidRight() && !f.avoidLeft())
          {
            f.updateDists();
          }
      }
      else if(f.strafeRightToAlignWithWall() || !f.canGoLeft())
      {
        f.logfile << "canGoForward() && (strafeRightToAlignWithWall() || !canGoLeft())" << endl;
        f.logfile << "strafeRight called" << endl;
        f.strafeRight();
        while (!f.avoidRight() && !f.avoidLeft())
        {
          f.updateDists();
        }
      }
      else if (!f.tooFarOnLeft() && !f.canGoLeft())
      {
        f.logfile << "canGoForward && !toofarOnLeft && !canGoLeft" << endl;
        f.logfile << "goForward called" << endl;
        f.goForward();
        while (!f.avoidRight() && !f.avoidLeft())
        {
          f.updateDists();
        }
      }
    }
    else
    {
      f.stop();
    }

  }


  /*while (true)
  {
    while (f.canForward())
    {
      f.updateDists();
      f.goForward();
      //f.holdTheFuckUp(1000000);
      if (f.tooFarOnLeft())
      {
        f.turnLeft();
      }
      else if (f.canGoLeft())
      {
        f.turnRight()
      }
    }
    if (!f.canGoForward())
    {
      f.stop();
      f.reverse();
      f.turnLeft();
    }
  }*/
  f.drv->stop();
  f.drv->stopMotor();

  rp::standalone::rplidar::RPlidarDriver::DisposeDriver(f.drv);

  return 0;
}
