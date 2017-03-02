/*
 * mavi-sensors.hpp
 * ----------------
 *
 * ...
 */

#ifndef MAVI_SENSORS_HPP
#define MAVI_SENSORS_HPP

#include <cfloat>
#include <pthread.h>

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

const unsigned int MAVI_US_TRIG_TIMEOUT = 5000;  // us
const unsigned int MAVI_US_ECHO_TIMEOUT = 50000; // us

double maviPollSensor(MaviSensorID sensor);

class MaviSensorFilter
{
private:
	MaviSensorID sensor;
	unsigned int samplePeriod; // us
	int windowSize;
	double *window;
	double sampleSum;
	pthread_t thread;
	pthread_rwlock_t lock;
	bool running;

	void *filterRoutine(void*);

public:
	MaviSensorFilter(MaviSensorID s, unsigned int per, int sz);
	~MaviSensorFilter(void);

	void startFiltering(void);
	void stopFiltering(void);
	double poll(void);
};

#endif
