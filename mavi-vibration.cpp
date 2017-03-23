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
double speeds[3];

void *vibrateFunc(void *v)
{
	int vi = (int)v;

	for (int i = 0; i < 3; i++)
	{
		digitalWrite(pins[vi], 1);
		delay((unsigned int)(durations[vi] * speeds[vi]));
		digitalWrite(pins[vi], 0);
		delay((unsigned int)(durations[vi] * (1.0 - speeds[vi])));
	}

	return NULL;
}

void maviVibrate(MaviVibratorID vibrators, unsigned int duration, double speed)
{
	static pthread_t vibThreads[3];

	for (int vi = 0; id < 3; id++)
	{
		if (vibrators & (1 << vi))
		{
			pthread_cancel(vibThreads[vi]);
			durations[vi] = duration;
			speeds[vi] = speed;
			pthread_create(&vibThreads[vi], NULL, vibrateFunc, (void*)vi);
		}
	}
}
