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
void maviStartAllFilters(void);
void maviStopAllFilters(void);

struct MaviSensorFilter
{
	MaviSensorID sensor;
	unsigned int samplePeriod; // us
	int bufferSize;
	double *buffer;
	double sampleSum;
	bool running;
	bool bufferFull;
	pthread_t thread;
	pthread_rwlock_t sumLock;
	pthread_mutex_t  fullBufferLock;
	pthread_cond_t   fullBufferCond;

	MaviSensorFilter(MaviSensorID s, unsigned int per, int sz);
	~MaviSensorFilter(void);

	inline MaviSensorID getSensor(void)       { return this->sensor;       }
	inline unsigned int getSamplePeriod(void) { return this->samplePeriod; }
	inline          int getBufferSize(void)   { return this->bufferSize;   }

	MaviSensorID setSensor(MaviSensorID newSensor);
	unsigned int setSamplePeriod(unsigned int newPeriod);
	int setBufferSize(int newSize);

	void startFiltering(void);
	void stopFiltering(void);
	double poll(void);
};

extern MaviSensorFilter
	maviIRSFilter,
	maviIRMFilter,
	maviIRLFilter,
	maviUSLFilter,
	maviUSRFilter;

#endif
