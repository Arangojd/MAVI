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
	maviIRFilter(
		MAVI_IR_FILTER_PERIOD, MAVI_IR_FILTER_BUFSIZE, 3,
		MAVI_SENSOR_IRS, MAVI_SENSOR_IRM, MAVI_SENSOR_IRL),
	maviSRFilter(
		MAVI_SR_FILTER_PERIOD, MAVI_SR_FILTER_BUFSIZE, 1,
		MAVI_SENSOR_SR),
	maviUSLFilter(
		MAVI_US_FILTER_PERIOD, MAVI_US_FILTER_BUFSIZE, 2,
		MAVI_SENSOR_USLL, MAVI_SENSOR_USLR),
	maviUSUFilter(
		MAVI_US_FILTER_PERIOD, MAVI_US_FILTER_BUFSIZE, 2,
		MAVI_SENSOR_USUL, MAVI_SENSOR_USUR);

void maviSetSensorPinModes(void)
{
	pinMode(MAVI_DPIN_VL, OUTPUT);
	pinMode(MAVI_DPIN_VC, OUTPUT);
	pinMode(MAVI_DPIN_VR, OUTPUT);

	pinMode(MAVI_DPIN_USLL_TRIG, OUTPUT);
	pinMode(MAVI_DPIN_USLR_TRIG, OUTPUT);
	pinMode(MAVI_DPIN_USUL_TRIG, OUTPUT);
	pinMode(MAVI_DPIN_USUR_TRIG, OUTPUT);
	pinMode(MAVI_DPIN_USLL_ECHO,  INPUT);
	pinMode(MAVI_DPIN_USLR_ECHO,  INPUT);
	pinMode(MAVI_DPIN_USUL_ECHO,  INPUT);
	pinMode(MAVI_DPIN_USUR_ECHO,  INPUT);
}

void maviStartAllFilters(void)
{
	maviIRFilter.startFiltering();
	maviSRFilter.startFiltering();
	maviUSLFilter.startFiltering();
	maviUSUFilter.startFiltering();
}

