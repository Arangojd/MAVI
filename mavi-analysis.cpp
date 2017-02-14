/*
 * mavi-analysis.cpp
 * -----------------
 *
 * ...
 */

#include <cmath>
#include <iostream>
#include "mavi-analysis.hpp"
#include "mavi-sensors.hpp"

using namespace std;

MaviNextStepKind maviNextStepScan(void)
{
	double irDist, irHeight, relative_Dif;

	irDist = maviPollSensor(MAVI_SENSOR_IRS);

	if (irDist == MAVI_BAD_SENSOR_READING)
		return MAVI_NEXTSTEP_ERROR;

	irHeight = irDist * cos(refAngleIRS);
	dif = refHeightBelt - irHeight;

	if (abs(relative_Dif) < 5)
		return MAVI_NEXTSTEP_NOTHING;
	else if (relative_Dif > 15 && relative_Dif < 25)
		return MAVI_NEXTSTEP_STEP_UP;
	else if (relative_Dif < -15 && relative_Dif > -25)
		return MAVI_NEXTSTEP_STEP_DOWN;
	else
		return MAVI_NEXTSTEP_OBSTACLE;
}

MaviSlopeKind maviSlopeScan(void)
{
	double irMDist, irLDist, irM_RelativeHeight, irL_RelativeHeight;

	irMDist = maviPollSensor(MAVI_SENSOR_IRM);
	irLDist = maviPollSensor(MAVI_SENSOR_IRL);

	if (irMDist == MAVI_BAD_SENSOR_READING || irLDist == MAVI_BAD_SENSOR_READING)
		return MAVI_SLOPE_ERROR;

	irM_RelativeHeight = refHeightBelt - irMDist * cos(refAngleIRM);
	irL_RelativeHeight = refHeightBelt - irLDist * cos(refAngleIRL);

	if (abs(irM_relativeHeight > 5))
	{
		if (abs(irL_relativeHeight > 5))
		{
			if (irM_relativeHeight < 0 && irL_relativeHeight < 0)
			{
				return MAVI_SLOPE_DESCENDING;
			}
			else
			{
				double irM_RelativeLength, irL_RelativeLength, slope;

				irM_RelativeLength = irMDist * sin(refAngleIRM);
				irL_RelativeLength = irLDist * sin(refAngleIRL);

				slope = (irL_RelativeLength - irM_RelativeLength) / (irL_RelativeHeight - irM_RelativeHeight);

				if (slope >= 0.5 && slope <= 0.7)
					return MAVI_SLOPE_ASCENDING;
				else
					return MAVI_SLOPE_OTHER;
			}
		}
		else
		{
			return MAVI_SLOPE_OTHER;
		}
	}
	else if (abs(irL_RelativeHeight) > 5)
	{
		return MAVI_SLOPE_OTHER;
	}
	else
	{
		return MAVI_SLOPE_FLAT;
	}
}

MaviMidRangeKind maviMidRangeScan(void)
{
	MaviMidRangeKind scanResult = MAVI_MIDRANGE_NOTHING;

	double
		usLDist = maviPollSensor(MAVI_SENSOR_USL),
		usRDist = maviPollSensor(MAVI_SENSOR_USR);

	if (usLDist == MAVI_BAD_SENSOR_READING || usRDist == MAVI_BAD_SENSOR_READING)
		return MAVI_MIDRANGE_ERROR;

	if (abs(refDistUSL - usLDist) > 5) scanResult |= MAVI_MIDRANGE_LEFT;
	if (abs(refDistUSR - usRDist) > 5) scanResult |= MAVI_MIDRANGE_RIGHT;

	return scanResult;
}

void *maviSenseAndAnalyze(void* args)
{
	MaviNextStepKind nextStepScan;
	MaviSlopeKind slopeScan;

	while (maviState != MAVI_STATE_SHUTDOWN)
	{
		nextStepScan = maviNextStepScan();
		slopeScan = maviSlopeScan();

		switch (nextStepScan)
		{
		case MAVI_NEXTSTEP_NOTHING:
			switch (slopeScan)
			{
			case MAVI_SLOPE_ASCENDING:
				cout << "Verbal Output: Ascending Stairs Ahead";
				break;

			case MAVI_SLOPE_DESCENDING:
				cout << "Verbal Output: Descending Stairs Ahead";
				break;

			case MAVI_SLOPE_FLAT:
				switch (maviMidRangeScan())
				{
				case MAVI_MIDRANGE_BOTH:
					cout << "Vibration Output: Center (Obstacle Ahead)";
					break;

				case MAVI_MIDRANGE_LEFT:
					cout << "Vibration Output: Left (Obstacle on Left)";
					break;

				case MAVI_MIDRANGE_RIGHT:
					cout << "Vibration Output: Right (Obstacle on Right)";
					break;

				case MAVI_MIDRANGE_NOTHING:
					cout << "No Obstacle Detected";
					break;

				default:
					cout << "Sensing and Analysis Error: Received invalid mid range scan data.";
					break;
				}

				break;

			case MAVI_SLOPE_OTHER:
				cout << "Vibration Output: Center (Obstacle Ahead)";
				break;

			default:
				cout << "Sensing and Analysis Error: Received invalid slope scan data.";
				break;
			}

		case MAVI_NEXTSTEP_STEPUP:
			switch (slopeScan)
			{
			case MAVI_SLOPE_ASCENDING:
				cout << "Verbal Output: (First) Step Up";
				break;

			case MAVI_SLOPE_FLAT:
				cout << "Verbal Output: Single/Last Step Up";
				break;

			case MAVI_SLOPE_DESCENDING:
			case MAVI_SLOPE_OTHER:
				cout << "Omni-Output: Immediate Hazard Warning";
				break;

			default:
				cout << "Sensing and Analysis Error: Received invalid slope scan data.";
				break;
			}

		case MAVI_NEXTSTEP_STEP_DOWN:
			switch (slopeScan)
			{
			case MAVI_SLOPE_DESCENDING:

				cout << "Verbal Output: (First) Step Down";
				break;

			case MAVI_SLOPE_FLAT:

				cout << "Verbal Output: Single/Last Step Down";
				break;

			case MAVI_SLOPE_ASCENDING:
			case MAVI_SLOPE_OTHER:

				cout << "Omni-Output: Immediate Hazard Warning";
				break;

			default:
				cout << "Sensing and Analysis Error: Received invalid slope scan data.";
				break;
			}

		case MAVI_NEXTSTEP_OBSTACLE:
			cout << "Omni-Output: Immediate Hazard Warning";
			break;

		default:
			cout << "Sensing and Analysis Error: Received invalid next step data.";
			break;
		}

		// NOTE: Exactly one of the following two statements should be commented out.

		//~ sleep(1);
		delay(1000);
	}
}
