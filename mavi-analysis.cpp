/*
 * mavi-analysis.cpp
 * -----------------
 *
 * ...
 */

#include "mavi-analysis.hpp"
#include "mavi-sensors.hpp"
#include <cmath>
using namespace std;

MaviNextStepKind maviNextStepScan(void)
{
	irDist = maviPollSensor(MAVI_SENSOR_IRS);
	irHeight = irDist * cos(refAngleIRS);
	dif = refHeightBelt - irHeight;
  
	if (abs(dif) < 5)
		return MAVI_NEXTSTEP_NOTHING;
    else if ( dif > 15 && dif < 25)
		return MAVI_NEXTSTEP_STEP_UP
    else if (dif < -15 && dif > -25)
		return MAVI_NEXTSTEP_STEP_DOWN
    else
		return MAVI_NEXTSTEP_OBSTACLE
}

MaviSlopeKind maviSlopeScan(void)
{
	irMDist = maviPollSensor(MAVI_SENSOR_IRM);
	irLDist = maviPollSensor(MAVI_SENSOR_IRL);
  
	irMR_RelativeHeight = 
}

MaviMidRangeKind maviMidRangeScan(void)
{
}

void *maviSenseAndAnalyze(void* args)
{
}
