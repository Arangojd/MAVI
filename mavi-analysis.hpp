/*
 * mavi-analysis.hpp
 * -----------------
 *
 * ...
 */

#ifndef MAVI_ANALYSIS_HPP
#define MAVI_ANALYSIS_HPP

const int MAVI_ANALYSIS_INITIAL_DELAY = 100; // ms
const int MAVI_ANALYSIS_SAMPLE_PERIOD = 1000; // ms

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
	MAVI_MIDRANGE_NOTHING,
	MAVI_MIDRANGE_RIGHT,
	MAVI_MIDRANGE_LEFT,
	MAVI_MIDRANGE_BOTH,
	MAVI_MIDRANGE_ERROR
};

MaviNextStepKind maviNextStepScan(void);

MaviSlopeKind maviSlopeScan(void);

MaviMidRangeKind maviMidRangeScan(void);

void *maviSenseAndAnalyze(void*);

#endif
