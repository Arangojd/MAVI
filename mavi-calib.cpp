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
	refSlope = MAVI_DEFAULT_REF_SLOPE;

int maviCalibration(void)
{
	double irSDist, irMDist, irLDist, usLDist, usRDist;

	cout << "Calibration Started" << endl;
	maviAudioPlay(MAVI_AUDIO_CALIB_STARTED);

	irSDist = maviIRSFilter.poll();
	if (abs(refDistIRS - irSDist) > MAVI_ERROR_MARGIN) {
		cout << "Calibration Failed" << endl << endl;
		maviAudioPlay(MAVI_AUDIO_CALIB_FAILED);
		return 1;
	}

	irMDist = maviIRMFilter.poll();
	if (abs(refDistIRM - irMDist) > MAVI_ERROR_MARGIN) {
		cout << "Calibration Failed" << endl << endl;
		maviAudioPlay(MAVI_AUDIO_CALIB_FAILED);
		return 1;
	}

	irLDist = maviIRLFilter.poll();
	if (abs(refDistIRL - irLDist) > MAVI_ERROR_MARGIN) {
		cout << "Calibration Failed" << endl << endl;
		maviAudioPlay(MAVI_AUDIO_CALIB_FAILED);
		return 1;
	}

	usLDist = maviUSLFilter.poll();
	if (abs(refDistUSL - usLDist) > MAVI_ERROR_MARGIN) {
		cout << "Calibration Failed" << endl << endl;
		maviAudioPlay(MAVI_AUDIO_CALIB_FAILED);
		return 1;
	}

	usRDist = maviUSRFilter.poll();
	if (abs(refDistUSR - usRDist) > MAVI_ERROR_MARGIN) {
		cout << "Calibration Failed" << endl << endl;
		maviAudioPlay(MAVI_AUDIO_CALIB_FAILED);
		return 1;
	}

	refDistIRS = irSDist;
	refDistIRM = irMDist;
	refDistIRL = irLDist;
	refDistUSL = usLDist;
	refDistUSR = usRDist;
	refSlope = maviGetRefSlope(refDistIRS, refDistIRM, refDistIRL);

	cout << "Calibration Sucessful" << endl << endl;
	maviAudioPlay(MAVI_AUDIO_CALIB_SUCCESS);
	return 0;
}
