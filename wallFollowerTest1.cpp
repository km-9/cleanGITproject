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
    if (avoidLeft())
    {
      loopCounter = 0;
      strafeRight();
      int i = 0;
      while (avoidLeft())
      {
        if (i % 16667 == 0)
        {
          updateDists();
        }
        i++;
      }
      goto UPDATE;
    }
    else if (avoidRight())
    {
      loopCounter = 0;
      strafeLeft();
      while (avoidRight() && !avoidLeft())
      {
        if (i % 16667 == 0)
        {
          updateDists();
        }
        i++;
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
          strafeLeft();
          while (!avoidRight() && !avoidLeft())
          {
            if (i % 16667 == 0)
            {
              updateDists();
            }
            i++;
          }
          goto UPDATE;
      }
      else if(strafeRightToAlignWithWall() || !canGoLeft())
      {
        logfile << "canGoForward() && (strafeRightToAlignWithWall() || !canGoLeft())" << endl;
        logfile << "strafeRight called" << endl;
        strafeRight();
        while (!avoidRight() && !avoidLeft())
        {
          if (i % 16667 == 0)
          {
            updateDists();
          }
          i++;
        }
        goto UPDATE;
      }
      else if (!tooFarOnLeft() && !canGoLeft())
      {
        logfile << "canGoForward && !toofarOnLeft && !canGoLeft" << endl;
        logfile << "goForward called" << endl;
        goForward();
        while (!avoidRight() && !avoidLeft())
        {
          if (i % 16667 == 0)
          {
            updateDists();
          }
          i++;
        }
        goto UPDATE;
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
