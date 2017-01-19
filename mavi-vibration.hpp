/*
 * mavi-vibration.hpp
 * ------------------
 *
 * This header file contains the interface for the vibration function.
 * When called, the appropriate motor will vibrate for the specified duration and with the specified intensity.
 */

#ifndef MAVI_VIBRATION_HPP
#define MAVI_VIBRATION_HPP

enum maviVibratorID
{
	MAVI_VIB_L,
	MAVI_VIB_C,
	MAVI_VIB_R
};

void maviVibrate(maviVibratorID vibrator, double force, long duration);

#endif
