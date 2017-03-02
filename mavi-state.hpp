/*
 * mavi-state.hpp
 * --------------
 *
 * ...
 */

#ifndef MAVI_STATE_HPP
#define MAVI_STATE_HPP

enum MaviState
{
	MAVI_STATE_PREINIT,
	MAVI_STATE_RUNNING,
	MAVI_STATE_PAUSED,
	MAVI_STATE_CALIB,
	MAVI_STATE_SHUTDOWN
};

void maviInitStateMonitor(void);
void maviTeardownStateMonitor(void);
MaviState maviGetState(void);
void maviSetState(MaviState newState);

#endif
