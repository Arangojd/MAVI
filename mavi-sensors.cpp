/*
 * mavi-sensors.cpp
 * ----------------
 *
 * ...
 */

#include <wiringPi.h>
#include "mavi-sensors.hpp"
#include "mavi-pins.hpp"

#define MAVI_INVALID_SENSOR_ID 0.0

maviPin maviSensorPinMapping(maviSensorID sensor)
{
	switch (sensor)
	{
	case MAVI_SENSOR_IRL: return MAVI_PIN_IRL;
	case MAVI_SENSOR_IRM: return MAVI_PIN_IRM;
	case MAVI_SENSOR_IRS: return MAVI_PIN_IRS;
	case MAVI_SENSOR_USL: return MAVI_PIN_USL;
	case MAVI_SENSOR_USR: return MAVI_PIN_USR;
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

	// TODO
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
