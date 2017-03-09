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

void maviPowerButtonPressed(void)
{
	static unsigned int timestamp = 0;

	if (millis() - timestamp >= 100)
	{
		timestamp = millis();
		cout << "Power button pressed; raising SIGINT";
		raise(SIGINT);
	}
}

void maviPauseButtonPressed(void)
{
	static unsigned int timestamp = 0;

	if (millis() - timestamp >= 100)
	{
		timestamp = millis();
		cout << "Pause button pressed; raising SIGUSR1";
		raise(SIGUSR1);
	}
}

void maviCalibButtonPressed(void)
{
	static unsigned int timestamp = 0;

	if (millis() - timestamp >= 100)
	{
		timestamp = millis();
		cout << "Calibrate button pressed; raising SIGUSR2";
		raise(SIGUSR2);
	}
}

void maviRegisterButtonISRs(void)
{
	wiringPiISR(MAVI_DPIN_POWER, INT_EDGE_FALLING, maviPowerButtonPressed);
	wiringPiISR(MAVI_DPIN_PAUSE, INT_EDGE_FALLING, maviPauseButtonPressed);
	wiringPiISR(MAVI_DPIN_CALIB, INT_EDGE_FALLING, maviCalibButtonPressed);
}
