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
	//cout << endl << "SIGINT received; ";

	switch (maviGetState())
	{
	case MAVI_STATE_PREINIT:
		//cout << "MAVI not yet initialized; ignoring";
		break;
	case MAVI_STATE_SHUTDOWN:
		//cout << "MAVI already shutting down; ignoring";
		break;
	default:
		maviSetState(MAVI_STATE_SHUTDOWN);
		break;
	}

	cout << endl;
}

void onUsr1(int s)
{
	//cout << endl << "SIGUSR1 received; ";

	switch (maviGetState())
	{
	case MAVI_STATE_RUNNING:
		maviSetState(MAVI_STATE_PAUSED);
		break;
	case MAVI_STATE_PAUSED:
		maviSetState(MAVI_STATE_RUNNING);
		break;
	default:
		//cout << "Invalid current state; ignoring";
		break;
	}

	cout << endl;
}

void onUsr2(int s)
{
	//~ cout << endl << "SIGUSR2 received; ";

	switch (maviGetState())
	{
	case MAVI_STATE_PAUSED:
		maviSetState(MAVI_STATE_CALIB);
		maviCalibration();
		maviSetState(MAVI_STATE_PAUSED);
		break;
	default:
		//cout << "Invalid current state; ignoring";
		break;
	}

	cout << endl;
}

void maviRegisterSignalHandlers(void)
{
	signal(SIGINT, onInterrupt);
	signal(SIGUSR1, onUsr1);
	signal(SIGUSR2, onUsr2);
}
