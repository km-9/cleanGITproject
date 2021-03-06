#include "wallFollower.h"
#include "templateMatcher.h"
#include <stdlib.h>
#include <fstream>
#include "opencv2/opencv.hpp"
#include <wiringPi.h>
#include "serial_command.hpp"

#include <iostream>

using namespace templateMatch;
using namespace WallFollower;
using namespace std;
using namespace cv;

bool canLeft;
bool canRight;
bool canForward;

RNG rng(12345);


wallFollower f = wallFollower();

// ------------------------------
// talking to the arduino
// ------------------------------

int arduinoSerialPort;  // initialized below
const char ARDUINO_READ_SOUND = 's';
const char ARDUINO_READ_FLAME = 'f';

const int FLAME_FAR_LEFT = 4;
const int FLAME_MID_LEFT = 3;
const int FLAME_CENTER = 2;
const int FLAME_MID_RIGHT = 1;
const int FLAME_FAR_RIGHT = 0;


// ------------------------------
// wall following parameters
// ------------------------------

// desired distance from left wall (measured at 45 degree angle from robot)
const int desiredLeftDistance = 320;
const int desiredFrontDistance = 320;

// mean speed of the motors
const int forwardSpeed = 50;


void pause(double n) {
  usleep(1000000*n);
}


//returns true if the returned array is valid, false if not
bool readFlame(int flame[]) {
  sendArduinoCommand(arduinoSerialPort, ARDUINO_READ_FLAME);
  string response;
  response = readArduinoResponse(arduinoSerialPort);
  stringstream ss;
  ss << response;
  if (! (ss >> flame[0] >> flame[1] >> flame[2] >> flame[3] >> flame[4]) )
    return false;
  return true;
}

void orientToFlame()
{
  bool mid_is_max = false;
  int flame[5];
  do
  {
    if (readFlame(flame))
    {
      int max = 0;
      for (int i = 1; i < 5; i++)
      {
        if (flame[i] > flame[max])
        {
          max = i;
        }
      }

      if (max == 2)
      {
        mid_is_max = true;
      }
      else if (max < 2)
      {
        f.setMotorSpeed(20,-20);
        pause(0.3);
      }
      else
      {
        f.setMotorSpeed(-20,20);
        pause(.3);
      }
    }
  }
  while (!mid_is_max);
}

void showFlame() {
  int flame[5];
  while(true) {
    cout << readFlame(flame) << " ";
    for(int i=0; i < 5; i++) cout << flame[i] << " ";
    cout << endl;
  }
}


void waitForSound(void) {
  cout << "waiting for sound" << endl;
  sendArduinoCommand(arduinoSerialPort, ARDUINO_READ_SOUND);
  string response;
  while((response = readArduinoResponse(arduinoSerialPort))[0] != 'Y') {
    cout << "From Arduino: " << response;
    usleep(1000);
    sendArduinoCommand(arduinoSerialPort, ARDUINO_READ_SOUND);
  }

  cout << "we have tone" << endl;
}

void frontCaseHandler() {
  f.setMotorSpeed(0,0);
  pause(.5);
  f.setMotorSpeed(20, -20);
  pause(.6);
  f.setMotorSpeed(0,0);
}

void checkBumpSwitch() {
	if(!digitalRead(7)) {
		f.stop();
		pause(.5);
		f.setMotorSpeed(-10,-20);
		pause(.5);
    f.setMotorSpeed(10,-10);
    pause(.3);
    f.setMotorSpeed(20,20);
    pause(.3);
		f.stop();
	}
}

int main(int argc, char const *argv[]) {

  arduinoSerialPort = setupArduinoSerial();

  wiringPiSetup();
  pinMode(7, INPUT);

  showFlame();

  waitForSound();    // !!!!!!!!!!!!!!!!!!!

  while(true) {
    f.updateDists();
    checkBumpSwitch();
    int frontDistance = f.getDists(180);
   if(frontDistance > 0) {
 if(frontDistance < desiredFrontDistance) {
   frontCaseHandler();
 }
   }
    double leftDistance = f.avg(135,136);
    cout << leftDistance << endl;
    if (leftDistance > 0) {
      if (leftDistance < desiredLeftDistance)
	f.setMotorSpeed(forwardSpeed+40, forwardSpeed-40);
      else
	f.setMotorSpeed(forwardSpeed-40, forwardSpeed+40);
    }
  }
}
