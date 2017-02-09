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
	double irDist, irHeight, realtive_Dif;
	
	irDist = maviPollSensor(MAVI_SENSOR_IRS);
	irHeight = irDist * cos(refAngleIRS);
	dif = refHeightBelt - irHeight;
  
	if (abs(relative_Dif) < 5)
		return MAVI_NEXTSTEP_NOTHING;
	else if (relative_Dif > 15 && relative_Dif < 25)
		return MAVI_NEXTSTEP_STEP_UP
	else if (relative_Dif < -15 && relative_Dif > -25)
		return MAVI_NEXTSTEP_STEP_DOWN
	else
		return MAVI_NEXTSTEP_OBSTACLE
}

MaviSlopeKind maviSlopeScan(void)
{
	double irMDist, irLDist, irM_RelativeHeight, irL_RelativeHeight;
	
	irMDist = maviPollSensor(MAVI_SENSOR_IRM);
	irLDist = maviPollSensor(MAVI_SENSOR_IRL);
  
	irM_RelativeHeight = refHeightBelt - irMDist * cos(refAngleIRM);
	irL_RelativeHeight = refHeightBelt - irLDist * cos(refAngleIRL);
	
	if (abs(irM_relativeHeight > 5) {
		if (abs(irL_relativeHeight > 5) {
			if ((irM_relativeHeight < 0) && irL_relativeHeight < 0))
				return MAVI_SLOPE_DESCENDING;
			else {
				double irM_RelativeLength, irL_RelativeLength, slope;
					
				irM_RelativeLength = irMDist * sin(refAngleIRM);
				irL_RelativeLength = irLDist * sin(refAngleIRL);
				
				slope = (irL_RelativeLength - irM_RelativeLength)/(irL_RelativeHeight - irM_RelativeHeight);
				
				if (slope >= 0.5 && slope <= 0.7)
					return MAVI_SLOPE_ASCENDING;
				else
					return MAVI_SLOPE_OTHER;
			}
		}
		else
			return MAVI_SLOPE_OTHER;
	}
	else if (abs(irL_RelativeHeight) > 5)
		    return MAVI_SLOPE_OTHER;
	else
		    return MAVI_SLOPE_FLAT;
	
					
	
		
}

MaviMidRangeKind maviMidRangeScan(void)
{
	double usLDist, usRDist;
	
	usLDist= maviPollSensor(MAVI_SENSOR_USL);
	usRDist = maviPollSensor(MAVI_SENSOR_USR);
	
	if (abs(refDistUSL - usLDist) > 5) {
		if (abs(refDistUSR - usRDist) > 5)
			return MAVI_MIDRANGE_BOTH;
		else
			return MAVI_MIDRANGE_LEFT;
	}
	else if (abs(refDistUSR - usRDist) > 5)
		return MAVI_MIDRANGE_RIGHT;
	else
		return MAVI_MIDRANGE_NEITHER;
}

void *maviSenseAndAnalyze(void* args)
{
	while (true) {
		MaviNextStepKind nextStepScan = maviNextStepScan();
		MaviSlopeKind slopeScan = maviSLopeScan();
		
		if (nextStepScan == MAVI_NEXTSTEP_NOTHING) {
			if (slopeScan == MAVI_SLOPE_ASCENDING)
				cout << "Verbal Output: Ascending Stairs Ahead";
			else if (slopeScan == MAVI_SLOPE_DESCENDING)
				cout << "Verbal Output: Descending Stairs Ahead";
			else if (slopeScan == MAVI_SLOPE_OTHER)
				cout << "Vibration Output: Center (Obstacle Ahead)";
			else if (slopeScan == MAVI_SLOPE_FLAT) {
				MaviMidRangeScan midRangeScan = maviMidRangeScan();
				
				if (midRangeScan == MAVI_MIDRANGE_BOTH)
					cout << "Vibration Output: Center (Obstacle Ahead)";
				else if (midRangeScan == MAVI_MIDRANGE_LEFT)
					cout << "Vibration Output: Left (Obstacle on Left)";
				else if (midRangeScan == MAVI_MIDRANGE_RIGHT)
					cout << "Vibration Output: Right (Obstacle on Right)";
				else if (midRangeScan == MAVI_MIDRANGE_NEITHER)
					cout << "No Obstacle Detected";
				else
					cout << "Sensing and Analysis Error: Received invalid mid range scan data.";
			}
			else
				cout << "Sensing and Analysis Error: Received invalid slope scan data.";
		}
		else if (nextStepScan == MAVI_NEXTSTEP_OTHER)
			cout << "Audio Output: Immediate Hazard Warning";
		else if (nextStepScan == MAVI_NEXTSTEP_STEPUP) {
			if (slopeScan == MAVI_SLOPE_ASCENDING)
				cout << "Verbal Output: First Step Up";
			else if (slopeScan == MAVI_SLOPE_FLAT)
				cout << "Verbal Output: Single Step Up";
			else
				cout << "Audio Output: Immediate Hazard Warning";
		}
		else if (nextStepScan == MAVI_NEXTSTEP_STEPDOWN) {
			if (slopeScan == MAVI_SLOPE_DESCENDING)
				cout << "Verbal Output: First Step Down";
			else if (slopeScan == MAVI_SLOPE_FLAT)
				cout << "Verbal Output: Single Step Down";
			else
				cout << "Audio Output: Immediate Hazard Warning";
		}
		else
			cout << "Sensing and Analysis Error: Received invalid next step data.";
	}
}
