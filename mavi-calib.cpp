/*
 * mavi-calib.cpp
 * --------------
 *
 * ...
 */

#include "mavi-calib.hpp"

double
	refDistUSL    = MAVI_DEFAULT_REF_DIST_USL,
	refDistUSR    = MAVI_DEFAULT_REF_DIST_USR,
	refDistIRS    = MAVI_DEFAULT_REF_DIST_IRS,
	refDistIRM    = MAVI_DEFAULT_REF_DIST_IRM,
	refDistIRL    = MAVI_DEFAULT_REF_DIST_IRL,
	refAngleIRS   = MAVI_DEFAULT_REF_ANGLE_IRS,
	refAngleIRM   = MAVI_DEFAULT_REF_ANGLE_IRM,
	refAngleIRL   = MAVI_DEFAULT_REF_ANGLE_IRL,
	refBeltHeight = MAVI_DEFAULT_REF_BELT_HEIGHT,
	stairHeight   = MAVI_STAIR_HEIGHT,
	minStairSlope = MAVI_MIN_STAIR_SLOPE,
	maxStairSlope = MAVI_MAX_STAIR_SLOPE,
	errorMargin	  = MAVI_ERROR_MARGIN;
