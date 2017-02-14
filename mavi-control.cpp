/*
 * mavi-control.cpp
 * ----------------
 *
 * ...
 */

#include <wiringPi.h>
#include <wiringPiI2C.h>
//~ #include <ao/ao.h>
#include <pthread.h>

#include "mavi-control.hpp"
#include "mavi-analysis.hpp"
#include "mavi-feedback.hpp"
#include "mavi-buttons.hpp"
#include "mavi-pins.hpp"
#include "mavi-state.hpp"

void maviInit(pthread_t *saThread, pthread_t *fbThread)
{
	#ifdef MAVI_PINTYPE_BCM
		wiringPiSetupGpio();
	#else
		wiringPiSetup();
	#endif

	adc = wiringPiI2CSetup(0x22);

	// MAVI is an HRT system; shift this process to the maximum possible priority.
	piHiPri(99);

	//~ pinMode(MAVI_DPIN_VL, OUTPUT);
	//~ pinMode(MAVI_DPIN_VL, OUTPUT);
	//~ pinMode(MAVI_DPIN_VL, OUTPUT);

	pinMode(MAVI_DPIN_USL_TRIG, OUTPUT);
	pinMode(MAVI_DPIN_USR_TRIG, OUTPUT);
	pinMode(MAVI_DPIN_USL_ECHO,  INPUT);
	pinMode(MAVI_DPIN_USR_ECHO,  INPUT);

	//~ wiringPiISR(MAVI_DPIN_POWER, INT_EDGE_FALLING, maviPowerButtonPressed);
	//~ wiringPiISR(MAVI_DPIN_PAUSE, INT_EDGE_FALLING, maviPauseButtonPressed);
	//~ wiringPiISR(MAVI_DPIN_CALIB, INT_EDGE_FALLING, maviCalibButtonPressed);

	//~ ao_initialize();

	// TODO: Load audio resources

	//~ pthread_create(saThread, NULL, maviSenseAndAnalyze, NULL);
	//~ pthread_create(fbThread, NULL, maviProvideFeedback, NULL);

	maviState = MAVI_STATE_RUNNING;
}

void maviShutdown(pthread_t *saThread, pthread_t *fbThread)
{
	maviState = MAVI_STATE_SHUTDOWN;

	//~ pthread_join(saThread, NULL);
	//~ pthread_join(fbThread, NULL);

	// TODO: Other teardown code here

	exit(0);
}
