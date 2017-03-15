/*
 * mavi-sensors.cpp
 * ----------------
 *
 * ...
 */

#include <iostream>
using namespace std;

#include <pthread.h>
#include <wiringPi.h>
#include <cmath>

#include "mavi-sensors.hpp"
#include "mavi-pins.hpp"
#include "mavi-mcp3008.hpp"
#include "interpolate.hpp"

MaviSensorFilter
	maviIRSFilter(MAVI_SENSOR_IRS, 35000, 20),
	maviIRMFilter(MAVI_SENSOR_IRM, 35000, 20),
	maviIRLFilter(MAVI_SENSOR_IRL, 35000, 20),
	maviUSLFilter(MAVI_SENSOR_USL, 200000, 4),
	maviUSRFilter(MAVI_SENSOR_USR, 200000, 4);

void maviStartAllFilters(void)
{
	maviIRSFilter.startFiltering();
	maviIRMFilter.startFiltering();
	maviIRLFilter.startFiltering();
	maviUSLFilter.startFiltering();
	maviUSRFilter.startFiltering();
}

void maviStopAllFilters(void)
{
	maviIRSFilter.stopFiltering();
	maviIRMFilter.stopFiltering();
	maviIRLFilter.stopFiltering();
	maviUSLFilter.stopFiltering();
	maviUSRFilter.stopFiltering();
}

MaviAnalogPin maviIRSensorPinMapping(MaviSensorID sensor)
{
	switch (sensor)
	{
		case MAVI_SENSOR_IRS: return MAVI_APIN_IRS;
		case MAVI_SENSOR_IRM: return MAVI_APIN_IRM;
		case MAVI_SENSOR_IRL: return MAVI_APIN_IRL;
		default:              return MAVI_APIN_INVALID;
	}
}

MaviDigitalPin maviUSTrigPinMapping(MaviSensorID sensor)
{
	switch (sensor)
	{
		case MAVI_SENSOR_USL: return MAVI_DPIN_USL_TRIG;
		case MAVI_SENSOR_USR: return MAVI_DPIN_USR_TRIG;
		default:              return MAVI_DPIN_INVALID;
	}
}

MaviDigitalPin maviUSEchoPinMapping(MaviSensorID sensor)
{
	switch (sensor)
	{
		case MAVI_SENSOR_USL: return MAVI_DPIN_USL_ECHO;
		case MAVI_SENSOR_USR: return MAVI_DPIN_USR_ECHO;
		default:              return MAVI_DPIN_INVALID;
	}
}

double maviPollSensorIR(MaviSensorID sensor)
{
	MaviAnalogPin pin = maviIRSensorPinMapping(sensor);

	if (pin == MAVI_APIN_INVALID)
	{
		// This function was called with the wrong sensor type!
		return MAVI_INVALID_SENSOR_ID;
	}

	if (sensor == MAVI_SENSOR_IRS)
		return 9489.6 * 2.54 * pow(maviMCP3008ReadRaw(pin), -1.115);
	else
		return 495542705.42 * 2.54 * pow(maviMCP3008ReadRaw(pin), -2.620);

}

double maviPollSensorUS(MaviSensorID sensor)
{
	#ifdef MAVI_SIMULTANEOUS_US
		static pthread_mutex_t
			mut_usl = PTHREAD_MUTEX_INITIALIZER,
			mut_usr = PTHREAD_MUTEX_INITIALIZER;
		pthread_mutex_t *mut = (sensor == MAVI_SENSOR_USL ? &mut_usl : &mut_usr);
	#else
		static pthread_mutex_t mut_us = PTHREAD_MUTEX_INITIALIZER;
		pthread_mutex_t *mut = &mut_us;
	#endif

	MaviDigitalPin
		trigPin = maviUSTrigPinMapping(sensor),
		echoPin = maviUSEchoPinMapping(sensor);

	if (trigPin == MAVI_DPIN_INVALID || echoPin == MAVI_DPIN_INVALID)
	{
		// This function was called with the wrong sensor type!
		return MAVI_INVALID_SENSOR_ID;
	}

	unsigned int st, et;

	pthread_mutex_lock(mut);

	// Send trigger pulse
	digitalWrite(trigPin, 1);
	delayMicroseconds(10);
	digitalWrite(trigPin, 0);
	et = micros();

	// Wait for sensor to process trigger
	while (!digitalRead(echoPin) && micros() - et < MAVI_US_TRIG_TIMEOUT);
	st = micros();

	if (st - et >= MAVI_US_TRIG_TIMEOUT)
	{
		pthread_mutex_unlock(mut);
		return MAVI_BAD_SENSOR_READING;
	}

	// Wait for sensor to receive echo
	while (digitalRead(echoPin) && micros() - st < MAVI_US_ECHO_TIMEOUT);
	et = micros();
	pthread_mutex_unlock(mut);

	if (et - st >= MAVI_US_ECHO_TIMEOUT)
		return MAVI_BAD_SENSOR_READING;
	else
		// Speed of sound varies based on temperature, air pressure, and
		// humidity, but we'll assume it's 343 m/s, or 0.0343 cm/us
		return (et - st) * 0.01715;
}

