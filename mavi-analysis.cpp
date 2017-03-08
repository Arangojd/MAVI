/*
 * mavi-analysis.cpp
 * -----------------
 *
 * ...
 */

#include <wiringPi.h>
#include <cmath>
#include <iostream>
#include <iomanip>

#include "mavi-analysis.hpp"
#include "mavi-sensors.hpp"
#include "mavi-mcp3008.hpp"
#include "mavi-audio.hpp"
#include "mavi-calib.hpp"
#include "mavi-state.hpp"

using namespace std;

//~ MaviSensorFilter
	//~ maviIRSFilter(MAVI_SENSOR_IRS, 5000, 20),
	//~ maviIRMFilter(MAVI_SENSOR_IRM, 5000, 20),
	//~ maviIRLFilter(MAVI_SENSOR_IRL, 5000, 20),
	//~ maviUSLFilter(MAVI_SENSOR_USL, 5000, 20),
	//~ maviUSRFilter(MAVI_SENSOR_USR, 5000, 20);

MaviNextStepKind maviNextStepScan(void)
{
	double irDist, irHeight, relative_Dif;

	irDist = maviPollSensor(MAVI_SENSOR_IRS); //maviIRSFilter.poll();

	if (irDist == MAVI_BAD_SENSOR_READING)
		return MAVI_NEXTSTEP_ERROR;

	cout << "IR_S Distance: " << irDist << endl;

	//irHeight = irDist * cos(refAngleIRS);
	relative_Dif = refDistIRS - irDist;

	if (abs(relative_Dif) <= errorMargin)
		return MAVI_NEXTSTEP_NOTHING;
	else if (relative_Dif > errorMargin && relative_Dif < (stairHeight+errorMargin))
		return MAVI_NEXTSTEP_STEP_UP;
	else if (relative_Dif < -(errorMargin) && relative_Dif > -(stairHeight+errorMargin))
		return MAVI_NEXTSTEP_STEP_DOWN;
	else
		return MAVI_NEXTSTEP_OBSTACLE;
}

