#include "wallFollower.h"

using namespace WallFollower;
int main (int argc, char const *argv[])
{
  wallFollower f = wallFollower();
  cout << "constructor finished" << endl;
  f.goForward();
  cout << "goForward finished" << endl;
  f.holdTheFuckUp(10000000);
  cout << "holdTheFuckUp"

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
