/*
 * mavi-analysis.cpp
 * -----------------
 *
 * ...
 */

#include <wiringPi.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <cmath>

#include "mavi-analysis.hpp"
#include "mavi-sensors.hpp"
#include "mavi-audio.hpp"
#include "mavi-calib.hpp"
#include "mavi-state.hpp"
#include "mavi-math.hpp"
#include "mavi-feedback.hpp"

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

unsigned int maviSendFeedback(MaviFeedbackID id, unsigned int feedbackTimerStart)
{
	// system notifications (always output but never reset timer)
	switch(id)
	{
		case MAVI_FEEDBACK_SYSTEM_READY:
			cout << "System Ready" << endl;
			maviAudioPlay(MAVI_AUDIO_SYSTEM_READY);
			return feedbackTimerStart;

		case MAVI_FEEDBACK_SYSTEM_PAUSED:
			cout << "System Paused" << endl;
			maviAudioPlay(MAVI_AUDIO_SYSTEM_PAUSED);
			return feedbackTimerStart;

		case MAVI_FEEDBACK_SYSTEM_SHUTDOWN:
			cout << "System Shutting Down" << endl;
			maviAudioPlay(MAVI_AUDIO_SYSTEM_SHUTDOWN);
			return feedbackTimerStart;

		default:
			break;
	}

	float t_elapsed = millis() - feedbackTimerStart;

	if (t_elapsed >= MAVI_ANALYSIS_OUTPUT_PERIOD)
	{
		// normal notifications (only output periodically and reset timer)
		switch(id)
		{
			case MAVI_FEEDBACK_IM_HAZARD:
				cout << "Immediate Hazard" << endl;
				maviAudioPlay(MAVI_AUDIO_WARNING_IM_HAZARD);
				break;

			case MAVI_FEEDBACK_STEP_FINAL:
				cout << "Final Step" << endl;
				maviAudioPlay(MAVI_AUDIO_STEP_FINAL);
				break;

			case MAVI_FEEDBACK_STEP_FIRSTDOWN:
				cout << "First Step Down" << endl;
				maviAudioPlay(MAVI_AUDIO_STEP_FIRSTDOWN);
				break;

			case MAVI_FEEDBACK_STEP_FIRSTUP:
				cout << "First Step Up" << endl;
				maviAudioPlay(MAVI_AUDIO_STEP_FIRSTUP);
				break;

			case MAVI_FEEDBACK_STEP_SINGLEDOWN:
				cout << "Single Step Down" << endl;
				maviAudioPlay(MAVI_AUDIO_STEP_SINGLEDOWN);
				break;

			case MAVI_FEEDBACK_STEP_SINGLEUP:
				cout << "Single Step Up" << endl;
				maviAudioPlay(MAVI_AUDIO_STEP_SINGLEUP);
				break;

			case MAVI_FEEDBACK_STAIRS_ASC:
				cout << "Stairs Ahead: Ascending" << endl;
				maviAudioPlay(MAVI_AUDIO_STAIRS_ASC);
				break;

			case MAVI_FEEDBACK_STAIRS_DESC:
				cout << "Stairs Ahead: Descending" << endl;
				maviAudioPlay(MAVI_AUDIO_STAIRS_DESC);
				break;

			case MAVI_FEEDBACK_VIBRATE_CENTER:
				cout << "Vibrate Center" << endl;
				maviAudioPlay(MAVI_AUDIO_VIBRATE_CENTER);
				break;

			case MAVI_FEEDBACK_VIBRATE_LEFT:
				cout << "Vibrate Left" << endl;
				maviAudioPlay(MAVI_AUDIO_VIBRATE_LEFT);
				break;

			case MAVI_FEEDBACK_VIBRATE_RIGHT:
				cout << "Vibrate Right" << endl;
				maviAudioPlay(MAVI_AUDIO_VIBRATE_RIGHT);
				break;

			case MAVI_FEEDBACK_VIBRATE_BOTH:
				cout << "Vibrate Both" << endl;
				maviAudioPlay(MAVI_AUDIO_VIBRATE_LEFT);
				maviAudioPlay(MAVI_AUDIO_VIBRATE_RIGHT);
				break;

			default:
				break;
		}
	}
	else
	{
		// priority notifications (always output and reset timer)

		switch(id)
		{
			case MAVI_FEEDBACK_IM_HAZARD:
				cout << "Immediate Hazard" << endl;
				maviAudioPlay(MAVI_AUDIO_WARNING_IM_HAZARD);
				break;

			case MAVI_FEEDBACK_STEP_FINAL:
				cout << "Final Step" << endl;
				maviAudioPlay(MAVI_AUDIO_STEP_FINAL);
				break;

			case MAVI_FEEDBACK_STEP_FIRSTDOWN:
				cout << "First Step Down" << endl;
				maviAudioPlay(MAVI_AUDIO_STEP_FIRSTDOWN);
				break;

			case MAVI_FEEDBACK_STEP_FIRSTUP:
				cout << "First Step Up" << endl;
				maviAudioPlay(MAVI_AUDIO_STEP_FIRSTUP);
				break;

			case MAVI_FEEDBACK_STEP_SINGLEDOWN:
				cout << "Single Step Down" << endl;
				maviAudioPlay(MAVI_AUDIO_STEP_SINGLEDOWN);
				break;

			case MAVI_FEEDBACK_STEP_SINGLEUP:
				cout << "Single Step Up" << endl;
				maviAudioPlay(MAVI_AUDIO_STEP_SINGLEUP);
				break;

			default:
				break;
		}
	}

	return millis();
}

