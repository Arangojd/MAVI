/*
 * mavi-buttons.cpp
 * ----------------
 *
 * ...
 */

#include <iostream>
#include <csignal>
#include <wiringPi.h>

#include "mavi-buttons.hpp"
#include "mavi-pins.hpp"
#include "mavi-audio.hpp"

using namespace std;

void maviPowerButtonPressed(void) { raise(SIGINT);  }
void maviPauseButtonPressed(void) { raise(SIGUSR1); }
void maviCalibButtonPressed(void) { raise(SIGUSR2); }

void maviRegisterButtonISRs(void)
{
	wiringPiISR(MAVI_DPIN_POWER, INT_EDGE_FALLING, maviPowerButtonPressed);
	wiringPiISR(MAVI_DPIN_CALIB, INT_EDGE_FALLING, maviCalibButtonPressed);
	wiringPiISR(MAVI_DPIN_PAUSE, INT_EDGE_FALLING, maviPauseButtonPressed);
}
