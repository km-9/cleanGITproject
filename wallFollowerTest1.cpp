#include "wallFollower.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>

using namespace WallFollower;
using namespace std;
int main (int argc, char const *argv[])
{
  cout << "in main" << endl;
  wallFollower f = wallFollower();
  //cout << "constructor finished" << endl;
  //f.goForward();
  //cout << "goForward finished" << endl;
  //f.holdTheFuckUp(10000000);
  //cout << "holdTheFuckUp" << endl;

  while (true)
  {
    if (f.avoidLeft())
    {
      int loopCounter = 0;
      f.strafeRight();
      int i = 0;
      while (f.avoidLeft())
      {
        if (i % 16667 == 0)
        {
          f.updateDists();
        }
        i++;
      }
    }
    else if (f.avoidRight())
    {
      loopCounter = 0;
      f.strafeLeft();
      int i = 0;
      while (f.avoidRight() && !f.avoidLeft())
      {
        if (i % 16667 == 0)
        {
          f.updateDists();
        }
        i++;
      }
    }
    else if(f.canGoForward())
    {
      loopCounter = 0;
      if(f.strafeLeftToAlignWithWall() || f.tooFarOnLeft())
      {
          f.logfile << "canGoForward() && (strafeLeftToAlignWithWall() || tooFarOnLeft())" << endl;
          f.logfile << "strafeLeft called" << endl;
          f.strafeLeft();
          int i = 0;
          while (!f.avoidRight() && !f.avoidLeft())
          {
            if (i % 16667 == 0)
            {
              f.updateDists();
            }
            i++;
          }
      }
      else if(f.strafeRightToAlignWithWall() || !f.canGoLeft())
      {
        logfile << "canGoForward() && (strafeRightToAlignWithWall() || !canGoLeft())" << endl;
        logfile << "strafeRight called" << endl;
        f.strafeRight();
        int i = 0;
        while (!f.avoidRight() && !f.avoidLeft())
        {
          if (i % 16667 == 0)
          {
            f.updateDists();
          }
          i++;
        }
      }
      else if (!f.tooFarOnLeft() && !f.canGoLeft())
      {
        logfile << "canGoForward && !toofarOnLeft && !canGoLeft" << endl;
        logfile << "goForward called" << endl;
        f.goForward();
        int i = 0;
        while (!f.avoidRight() && !f.avoidLeft())
        {
          if (i % 16667 == 0)
          {
            f.updateDists();
          }
          i++;
        }
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