void maviMobilityAssistance(void)
{
	float t_elapsed;
	unsigned int analysisTimerStart, feedbackTimerStart;

	feedbackTimerStart = 0;
	analysisTimerStart = 0;

	while (maviGetState() == MAVI_STATE_RUNNING)
	{
		t_elapsed = millis() - analysisTimerStart;

		if (t_elapsed < MAVI_ANALYSIS_SAMPLE_PERIOD)
			delay(MAVI_ANALYSIS_SAMPLE_PERIOD - t_elapsed);

		analysisTimerStart = millis();

		switch (maviNextStepScan())
		{
		case MAVI_NEXTSTEP_NOTHING:
			switch (maviSlopeScan())
			{
			case MAVI_SLOPE_ASCENDING:
				cout << "Next step safe - slope ascending";
				feedbackTimerStart = maviSendFeedback(MAVI_FEEDBACK_STAIRS_ASC, feedbackTimerStart);
				break;

			case MAVI_SLOPE_DESCENDING:
				cout << "Next step safe - slope descending";
				feedbackTimerStart = maviSendFeedback(MAVI_FEEDBACK_SYSTEM_READY, feedbackTimerStart);
				break;

			case MAVI_SLOPE_FLAT:
				switch (maviMidRangeScan())
				{
				case MAVI_MIDRANGE_BOTH:
					cout << "Next step safe - slope flat - left & right";
					feedbackTimerStart = maviSendFeedback(MAVI_FEEDBACK_VIBRATE_BOTH, feedbackTimerStart);
					break;

				case MAVI_MIDRANGE_LEFT:
					cout << "Next step safe - slope flat - left";
					feedbackTimerStart = maviSendFeedback(MAVI_FEEDBACK_VIBRATE_LEFT, feedbackTimerStart);
					break;

				case MAVI_MIDRANGE_RIGHT:
					cout << "Next step safe - slope flat - right";
					feedbackTimerStart = maviSendFeedback(MAVI_FEEDBACK_VIBRATE_RIGHT, feedbackTimerStart);
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
				cout << "Next step safe - slope other";
				feedbackTimerStart = maviSendFeedback(MAVI_FEEDBACK_VIBRATE_CENTER, feedbackTimerStart);
				break;

			default:
				cout << "Sensing and Analysis Error: Received invalid slope scan data.";
				break;
			}

			break;

		case MAVI_NEXTSTEP_STEP_UP:
			switch (maviSlopeScan())
			{
			case MAVI_SLOPE_ASCENDING:
				cout << "Next step up - slope ascending";
				feedbackTimerStart = maviSendFeedback(MAVI_FEEDBACK_STEP_FIRSTUP, feedbackTimerStart);
				break;

			case MAVI_SLOPE_FLAT:
				cout << "Next step up - slope flat";
				feedbackTimerStart = maviSendFeedback(MAVI_FEEDBACK_STEP_SINGLUP, feedbackTimerStart);
				break;

			case MAVI_SLOPE_DESCENDING:
				cout << "Next step up - Slope descending";
				feedbackTimerStart = maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD, feedbackTimerStart);
				break;

			case MAVI_SLOPE_OTHER:
				cout << "Next step up - Slope other";
				feedbackTimerStart = maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD, feedbackTimerStart);
				break;

			default:
				cout << "Sensing and Analysis Error: Received invalid slope scan data.";
				break;
			}

			break;

		case MAVI_NEXTSTEP_STEP_DOWN:
			switch (maviSlopeScan())
			{
			case MAVI_SLOPE_DESCENDING:
				cout << "Next step down - slope descending";
				feedbackTimerStart = maviSendFeedback(MAVI_FEEDBACK_STEP_FIRSTDOWN, feedbackTimerStart);
				break;

			case MAVI_SLOPE_FLAT:
				cout << "Next step down - slope flat";
				feedbackTimerStart = maviSendFeedback(MAVI_FEEDBACK_STEP_SINGLEDOWN, feedbackTimerStart);
				break;

			case MAVI_SLOPE_ASCENDING:
				cout << "Next step down - slope ascending";
				feedbackTimerStart = maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD, feedbackTimerStart);
				break;

			case MAVI_SLOPE_OTHER:
				cout << "Next step down - slope other";
				feedbackTimerStart = maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD, feedbackTimerStart);
				break;

			default:
				cout << "Sensing and Analysis Error: Received invalid slope scan data.";
				break;
			}

			break;

		case MAVI_NEXTSTEP_OBSTACLE:
			cout << "Next step unsafe";
			feedbackTimerStart = maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD, feedbackTimerStart);
			break;

		default:
			cout << "Sensing and Analysis Error: Received invalid next step data.";
			break;
		}

		cout << endl << endl;
	}

}

