/*
 * mavi-main.cpp
 * -------------
 *
 * This is the entry point for the MAVI program.
 */

#include <ao/ao.h>
#include <wiringPi.h>
#include <pthread.h>

#include "mavi-control.hpp"
#include "mavi-state.hpp"
#include "mavi-calib.hpp"

volatile MaviState_t maviState = MAVI_STATE_PREINIT;

int adc;

double
	refDistUSL = 100.0,
	refDistUSR = 100.0,
	refDistIRS = 80.0,
	refDistIRM = 100.0,
	refDistIRL = 150.0;

int main(int argc, char ** argv)
{
	// Initialization

	pthread_t spThread, fbThread;
	maviInit(&spThread, &fbThread);

	// Main Loop
	while (maviState != MAVI_STATE_SHUTDOWN)
	{
		// TODO
	}

	maviShutdown(&spThread, &fbThread);

	return 0;
}
