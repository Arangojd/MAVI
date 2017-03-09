/*
 * mavi-signals.cpp
 * ----------------
 *
 * ...
 */

#include <iostream>
#include <csignal>

#include "mavi-signals.hpp"
#include "mavi-state.hpp"
#include "mavi-calib.hpp"
#include "mavi-audio.hpp"

using namespace std;

void onInterrupt(int s)
{
	cout << endl << "SIGINT received; ";

	switch (maviGetState())
	{
	case MAVI_STATE_PREINIT:
		cout << "MAVI not yet initialized; ignoring";
		break;
	case MAVI_STATE_SHUTDOWN:
		cout << "MAVI already shutting down; ignoring";
		break;
	default:
		cout << "System Shutting Down";
		maviSetState(MAVI_STATE_SHUTDOWN);
		maviAudioPlay(MAVI_AUDIO_SYSTEM_SHUTDOWN);
		break;
	}

	cout << endl << endl;
}

void onUsr1(int s)
{
	cout << endl << "SIGUSR1 received; ";

	switch (maviGetState())
	{
	case MAVI_STATE_RUNNING:
		cout << "System Paused";
		maviSetState(MAVI_STATE_PAUSED);
		maviAudioPlay(MAVI_AUDIO_SYSTEM_PAUSED);
		break;
	case MAVI_STATE_PAUSED:
		cout << "System Ready";
		maviSetState(MAVI_STATE_RUNNING);
		maviAudioPlay(MAVI_AUDIO_SYSTEM_READY);
		break;
	default:
		cout << "Invalid current state; ignoring";
		break;
	}

	cout << endl << endl;
}

void onUsr2(int s)
{
	cout << endl << "SIGUSR2 received; ";

	switch (maviGetState())
	{
	case MAVI_STATE_PAUSED:
		cout << "Starting Calibration";
		maviSetState(MAVI_STATE_CALIB);
		maviAudioPlay(MAVI_AUDIO_CALIB_STARTED);
		if (maviCalibration()) {
			cout << "Calibration Failed";
			maviAudioPlay(MAVI_AUDIO_CALIB_FAILED);
		}
		else {
			cout << "Calibration Successful";
			maviAudioPlay(MAVI_AUDIO_CALIB_SUCCESS);
		}
		maviSetState(MAVI_STATE_PAUSED);
		maviAudioPlay(MAVI_AUDIO_SYSTEM_PAUSED);
		break;
	default:
		cout << "Invalid current state; ignoring";
		break;
	}

	cout << endl << endl;
}

void maviRegisterSignalHandlers(void)
{
	signal(SIGINT, onInterrupt);
	signal(SIGUSR1, onUsr1);
	signal(SIGUSR2, onUsr2);
}
