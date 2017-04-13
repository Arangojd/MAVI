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
	MAVI_SENSOR_SR,
	MAVI_SENSOR_USLL,
	MAVI_SENSOR_USLR,
	MAVI_SENSOR_USUL,
	MAVI_SENSOR_USUR
};

const double // BRODY: Fill these out (values in cm)
	MAVI_IRS_MIN_SANE,
	MAVI_IRS_MAX_SANE,
	MAVI_IRL_MIN_SANE,
	MAVI_IRL_MAX_SANE,
	MAVI_SR_MIN_SANE,
	MAVI_SR_MAX_SANE,
	MAVI_US_MIN_SANE,
	MAVI_US_MAX_SANE;

const unsigned int
	MAVI_IR_FILTER_PERIOD =  35000,
	MAVI_SR_FILTER_PERIOD =  30000,
	MAVI_US_FILTER_PERIOD = 200000;
const int
	MAVI_IR_FILTER_BUFSIZE = 10,
	MAVI_SR_FILTER_BUFSIZE = 10,
	MAVI_US_FILTER_BUFSIZE =  2;

const unsigned int
	MAVI_US_TRIG_TIMEOUT =   5000, // us
	MAVI_US_ECHO_TIMEOUT = 200000; // us

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
	bool running, bufferFull;
	pthread_t thread;
	pthread_rwlock_t lock;

	MaviSensorFilter(unsigned int samplePeriod, int bufferSize, int n, ...);
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
	maviSRFilter,
	maviUSLFilter,
	maviUSUFilter;

#endif
