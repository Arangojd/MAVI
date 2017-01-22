/*
 * mavi-control.hpp
 * ----------------
 *
 * ...
 */

#ifndef MAVI_CONTROL_HPP
#define MAVI_CONTROL_HPP

#include <pthread.h>

void maviInit    (pthread_t *spThread, pthread_t *fbThread);
void maviPause   (void);
void maviCalib   (void);
void maviShutdown(void);

#endif
