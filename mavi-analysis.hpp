/*
 * mavi-analysis.hpp
 * -----------------
 *
 * ...
 */

#ifndef MAVI_ANALYSIS_HPP
#define MAVI_ANALYSIS_HPP

#include "mavi-sensors.hpp"

extern double irSDist, irMDist, irLDist, srDist, usLLDist, usLRDist, usULDist, usURDist, slope;

const unsigned int MAVI_ANALYSIS_SAMPLE_PERIOD = 250; //MAVI_IR_FILTER_PERIOD * MAVI_IR_FILTER_BUFSIZE; // ms

enum MaviNextStepKind
{
	MAVI_NEXTSTEP_ERROR = -1,
	MAVI_NEXTSTEP_NOTHING,
	MAVI_NEXTSTEP_STEP_UP,
	MAVI_NEXTSTEP_STEP_DOWN,
	MAVI_NEXTSTEP_OBSTACLE
};

enum MaviSlopeKind
{
	MAVI_SLOPE_ERROR = -1,
	MAVI_SLOPE_NULL,
	MAVI_SLOPE_FLAT,
	MAVI_SLOPE_FLAT_STEP,
	MAVI_SLOPE_ASCENDING,
	MAVI_SLOPE_DESCENDING,
	MAVI_SLOPE_OTHER
};


enum MaviLongRangeKind
{
	MAVI_LONGRANGE_ERROR = -1,
	MAVI_LONGRANGE_NOTHING,
	MAVI_LONGRANGE_OBSTACLE
};

typedef signed char MaviMidRangeKind;

const MaviMidRangeKind
	MAVI_MIDRANGE_ERROR   = -1,
	MAVI_MIDRANGE_NOTHING = 0b00,
	MAVI_MIDRANGE_RIGHT   = 0b01,
	MAVI_MIDRANGE_LEFT    = 0b10,
	MAVI_MIDRANGE_BOTH    = 0b11;

typedef signed char MaviLowHangKind;

const MaviLowHangKind
	MAVI_LOWHANG_ERROR   = -1,
	MAVI_LOWHANG_NOTHING = 0b00,
	MAVI_LOWHANG_RIGHT   = 0b01,
	MAVI_LOWHANG_LEFT    = 0b10,
	MAVI_LOWHANG_BOTH    = 0b11;

MaviNextStepKind maviNextStepScan(void);
MaviSlopeKind    maviSlopeScan(void);
MaviLongRangeKind maviLongRangeScan(void);
MaviMidRangeKind maviMidRangeScan(void);
MaviLowHangKind  maviLowHangScan(void);

void maviStairAssistance(MaviSlopeKind stair_slope);
void maviMobilityAssistance(void);
void *maviSenseAndAnalyze(void*);

#endif
