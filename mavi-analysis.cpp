/*
 * mavi-analysis.cpp
 * -----------------
 *
 * ...
 */

#include <wiringPi.h>
#include <iostream>
#include <iomanip>
#include <cmath>

#include "mavi-analysis.hpp"
#include "mavi-sensors.hpp"
#include "mavi-audio.hpp"
#include "mavi-calib.hpp"
#include "mavi-state.hpp"
#include "mavi-math.hpp"

using namespace std;

MaviNextStepKind maviNextStepScan(void)
{
	double irDist, relativeDif_IRS;

	irDist = maviIRSFilter.poll();

	if (irDist == MAVI_BAD_SENSOR_READING)
		return MAVI_NEXTSTEP_ERROR;

	cout << "IR_S Distance: " << irDist << endl;

	relativeDif_IRS = refDistIRS - irDist;

	if (abs(relativeDif_IRS) <= MAVI_ERROR_MARGIN)
		return MAVI_NEXTSTEP_NOTHING;
	else if (relativeDif_IRS > MAVI_ERROR_MARGIN && relativeDif_IRS < (MAVI_REF_STAIR_HEIGHT + MAVI_ERROR_MARGIN))
		return MAVI_NEXTSTEP_STEP_UP;
	else if (relativeDif_IRS < -(MAVI_ERROR_MARGIN) && relativeDif_IRS > -(MAVI_REF_STAIR_HEIGHT + MAVI_ERROR_MARGIN))
		return MAVI_NEXTSTEP_STEP_DOWN;
	else
		return MAVI_NEXTSTEP_OBSTACLE;
}

MaviSlopeKind maviSlopeScan(void)
{
	double irSDist, irMDist, irLDist, relativeDif_IRM, relativeDif_IRL, slope;

	irSDist = maviIRSFilter.poll();
	irMDist = maviIRMFilter.poll();
	irLDist = maviIRLFilter.poll();

	cout << "IR_M Distance: " << irMDist << endl;
	cout << "IR_L Distance: " << irLDist << endl;

	if (irMDist == MAVI_BAD_SENSOR_READING || irLDist == MAVI_BAD_SENSOR_READING)
		return MAVI_SLOPE_ERROR;

	relativeDif_IRM = refDistIRM - irMDist;
	relativeDif_IRL = refDistIRL - irLDist;

	if (relativeDif_IRM < -(MAVI_ERROR_MARGIN/2) && relativeDif_IRL < -(MAVI_ERROR_MARGIN/2))
		return MAVI_SLOPE_DESCENDING;

	slope = maviGetSlope(irSDist, irMDist, irLDist);

	cout << "Slope: " << slope << endl;

	if (abs(slope) <= (MAVI_ERROR_MARGIN/200))
		return MAVI_SLOPE_FLAT;
	else if (abs(slope) >= MAVI_MIN_STAIR_SLOPE && abs(slope) <= MAVI_MAX_STAIR_SLOPE) {

		if (slope > 0)
			return MAVI_SLOPE_ASCENDING;
		else
			return MAVI_SLOPE_DESCENDING;
	}
	else
		return MAVI_SLOPE_OTHER;

	//~ if (abs(relativeDif_IRM) > MAVI_ERROR_MARGIN)
	//~ {
		//~ if (abs(relativeDif_IRL) > MAVI_ERROR_MARGIN)
		//~ {
			//~ if (relativeDif_IRM < 0 && relativeDif_IRL < 0)
			//~ {
				//~ return MAVI_SLOPE_DESCENDING;
			//~ }
			//~ else
			//~ {
				//~ double slope, irSDist;
				//~ irSDist = maviIRSFilter.poll();
//~
				//~ slope = maviGetSlope(irSDist, irMDist, irLDist);
//~
				//~ if (slope >= MAVI_MIN_STAIR_SLOPE && slope <= MAVI_MAX_STAIR_SLOPE)
					//~ return MAVI_SLOPE_ASCENDING;
				//~ else
					//~ return MAVI_SLOPE_OTHER;
			//~ }
		//~ }
		//~ else
		//~ {
			//~ return MAVI_SLOPE_OTHER;
		//~ }
	//~ }
	//~ else if (abs(relativeDif_IRL) > MAVI_ERROR_MARGIN)
	//~ {
		//~ return MAVI_SLOPE_OTHER;
	//~ }
	//~ else
	//~ {
		//~ return MAVI_SLOPE_FLAT;
	//~ }
}

