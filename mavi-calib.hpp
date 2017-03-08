/*
 * mavi-calib.hpp
 * --------------
 *
 * ...
 */

#ifndef MAVI_CALIB_HPP
#define MAVI_CALIB_HPP

const double
	MAVI_DEFAULT_REF_DIST_USL    = 100.0, // cm
	MAVI_DEFAULT_REF_DIST_USR    = 100.0, // cm
	MAVI_DEFAULT_REF_DIST_IRS    = 110.0, // cm
	MAVI_DEFAULT_REF_DIST_IRM    = 150.0, // cm
	MAVI_DEFAULT_REF_DIST_IRL    = 280.0, // cm
	MAVI_DEFAULT_REF_ANGLE_IRS   = 0.785, // rad, TODO
	MAVI_DEFAULT_REF_ANGLE_IRM   = 0.911, // rad, TODO
	MAVI_DEFAULT_REF_ANGLE_IRL   = 1.236, // rad, TODO
	MAVI_DEFAULT_REF_BELT_HEIGHT = 92.0,  // cm,  TODO
	MAVI_STAIR_HEIGHT            = 20.0,  // cm
	MAVI_MIN_STAIR_SLOPE         = 0.5,
	MAVI_MAX_STAIR_SLOPE         = 0.7,
	MAVI_ERROR_MARGIN            = 20.0;  // cm

extern double
	refDistUSL,    // cm
	refDistUSR,    // cm
	refDistIRS,    // cm
	refDistIRM,    // cm
	refDistIRL,    // cm
	refAngleIRS,   // rad
	refAngleIRM,   // rad
	refAngleIRL,   // rad
	refBeltHeight, // cm
	stairHeight,   // cm
	minStairSlope,
	maxStairSlope,
	errorMargin;   // cm

#endif
