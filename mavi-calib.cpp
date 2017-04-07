/*
 * mavi-calib.cpp
 * --------------
 *
 * ...
 */

#include <cmath>
#include <iostream>
#include <fstream>
#include <wiringPi.h>

#include "mavi-sensors.hpp"
#include "mavi-audio.hpp"
#include "mavi-calib.hpp"
#include "mavi-math.hpp"
#include "mavi-analysis.hpp"
#include "mavi-feedback.hpp"

using namespace std;

double
	MAVI_REF_DIST_IRS = MAVI_DEFAULT_REF_DIST_IRS,
	MAVI_REF_DIST_IRM = MAVI_DEFAULT_REF_DIST_IRM,
	MAVI_REF_DIST_IRL = MAVI_DEFAULT_REF_DIST_IRL,
	MAVI_ERROR_IRS 	  = MAVI_ERROR_PERC * MAVI_REF_DIST_IRS,
	MAVI_ERROR_IRM 	  = MAVI_ERROR_PERC * MAVI_REF_DIST_IRM,
	MAVI_ERROR_IRL 	  = MAVI_ERROR_PERC * MAVI_REF_DIST_IRL;

int maviCalibration(void)
{
	double
		irSDist = 0.0,
		irMDist = 0.0,
		irLDist = 0.0;

	maviSendFeedback(MAVI_FEEDBACK_CALIB_STARTED);

	maviStartAllFilters();

	for (int i = 0; i < MAVI_CALIB_SAMPLE_COUNT; i++)
	{
		irSDist += maviIRFilter.poll(MAVI_SENSOR_IRS);
		irMDist += maviIRFilter.poll(MAVI_SENSOR_IRM);
		irLDist += maviIRFilter.poll(MAVI_SENSOR_IRL);

		delay(MAVI_ANALYSIS_SAMPLE_PERIOD);
	}

	maviStopAllFilters();

	irSDist /= MAVI_CALIB_SAMPLE_COUNT;
	irMDist /= MAVI_CALIB_SAMPLE_COUNT;
	irLDist /= MAVI_CALIB_SAMPLE_COUNT;

	if (abs(MAVI_REF_DIST_IRS - irSDist) > MAVI_ERROR_IRS ||
	    abs(MAVI_REF_DIST_IRM - irMDist) > MAVI_ERROR_IRM ||
	    abs(MAVI_REF_DIST_IRL - irLDist) > MAVI_ERROR_IRL)
	{
		maviSendFeedback(MAVI_FEEDBACK_CALIB_FAILED);

	}
	else
	{
		MAVI_REF_DIST_IRS = irSDist;
		MAVI_REF_DIST_IRM = irMDist;
		MAVI_REF_DIST_IRL = irLDist;
		MAVI_ERROR_IRS = MAVI_ERROR_PERC * MAVI_REF_DIST_IRS;
		MAVI_ERROR_IRM = MAVI_ERROR_PERC * MAVI_REF_DIST_IRM;
		MAVI_ERROR_IRL = MAVI_ERROR_PERC * MAVI_REF_DIST_IRL;

		maviSaveCalibration();
		maviSendFeedback(MAVI_FEEDBACK_CALIB_SUCCESS);
	}

	cout <<
		"IRS   = " << irSDist    << " +/- " << MAVI_ERROR_IRS   << endl <<
		"IRM   = " << irMDist    << " +/- " << MAVI_ERROR_IRM   << endl <<
		"IRL   = " << irLDist    << " +/- " << MAVI_ERROR_IRL   << endl << endl;

	return 0;
}

void maviSaveCalibration(void)
{
	fstream calibfile("calib.dat", ios_base::out);
	calibfile << MAVI_REF_DIST_IRS << ' ' << MAVI_REF_DIST_IRM << ' ' << MAVI_REF_DIST_IRL << endl;
	calibfile.close();
}

void maviLoadCalibration(void)
{
	fstream calibfile("calib.dat", ios_base::in);
	if (calibfile.fail()) return;
	calibfile >> MAVI_REF_DIST_IRS >> MAVI_REF_DIST_IRM >> MAVI_REF_DIST_IRL;
	calibfile.close();

	MAVI_ERROR_IRS = MAVI_ERROR_PERC * MAVI_REF_DIST_IRS;
	MAVI_ERROR_IRM = MAVI_ERROR_PERC * MAVI_REF_DIST_IRM;
	MAVI_ERROR_IRL = MAVI_ERROR_PERC * MAVI_REF_DIST_IRL;
}
