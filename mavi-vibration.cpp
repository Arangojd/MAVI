/*
 * mavi-vibration.cpp
 * ------------------
 *
 * ...
 */

#include <wiringPi.h>
#include "mavi-vibration.hpp"
#include "mavi-pins.hpp"

maviPin maviVibratorPinMapping(maviVibratorID vibrator)
{
	switch (vibrator)
	{
	case MAVI_VIB_C: return MAVI_PIN_VC;
	case MAVI_VIB_L: return MAVI_PIN_VL;
	case MAVI_VIB_R: return MAVI_PIN_VR;
	};
}

void maviVibrate(maviVibratorID vibrator, double force, long duration)
{
	// TODO
}
