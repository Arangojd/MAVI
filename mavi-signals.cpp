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
		cout << "Beginning shutdown";
		maviSetState(MAVI_STATE_SHUTDOWN);
		break;
	}

	cout << endl;
}

void onUsr1(int s)
{
	cout << endl << "SIGUSR1 received; ";

	switch (maviGetState())
	{
	case MAVI_STATE_RUNNING:
		cout << "Pausing";
		maviSetState(MAVI_STATE_PAUSED);
		break;
	case MAVI_STATE_PAUSED:
		cout << "Unpausing";
		maviSetState(MAVI_STATE_RUNNING);
		break;
	default:
		cout << "Invalid current state; ignoring";
		break;
	}

	cout << endl;
}

void onUsr2(int s)
{
	cout << endl << "SIGUSR2 received; ";

	switch (maviState)
	{
	case MAVI_STATE_PAUSED:
		cout << "Beginning calibration";
		maviSetState(MAVI_STATE_CALIB);
		break;
	default:
		cout << "Invalid current state; ignoring";
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
