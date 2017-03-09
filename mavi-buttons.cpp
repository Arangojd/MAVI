/*
 * mavi-buttons.cpp
 * ----------------
 *
 * ...
 */
#include <iostream>

#include "mavi-buttons.hpp"
#include "mavi-pins.hpp"
#include "mavi-state.hpp"
#include "mavi-audio.hpp"

using namespace std;

MaviDigitalPin maviButtonPinMapping(MaviButtonID button)
{
	switch (button)
	{
		case MAVI_BUTTON_POWER: return MAVI_DPIN_POWER;
		case MAVI_BUTTON_PAUSE: return MAVI_DPIN_PAUSE;
		case MAVI_BUTTON_CALIB: return MAVI_DPIN_CALIB;
		default:                return MAVI_DPIN_INVALID;
	}
}

void maviPowerButtonPressed(void)
{
	switch (maviGetState())
	{
	case MAVI_STATE_PREINIT:
	case MAVI_STATE_SHUTDOWN:
		break;
	default:
		cout << "Shutting Down" << endl;
		maviSetState(MAVI_STATE_SHUTDOWN);
		maviAudioPlay(MAVI_AUDIO_SYSTEM_SHUTDOWN);
		break;
	}
}

void maviPauseButtonPressed(void)
{
	switch (maviGetState())
	{
	case MAVI_STATE_RUNNING:
		cout << "Pausing" << endl;
		maviSetState(MAVI_STATE_PAUSED);
		maviAudioPlay(MAVI_AUDIO_SYSTEM_PAUSED);
		cout << "Paused" << endl << endl;
		break;
	case MAVI_STATE_PAUSED:
		cout << "Starting" << endl;
		maviSetState(MAVI_STATE_RUNNING);
		maviAudioPlay(MAVI_AUDIO_SYSTEM_READY);
		cout << "Ready" << endl << endl;
		break;
	default:
		break;
	}
}

void maviCalibButtonPressed(void)
{
	switch (maviGetState())
	{
	case MAVI_STATE_RUNNING: // NOTE: Can we go directly from running to calibrating?
	case MAVI_STATE_PAUSED:
		maviSetState(MAVI_STATE_CALIB);
		break;
	default:
		break;
	}
}