void maviStopAllFilters(void)
{
	maviIRFilter.stopFiltering();
	maviSRFilter.stopFiltering();
	maviUSLFilter.stopFiltering();
	maviUSUFilter.stopFiltering();
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

MaviAnalogPin maviSRSensorPinMapping(MaviSensorID sensor)
{
	switch (sensor)
	{
		case MAVI_SENSOR_SR: return MAVI_APIN_SR;
		default:             return MAVI_APIN_INVALID;
	}
}

MaviDigitalPin maviUSTrigPinMapping(MaviSensorID sensor)
{
	switch (sensor)
	{
		case MAVI_SENSOR_USLL: return MAVI_DPIN_USLL_TRIG;
		case MAVI_SENSOR_USLR: return MAVI_DPIN_USLR_TRIG;
		case MAVI_SENSOR_USUL: return MAVI_DPIN_USUL_TRIG;
		case MAVI_SENSOR_USUR: return MAVI_DPIN_USUR_TRIG;
		default:               return MAVI_DPIN_INVALID;
	}
}

MaviDigitalPin maviUSEchoPinMapping(MaviSensorID sensor)
{
	switch (sensor)
	{
		case MAVI_SENSOR_USLL: return MAVI_DPIN_USLL_ECHO;
		case MAVI_SENSOR_USLR: return MAVI_DPIN_USLR_ECHO;
		case MAVI_SENSOR_USUL: return MAVI_DPIN_USUL_ECHO;
		case MAVI_SENSOR_USUR: return MAVI_DPIN_USUR_ECHO;
		default:               return MAVI_DPIN_INVALID;
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

double maviPollSensorSR(MaviSensorID sensor)
{
	MaviAnalogPin pin = maviSRSensorPinMapping(sensor);

	if (pin == MAVI_APIN_INVALID)
	{
		// This function was called with the wrong sensor type!
		return MAVI_INVALID_SENSOR_ID;
	}

	return 2.54 / 2 * maviMCP3008ReadRaw(pin);
}

double maviPollSensorUS(MaviSensorID sensor)
{
	static pthread_mutex_t
		mut_upper = PTHREAD_MUTEX_INITIALIZER,
		mut_lower = PTHREAD_MUTEX_INITIALIZER;

	MaviDigitalPin
		trigPin = maviUSTrigPinMapping(sensor),
		echoPin = maviUSEchoPinMapping(sensor);

	if (trigPin == MAVI_DPIN_INVALID || echoPin == MAVI_DPIN_INVALID)
	{
		// This function was called with the wrong sensor type!
		return MAVI_INVALID_SENSOR_ID;
	}

	pthread_mutex_t *mut;
	unsigned int st, et;

	switch (sensor)
	{
	case MAVI_SENSOR_USLL:
	case MAVI_SENSOR_USLR:
		mut = &mut_lower;
		break;
	default:
		mut = &mut_upper;
		break;
	}

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

	case MAVI_SENSOR_SR:
		return maviPollSensorSR(sensor);

	case MAVI_SENSOR_USLL:
	case MAVI_SENSOR_USLR:
	case MAVI_SENSOR_USUL:
	case MAVI_SENSOR_USUR:
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
		pthread_rwlock_wrlock(&filter->lock);									\
																				\
		for (j = 0; j < filter->numSensors; j++)								\
			filter->sampleSums[j] += currentReading[j] - filter->buffers[j][k];	\
																				\
		pthread_rwlock_unlock(&filter->lock);									\
																				\
		for (j = 0; j < filter->numSensors; j++)								\
			filter->buffers[j][k] = currentReading[j];							\
	}

	#define takeInitialSamples(k)								\
	for (j = 0; j < filter->numSensors; j++)					\
	{															\
		currentReading[j] = maviPollSensor(filter->sensors[j]);	\
		filter->sampleSums[j] += currentReading[j];				\
		filter->buffers[j][k] = currentReading[j];				\
	}

	#define waitForNext()									\
	if (filter->running)									\
	{														\
		nextSample += filter->samplePeriod;					\
		if (nextSample - micros() <= filter->samplePeriod)	\
			delayMicroseconds(nextSample - micros());		\
	}

	MaviSensorFilter *filter = (MaviSensorFilter*)arg;
	int i, j;
	unsigned int nextSample = micros();
	double currentReading[filter->numSensors];

	pthread_rwlock_wrlock(&filter->lock);

	for (i = 0; i < filter->bufferSize - 1 && filter->running; i++)
	{
		takeInitialSamples(i);
		waitForNext();
	}

	if (filter->running)
	{
		takeInitialSamples(i);
		filter->bufferFull = true;
	}

	pthread_rwlock_unlock(&filter->lock);
	waitForNext();

	for (i = 0; filter->running; i = (i+1) % filter->bufferSize)
	{
		takeSamples(i);
		waitForNext();
	}

	#undef takeSamples
	#undef takeInitialSamples
	#undef waitForNext

	return NULL;
}

MaviSensorFilter::MaviSensorFilter(unsigned int period, int bsize, int n, ...)
{
	va_list args;
	va_start(args, n);

	this->numSensors   = n;
	this->sensors      = new MaviSensorID[n];
	this->samplePeriod = period;
	this->bufferSize   = bsize;
	this->buffers      = new double*[n];
	this->sampleSums   = new double[n];
	this->running      = false;
	this->bufferFull   = false;
	this->lock         = PTHREAD_RWLOCK_INITIALIZER;

	for (int i = 0; i < n; i++)
	{
		this->sensors[i] = (MaviSensorID)va_arg(args, int);
		this->buffers[i] = new double[bsize];
	}

	va_end(args);
}

MaviSensorFilter::~MaviSensorFilter(void)
{
	this->running = false;
	pthread_join(this->thread, NULL);

	for (int i = 0; i < this->numSensors; i++)
		delete[] this->buffers[i];

	delete[] this->buffers;
	delete[] this->sensors;
	delete[] this->sampleSums;
	pthread_rwlock_destroy(&this->lock);
}

void MaviSensorFilter::startFiltering(void)
{
	if (this->running) return;

	int i, j;

	pthread_join(this->thread, NULL);

	for (i = 0; i < this->numSensors; i++)
	{
		for (j = 0; j < this->bufferSize; j++)
			this->buffers[i][j] = 0.0;

		this->sampleSums[i] = 0.0;
	}

	this->running    = true;
	this->bufferFull = false;

	pthread_create(&this->thread, NULL, filterRoutine, (void*)this);
}

void MaviSensorFilter::stopFiltering(void)
{
	this->running = false;
}

void MaviSensorFilter::restartFiltering(void)
{
	this->stopFiltering();
	this->startFiltering();
}

double MaviSensorFilter::poll(MaviSensorID sid)
{
	int sind;
	double sum, avg, minSane, maxSane;

	for (sind = 0; sind < this->numSensors; sind++)
		if (this->sensors[sind] == sid) break;

	if (sind == this->numSensors) return MAVI_INVALID_SENSOR_ID;

	switch (sid)
	{
	case MAVI_SENSOR_IRS:
		minSane = MAVI_IRS_MIN_SANE;
		maxSane = MAVI_IRS_MAX_SANE;
		break;

	case MAVI_SENSOR_IRM:
	case MAVI_SENSOR_IRL:
		minSane = MAVI_IRL_MIN_SANE;
		maxSane = MAVI_IRL_MAX_SANE;
		break;

	case MAVI_SENSOR_SR:
		minSane = MAVI_SR_MIN_SANE;
		maxSane = MAVI_SR_MAX_SANE;
		break;

	case MAVI_SENSOR_USLL:
	case MAVI_SENSOR_USLR:
	case MAVI_SENSOR_USUL:
	case MAVI_SENSOR_USUR:
		minSane = MAVI_US_MIN_SANE;
		maxSane = MAVI_US_MAX_SANE;
		break;
	}

	pthread_rwlock_rdlock(&this->lock);
	sum = this->sampleSums[sind];
	pthread_rwlock_unlock(&this->lock);
	avg = sum / this->bufferSize;

	if (avg < minSane || avg > maxSane)
		return MAVI_BAD_SENSOR_READING;
	else
		return avg;
}