void *maviSenseAndAnalyze(void* args)
{
	bool pauseFeedbackFlag = true;

	while (maviGetState() != MAVI_STATE_SHUTDOWN)
	{
		if (maviGetState() != MAVI_STATE_RUNNING)
		{
			if (maviGetState() == MAVI_STATE_PAUSED && pauseFeedbackFlag)
			{
				pauseFeedbackFlag = false;
				maviSendFeedback(MAVI_FEEDBACK_SYSTEM_PAUSED, feedbackTimerStart);
			}

			delay(MAVI_ANALYSIS_SAMPLE_PERIOD);
		}
		else
		{
			pauseFeedbackFlag = true;

			maviIRSFilter.startFiltering();
			maviIRMFilter.startFiltering();
			maviIRLFilter.startFiltering();
			maviUSLFilter.startFiltering();
			maviUSRFilter.startFiltering();

			maviSendFeedback(MAVI_FEEDBACK_SYSTEM_READY, feedbackTimerStart);

			maviMobilityAssistance();

			maviIRSFilter.stopFiltering();
			maviIRMFilter.stopFiltering();
			maviIRLFilter.stopFiltering();
			maviUSLFilter.stopFiltering();
			maviUSRFilter.stopFiltering();

		}
	}

	maviSendFeedback(MAVI_FEEDBACK_SYSTEM_SHUTDOWN, feedbackTimerStart);

	return NULL;
}
