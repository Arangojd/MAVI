/*
 * mavi-buttons.cpp
 * ----------------
 *
 * ...
 */

#include "mavi-buttons.hpp"
#include "mavi-pins.hpp"
#include "mavi-state.hpp"

MaviDigitalPin maviButtonPinMapping(MaviButtonID button)
{
	switch (button)
	{
	case MAVI_BUTTON_POWER: return MAVI_DPIN_POWER;
	case MAVI_BUTTON_PAUSE: return MAVI_DPIN_PAUSE;
	case MAVI_BUTTON_CALIB: return MAVI_DPIN_CALIB;
	default:                return MAVI_DPIN_INVALID;
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
