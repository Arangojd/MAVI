/*
 * mavi-analysis.hpp
 * -----------------
 *
 * ...
 */

#ifndef MAVI_ANALYSIS_HPP
#define MAVI_ANALYSIS_HPP

enum MaviNextStepKind
{
	MAVI_NEXTSTEP_NOTHING,
	MAVI_NEXTSTEP_STEP_UP,
	MAVI_NEXTSTEP_STEP_DOWN,
	MAVI_NEXTSTEP_OBSTACLE,
	MAVI_NEXTSTEP_ERROR
};

enum MaviSlopeKind
{
	MAVI_SLOPE_FLAT,
	MAVI_SLOPE_ASCENDING,
	MAVI_SLOPE_DESCENDING,
	MAVI_SLOPE_OTHER,
	MAVI_SLOPE_ERROR
};

enum MaviMidRangeKind
{
	MAVI_MIDRANGE_NOTHING = 0b00,
	MAVI_MIDRANGE_RIGHT   = 0b01,
	MAVI_MIDRANGE_LEFT    = 0b10,
	MAVI_MIDRANGE_BOTH    = 0b11,
	MAVI_MIDRANGE_ERROR
};

MaviNextStepKind maviNextStepScan(void);

MaviSlopeKind maviSlopeScan(void);

MaviMidRangeKind maviMidRangeScan(void);

void *maviSenseAndAnalyze(void*);

#endif
