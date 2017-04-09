/*
 * mavi-vibration.cpp
 * ------------------
 *
 * ...
 */

#include <wiringPi.h>
#include <pthread.h>

#include "mavi-vibration.hpp"
#include "mavi-feedback.hpp"
#include "mavi-pins.hpp"

const MaviDigitalPin pins[3] = {MAVI_DPIN_VR, MAVI_DPIN_VC, MAVI_DPIN_VL};
int v_counts[3];

void vibCleanupFunc(void *v)
{
	digitalWrite((int)v, 0);
}

void *vibrateFunc(void *v)
{
	int i, vi = (int)v;
	pthread_cleanup_push(vibCleanupFunc, (void*)(pins[vi]));

	for (i = 0; i < (v_counts[vi] - 1); i++)
	{
		digitalWrite(pins[vi], 1);
		delay((unsigned int)((MAVI_VIBRATION_DURATION - MAVI_V_DELAY * (v_counts[vi] - 1)) / v_counts[vi]);
		digitalWrite(pins[vi], 0);
		delay(MAVI_V_DELAY);
	}

	digitalWrite(pins[vi], 1);
	delay((unsigned int)((MAVI_VIBRATION_DURATION - MAVI_V_DELAY * (v_counts[vi] - 1)) / v_counts[vi]);
	digitalWrite(pins[vi], 0);

	pthread_cleanup_pop(0);
	return NULL;
}

void maviVibrate(MaviVibratorID vibrators, int v_count)
{
	static pthread_t vibThreads[3];

	for (int vi = 0; vi < 3; vi++)
	{
		if (vibrators & (1 << vi))
		{
			pthread_cancel(vibThreads[vi]);
			v_counts[vi] = v_count;
			pthread_create(&vibThreads[vi], NULL, vibrateFunc, (void*)vi);
		}
	}
}
