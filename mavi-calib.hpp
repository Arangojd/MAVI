/*
 * mavi-calib.hpp
 * --------------
 *
 * ...
 */

#ifndef MAVI_CALIB_HPP
#define MAVI_CALIB_HPP

const int MAVI_CALIB_SAMPLE_COUNT = 5;

const double
	MAVI_DEFAULT_REF_DIST_IRS  = 125.0, // cm
	MAVI_DEFAULT_REF_DIST_IRM  = 180.0, // cm
	MAVI_DEFAULT_REF_DIST_IRL  = 270.0, // cm
	MAVI_DEFAULT_REF_DIST_USLL = 200.0, // cm
	MAVI_DEFAULT_REF_DIST_USLR = 200.0, // cm
	MAVI_DEFAULT_REF_DIST_USUL = 200.0, // cm
	MAVI_DEFAULT_REF_DIST_USUR = 200.0, // cm
	MAVI_DEFAULT_REF_ANGLE_IRS = 0.51,  // rad, angle between normal and IRS

	MAVI_REL_ANGLE_IRM = 0.31, // rad, angle between IRS and IRM
	MAVI_REL_ANGLE_IRL = 0.68, // rad, angle between IRS and IRL

	MAVI_MIN_DIST_USLL    =  30.0,  // cm
	MAVI_MIN_DIST_USLR    =  30.0,  // cm
	MAVI_MIN_DIST_USUL    =  30.0,  // cm
	MAVI_MIN_DIST_USUR    =  30.0,  // cm
	MAVI_MAX_DIST_USLL    =  200.0, // cm
	MAVI_MAX_DIST_USLR    =  200.0, // cm
	MAVI_MAX_DIST_USUL    =  200.0, // cm
	MAVI_MAX_DIST_USUR    =  200.0, // cm
	MAVI_STAIR_SLOPE_MIN  =  0.25,
	MAVI_STAIR_SLOPE_MAX  =  1.00,
	MAVI_STAIR_HEIGHT_MIN =  20.0,  // cm
	MAVI_STAIR_HEIGHT_MAX =  60.0,  // cm

	MAVI_ERROR_PERC  = 0.25, // %
	MAVI_ERROR_SLOPE = 0.20; // rad


extern double
	MAVI_REF_DIST_IRS, // cm
	MAVI_REF_DIST_IRM, // cm
	MAVI_REF_DIST_IRL, // cm
	MAVI_REF_ANGLE_IRM,    // rad
	MAVI_REF_ANGLE_IRL,    // rad
	MAVI_ERROR_IRS,    // cm
	MAVI_ERROR_IRM,    // cm
	MAVI_ERROR_IRL;    // cm

int maviCalibration(void);
void maviSaveCalibration(void);
void maviLoadCalibration(void);

#endif
