/*
 * mavi-calib.cpp
 * --------------
 *
 * ...
 */

#include <cmath>
#include <iostream>
#include <fstream>

#include "mavi-sensors.hpp"
#include "mavi-audio.hpp"
#include "mavi-calib.hpp"
#include "mavi-math.hpp"
#include "mavi-analysis.hpp"

using namespace std;

double
	refDistUSL = MAVI_DEFAULT_REF_DIST_USL,
	refDistUSR = MAVI_DEFAULT_REF_DIST_USR,
	refDistIRS = MAVI_DEFAULT_REF_DIST_IRS,
	refDistIRM = MAVI_DEFAULT_REF_DIST_IRM,
	refDistIRL = MAVI_DEFAULT_REF_DIST_IRL,
	refSlope   = MAVI_DEFAULT_REF_SLOPE;

int maviCalibration(void)
{
	double
		irSDist = 0.0,
		irMDist = 0.0,
		irLDist = 0.0;

	cout << "Calibration Started" << endl;
	maviAudioPlay(MAVI_AUDIO_CALIB_STARTED);

	for (int i = 0; i < MAVI_CALIB_SAMPLE_COUNT; i++)
	{
		irSDist += maviPollSensor(MAVI_SENSOR_IRS);
		irMDist += maviPollSensor(MAVI_SENSOR_IRM);
		irLDist += maviPollSensor(MAVI_SENSOR_IRL);
	}

	irSDist /= MAVI_CALIB_SAMPLE_COUNT;
	irMDist /= MAVI_CALIB_SAMPLE_COUNT;
	irLDist /= MAVI_CALIB_SAMPLE_COUNT;

	if (abs(refDistIRS - irSDist) >  4 * MAVI_ERROR_MARGIN_IRS ||
	    abs(refDistIRM - irMDist) >  6 * MAVI_ERROR_MARGIN_IRM ||
	    abs(refDistIRL - irLDist) > 10 * MAVI_ERROR_MARGIN_IRL)
	{
		cout << "Calibration Failed" << endl << endl;
		maviAudioPlay(MAVI_AUDIO_CALIB_FAILED);
		return 1;
	}

	refDistIRS = irSDist;
	refDistIRM = irMDist;
	refDistIRL = irLDist;
	refSlope = maviGetRefSlope(refDistIRS, refDistIRM, refDistIRL);

	maviSaveCalibration();

	cout <<
		"IRS   = " << refDistIRS << endl <<
		"IRM   = " << refDistIRM << endl <<
		"IRL   = " << refDistIRL << endl <<
		"SLOPE = " << refSlope   << endl << endl <<
		"Calibration Successful"  << endl << endl;

	maviAudioPlay(MAVI_AUDIO_CALIB_SUCCESS);

	return 0;
}

void maviSaveCalibration(void)
{
	fstream calibfile("calib.dat", ios_base::out | ios_base::binary);
	calibfile << refDistIRS << refDistIRM << refDistIRL;
	calibfile.close();
}

void maviLoadCalibration(void)
{
	fstream calibfile("calib.dat", ios_base::in | ios_base::binary);
	if (calibfile.fail()) return;
	calibfile >> refDistIRS >> refDistIRM >> refDistIRL;
	calibfile.close();
	refSlope = maviGetRefSlope(refDistIRS, refDistIRM, refDistIRL);
}
