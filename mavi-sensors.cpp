/*
 * mavi-sensors.cpp
 * ----------------
 *
 * ...
 */

#include <wiringPi.h>
#include <cfloat>
#include "mavi-sensors.hpp"
#include "mavi-pins.hpp"
#include "mavi-adc.hpp"
#include "interpolate.hpp"

#define MAVI_INVALID_SENSOR_ID  DBL_MIN
#define MAVI_BAD_SENSOR_READING DBL_MAX

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
	double
		adcsig  = 0.0, // (Filtered) output from the ADC (relevant bits only)
		voltage = 0.0; // Voltage corresponding to that output (V)

	MaviAnalogPin pin = maviIRSensorPinMapping(sensor);

	if (pin == MAVI_APIN_INVALID)
	{
		// This function was called with the wrong sensor type!
		return MAVI_INVALID_SENSOR_ID;
	}

	for (int i = 0; i < MAVI_IR_FILTER_WINDOW_SIZE; i++)
	{
		adcsig += (double)maviADCRead(pin);
		delayMicroseconds(MAVI_IR_FILTER_SAMPLE_PERIOD);
	}

	voltage = lerp(adcsig / MAVI_IR_FILTER_WINDOW_SIZE, 0.0, (double)(1 << 10 - 1), 0.0, 3.3);

	if (sensor == MAVI_SENSOR_IRS)
		return voltage < 0.5 || voltage > 2.5 ? MAVI_BAD_SENSOR_READING : (1.0 / lerp(voltage, 0.5, 2.5, 1.0 / 150.0, 1.0 / 20.0));
	else
		return voltage < 1.4 || voltage > 2.5 ? MAVI_BAD_SENSOR_READING : (1.0 / lerp(voltage, 1.4, 2.5, 0.002, 0.01));
}

double maviPollSensorUS(MaviSensorID sensor)
{
	if (sensor != MAVI_SENSOR_USL && sensor != MAVI_SENSOR_USR)
	{
		// This function was called with the wrong sensor type!
		return MAVI_INVALID_SENSOR_ID;
	}

	MaviDigitalPin
		trigPin = maviUSTrigPinMapping(sensor),
		echoPin = maviUSEchoPinMapping(sensor);

	unsigned int st, et;

	// Send trigger pulse
	digitalWrite(trigPin, 1);
	delayMicroseconds(10);
	digitalWrite(trigPin, 0);
	et = micros();

	// Wait for sensor to process trigger
	while (!digitalRead(echoPin) && micros() - et < MAVI_US_TRIG_TIMEOUT);
	st = micros();

	if (st - et < MAVI_US_TRIG_TIMEOUT) return MAVI_BAD_SENSOR_READING;

	// Wait for sensor to receive echo
	while (digitalRead(echoPin) && micros() - st < MAVI_US_ECHO_TIMEOUT);
	et = micros();

	if (et - st < MAVI_US_ECHO_TIMEOUT) return MAVI_BAD_SENSOR_READING;

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
	};
}
