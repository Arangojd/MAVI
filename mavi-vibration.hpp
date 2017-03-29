/*
 * mavi-vibration.hpp
 * ------------------
 *
 * This header file contains the interface maviVibrate(MaviVibratorID, unsigned int).
 * When called, the appropriate motors will vibrate for the specified duration.
 */

#ifndef MAVI_VIBRATION_HPP
#define MAVI_VIBRATION_HPP

typedef int MaviVibratorID;

const MaviVibratorID
	MAVI_VIB_L = 0b100,
	MAVI_VIB_C = 0b010,
	MAVI_VIB_R = 0b001;

const int MAVI_V_COUNT = 2;

const double
	MAVI_VSPEED_SY = 1.0,
	MAVI_VSPEED_HI = 0.90,
	MAVI_VSPEED_MD = 0.75,
	MAVI_VSPEED_LO = 0.60;

void maviVibrate(MaviVibratorID vibrators, unsigned int duration, double speed);

#endif