double maviPollSensor(MaviSensorID sensor)
{
	switch (sensor)
	{
	case MAVI_SENSOR_IRL:
	case MAVI_SENSOR_IRM:
	case MAVI_SENSOR_IRS:
		return maviPollSensorIR(sensor);

	case MAVI_SENSOR_USL:
	case MAVI_SENSOR_USR:
		return maviPollSensorUS(sensor);

	default:
		return MAVI_INVALID_SENSOR_ID;
	}
}

void *filterRoutine(void *arg)
{
	#define takeSample(k)											\
	{																\
		currentReading = maviPollSensor(filter->sensor);			\
		pthread_rwlock_wrlock(&filter->sumLock);					\
		filter->sampleSum += currentReading - filter->buffer[k];	\
		pthread_rwlock_unlock(&filter->sumLock);					\
		filter->buffer[k] = currentReading;							\
	}

	#define waitForNext()									\
	{														\
		nextSample += filter->samplePeriod;					\
		if (nextSample - micros() <= filter->samplePeriod)	\
			delayMicroseconds(nextSample - micros());		\
	}

	MaviSensorFilter *filter = (MaviSensorFilter*)arg;
	int i = 0;
	unsigned int nextSample = micros();
	double currentReading;

	for (i = 0; i < filter->bufferSize - 1 && filter->running; i++)
	{
		takeSample(i);
		waitForNext();
	}

	if (filter->running)
	{
		takeSample(i);
		filter->bufferFull = true;
	}

	pthread_cond_broadcast(&filter->fullBufferCond);
	waitForNext();

	for (i = 0; filter->running; i = (i+1) % filter->bufferSize)
	{
		takeSample(i);
		waitForNext();
	}

	#undef takeSample
	#undef waitForNext

	return NULL;
}

MaviSensorFilter::MaviSensorFilter(MaviSensorID s, unsigned int per, int sz)
{
	this->sensor         = s;
	this->samplePeriod   = per;
	this->bufferSize     = sz;
	this->buffer         = new double[this->bufferSize];
	this->sampleSum      = 0.0;
	this->running        = false;
	this->bufferFull     = false;
	this->sumLock        = PTHREAD_RWLOCK_INITIALIZER;
	this->fullBufferLock = PTHREAD_MUTEX_INITIALIZER;
	this->fullBufferCond = PTHREAD_COND_INITIALIZER;
}

MaviSensorFilter::~MaviSensorFilter(void)
{
	this->running = false;
	pthread_join(this->thread, NULL);
	delete[] this->buffer;
	pthread_rwlock_destroy(&this->sumLock);
	pthread_mutex_destroy(&this->fullBufferLock);
	pthread_cond_destroy(&this->fullBufferCond);
}

MaviSensorID MaviSensorFilter::setSensor(MaviSensorID newSensor)
{
	if (!this->running) this->sensor = newSensor;
	return this->sensor;
}

unsigned int MaviSensorFilter::setSamplePeriod(unsigned int newPeriod)
{
	if (!this->running) this->samplePeriod = newPeriod;
	return this->samplePeriod;
}

int MaviSensorFilter::setBufferSize(int newSize)
{
	if (!this->running)
	{
		this->bufferSize = newSize;
		delete[] this->buffer;
		this->buffer = new double[this->bufferSize];
	}

	return this->bufferSize;
}

void MaviSensorFilter::startFiltering(void)
{
	for (int i = 0; i < this->bufferSize; i++)
		this->buffer[i] = 0.0;

	this->sampleSum = 0.0;
	this->running = true;
	this->bufferFull = false;

	pthread_create(&this->thread, NULL, filterRoutine, (void*)this);
}

void MaviSensorFilter::stopFiltering(void)
{
	this->running = false;
	//~ pthread_join(this->thread, NULL);
}

double MaviSensorFilter::poll(void)
{
	double v;

	if (this->running && !this->bufferFull)
	{
		pthread_mutex_lock(&this->fullBufferLock);
		pthread_cond_wait(&this->fullBufferCond, &this->fullBufferLock);
		pthread_mutex_unlock(&this->fullBufferLock);
	}

	pthread_rwlock_rdlock(&this->sumLock);
	v = this->sampleSum;
	pthread_rwlock_unlock(&this->sumLock);

	return v / this->bufferSize;
}
