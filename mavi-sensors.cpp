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
#include <cstdarg>

#include "mavi-sensors.hpp"
#include "mavi-pins.hpp"
#include "mavi-mcp3008.hpp"
#include "interpolate.hpp"

MaviSensorFilter
	maviIRFilter(35000, 20, 3, MAVI_SENSOR_IRS, MAVI_SENSOR_IRM, MAVI_SENSOR_IRL),
	maviUSFilter(200000, 4, 2, MAVI_SENSOR_USL, MAVI_SENSOR_USR);

void maviStartAllFilters(void)
{
	maviIRFilter.startFiltering();
	maviUSFilter.startFiltering();
}

void maviStopAllFilters(void)
{
	maviIRFilter.stopFiltering();
	maviUSFilter.stopFiltering();
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
	#define takeSamples(k)														\
	{																			\
		for (j = 0; j < filter->numSensors; j++)								\
			currentReading[j] = maviPollSensor(filter->sensors[j]);				\
																				\
		pthread_rwlock_wrlock(&filter->sumLock);								\
																				\
		for (j = 0; j < filter->numSensors; j++)								\
			filter->sampleSums[j] += currentReading[j] - filter->buffer[j][k];	\
																				\
		pthread_rwlock_unlock(&filter->sumLock);								\
																				\
		for (j = 0; j < filter->numSensors; j++)								\
			filter->buffer[j][k] = currentReading[j];							\
	}

	#define waitForNext()									\
	{														\
		nextSample += filter->samplePeriod;					\
		if (nextSample - micros() <= filter->samplePeriod)	\
			delayMicroseconds(nextSample - micros());		\
	}

	MaviSensorFilter *filter = (MaviSensorFilter*)arg;
	int i, j;
	unsigned int nextSample = micros();
	double currentReading[filter->numSensors];

	for (i = 0; i < filter->bufferSize - 1 && filter->running; i++)
	{
		takeSamples(i);
		waitForNext();
	}

	if (filter->running)
	{
		takeSamples(i);
		filter->bufferFull = true;
	}

	pthread_cond_broadcast(&filter->fullBufferCond);
	waitForNext();

	for (i = 0; filter->running; i = (i+1) % filter->bufferSize)
	{
		takeSamples(i);
		waitForNext();
	}

	#undef takeSample
	#undef waitForNext

	return NULL;
}

MaviSensorFilter::MaviSensorFilter(unsigned int period, int bsize, int n, ...)
{
	int i, j;
	va_list args;
	va_start(args, n);

	this->numSensors     = n;
	this->sensors        = new MaviSensorID[n];
	this->samplePeriod   = period;
	this->bufferSize     = bsize;
	this->buffers        = new double[n][bsize];
	this->sampleSums     = new double[n];
	this->running        = false;
	this->bufferFull     = false;
	this->sumLock        = PTHREAD_RWLOCK_INITIALIZER;
	this->fullBufferLock = PTHREAD_MUTEX_INITIALIZER;
	this->fullBufferCond = PTHREAD_COND_INITIALIZER;

	for (i = 0; i < n; i++)
	{
		this->sensors[i] = va_arg(args, MaviSensorID);
		this->sampleSums[i] = 0.0;

		for (j = 0; j < bsize; j++)
			this->buffers[i][j] = 0.0;
	}

	va_end(args);
}

MaviSensorFilter::~MaviSensorFilter(void)
{
	this->running = false;
	pthread_join(this->thread, NULL);
	delete[] this->buffers;
	delete[] this->sensors;
	delete[] this->sampleSums;
	pthread_rwlock_destroy(&this->sumLock);
	pthread_mutex_destroy(&this->fullBufferLock);
	pthread_cond_destroy(&this->fullBufferCond);
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
		delete[] this->buffers;
		this->buffers = new double[this->numSensors][this->bufferSize];
	}

	return this->bufferSize;
}

void MaviSensorFilter::startFiltering(void)
{
	int i, j;

	for (i = 0; i < this->numSensors; i++)
	{
		for (j = 0; j < this->bufferSize; j++)
			this->buffers[i][j] = 0.0;

		this->sampleSums[i] = 0.0;
	}

	this->running = true;
	this->bufferFull = false;

	pthread_create(&this->thread, NULL, filterRoutine, (void*)this);
}

void MaviSensorFilter::stopFiltering(void)
{
	this->running = false;
	//~ pthread_join(this->thread, NULL);
}

double MaviSensorFilter::poll(MaviSensorID sid)
{
	int sind;
	double v;

	for (sind = 0; sind < this->numSensors; sind++)
		if (this->sensors[sind] == sid) break;

	if (sind == this->numSensors) return MAVI_INVALID_SENSOR_ID;

	if (this->running && !this->bufferFull)
	{
		pthread_mutex_lock(&this->fullBufferLock);
		pthread_cond_wait(&this->fullBufferCond, &this->fullBufferLock);
		pthread_mutex_unlock(&this->fullBufferLock);
	}

	pthread_rwlock_rdlock(&this->sumLock);
	v = this->sampleSums[sind];
	pthread_rwlock_unlock(&this->sumLock);

	return v / this->bufferSize;
}
