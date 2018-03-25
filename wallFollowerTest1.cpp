#include "wallFollower.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>

using namespace WallFollower;
using namespace std;

bool canLeft;
bool canRight;
bool canForward;

int main (int argc, char const *argv[])
{
  cout << "in main" << endl;
  wallFollower f = wallFollower();
  //cout << "constructor finished" << endl;
  //f.goForward();
  //cout << "goForward finished" << endl;
  //f.holdTheFuckUp(10000000);
  //cout << "holdTheFuckUp" << endl;
// TODO f.startWallFollowingLeft();

while(true){
  f.updateDists();
  //Left Vals
  double leftAvg = 0;
  for (int i = 89; i < 93; i++){
    leftAvg = leftAvg + f.dists[i];
  }
  leftAvg = leftAvg/3;
  //Front Vals
  double frontAvg = 0;
  for (int i = 175; i < 185; i++){
    frontAvg = frontAvg + f.dists[i];
  }
  frontAvg = frontAvg/10;

  f.frontHandler(frontAvg, leftAvg);
  f.leftWallFollower(leftAvg);
}

//Closes thing to working
while (true){
    usleep(10000);
    f.updateDists();
    //check immediate left
    double avg = 0;
    for (int i = 89; i < 93; i++){
      avg = avg + f.dists[i];
    }
    avg = avg/3;
    while (f.dists[180] < 250){
      f.stop();
      f.updateDists();
      usleep(10000);
    }
    if (avg > 250){
      f.swayToLeft();
    }
    if (avg < 250){
      f.swayToRight();
    }
    if (avg == 250){
      f.goForward();
    }

    //check forward left
    for (int i = 93; i < 175; i++){
      if(f.dists[i] < 200){
        f.swayToRight();
        canLeft = false;
      }
      else{
        f.goForward();
      }
    }

    //check front
    for (int i = 175; i < 185; i ++){
      if(f.dists[i] < 300){
        f.stop();
        while (f.dists[i] < 300){
          //f.reverse();
          f.stop();
          f.updateDists();
        }
        if (canLeft){
          f.turnLeft(80);
        }
        else if (canRight){
        f.turnRight(80);
      }
        else
        f.turnRight(180);
      }
    }

    //check front-right
    for (int i = 185; i < 215; i++){
      if(f.dists[i] < 250){
        f.turnLeft(45);
        canRight = false;
        break;
      }
      else
      canRight = true;
    }

    //check forward right
    for (int i = 267; i < 215; i++){
      if(f.dists[i] < 200){
        f.strafeLeft();
        canRight = false;
      }
      else
      canRight = true;
    }

    //check immediate right
    for (int i = 267; i < 270; i ++){
      if(f.dists[i] < 200){
        f.strafeLeft();
        canRight = false;
      }
      else
      canRight = true;
    }
  }

  /*
  while (true){
    f.updateDists();

    for (int i = 178; i < 182; i ++){
      if(f.dists[i] < 200){
        f.stop();
        while (f.dists[i] < 250){
          f.reverse();
          f.updateDists();
        }
        if (f.dists[90] > 500){
          f.turnLeft(80);
        }
        if (f.dists[270]){
          f.turnRight(80);
        }
      }
    }

    for (int i = )

    for (int i = 90; i < 95; i++){
      if (f.dists[i] < 200 && f.dists[i+82] > 250){
        f.strafeRight();
        canLeft = false;
        break;
      }
      f.strafeLeftToAlignWithWall();
      canLeft = true;
    }

    for (int i = 95; i < 145; i++){
      if(f.dists[i] < 300 && f.dists[i] > 300){
        f.strafeRight();
        canLeft = false;
        break
      }
      f.strafeLeftToAlignWithWall();
      canLeft = true;
    }

    for (int i = 145; i < 178; i++){
      if(f.dists[i] < 250){
        f.turnRight(45);
        canLeft = false;
        break;
      }
      canLeft = true;
    }
  */

  /*while (true){
    f.strafeRight();
  }

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
