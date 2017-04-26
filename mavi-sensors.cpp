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
	maviIRSFilter (MAVI_SENSOR_IRS,  MAVI_IR_FILTER_PERIOD, MAVI_IR_FILTER_BUFSIZE),
	maviIRMFilter (MAVI_SENSOR_IRM,  MAVI_IR_FILTER_PERIOD, MAVI_IR_FILTER_BUFSIZE),
	maviIRLFilter (MAVI_SENSOR_IRL,  MAVI_IR_FILTER_PERIOD, MAVI_IR_FILTER_BUFSIZE),
	maviSRFilter  (MAVI_SENSOR_SR,   MAVI_SR_FILTER_PERIOD, MAVI_SR_FILTER_BUFSIZE),
	maviUSLLFilter(MAVI_SENSOR_USLL, MAVI_US_FILTER_PERIOD, MAVI_US_FILTER_BUFSIZE),
	maviUSLRFilter(MAVI_SENSOR_USLR, MAVI_US_FILTER_PERIOD, MAVI_US_FILTER_BUFSIZE),
	maviUSULFilter(MAVI_SENSOR_USUL, MAVI_US_FILTER_PERIOD, MAVI_US_FILTER_BUFSIZE),
	maviUSURFilter(MAVI_SENSOR_USUR, MAVI_US_FILTER_PERIOD, MAVI_US_FILTER_BUFSIZE);

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
	maviIRSFilter.startFiltering();
	maviIRMFilter.startFiltering();
	maviIRLFilter.startFiltering();
	maviSRFilter.startFiltering();
	maviUSLLFilter.startFiltering();
	maviUSLRFilter.startFiltering();
	maviUSULFilter.startFiltering();
	maviUSURFilter.startFiltering();
}

void maviStopAllFilters(void)
{
	maviIRSFilter.stopFiltering();
	maviIRMFilter.stopFiltering();
	maviIRLFilter.stopFiltering();
	maviSRFilter.stopFiltering();
	maviUSLLFilter.stopFiltering();
	maviUSLRFilter.stopFiltering();
	maviUSULFilter.stopFiltering();
	maviUSURFilter.stopFiltering();
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
	static unsigned int
		lastEcho_upper = 0,
		lastEcho_lower = 0;

	MaviDigitalPin
		trigPin = maviUSTrigPinMapping(sensor),
		echoPin = maviUSEchoPinMapping(sensor);

	if (trigPin == MAVI_DPIN_INVALID || echoPin == MAVI_DPIN_INVALID)
	{
		// This function was called with the wrong sensor type!
		return MAVI_INVALID_SENSOR_ID;
	}

	pthread_mutex_t *mut;
	unsigned int st, et, *lastEcho;

	switch (sensor)
	{
	case MAVI_SENSOR_USLL:
	case MAVI_SENSOR_USLR:
		mut = &mut_lower;
		lastEcho = &lastEcho_lower;
		break;
	default:
		mut = &mut_upper;
		lastEcho = &lastEcho_upper;
		break;
	}

	pthread_mutex_lock(mut);

	if (micros() - *lastEcho < MAVI_US_TIME_BUFFER)
		delayMicroseconds(MAVI_US_TIME_BUFFER + *lastEcho - micros());

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

	if (et - st >= MAVI_US_ECHO_TIMEOUT)
	{
		pthread_mutex_unlock(mut);
		return MAVI_BAD_SENSOR_READING;
	}

	*lastEcho = et;
	pthread_mutex_unlock(mut);

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
	#define takeSample(k)											\
	{																\
		currentReading = maviPollSensor(filter->sensorID);			\
		pthread_rwlock_wrlock(&filter->lock);						\
		filter->sampleSum += currentReading - filter->buffer[k];	\
		pthread_rwlock_unlock(&filter->lock);						\
		filter->buffer[k] = currentReading;							\
	}

	#define takeInitialSample(k)							\
	{														\
		currentReading = maviPollSensor(filter->sensorID);	\
		filter->sampleSum += currentReading;				\
		filter->buffer[k] = currentReading;					\
	}

	#define waitForNext()									\
	if (filter->running)									\
	{														\
		nextSample += filter->samplePeriod;					\
		if (nextSample - micros() <= filter->samplePeriod)	\
			delayMicroseconds(nextSample - micros());		\
	}

	MaviSensorFilter *filter = (MaviSensorFilter*)arg;
	int i;
	unsigned int nextSample = micros();
	double currentReading;

	pthread_rwlock_wrlock(&filter->lock);

	for (i = 0; i < filter->bufferSize - 1 && filter->running; i++)
	{
		takeInitialSample(i);
		waitForNext();
	}

	if (filter->running)
	{
		takeInitialSample(i);
		filter->bufferFull = true;
	}

	pthread_rwlock_unlock(&filter->lock);
	waitForNext();

	for (i = 0; filter->running; i = (i+1) % filter->bufferSize)
	{
		takeSample(i);
		waitForNext();
	}

	#undef takeSample
	#undef takeInitialSample
	#undef waitForNext

	return NULL;
}

MaviSensorFilter::MaviSensorFilter(MaviSensorID sid, unsigned int speriod, int bsize)
{
	this->sensorID     = sid;
	this->samplePeriod = period;
	this->bufferSize   = bsize;
	this->buffer       = new double[n];
	this->running      = false;
	this->bufferFull   = false;
	this->lock         = PTHREAD_RWLOCK_INITIALIZER;
}

MaviSensorFilter::~MaviSensorFilter(void)
{
	this->running = false;
	pthread_join(this->thread, NULL);
	delete[] this->buffer;
	pthread_rwlock_destroy(&this->lock);
}

void MaviSensorFilter::startFiltering(void)
{
	if (this->running) return;

	pthread_join(this->thread, NULL);

	for (int i = 0; i < this->bufferSize; i++)
		this->buffer[i] = 0.0;

	this->sampleSum  = 0.0;
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

double MaviSensorFilter::poll(void)
{
	double sum, avg, minSane, maxSane;

	switch (this->sensorID)
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
	sum = this->sampleSum;
	pthread_rwlock_unlock(&this->lock);
	avg = sum / this->bufferSize;

	if (avg < minSane || avg > maxSane)
		return MAVI_BAD_SENSOR_READING;
	else
		return avg;
}
