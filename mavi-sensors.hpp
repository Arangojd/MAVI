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

//~ #define MAVI_SIMULTANEOUS_US

#define MAVI_INVALID_SENSOR_ID  DBL_MIN
#define MAVI_BAD_SENSOR_READING DBL_MAX

enum MaviSensorID
{
	MAVI_SENSOR_IRS,
	MAVI_SENSOR_IRM,
	MAVI_SENSOR_IRL,
	MAVI_SENSOR_USLL,
	MAVI_SENSOR_USLR,
	MAVI_SENSOR_USUL,
	MAVI_SENSOR_USUR
};

const unsigned int MAVI_IR_FILTER_PERIOD =  35000;
const unsigned int MAVI_US_FILTER_PERIOD = 200000;
const int MAVI_IR_FILTER_BUFSIZE = 10;
const int MAVI_US_FILTER_BUFSIZE =  2;

const unsigned int MAVI_US_TRIG_TIMEOUT =   5000; // us
const unsigned int MAVI_US_ECHO_TIMEOUT = 200000; // us

void maviSetSensorPinModes(void);
double maviPollSensor(MaviSensorID sensor);
void maviStartAllFilters(void);
void maviStopAllFilters(void);

struct MaviSensorFilter
{
	int numSensors;
	MaviSensorID *sensors;
	unsigned int samplePeriod; // us
	int bufferSize;
	double **buffers;
	double *sampleSums;
	bool running;
	bool bufferFull;
	pthread_t thread;
	pthread_rwlock_t lock;

	MaviSensorFilter(unsigned int period, int bsize, int n, ...);
	~MaviSensorFilter(void);

	inline unsigned int getSamplePeriod(void) { return this->samplePeriod; }
	inline          int getBufferSize(void)   { return this->bufferSize;   }

	void startFiltering(void);
	void stopFiltering(void);
	void restartFiltering(void);
	double poll(MaviSensorID sid);
};

extern MaviSensorFilter
	maviIRFilter,
	maviUSLFilter,
	maviUSUFilter;

#endif
