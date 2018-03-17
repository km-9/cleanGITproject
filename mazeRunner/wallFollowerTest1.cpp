#include "wallFollower.h"

using namespace wallFollower;
int main (int argc, char const *argv[])
{
  wallFollower f;
  f.goForward();
  f.holdTheFuckUp(10000000);

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

  f->drv->stop();
  f->drv->stopMotor();

  RPlidarDriver::DisposeDriver(f->drv);

  return 0;
}
