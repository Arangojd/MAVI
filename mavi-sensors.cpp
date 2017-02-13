/*
 * mavi-sensors.cpp
 * ----------------
 *
 * ...
 */

#include <wiringPi.h>
#include "mavi-sensors.hpp"
#include "mavi-pins.hpp"
#include "mavi-adc.hpp"

#define MAVI_INVALID_SENSOR_ID -1.0

MaviAnalogPin maviIRSensorPinMapping(MaviSensorID sensor)
{
	switch (sensor)
	{
	case MAVI_SENSOR_IRS: return MAVI_PIN_IRS;
	case MAVI_SENSOR_IRM: return MAVI_PIN_IRM;
	case MAVI_SENSOR_IRL: return MAVI_PIN_IRL;
	default: return 0;
	};
}

MaviDigitalPin maviUSTrigPinMapping(MaviSensorID sensor)
{
	switch (sensor)
	{
	case MAVI_SENSOR_USL: return MAVI_PIN_USL_TRIG;
	case MAVI_SENSOR_USR: return MAVI_PIN_USR_TRIG;
	default: return 0;
	};
}

MaviDigitalPin maviUSEchoPinMapping(MaviSensorID sensor)
{
	switch (sensor)
	{
	case MAVI_SENSOR_USL: return MAVI_PIN_USL_ECHO;
	case MAVI_SENSOR_USR: return MAVI_PIN_USR_ECHO;
	default: return 0;
	};
}

double maviPollSensorShortIR(MaviSensorID sensor)
{
	if (sensor != MAVI_SENSOR_IRS)
	{
		// This function was called with the wrong sensor type!
		return MAVI_INVALID_SENSOR_ID;
	}

	int sig = maviADCRead(maviIRSensorPinMapping(sensor));
	double voltage = lerp((double)sig, 0.0, (double)(1 << 9), 0.0, 3.3); // Volts
	double distance = lerp(voltage, 0.0, 3.3, 20, 150); // Centimetres

	// FIXME

	return distance;
}

double maviPollSensorLongIR(MaviSensorID sensor)
{
	if (sensor != MAVI_SENSOR_IRM && sensor != MAVI_SENSOR_IRL)
	{
		// This function was called with the wrong sensor type!
		return MAVI_INVALID_SENSOR_ID;
	}

	int sig = maviADCRead(maviIRSensorPinMapping(sensor));
	double voltage = lerp((double)sig, 0.0, (double)(1 << 9), 0.0, 3.3); // Volts
	double distance = lerp(voltage, 0.0, 3.3, 100.0, 550.0); // Centimetres

	// FIXME

	return distance;
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

	// Wait for sensor to process trigger
	while (!digitalRead(echoPin));
	st = micros();

	// Wait for sensor to receive echo
	while (digitalRead(echoPin));
	et = micros();

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

// Simultaneously poll and store distance values from the
// short, medium, and long IR sensors, in that order.
void maviPollAllIR(double dist[3])
{
	static const MaviAnalogPin IR_PINS[3] = {MAVI_PIN_IRS, MAVI_PIN_IRM, MAVI_PIN_IRL};
	int sigs[3];

	maviADCReadAll(3, IR_PINS, sigs);

	// TODO
}
