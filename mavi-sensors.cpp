/*
 * mavi-sensors.cpp
 * ----------------
 *
 * ...
 */

#include <wiringPi.h>
#include "mavi-sensors.hpp"
#include "mavi-pins.hpp"

#define MAVI_INVALID_SENSOR_ID -1.0

maviPin maviIRSensorPinMapping(maviSensorID sensor)
{
	switch (sensor)
	{
	case MAVI_SENSOR_IRL: return MAVI_PIN_IRL;
	case MAVI_SENSOR_IRM: return MAVI_PIN_IRM;
	case MAVI_SENSOR_IRS: return MAVI_PIN_IRS;
	default: return 0;
	};
}

maviPin maviUSTrigPinMapping(maviSensorID sensor)
{
	switch (sensor)
	{
	case MAVI_SENSOR_USL: return MAVI_PIN_USL_TRIG;
	case MAVI_SENSOR_USR: return MAVI_PIN_USR_TRIG;
	default: return 0;
	};
}

maviPin maviUSEchoPinMapping(maviSensorID sensor)
{
	switch (sensor)
	{
	case MAVI_SENSOR_USL: return MAVI_PIN_USL_ECHO;
	case MAVI_SENSOR_USR: return MAVI_PIN_USR_ECHO;
	default: return 0;
	};
}

double maviPollSensorShortIR(maviSensorID sensor)
{
	if (sensor != MAVI_SENSOR_IRS)
	{
		// This function was called with the wrong sensor type!
		return MAVI_INVALID_SENSOR_ID;
	}

	// TODO
}

double maviPollSensorLongIR(maviSensorID sensor)
{
	if (sensor != MAVI_SENSOR_IRM && sensor != MAVI_SENSOR_IRL)
	{
		// This function was called with the wrong sensor type!
		return MAVI_INVALID_SENSOR_ID;
	}

	// TODO
}

double maviPollSensorUS(maviSensorID sensor)
{
	if (sensor != MAVI_SENSOR_USL && sensor != MAVI_SENSOR_USR)
	{
		// This function was called with the wrong sensor type!
		return MAVI_INVALID_SENSOR_ID;
	}

	maviPin
		trigPin = maviUSTrigPinMapping(sensor),
		echoPin = maviUSEchoPinMapping(sensor);

	unsigned int startT, endT;

	// Send trigger pulse
	digitalWrite(trigPin, 1);
	delay(1); // ms
	digitalWrite(trigPin, 0);

	// Measure time to echo
	startT = micros();
	while (!digitalRead(echoPin)); // Busy-wait for echo signal
	endT = micros();

	// TODO: Detect time wrapping
	// micros() wraps every 71 minutes, so it's conceivable
	// (although unlikely) that we'll end up with startT > endT.

	// Speed of sound varies based on temperature, air pressure, and
	// humidity, but we'll assume it's 343 m/s, or 0.0343 cm/us
	return ((endT - startT) >> 1) * 0.0343;
}

double maviPollSensor(maviSensorID sensor)
{
	switch (sensor)
	{
	case MAVI_SENSOR_IRL:
	case MAVI_SENSOR_IRM:
		return maviPollSensorLongIR(sensor);

	case MAVI_SENSOR_IRS:
		return maviPollSensorShortIR(sensor);

	case MAVI_SENSOR_USL:
	case MAVI_SENSOR_USR:
		return maviPollSensorUS(sensor);

	default:
		return MAVI_INVALID_SENSOR_ID;
	};
}
