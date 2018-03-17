#include "wallFollower.h"
#include "wallFollower.cpp"
using namespace std;

namespace mazeNav
{
  mazeNav::mazeNav(){
  wallFollower();

  }

  mazeNav::spinWheelTest(int i, int j, int k){
    setPWM(0, 0, 150);
    setPWM(1, 0, 600);
    usleep(1000000);
    setPWM(0, 0, 150);
    setPWM(1, 0, 0);
    usleep(1000000);
    setPWM(0, 0, 0);
    setPWM(1, 0, 600);
    usleep(1000000);
    setPWM(0, 0, 150);
    setPWM(1, 0, 600);
    usleep(1000000);
    setPWM(0, 0, 0);
    setPWM(1, 0, 600);
    usleep(1000000);
    setPWM(0, 0, 0);
    setPWM(0, 0, 0);
  }



}
