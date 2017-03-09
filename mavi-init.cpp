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
#include "mavi-signals.hpp"
#include "mavi-state.hpp"

#include "mavi-mcp3008.hpp"

void maviInit(void)
{
	maviRegisterSignalHandlers();

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

	maviMCP3008Init();
	maviRegisterButtonISRs();

	maviSetState(MAVI_STATE_RUNNING);
}
