/*
 * mavi-control.hpp
 * ----------------
 *
 * ...
 */

#ifndef MAVI_CONTROL_HPP
#define MAVI_CONTROL_HPP

#include <pthread.h>

void maviInit    (pthread_t *saThread, pthread_t *fbThread);
void maviShutdown(pthread_t *saThread, pthread_t *fbThread);

#endif
