/*
 * mavi-control.cpp
 * ----------------
 *
 * ...
 */

#include <wiringPi.h>
#include <pthread.h>

#include "mavi-control.hpp"
#include "mavi-analysis.hpp"
#include "mavi-feedback.hpp"
#include "mavi-buttons.hpp"
#include "mavi-pins.hpp"
#include "mavi-state.hpp"

void maviInit(pthread_t *saThread, pthread_t *fbThread)
{
	maviInitStateMonitor();

	#ifdef MAVI_PINTYPE_BCM
		wiringPiSetupGpio();
	#else
		wiringPiSetup();
	#endif

	// MAVI is an HRT system; shift this process to the maximum possible priority.
	piHiPri(99);

	// Set pin modes

	pinMode(MAVI_DPIN_VL, OUTPUT);
	pinMode(MAVI_DPIN_VC, OUTPUT);
	pinMode(MAVI_DPIN_VR, OUTPUT);

	pinMode(MAVI_DPIN_USL_TRIG, OUTPUT);
	pinMode(MAVI_DPIN_USR_TRIG, OUTPUT);
	pinMode(MAVI_DPIN_USL_ECHO,  INPUT);
	pinMode(MAVI_DPIN_USR_ECHO,  INPUT);

	// Register button ISRs
	// Or don't, because no buttons on the current prototype! :(

	//~ wiringPiISR(MAVI_DPIN_POWER, INT_EDGE_FALLING, maviPowerButtonPressed);
	//~ wiringPiISR(MAVI_DPIN_PAUSE, INT_EDGE_FALLING, maviPauseButtonPressed);
	//~ wiringPiISR(MAVI_DPIN_CALIB, INT_EDGE_FALLING, maviCalibButtonPressed);

	pthread_create(saThread, NULL, maviSenseAndAnalyze, NULL);
	pthread_create(fbThread, NULL, maviProvideFeedback, NULL);

	maviSetState(MAVI_STATE_RUNNING);
}

void maviShutdown(pthread_t *saThread, pthread_t *fbThread)
{
	maviSetState(MAVI_STATE_SHUTDOWN);

	pthread_join(saThread, NULL);
	pthread_join(fbThread, NULL);

	maviTeardownStateMonitor();
}
