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

  ofstream myfile;
  myfile.open ("lidar_log.txt");

while (true)
{
  for(int i = 0; i < 360; i++) {
    myfile << "Theta: " << i << f.getDists(i) << endl;
  }
  usleep(3000000);
}

  myfile.close();

  while(true)
  {
    f.updateDists();
    if(f.canGoForward() && !f.tooFarOnLeft() && !f.tooCloseOnLeft())
    {
      while(f.canGoForward() && !f.tooFarOnLeft() && !f.tooCloseOnLeft())
      {
        cout << "canGoForward && !toofarOnLeft && !tooCloseOnLeft" << endl;
        f.updateDists();
        f.goForward();
      }
    }
    else if(f.canGoForward() && f.tooFarOnLeft() && !f.tooCloseOnLeft())
    {
      while(f.canGoForward() && f.tooFarOnLeft() && !f.tooCloseOnLeft())
      {
        cout << "canGoForward && toofarOnLeft && !tooCloseOnLeft" << endl;
        f.updateDists();
        f.strafeLeft();
      }
    }
    else if(f.canGoForward() && !f.tooFarOnLeft() && f.tooCloseOnLeft())
    {
      while (f.canGoForward() && !f.tooFarOnLeft() && f.tooCloseOnLeft())
      {
        cout << "canGoForward && !toofarOnLeft && tooCloseOnLeft" << endl;
        f.updateDists();
        f.strafeRight();
      }
    }
    else if(!f.canGoForward() && f.canGoLeft())
    {
      cout << "canGoForward && canGoLeft" << endl;
      //might need to be looked at
      f.turnLeft(90);
    }
    else if(!f.canGoForward() && !f.canGoLeft() && f.canGoRight())
    {
      cout << "canGoForward && !canGoLeft && canGoRight" << endl;
      //f.turnRight(90);
    }
    else
    {
      cout << "180" << endl;
      //let's mke it a 180 turn
      //f.turnLeft(180);
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
      else if (f.tooCloseOnLeft())
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
