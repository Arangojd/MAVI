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
#include "mavi-pins.hpp"
#include "mavi-state.hpp"

void maviInit(pthread_t *spThread, pthread_t *fbThread)
{
	#ifdef MAVI_PINTYPE_BCM
		wiringPiSetupGpio();
	#else
		wiringPiSetup();
	#endif

	ao_initialize();

	pthread_create(spThread, NULL, maviSenseAndAnalyze, NULL);
	pthread_create(fbThread, NULL, maviProvideFeedback, NULL);

	maviState = MAVI_STATE_RUNNING;
}

void maviPause(void)
{
	maviState = MAVI_STATE_PAUSED;
}

void maviCalib(void)
{
	maviState = MAVI_STATE_CALIB;
}

void maviShutdown(void)
{
	maviState = MAVI_STATE_SHUTDOWN;

	// TODO
}