MaviMidRangeKind maviMidRangeScan(void)
{
	int scanResult = 0;

	double usLDist, usRDist, relativeDif_USL, relativeDif_USR;

	usLDist = maviUSLFilter.poll();
	usRDist = maviUSRFilter.poll();

	cout << "test" << endl;

	if (usLDist == MAVI_BAD_SENSOR_READING || usRDist == MAVI_BAD_SENSOR_READING)
		return MAVI_MIDRANGE_ERROR;

	relativeDif_USL = refDistUSL - usLDist;
	relativeDif_USR = refDistUSR - usRDist;

	if (abs(relativeDif_USL) >= MAVI_ERROR_MARGIN) scanResult |= 0b01;
	if (abs(relativeDif_USR) >= MAVI_ERROR_MARGIN) scanResult |= 0b10;

	switch (scanResult)
	{
		case 0b00: return MAVI_MIDRANGE_NOTHING;
		case 0b01: return MAVI_MIDRANGE_RIGHT;
		case 0b10: return MAVI_MIDRANGE_LEFT;
		case 0b11: return MAVI_MIDRANGE_BOTH;
		default:   return MAVI_MIDRANGE_ERROR;
	}

}

void *maviSenseAndAnalyze(void* args)
{
	unsigned int nextCycle, pauseFeedbackFlag;

	MaviNextStepKind nextStepScan;
	MaviSlopeKind slopeScan;

	pauseFeedbackFlag = 1;

	while (maviGetState() != MAVI_STATE_SHUTDOWN)
	{

		if (maviGetState() != MAVI_STATE_RUNNING) {

			if (maviGetState() == MAVI_STATE_PAUSED && pauseFeedbackFlag) {
				pauseFeedbackFlag = 0;
				cout << "System Paused" << endl;
				maviAudioPlay(MAVI_AUDIO_SYSTEM_PAUSED);
			}

			delay(MAVI_ANALYSIS_SAMPLE_PERIOD);
		}
		else {

			nextCycle = millis() + MAVI_ANALYSIS_INITIAL_DELAY;
			pauseFeedbackFlag = 1;

			maviIRSFilter.startFiltering();
			maviIRMFilter.startFiltering();
			maviIRLFilter.startFiltering();
			maviUSLFilter.startFiltering();
			maviUSRFilter.startFiltering();

			cout << "System Ready" << endl;
			maviAudioPlay(MAVI_AUDIO_SYSTEM_READY);

			while (maviGetState() == MAVI_STATE_RUNNING)
			{

				if (nextCycle - millis() <= MAVI_ANALYSIS_SAMPLE_PERIOD)
					delay(nextCycle - millis());

				nextCycle += MAVI_ANALYSIS_SAMPLE_PERIOD;

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
							maviAudioPlay(MAVI_AUDIO_VIBRATE_LEFT);
							maviAudioPlay(MAVI_AUDIO_VIBRATE_RIGHT);
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
			}

			maviIRSFilter.stopFiltering();
			maviIRMFilter.stopFiltering();
			maviIRLFilter.stopFiltering();
			maviUSLFilter.stopFiltering();
			maviUSRFilter.stopFiltering();

		}
	}

	cout << "System Shutting Down" << endl;
	maviAudioPlay(MAVI_AUDIO_SYSTEM_SHUTDOWN);

	return NULL;
}
