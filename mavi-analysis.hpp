/*
 * mavi-analysis.hpp
 * -----------------
 *
 * ...
 */

#ifndef MAVI_ANALYSIS_HPP
#define MAVI_ANALYSIS_HPP

#include "mavi-feedback.hpp"

const unsigned int MAVI_ANALYSIS_SAMPLE_PERIOD = 250; 	// ms
const unsigned int MAVI_VERBAL_OUTPUT_PERIOD = 2000;	// ms
const unsigned int MAVI_VIBRATION_OUTPUT_PERIOD = 1000;	// ms

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
	MAVI_SLOPE_FLAT,
	MAVI_SLOPE_ASCENDING,
	MAVI_SLOPE_DESCENDING,
	MAVI_SLOPE_OTHER
};

enum MaviMidRangeKind
{
	MAVI_MIDRANGE_ERROR = -1,
	MAVI_MIDRANGE_NOTHING,
	MAVI_MIDRANGE_RIGHT,
	MAVI_MIDRANGE_LEFT,
	MAVI_MIDRANGE_BOTH
};

MaviNextStepKind maviNextStepScan(void);

MaviSlopeKind maviSlopeScan(void);

MaviMidRangeKind maviMidRangeScan(void);

unsigned int maviSendFeedback(MaviFeedbackID id, unsigned int feedbackTimerStart);

void maviMobilityAssistance(void);

void *maviSenseAndAnalyze(void*);

#endif
