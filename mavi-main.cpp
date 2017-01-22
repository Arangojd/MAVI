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

#define ever (;;)

volatile maviState_t maviState = MAVI_STATE_PREINIT;

int main(int argc, char ** argv)
{
	// Initialization

	pthread_t spThread, fbThread;
	maviInit(&spThread, &fbThread);

	// Main Loop
	for ever
	{
		// TODO
	}
}
