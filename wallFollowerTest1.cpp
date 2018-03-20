#include "wallFollower.h"
#include <stdlib.h>

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

  while(true)
  {
    f.updateDists();
    if(f.canGoForward() && !f.tooFarOnLeft() && !f.canGoLeft())
    {
      f.logfile << "canGoForward && !toofarOnLeft && !canGoLeft" << endl;
      f.logfile << "goForward called" << endl;
      while(f.canGoForward() && !f.tooFarOnLeft() && !f.canGoLeft())
      {
        f.updateDists();
        f.goForward();
      }
    }
    else if(f.canGoForward() && f.tooFarOnLeft() && !f.canGoLeft())
    {
      f.logfile << "canGoForward && toofarOnLeft && !canGoLeft" << endl;
      f.logfile << "strafeLeft called" << endl;
      while(f.canGoForward() && f.tooFarOnLeft() && !f.canGoLeft())
      {
        f.updateDists();
        f.strafeLeft();
      }
    }
    else if(f.canGoForward() && !f.tooFarOnLeft() && f.canGoLeft())
    {
      f.logfile << "canGoForward && !toofarOnLeft && canGoLeft" << endl;
      f.logfile << "strafeRight called" << endl;
      while (f.canGoForward() && !f.tooFarOnLeft() && f.canGoLeft())
      {
        f.updateDists();
        f.strafeRight();
      }
    }
    else if(!f.canGoForward() && f.canGoLeft())
    {
      f.logfile << "canGoForward && canGoLeft" << endl;
      //might need to be looked at
      f.turnLeft(90);
    }
    else if(!f.canGoForward() && !f.canGoLeft() && f.canGoRight())
    {
      f.logfile << "canGoForward && !canGoLeft && canGoRight" << endl;
      //f.turnRight(90);
    }
    else
    {
      f.logfile << "180" << endl;
      //let's mke it a 180 turn
      f.turnLeft(180);
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

  f.logfile.close();
  f.drv->stop();
  f.drv->stopMotor();

  rp::standalone::rplidar::RPlidarDriver::DisposeDriver(f.drv);

  return 0;
}
