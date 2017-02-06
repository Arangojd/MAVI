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

volatile maviState_t maviState = MAVI_STATE_PREINIT;

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
