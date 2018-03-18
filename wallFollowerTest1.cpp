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

  f.max_left_dist = 175;
  f.min_forward_dist = 175;

  while(true){
    cout << "strafing left now" << endl;
    f.strafeLeft();
    usleep (5000000);
    f.strafeRight();
  }

  while(true) {
    f.updateDists();
    if(f.canGoForward() && !f.tooFarOnLeft()) {
      while(f.canGoForward() && !f.tooFarOnLeft()){
      f.updateDists();
      f.goForward();
    }
    }
    else if(f.canGoForward() && f.tooFarOnLeft()) {
      while(f.canGoForward() && f.tooFarOnLeft()){
      f.updateDists();
      f.strafeLeft();
    }
    }
    else if(!f.canGoForward() && f.canGoLeft()) {
      f.turnLeft(90);
      if (f.canGoForward()){
        f.goForward();
      }
    }
    else if(!f.canGoForward() && !f.canGoLeft() && f.canGoRight()) {
      f.turnRight(90);
    }
    else {
      f.reverse();
      usleep(1000000);
      if (f.canGoLeft()){
        f.turnLeft(90);
      }
      else {
        f.turnRight(90);
      }
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
