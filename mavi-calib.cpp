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

	double irSDist, irMDist, irLDist, usLDist, usRDist;

	cout << "Calibration Started" << endl;
	maviAudioPlay(MAVI_AUDIO_CALIB_STARTED);

	irSDist = maviIRSFilter.poll(); if (abs(refDistIRS - irSDist) > 2 * MAVI_ERROR_MARGIN) calibFailed();
	irMDist = maviIRMFilter.poll(); if (abs(refDistIRM - irMDist) > 2 * MAVI_ERROR_MARGIN) calibFailed();
	irLDist = maviIRLFilter.poll(); if (abs(refDistIRL - irLDist) > 2 * MAVI_ERROR_MARGIN) calibFailed();
	//~ usLDist = maviUSLFilter.poll(); if (abs(refDistUSL - usLDist) > 2 * MAVI_ERROR_MARGIN) calibFailed();
	//~ usRDist = maviUSRFilter.poll(); if (abs(refDistUSR - usRDist) > 2 * MAVI_ERROR_MARGIN) calibFailed();

	refDistIRS = irSDist;
	refDistIRM = irMDist;
	refDistIRL = irLDist;
	//~ refDistUSL = usLDist;
	//~ refDistUSR = usRDist;
	refSlope = maviGetRefSlope(refDistIRS, refDistIRM, refDistIRL);

	cout << "Calibration Sucessful" << endl << endl;
	maviAudioPlay(MAVI_AUDIO_CALIB_SUCCESS);

	cout << "IRS 	= 	" << refDistIRS << endl;
	cout << "IRM 	= 	" << refDistIRM << endl;
	cout << "IRL 	= 	" << refDistIRL << endl;
	cout << "USL 	= 	" << refDistUSL << endl;
	cout << "USR 	= 	" << refDistUSR << endl;
	cout << "SLOPE 	= 	" << refSlope << endl;

	maviStopAllFilters();

	#undef calibFailed

	return 0;
}
