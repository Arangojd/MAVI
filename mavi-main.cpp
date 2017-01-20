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
#include "mavi-processing.hpp"
#include "mavi-feedback.hpp"

int main(int argc, char ** argv)
{
	// Initialization

	maviState state = MAVI_STATE_PREINIT;
	pthread_t spThread, fbThread;

	maviInit(&state, &spThread, &fbThread);

	// Main Loop
	while (1)
	{
		// TODO
	}
}
