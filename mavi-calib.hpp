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
	MAVI_DEFAULT_REF_DIST_IRS    =  80.0, // cm
	MAVI_DEFAULT_REF_DIST_IRM    = 100.0, // cm
	MAVI_DEFAULT_REF_DIST_IRL    = 150.0, // cm
	MAVI_DEFAULT_REF_ANGLE_IRS   =   0.0, // rad, TODO
	MAVI_DEFAULT_REF_ANGLE_IRM   =   0.0, // rad, TODO
	MAVI_DEFAULT_REF_ANGLE_IRL   =   0.0, // rad, TODO
	MAVI_DEFAULT_REF_BELT_HEIGHT =   0.0; // cm,  TODO

extern double
	refDistUSL,    // cm
	refDistUSR,    // cm
	refDistIRS,    // cm
	refDistIRM,    // cm
	refDistIRL,    // cm
	refAngleIRS,   // rad
	refAngleIRM,   // rad
	refAngleIRL,   // rad
	refBeltHeight; // cm

#endif
