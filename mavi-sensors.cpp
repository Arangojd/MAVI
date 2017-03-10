/*
 * mavi-sensors.cpp
 * ----------------
 *
 * ...
 */

#include <pthread.h>
#include <wiringPi.h>
#include <cmath>

#include "mavi-sensors.hpp"
#include "mavi-pins.hpp"
#include "mavi-mcp3008.hpp"
#include "interpolate.hpp"

MaviSensorFilter
	maviIRSFilter(MAVI_SENSOR_IRS, 20000, 20),
	maviIRMFilter(MAVI_SENSOR_IRM, 35000, 20),
	maviIRLFilter(MAVI_SENSOR_IRL, 35000, 20),
	maviUSLFilter(MAVI_SENSOR_USL, 50000, 20),
	maviUSRFilter(MAVI_SENSOR_USR, 50000, 20);

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
	static pthread_mutex_t
		mut_usl = PTHREAD_MUTEX_INITIALIZER,
		mut_usr = PTHREAD_MUTEX_INITIALIZER;

	MaviDigitalPin
		trigPin = maviUSTrigPinMapping(sensor),
		echoPin = maviUSEchoPinMapping(sensor);

	if (trigPin == MAVI_DPIN_INVALID || echoPin == MAVI_DPIN_INVALID)
	{
		// This function was called with the wrong sensor type!
		return MAVI_INVALID_SENSOR_ID;
	}

	unsigned int st, et;

	pthread_mutex_t *mut = (sensor == MAVI_SENSOR_USL ? &mut_usl : &mut_usr);
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
	MaviSensorFilter *filter = (MaviSensorFilter*)arg;
	int i = 0;
	unsigned int nextSample = micros();

	while (filter->running)
	{
		nextSample += filter->samplePeriod;

		pthread_rwlock_wrlock(&filter->lock);
		{
			filter->sampleSum -= filter->window[i];
			filter->window[i] = maviPollSensor(filter->sensor);
			filter->sampleSum += filter->window[i];
		}
		pthread_rwlock_unlock(&filter->lock);

		i = (i+1) % filter->windowSize;

		if (nextSample - micros() <= filter->samplePeriod)
			delayMicroseconds(nextSample - micros());
	}

	return NULL;
}

MaviSensorFilter::MaviSensorFilter(MaviSensorID s, unsigned int per, int sz)
{
	this->sensor = s;
	this->samplePeriod = per;
	this->windowSize = sz;
	this->window = new double[this->windowSize];
	pthread_rwlock_init(&this->lock, NULL);
}

MaviSensorFilter::~MaviSensorFilter(void)
{
	delete[] this->window;
	pthread_rwlock_destroy(&this->lock);
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

int MaviSensorFilter::setWindowSize(int newSize)
{
	if (!this->running)
	{
		this->windowSize = newSize;
		delete[] this->window;
		this->window = new double[this->windowSize];
	}

	return this->windowSize;
}

void MaviSensorFilter::startFiltering(void)
{
	for (int i = 0; i < this->windowSize; i++)
		this->window[i] = 0.0;

	this->sampleSum = 0.0;
	this->running = true;

	pthread_create(&this->thread, NULL, filterRoutine, (void*)this);
}

void MaviSensorFilter::stopFiltering(void)
{
	this->running = false;
	pthread_join(this->thread, NULL);
}

double MaviSensorFilter::poll(void)
{
	double v;

	pthread_rwlock_rdlock(&this->lock);
	v = this->sampleSum;
	pthread_rwlock_unlock(&this->lock);

	return v / this->windowSize;
}
