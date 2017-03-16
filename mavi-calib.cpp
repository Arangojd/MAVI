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

	int i, sample_count = 10;
	double irSDist = 0, irMDist = 0, irLDist = 0;

	cout << "Calibration Started" << endl;
	maviAudioPlay(MAVI_AUDIO_CALIB_STARTED);

	for (i=0; i < sample_count; i++)
	{
		irSDist += maviIRSFilter.poll();
		irMDist += maviIRMFilter.poll();
		irLDist += maviIRLFilter.poll();
	}

	irSDist = irSDist/sample_count;
	irMDist = irMDist/sample_count;
	irLDist = irLDist/sample_count;

	if (abs(refDistIRS - irSDist) > 4 * MAVI_ERROR_MARGIN_IRS) calibFailed();
	if (abs(refDistIRM - irMDist) > 6 * MAVI_ERROR_MARGIN_IRM) calibFailed();
	if (abs(refDistIRL - irLDist) > 10 * MAVI_ERROR_MARGIN_IRL) calibFailed();

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