MaviSlopeKind maviSlopeScan(void)
{
	double irMDist, irLDist, irM_RelativeHeight, irL_RelativeHeight;

	irMDist = maviPollSensor(MAVI_SENSOR_IRM); //maviIRMFilter.poll();
	irLDist = maviPollSensor(MAVI_SENSOR_IRL); //maviIRLFilter.poll();

	cout << "IR_M Distance: " << irMDist << endl;
	cout << "IR_L Distance: " << irLDist << endl;

	if (irMDist == MAVI_BAD_SENSOR_READING || irLDist == MAVI_BAD_SENSOR_READING)
		return MAVI_SLOPE_ERROR;

	irM_RelativeHeight = refBeltHeight - ( irMDist * cos(refAngleIRM) );
	irL_RelativeHeight = refBeltHeight - ( irLDist * cos(refAngleIRL) );

	if (abs(irM_RelativeHeight) > errorMargin)
	{
		if (abs(irL_RelativeHeight) > errorMargin)
		{
			if (irM_RelativeHeight < 0 && irL_RelativeHeight < 0)
			{
				return MAVI_SLOPE_DESCENDING;
			}
			else
			{
				double irM_RelativeLength, irL_RelativeLength, slope;

				irM_RelativeLength = irMDist * sin(refAngleIRM);
				irL_RelativeLength = irLDist * sin(refAngleIRL);

				slope = (irL_RelativeLength - irM_RelativeLength) / (irL_RelativeHeight - irM_RelativeHeight);

				if (slope >= minStairSlope && slope <= maxStairSlope)
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
	else if (abs(irL_RelativeHeight) > errorMargin)
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
	//~ int scanResult = 0;
//~
	//~ double
		//~ usLDist = maviPollSensor(MAVI_SENSOR_USL), //maviUSLFilter.poll();
		//~ usRDist = maviPollSensor(MAVI_SENSOR_USR); //maviUSRFilter.poll();
	//~
	//~ if (usLDist == MAVI_BAD_SENSOR_READING || usRDist == MAVI_BAD_SENSOR_READING)
		//~ return MAVI_MIDRANGE_ERROR;
//~
	//~ if (abs(refDistUSR - usRDist) >= errorMargin) scanResult |= 0b01;
	//~ if (abs(refDistUSL - usLDist) >= errorMargin) scanResult |= 0b10;
//~
	//~ switch (scanResult)
	//~ {
		//~ case 0b00: return MAVI_MIDRANGE_NOTHING;
		//~ case 0b01: return MAVI_MIDRANGE_RIGHT;
		//~ case 0b10: return MAVI_MIDRANGE_LEFT;
		//~ case 0b11: return MAVI_MIDRANGE_BOTH;
		//~ default:   return MAVI_MIDRANGE_ERROR;
	//~ }

	return MAVI_MIDRANGE_NOTHING;
}

void *maviSenseAndAnalyze(void* args)
{
	//~ maviIRSFilter.startFiltering();
	//~ maviIRMFilter.startFiltering();
	//~ maviIRLFilter.startFiltering();
	//~ maviUSLFilter.startFiltering();
	//~ maviUSRFilter.startFiltering();

	MaviNextStepKind nextStepScan;
	MaviSlopeKind slopeScan;

	unsigned int nextCycle = millis() + MAVI_ANALYSIS_INITIAL_DELAY;

	while (maviGetState() != MAVI_STATE_SHUTDOWN)
	{
		if (nextCycle - millis() <= MAVI_ANALYSIS_SAMPLE_PERIOD)
			delay(nextCycle - millis());

		nextCycle += MAVI_ANALYSIS_SAMPLE_PERIOD;

		//~ cout <<
			//~ "IR Sensor signals:" << endl <<
			//~ "  Short Medium   Long" << endl << hex << right <<
			//~ setw(7) << maviMCP3008ReadRaw(MAVI_APIN_IRS) <<
			//~ setw(7) << maviMCP3008ReadRaw(MAVI_APIN_IRM) <<
			//~ setw(7) << maviMCP3008ReadRaw(MAVI_APIN_IRL) <<
			//~ endl << endl << dec << left;

		nextStepScan = maviNextStepScan();
		slopeScan = maviSlopeScan();

		switch (nextStepScan)
		{
		case MAVI_NEXTSTEP_NOTHING:
			switch (slopeScan)
			{
			case MAVI_SLOPE_ASCENDING:
				maviAudioPlay(MAVI_AUDIO_STAIRS_ASC);
				cout << "Next step safe - slope ascending";
				break;

			case MAVI_SLOPE_DESCENDING:
				maviAudioPlay(MAVI_AUDIO_STAIRS_DESC);
				cout << "Next step safe - slope descending";
				break;

			case MAVI_SLOPE_FLAT:
				switch (maviMidRangeScan())
				{
				case MAVI_MIDRANGE_BOTH:
					//maviAudioPlay(MAVI_AUDIO_VIBRATE_BOTH);
					cout << "Next step safe - slope flat - left & right";
					break;

				case MAVI_MIDRANGE_LEFT:
					maviAudioPlay(MAVI_AUDIO_VIBRATE_LEFT);
					cout << "Next step safe - slope flat - left";
					break;

				case MAVI_MIDRANGE_RIGHT:
					maviAudioPlay(MAVI_AUDIO_VIBRATE_RIGHT);
					cout << "Next step safe - slope flat - right";
					break;

				case MAVI_MIDRANGE_NOTHING:
					cout << "Next step safe - slope flat - nothing";
					break;

				default:
					cout << "Sensing and Analysis Error: Received invalid mid range scan data.";
					break;
				}

				break;

			case MAVI_SLOPE_OTHER:
				maviAudioPlay(MAVI_AUDIO_VIBRATE_CENTER);
				cout << "Next step safe - slope other";
				break;

			default:
				cout << "Sensing and Analysis Error: Received invalid slope scan data.";
				break;
			}

			break;

		case MAVI_NEXTSTEP_STEP_UP:
			switch (slopeScan)
			{
			case MAVI_SLOPE_ASCENDING:
				maviAudioPlay(MAVI_AUDIO_STEP_FIRSTUP);
				cout << "Next step up - slope ascending";
				break;

			case MAVI_SLOPE_FLAT:
				maviAudioPlay(MAVI_AUDIO_STEP_SINGLEUP);
				cout << "Next step up - slope flat";
				break;

			case MAVI_SLOPE_DESCENDING:
				maviAudioPlay(MAVI_AUDIO_IM_HAZARD);
				cout << "Next step up - Slope descending";
				break;

			case MAVI_SLOPE_OTHER:
				maviAudioPlay(MAVI_AUDIO_IM_HAZARD);
				cout << "Next step up - Slope other";
				break;

			default:
				cout << "Sensing and Analysis Error: Received invalid slope scan data.";
				break;
			}

			break;

		case MAVI_NEXTSTEP_STEP_DOWN:
			switch (slopeScan)
			{
			case MAVI_SLOPE_DESCENDING:
				maviAudioPlay(MAVI_AUDIO_STEP_FIRSTDOWN);
				cout << "Next step down - slope descending";
				break;

			case MAVI_SLOPE_FLAT:
				maviAudioPlay(MAVI_AUDIO_STEP_FIRSTDOWN);
				cout << "Next step down - slope flat";
				break;

			case MAVI_SLOPE_ASCENDING:
				maviAudioPlay(MAVI_AUDIO_IM_HAZARD);
				cout << "Next step down - slope ascending";
				break;

			case MAVI_SLOPE_OTHER:
				maviAudioPlay(MAVI_AUDIO_IM_HAZARD);
				cout << "Next step down - slope other";
				break;

			default:
				cout << "Sensing and Analysis Error: Received invalid slope scan data.";
				break;
			}

			break;

		case MAVI_NEXTSTEP_OBSTACLE:
			maviAudioPlay(MAVI_AUDIO_IM_HAZARD);
			cout << "Next step unsafe";
			break;

		default:
			cout << "Sensing and Analysis Error: Received invalid next step data.";
			break;
		}

		cout << endl << endl;

		//~ end = millis();

		//~ if (end - start < minCycle)
			//~ delay(minCycle - (end - start));
	}

	cout << "Shutting down" << endl;

	//~ maviIRSFilter.stopFiltering();
	//~ maviIRMFilter.stopFiltering();
	//~ maviIRLFilter.stopFiltering();
	//~ maviUSLFilter.stopFiltering();
	//~ maviUSRFilter.stopFiltering();

	return NULL;
}
