/*
 * mavi-init.cpp
 * -------------
 *
 * ...
 */

#include <wiringPi.h>

#include "mavi-init.hpp"
#include "mavi-calib.hpp"
#include "mavi-buttons.hpp"
#include "mavi-pins.hpp"
#include "mavi-sensors.hpp"
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

	// Load existing calibration data (if present)
	maviLoadCalibration();

	// MAVI is an HRT system; shift this process to the maximum possible priority.
	piHiPri(99);

	maviSetSensorPinModes();
	maviMCP3008Init();
	maviRegisterButtonISRs();

	maviSetState(MAVI_STATE_RUNNING);
}
