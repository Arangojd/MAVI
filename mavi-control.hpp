/*
 * mavi-control.hpp
 * ----------------
 *
 * ...
 */

#ifndef MAVI_CONTROL_HPP
#define MAVI_CONTROL_HPP

#include <pthread.h>

enum maviState
{
	MAVI_STATE_PREINIT,
	MAVI_STATE_RUNNING,
	MAVI_STATE_PAUSED,
	MAVI_STATE_CALIB,
	MAVI_STATE_SHUTDOWN
};

void maviInit    (maviState *state, pthread_t *spThread, pthread_t *fbThread);
void maviPause   (maviState *state);
void maviCalib   (maviState *state);
void maviShutdown(maviState *state);

#endif
