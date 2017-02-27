/*
 * mavi-main.cpp
 * -------------
 *
 * This is the entry point for the MAVI program.
 */

#include <wiringPi.h>
#include <cstdlib>
#include <csignal>
#include <iostream>

#include "mavi-control.hpp"
#include "mavi-state.hpp"
#include "mavi-calib.hpp"

using namespace std;

volatile MaviState_t maviState = MAVI_STATE_PREINIT;

int adc;

double
	refDistUSL = 100.0,
	refDistUSR = 100.0,
	refDistIRS = 80.0,
	refDistIRM = 100.0,
	refDistIRL = 150.0,
	refAngleIRS,
	refAngleIRM,
	refAngleIRL,
	refHeightBelt;

void onInterrupt(int s)
{
	cout << "SIGINT received; exiting" << endl;
	maviShutdown();
}

int main(int argc, char ** argv)
{
	signal(SIGINT, onInterrupt);

	pthread_t saThread, fbThread;
	maviInit(&saThread, &fbThread);

	return 0;
}
