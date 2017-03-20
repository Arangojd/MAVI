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

const MaviDigitalPin pins[] = {MAVI_DPIN_VR, MAVI_DPIN_VC, MAVI_DPIN_VL};
unsigned int halfperiods[3];
unsigned int durations[3];

void maviVibrate(MaviVibratorID vibrators, double force, unsigned int duration)
{
	static pthread_t vibThreads[3];

	unsigned int hp = (unsigned int)(1.0 / force); // Numerator may need to be larger

	for (int i = 0; i < 3; i++)
	{
		if (vibrators & (1 << i))
		{
			pthread_cancel(vibThreads[i]);
			halfperiods[i] = hp;
			durations[i] = duration;
			pthread_create(&vibThreads[i], NULL, vibrateFunc, (void*)i);
		}
	}
}

void *vibrateFunc(void *v)
{
	int i = (int)v;
	unsigned int end_t = millis() + durations[i];

	while (millis() < end_t)
	{
		digitalWrite(pins[i], 1);
		delay(halfperiods[i]);
		digitalWrite(pins[i], 0);
		delay(halfperiods[i]);
	}

	return NULL;
}
