/*
 * mavi-calib.hpp
 * --------------
 *
 * ...
 */

#ifndef MAVI_CALIB_HPP
#define MAVI_CALIB_HPP

const double
	MAVI_DEFAULT_REF_DIST_USL = 100.0, // cm  79 84 89
	MAVI_DEFAULT_REF_DIST_USR = 100.0, // cm
	MAVI_DEFAULT_REF_DIST_IRS = 85.0,  // cm
	MAVI_DEFAULT_REF_DIST_IRM = 140.0, // cm
	MAVI_DEFAULT_REF_DIST_IRL = 225.0, // cm
	MAVI_DEFAULT_REF_SLOPE = -0.556, //-0.556,

	MAVI_REF_ANGLE_IRM = 0.419, // rad, angle between IRS and IRM
	MAVI_REF_ANGLE_IRL = 0.663, // rad, angle between IRS and IRL
	MAVI_REF_STAIR_HEIGHT = 20.0,  // cm
	MAVI_MIN_STAIR_SLOPE = 0.5,
	MAVI_MAX_STAIR_SLOPE = 0.7,
	MAVI_ERROR_MARGIN = 20.0;  // cm

extern double
	refDistUSL,    // cm
	refDistUSR,    // cm
	refDistIRS,    // cm
	refDistIRM,    // cm
	refDistIRL,    // cm
	refSlope;

int maviCalibration(void);

#endif
