/*
 * mavi-sensors.hpp
 * ----------------
 *
 * ...
 */

#ifndef MAVI_SENSORS_HPP
#define MAVI_SENSORS_HPP

#include <cfloat>

#define MAVI_INVALID_SENSOR_ID  DBL_MIN
#define MAVI_BAD_SENSOR_READING DBL_MAX

enum MaviSensorID
{
	MAVI_SENSOR_IRS,
	MAVI_SENSOR_IRM,
	MAVI_SENSOR_IRL,
	MAVI_SENSOR_USL,
	MAVI_SENSOR_USR
};

const unsigned int MAVI_IR_FILTER_SAMPLE_PERIOD = 2; // us
const unsigned int MAVI_IR_FILTER_WINDOW_SIZE = 20;

const unsigned int MAVI_US_TRIG_TIMEOUT = 5000;  // us
const unsigned int MAVI_US_ECHO_TIMEOUT = 50000; // us

double maviPollSensor(MaviSensorID sensor);

#endif
