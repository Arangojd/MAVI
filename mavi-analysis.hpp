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
	MAVI_NEXTSTEP_STAIRS,
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
	MAVI_MIDRANGE_LEFT,
	MAVI_MIDRANGE_RIGHT,
	MAVI_MIDRANGE_BOTH,
	MAVI_MIDRANGE_NEITHER
};

MaviNextStepKind maviNextStepScan(void);

MaviSlopeKind maviSlopeScan(void);

MaviMidRangeKind maviMidRangeScan(void);

void *maviSenseAndAnalyze(void*);

#endif
