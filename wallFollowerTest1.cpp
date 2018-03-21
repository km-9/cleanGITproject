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
    if (f.canGoForward())
    {
      if (!tooFarOnLeft() && !canGoLeft())
      {
        logfile << "canGoForward && !toofarOnLeft && !canGoLeft" << endl;
        logfile << "goForward called" << endl;
        while(canGoForward() && !tooFarOnLeft() && !canGoLeft())
        {
          updateDists();
          goForward();
        }
      }
      else if(strafeLeftToAlignWithWall() || tooFarOnLeft())
      {
          logfile << "canGoForward() && (strafeLeftToAlignWithWall() || tooFarOnLeft())" << endl;
          logfile << "strafeLeft called" << endl;
          while(canGoForward() && (strafeLeftToAlignWithWall() || tooFarOnLeft()))
          {
            updateDists();
            strafeLeft();
          }
      }
      else if(strafeRightToAlignWithWall() || !canGoLeft())
      {
        logfile << "canGoForward() && (strafeRightToAlignWithWall() || !canGoLeft())" << endl;
        logfile << "strafeRight called" << endl;
        while (canGoForward() && (strafeRightToAlignWithWall() || !canGoLeft()))
        {
          updateDists();
          strafeRight();
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
