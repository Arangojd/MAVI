/*
 * mavi-calib.cpp
 * --------------
 *
 * ...
 */

#include <cmath>
#include <iostream>

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
	#define calibFailed()								\
	{													\
		cout << "Calibration Failed" << endl << endl;	\
		maviAudioPlay(MAVI_AUDIO_CALIB_FAILED);			\
		return 1;										\
	}

	maviStartAllFilters();

	double irSDist, irMDist, irLDist;

	cout << "Calibration Started" << endl;
	maviAudioPlay(MAVI_AUDIO_CALIB_STARTED);

	irSDist = maviIRSFilter.poll(); if (abs(refDistIRS - irSDist) > 2 * MAVI_ERROR_MARGIN) calibFailed();
	irMDist = maviIRMFilter.poll(); if (abs(refDistIRM - irMDist) > 2 * MAVI_ERROR_MARGIN) calibFailed();
	irLDist = maviIRLFilter.poll(); if (abs(refDistIRL - irLDist) > 3 * MAVI_ERROR_MARGIN) calibFailed();

	refDistIRS = irSDist;
	refDistIRM = irMDist;
	refDistIRL = irLDist;
	refSlope = maviGetRefSlope(refDistIRS, refDistIRM, refDistIRL);

	cout << "IRS 	= 	" << refDistIRS << endl;
	cout << "IRM 	= 	" << refDistIRM << endl;
	cout << "IRL 	= 	" << refDistIRL << endl;
	cout << "SLOPE 	= 	" << refSlope << endl;
	cout << endl;

	maviStopAllFilters();

	cout << "Calibration Sucessful" << endl << endl;
	maviAudioPlay(MAVI_AUDIO_CALIB_SUCCESS);

	#undef calibFailed

	return 0;
}
