/*
 * mavi-buttons.hpp
 * ----------------
 *
 * ...
 */

#ifndef MAVI_BUTTONS_HPP
#define MAVI_BUTTONS_HPP

enum maviButtonID
{
	MAVI_BUTTON_POWER,
	MAVI_BUTTON_PAUSE,
	MAVI_BUTTON_CALIB
};

void maviPowerButtonPressed(void);
void maviPauseButtonPressed(void);
void maviCalibButtonPressed(void);

#endif
