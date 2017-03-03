/*
 * mavi-sensors.cpp
 * ----------------
 *
 * ...
 */

#include <wiringPi.h>
#include "mavi-sensors.hpp"
#include "mavi-pins.hpp"
#include "mavi-mcp3008.hpp"
#include "interpolate.hpp"

MaviAnalogPin maviIRSensorPinMapping(MaviSensorID sensor)
{
	switch (sensor)
	{
		case MAVI_SENSOR_IRS: return MAVI_APIN_IRS;
		case MAVI_SENSOR_IRM: return MAVI_APIN_IRM;
		case MAVI_SENSOR_IRL: return MAVI_APIN_IRL;
		default:              return MAVI_APIN_INVALID;
	};
}

MaviDigitalPin maviUSTrigPinMapping(MaviSensorID sensor)
{
	switch (sensor)
	{
		case MAVI_SENSOR_USL: return MAVI_DPIN_USL_TRIG;
		case MAVI_SENSOR_USR: return MAVI_DPIN_USR_TRIG;
		default:              return MAVI_DPIN_INVALID;
	};
}

MaviDigitalPin maviUSEchoPinMapping(MaviSensorID sensor)
{
	switch (sensor)
	{
		case MAVI_SENSOR_USL: return MAVI_DPIN_USL_ECHO;
		case MAVI_SENSOR_USR: return MAVI_DPIN_USR_ECHO;
		default:              return MAVI_DPIN_INVALID;
	};
}

double maviPollSensorIR(MaviSensorID sensor)
{
	double voltage; // Sensor voltage reading (as interpreted by the ADC)

	MaviAnalogPin pin = maviIRSensorPinMapping(sensor);

	if (pin == MAVI_APIN_INVALID)
	{
		// This function was called with the wrong sensor type!
		return MAVI_INVALID_SENSOR_ID;
	}

	voltage = maviMCP3008Read(pin);

	if (sensor == MAVI_SENSOR_IRS)
		return voltage < 0.5 || voltage > 2.5 ? MAVI_BAD_SENSOR_READING : (1.0 / lerp(voltage, 0.5, 2.5, 1.0 / 150.0, 1.0 / 20.0));
	else
		return voltage < 1.4 || voltage > 2.5 ? MAVI_BAD_SENSOR_READING : (1.0 / lerp(voltage, 1.4, 2.5, 0.002, 0.01));
}

double maviPollSensorUS(MaviSensorID sensor)
{
	MaviDigitalPin
		trigPin = maviUSTrigPinMapping(sensor),
		echoPin = maviUSEchoPinMapping(sensor);

	if (trigPin == MAVI_DPIN_INVALID || echoPin == MAVI_DPIN_INVALID)
	{
		// This function was called with the wrong sensor type!
		return MAVI_INVALID_SENSOR_ID;
	}

	unsigned int st, et;

	// Send trigger pulse
	digitalWrite(trigPin, 1);
	delayMicroseconds(10);
	digitalWrite(trigPin, 0);
	et = micros();

	// Wait for sensor to process trigger
	while (!digitalRead(echoPin) && micros() - et < MAVI_US_TRIG_TIMEOUT);
	st = micros();

	if (st - et >= MAVI_US_TRIG_TIMEOUT) return MAVI_BAD_SENSOR_READING;

	// Wait for sensor to receive echo
	while (digitalRead(echoPin) && micros() - st < MAVI_US_ECHO_TIMEOUT);
	et = micros();

	if (et - st >= MAVI_US_ECHO_TIMEOUT) return MAVI_BAD_SENSOR_READING;

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
	if (!this->running) this->windowSize = newSize;
	return this->windowSize;
}

void MaviSensorFilter::startFiltering(void)
{
	for (int i = 0; i < this->windowSize; i++)
		this->window[i] = 0.0;

	this->sampleSum = 0.0;
	this->running = true;

	pthread_create(&this->thread, NULL, this->filterRoutine, NULL);
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

void *MaviSensorFilter::filterRoutine(void *arg)
{
	int i = 0;
	unsigned int nextSample = micros();

	while (this->running)
	{
		nextSample += this->samplePeriod;

		pthread_rwlock_wrlock(&this->lock);
		{
			this->sampleSum -= this->window[i];
			this->window[i] = maviPollSensor(this->sensor);
			this->sampleSum += this->window[i];
		}
		pthread_rwlock_unlock(&this->lock);

		i = (i+1) % this->windowSize;
		delayMicroseconds(nextSample - micros());
	}

	return NULL;
}
