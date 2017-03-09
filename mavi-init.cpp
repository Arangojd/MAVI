/*
 * mavi-init.cpp
 * -------------
 *
 * ...
 */

#include <wiringPi.h>

#include "mavi-init.hpp"

#include "mavi-buttons.hpp"
#include "mavi-pins.hpp"
#include "mavi-state.hpp"

#include "mavi-mcp3008.hpp"

void maviInit(void)
{
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

	// Initialize ADC
	maviMCP3008Init();

	// Register button ISRs

	wiringPiISR(MAVI_DPIN_POWER, INT_EDGE_FALLING, maviPowerButtonPressed);
	wiringPiISR(MAVI_DPIN_PAUSE, INT_EDGE_FALLING, maviPauseButtonPressed);
	wiringPiISR(MAVI_DPIN_CALIB, INT_EDGE_FALLING, maviCalibButtonPressed);

	maviSetState(MAVI_STATE_RUNNING);
}
