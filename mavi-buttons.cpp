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

static unsigned int timestamp_s = 0, timestamp_p = 0, timestamp_c = 0;

void maviPowerButtonPressed(void)
{
//	static unsigned int timestamp = 0;

	if (millis() - timestamp >= 1000)
	{
		timestamp = millis();
		cout << "Power button pressed; raising SIGINT" << endl;
		raise(SIGINT);
	}
}

void maviPauseButtonPressed(void)
{
//	static unsigned int timestamp = 0;

	if (millis() - timestamp >= 1000)
	{
		timestamp = millis();
		cout << "Pause button pressed; raising SIGUSR1" << endl;
		raise(SIGUSR1);
	}
}

void maviCalibButtonPressed(void)
{
//	static unsigned int timestamp = 0;

	if (millis() - timestamp >= 5000)
	{
		timestamp = millis();
		cout << "Calibration button pressed; raising SIGUSR2" << endl;
		raise(SIGUSR2);
	}
}

void maviRegisterButtonISRs(void)
{
	wiringPiISR(MAVI_DPIN_POWER, INT_EDGE_FALLING, maviPowerButtonPressed);
	wiringPiISR(MAVI_DPIN_CALIB, INT_EDGE_FALLING, maviCalibButtonPressed);
	wiringPiISR(MAVI_DPIN_PAUSE, INT_EDGE_FALLING, maviPauseButtonPressed);
}
