/*
 * mavi-main.cpp
 * -------------
 *
 * This is the entry point for the MAVI program.
 */

#include <wiringPi.h>
#include <csignal>

#include "mavi-state.hpp"
#include "mavi-calib.hpp"
#include "mavi-pins.hpp"
#include "mavi-sensors.hpp"
#include "mavi-adc.hpp"

using namespace std;

volatile MaviState_t maviState = MAVI_STATE_PREINIT;

int adc;

double
	refDistUSL = 100.0,
	refDistUSR = 100.0,
	refDistIRS = 80.0,
	refDistIRM = 100.0,
	refDistIRL = 150.0,
	refAngleIRS,
	refAngleIRM,
	refAngleIRL,
	refHeightBelt;

void onInterrupt(int s)
{
	cout << "SIGINT received; exiting" << endl;
	exit(0);
}

int main(int argc, char ** argv)
{
	// Initialization

	#ifdef MAVI_PINTYPE_BCM
		wiringPiSetupGpio();
	#else
		wiringPiSetup();
	#endif

	adc = wiringPiI2CSetup(0x22);

	// MAVI is an HRT system; shift this process to the maximum possible priority.
	piHiPri(99);

	pinMode(MAVI_DPIN_USL_TRIG, OUTPUT);
	pinMode(MAVI_DPIN_USR_TRIG, OUTPUT);
	pinMode(MAVI_DPIN_USL_ECHO,  INPUT);
	pinMode(MAVI_DPIN_USR_ECHO,  INPUT);

	signal(SIGINT, onInterrupt);

	maviState = MAVI_STATE_RUNNING;

	maviSenseAndAnalyze(NULL);

	return 0;
}
