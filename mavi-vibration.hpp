/*
 * mavi-vibration.hpp
 * ------------------
 *
 * This header file contains the interface for the vibration function.
 * When called, the appropriate motor will vibrate for the specified duration and with the specified intensity.
 */

#ifndef MAVI_VIBRATION_HPP
#define MAVI_VIBRATION_HPP

typedef int MaviVibratorID;

const MaviVibratorID
	MAVI_VIB_L = 0b100,
	MAVI_VIB_C = 0b010,
	MAVI_VIB_R = 0b001;

void maviVibrate(MaviVibratorID vibrators, double force, unsigned int duration);

#endif
