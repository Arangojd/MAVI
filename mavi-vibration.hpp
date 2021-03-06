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

const unsigned int MAVI_V_DELAY = 200;

void maviVibrate(MaviVibratorID vibrators, int v_count);

#endif
