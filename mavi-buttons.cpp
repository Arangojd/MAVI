/*
 * mavi-buttons.cpp
 * ----------------
 *
 * ...
 */

#include "mavi-buttons.hpp"
#include "mavi-pins.hpp"
#include "mavi-state.hpp"

maviPin maviButtonPinMapping(maviButtonID button)
{
	switch (button)
	{
	case MAVI_BUTTON_POWER: return MAVI_PIN_POWER;
	case MAVI_BUTTON_PAUSE: return MAVI_PIN_PAUSE;
	case MAVI_BUTTON_CALIB: return MAVI_PIN_CALIB;
	};
}

void maviPowerButtonPressed(void)
{
	switch (maviState)
	{
	case MAVI_STATE_PREINIT:
	case MAVI_STATE_SHUTDOWN:
		break;
	default:
		maviState = MAVI_STATE_SHUTDOWN;
		break;
	}
}

void maviPauseButtonPressed(void)
{
	switch (maviState)
	{
	case MAVI_STATE_RUNNING:
		maviState = MAVI_STATE_PAUSED;
		break;
	case MAVI_STATE_PAUSED:
		maviState = MAVI_STATE_RUNNING;
		break;
	default:
		break;
	}
}

void maviCalibButtonPressed(void)
{
	switch (maviState)
	{
	case MAVI_STATE_RUNNING: // NOTE: Can we go directly from running to calibrating?
	case MAVI_STATE_PAUSED:
		maviState = MAVI_STATE_CALIB;
		break;
	default:
		break;
	}
}
