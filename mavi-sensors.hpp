/*
 * mavi-sensors.hpp
 * ----------------
 *
 * ...
 */

#ifndef MAVI_SENSORS_HPP
#define MAVI_SENSORS_HPP

enum MaviSensorID
{
	MAVI_SENSOR_IRS,
	MAVI_SENSOR_IRM,
	MAVI_SENSOR_IRL,
	MAVI_SENSOR_USL,
	MAVI_SENSOR_USR
};

const unsigned int MAVI_US_TRIG_TIMEOUT = 5000;  // us
const unsigned int MAVI_US_ECHO_TIMEOUT = 50000; // us

double maviPollSensor(MaviSensorID sensor);

#endif
