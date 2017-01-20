/*
 * mavi-control.cpp
 * ----------------
 *
 * ...
 */

#include <wiringPi.h>
#include <ao/ao.h>
#include <pthread.h>

#include "mavi-control.hpp"
#include "mavi-analysis.hpp"
#include "mavi-feedback.hpp"

void maviInit(maviState *state, pthread_t *spThread, pthread_t *fbThread)
{
	wiringPiSetupGpio();
	ao_initialize();
	pthread_create(spThread, NULL, maviSenseAndAnalyze, (void*)state);
	pthread_create(fbThread, NULL, maviProvideFeedback, (void*)state);

	*state = MAVI_STATE_RUNNING;
}

void maviPause(maviState *state)
{
	*state = MAVI_STATE_PAUSED;
}

void maviCalib(maviState *state)
{
	*state = MAVI_STATE_CALIB;
}

void maviShutdown(maviState *state)
{
	*state = MAVI_STATE_SHUTDOWN;

	// TODO
}
