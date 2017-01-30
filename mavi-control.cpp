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

	pinMode(MAVI_PIN_VL, OUTPUT);
	pinMode(MAVI_PIN_VL, OUTPUT);
	pinMode(MAVI_PIN_VL, OUTPUT);

	pinMode(MAVI_PIN_IRS, INPUT);
	pinMode(MAVI_PIN_IRM, INPUT);
	pinMode(MAVI_PIN_IRL, INPUT);

	pinMode(MAVI_PIN_USL_TRIG, OUTPUT);
	pinMode(MAVI_PIN_USR_TRIG, OUTPUT);
	pinMode(MAVI_PIN_USL_ECHO,  INPUT);
	pinMode(MAVI_PIN_USR_ECHO,  INPUT);

	pinMode(MAVI_PIN_POWER, INPUT);
	pinMode(MAVI_PIN_PAUSE, INPUT);
	pinMode(MAVI_PIN_CALIB, INPUT);

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
