/*
 * mavi-vibration.cpp
 * ------------------
 *
 * ...
 */

#include <wiringPi.h>
#include <pthread.h>

#include "mavi-vibration.hpp"
#include "mavi-pins.hpp"

const MaviDigitalPin pins[3] = {MAVI_DPIN_VR, MAVI_DPIN_VC, MAVI_DPIN_VL};
unsigned int durations[3];

void *vibrateFunc(void *v)
{
	int i = (int)v;

	digitalWrite(pins[i], 1);
	delay(durations[i]);
	digitalWrite(pins[i], 0);

	return NULL;
}

void maviVibrate(MaviVibratorID vibrators, unsigned int duration)
{
	static pthread_t vibThreads[3];

	for (int i = 0; i < 3; i++)
	{
		if (vibrators & (1 << i))
		{
			pthread_cancel(vibThreads[i]);
			durations[i] = duration;
			pthread_create(&vibThreads[i], NULL, vibrateFunc, (void*)i);
		}
	}
}
