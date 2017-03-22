/*
 * mavi-calib.hpp
 * --------------
 *
 * ...
 */

#ifndef MAVI_CALIB_HPP
#define MAVI_CALIB_HPP

const int MAVI_CALIB_SAMPLE_COUNT = 40;

const double
	MAVI_DEFAULT_REF_DIST_IRS = 125.0, // cm
	MAVI_DEFAULT_REF_DIST_IRM = 180.0, // cm
	MAVI_DEFAULT_REF_DIST_IRL = 270.0, // cm
	MAVI_DEFAULT_REF_DIST_USL = 200.0, // cm
	MAVI_DEFAULT_REF_DIST_USR = 200.0, // cm
	MAVI_DEFAULT_REF_SLOPE    = -0.55,

	MAVI_REF_ANGLE_IRM      = 0.419, // rad, angle between IRS and IRM
	MAVI_REF_ANGLE_IRL      = 0.663, // rad, angle between IRS and IRL
	MAVI_STAIR_SLOPE_MIN    = 0.5,
	MAVI_STAIR_SLOPE_MAX    = 0.7,
	MAVI_STAIR_HEIGHT_MIN   = 25.0, // cm
	MAVI_STAIR_HEIGHT_MAX   = 45.0, // cm
	MAVI_ERROR_MARGIN_IRS   = 20.0, // cm
	MAVI_ERROR_MARGIN_IRM   = 20.0, // cm
	MAVI_ERROR_MARGIN_IRL   = 40.0, // cm
	MAVI_ERROR_MARGIN_USL   = 30.0, // cm
	MAVI_ERROR_MARGIN_USR   = 30.0, // cm
	MAVI_ERROR_MARGIN_SLOPE = 0.2;

extern double
	refDistUSL, // cm
	refDistUSR, // cm
	refDistIRS, // cm
	refDistIRM, // cm
	refDistIRL, // cm
	refSlope;

int maviCalibration(void);
void maviSaveCalibration(void);
void maviLoadCalibration(void);

#endif
