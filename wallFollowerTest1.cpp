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

void pause(double n) {
  usleep(1000000*n);
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
		f.setMotorSpeed(-10,20);
		pause(.5);
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

