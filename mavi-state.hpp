/*
 * mavi-state.hpp
 * --------------
 *
 * ...
 */

#ifndef MAVI_STATE_HPP
#define MAVI_STATE_HPP

enum maviState_t
{
	MAVI_STATE_PREINIT,
	MAVI_STATE_RUNNING,
	MAVI_STATE_PAUSED,
	MAVI_STATE_CALIB,
	MAVI_STATE_SHUTDOWN
};

extern volatile maviState_t maviState;

#endif
