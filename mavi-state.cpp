/*
 * mavi-state.cpp
 * --------------
 *
 * ...
 */

#include <pthread.h>
#include "mavi-state.hpp"

volatile MaviState currentState;
pthread_rwlock_t statelock;

void maviInitStateMonitor(void)
{
	currentState = MAVI_STATE_PREINIT;
	pthread_rwlock_init(&statelock, NULL);
}

void maviTeardownStateMonitor(void)
{
	pthread_rwlock_destroy(&statelock);
}

MaviState maviGetState(void)
{
	MaviState s;
	pthread_rwlock_rdlock(&statelock);
	s = currentState;
	pthread_rwlock_unlock(&statelock);
	return s;
}

void maviSetState(MaviState newState)
{
	pthread_rwlock_wrlock(&statelock);
	currentState = newState;
	pthread_rwlock_unlock(&statelock);
}
