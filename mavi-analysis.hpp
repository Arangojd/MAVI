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
	MAVI_NEXTSTEP_OBSTACLE
};

enum MaviSlopeKind
{
	MAVI_SLOPE_FLAT,
	MAVI_SLOPE_ASCENDING,
	MAVI_SLOPE_DESCENDING,
	MAVI_SLOPE_OTHER
};

enum MaviMidRangeKind
{
	MAVI_MIDRANGE_NEITHER = 0b00,
	MAVI_MIDRANGE_RIGHT   = 0b01,
	MAVI_MIDRANGE_LEFT    = 0b10,
	MAVI_MIDRANGE_BOTH    = 0b11
};

MaviNextStepKind maviNextStepScan(void);

MaviSlopeKind maviSlopeScan(void);

MaviMidRangeKind maviMidRangeScan(void);

void *maviSenseAndAnalyze(void*);

#endif
