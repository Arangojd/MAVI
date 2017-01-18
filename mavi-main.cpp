/*
 * mavi-main.cpp
 * -------------
 *
 * This is the entry point for the MAVI program.
 */

#include <ao/ao.h>
#include <wiringPi.h>
#include <pthread.h>

#include "mavi-processing.hpp"
#include "mavi-feedback.hpp"
#include "mavi-states.hpp"

int main(int argc, char ** argv)
{
	// Initialization

	wiringPiSetupGpio();
	ao_initialize();

	pthread_t processThread, feedbackThread;
	pthread_create( &processThread, NULL, maviSenseAndProcess, NULL);
	pthread_create(&feedbackThread, NULL, maviProvideFeedback, NULL);

	// Main Loop
	while (1)
	{
		// ...
	}
}
