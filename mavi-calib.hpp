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
	MAVI_DEFAULT_REF_DIST_IRS 	= 110.0, 	// cm
	MAVI_DEFAULT_REF_DIST_IRM 	= 150.0, 	// cm
	MAVI_DEFAULT_REF_DIST_IRL 	= 230.0, 	// cm
	MAVI_DEFAULT_REF_SLOPE		= -0.55,	// rad
	MAVI_DEFAULT_ERROR_IRS		= 25.0,		// cm
	MAVI_DEFAULT_ERROR_IRM		= 35.0,		// cm
	MAVI_DEFAULT_ERROR_IRL		= 45.0,		// cm

	MAVI_REF_ANGLE_IRM      = 0.42, 	// rad, angle between IRS and IRM
	MAVI_REF_ANGLE_IRL      = 0.66, 	// rad, angle between IRS and IRL
	MAVI_STAIR_SLOPE_MIN    = 0.5,		// rad
	MAVI_STAIR_SLOPE_MAX    = 0.7,		// rad
	MAVI_STAIR_HEIGHT_MIN   = 25.0, 	// cm
	MAVI_MAX_DIST_USL 		= 150.0, 	// cm
	MAVI_MAX_DIST_USR 		= 150.0, 	// cm
	MAVI_MIN_DIST_USL   	= 30.0, 	// cm
	MAVI_MIN_DIST_USR   	= 30.0, 	// cm
	MAVI_ERROR_SLOPE 		= 0.2;		// rad

extern double
	MAVI_REF_DIST_IRS,	// cm
	MAVI_REF_DIST_IRM,	// cm
	MAVI_REF_DIST_IRL,	// cm
	MAVI_REF_SLOPE,		// rad
	MAVI_ERROR_IRS,		// cm
	MAVI_ERROR_IRS,		// cm
	MAVI_ERROR_IRS;		// cm

int maviCalibration(void);
void maviSaveCalibration(void);
void maviLoadCalibration(void);

#endif
