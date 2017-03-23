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
#include "mavi-feedback.hpp"
#include "mavi-sensors.hpp"
#include "mavi-calib.hpp"
#include "mavi-state.hpp"
#include "mavi-math.hpp"

using namespace std;

unsigned int t_analysisStart;
double irSDist = -1.0, irMDist = -1.0, irLDist = -1.0, slope = 0, usLDist = -1.0, usRDist = -1.0;

MaviNextStepKind maviNextStepScan(void)
{
	double relativeDif_IRS;

	irSDist = maviIRFilter.poll(MAVI_SENSOR_IRS);

	if (irSDist == MAVI_BAD_SENSOR_READING)
		return MAVI_NEXTSTEP_ERROR;

	relativeDif_IRS = MAVI_REF_DIST_IRS - irSDist;

	if (abs(relativeDif_IRS) < MAVI_ERROR_IRS)
		return MAVI_NEXTSTEP_NOTHING;
	else if (relativeDif_IRS >= MAVI_STAIR_HEIGHT_MIN && relativeDif_IRS <= MAVI_STAIR_HEIGHT_MAX)
		return MAVI_NEXTSTEP_STEP_UP;
	else if (relativeDif_IRS <= -MAVI_STAIR_HEIGHT_MIN) // && relativeDif_IRS <= -MAVI_STAIR_HEIGHT_MAX)
		return MAVI_NEXTSTEP_STEP_DOWN;
	else
		return MAVI_NEXTSTEP_OBSTACLE;
}

MaviSlopeKind maviSlopeScan(void)
{
	double relativeDif_IRM, relativeDif_IRL;

	irMDist = maviIRFilter.poll(MAVI_SENSOR_IRM);
	irLDist = maviIRFilter.poll(MAVI_SENSOR_IRL);
	slope = maviGetSlope(irSDist, irMDist, irLDist);

	if (irMDist == MAVI_BAD_SENSOR_READING || irLDist == MAVI_BAD_SENSOR_READING)
		return MAVI_SLOPE_ERROR;

	relativeDif_IRM = MAVI_REF_DIST_IRM - irMDist;
	relativeDif_IRL = MAVI_REF_DIST_IRL - irLDist;

	if (abs(slope) < MAVI_ERROR_SLOPE)
	{
		if (relativeDif_IRM >= 1.2 * MAVI_STAIR_HEIGHT_MIN && relativeDif_IRM <= 1.2 * MAVI_STAIR_HEIGHT_MAX)
			return MAVI_SLOPE_FLAT_STEP;
		else
			return MAVI_SLOPE_FLAT;
	}
	else if (relativeDif_IRM >= MAVI_STAIR_HEIGHT_MIN && relativeDif_IRL >= MAVI_STAIR_HEIGHT_MIN
			&& abs(slope) >= MAVI_STAIR_SLOPE_MIN && abs(slope) <= MAVI_STAIR_SLOPE_MAX)
	{
		if (slope > 0)
			return MAVI_SLOPE_ASCENDING;
		else
			return MAVI_SLOPE_DESCENDING;
	}
	else if (relativeDif_IRM < -MAVI_ERROR_IRM && relativeDif_IRL < -MAVI_ERROR_IRL)
			return MAVI_SLOPE_DESCENDING;
	else
	{
		return MAVI_SLOPE_OTHER;
	}
}

MaviMidRangeKind maviMidRangeScan(void)
{
	usLDist = maviUSFilter.poll(MAVI_SENSOR_USL);
	usRDist = maviUSFilter.poll(MAVI_SENSOR_USR);

	if (usLDist == MAVI_BAD_SENSOR_READING || usRDist == MAVI_BAD_SENSOR_READING)
		return MAVI_MIDRANGE_ERROR;

	MaviMidRangeKind scanResult = MAVI_MIDRANGE_NOTHING;

	if (usLDist > MAVI_MIN_DIST_USL && usLDist <= MAVI_MAX_DIST_USL) scanResult |= MAVI_MIDRANGE_LEFT;
	if (usRDist > MAVI_MIN_DIST_USR && usRDist <= MAVI_MAX_DIST_USR) scanResult |= MAVI_MIDRANGE_RIGHT;

	return scanResult;
}

void maviStairAssistance(MaviSlopeKind stair_slope)
{
	unsigned int t_elapsed;

	while (maviGetState() == MAVI_STATE_RUNNING)
	{
		t_elapsed = millis() - t_analysisStart;

		if (t_elapsed < MAVI_ANALYSIS_SAMPLE_PERIOD)
			delay(MAVI_ANALYSIS_SAMPLE_PERIOD - t_elapsed);

		t_analysisStart = millis();

		switch (maviNextStepScan())
		{
		case MAVI_NEXTSTEP_NOTHING:
			cout << "ENDING STAIR ASSISTANCE" << endl << endl;
			return;

		case MAVI_NEXTSTEP_STEP_UP:
			switch (maviSlopeScan())
			{
			case MAVI_SLOPE_ASCENDING:
				if (stair_slope != MAVI_SLOPE_ASCENDING)
				{
					maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				}
				break;

			case MAVI_SLOPE_FLAT_STEP:
				maviSendFeedback(MAVI_FEEDBACK_STEP_FINAL);
				cout << endl << "ENDING STAIR ASSISTANCE" << endl << endl;
				delay(MAVI_ANALYSIS_SAMPLE_PERIOD);
				return;

			case MAVI_SLOPE_DESCENDING:
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_FLAT:
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_OTHER:
//				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
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
				if (stair_slope != MAVI_SLOPE_DESCENDING)
					maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_FLAT_STEP:
				maviSendFeedback(MAVI_FEEDBACK_STEP_FINAL);
				cout << "ENDING STAIR ASSISTANCE" << endl << endl;
				delay(MAVI_ANALYSIS_SAMPLE_PERIOD);
				return;

			case MAVI_SLOPE_ASCENDING:
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_FLAT:
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_OTHER:
//				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			default:
				cout << "Sensing and Analysis Error: Received invalid slope scan data.";
				break;
			}

			break;

		case MAVI_NEXTSTEP_OBSTACLE:
			switch (maviSlopeScan())
			{
			case MAVI_SLOPE_DESCENDING:
				if (stair_slope != MAVI_SLOPE_DESCENDING)
					maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_FLAT_STEP:
				maviSendFeedback(MAVI_FEEDBACK_STEP_FINAL);
				cout << "ENDING STAIR ASSISTANCE" << endl << endl;
				delay(MAVI_ANALYSIS_SAMPLE_PERIOD);
				return;

			case MAVI_SLOPE_ASCENDING:
				if (stair_slope != MAVI_SLOPE_ASCENDING)
					maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_FLAT:
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_OTHER:
//				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			default:
				cout << "Sensing and Analysis Error: Received invalid slope scan data.";
				break;
			}

		default:
			cout << "Sensing and Analysis Error: Received invalid next step data.";
			break;
		}
	}

	return;
}

void maviMobilityAssistance(void)
{
	unsigned int t_elapsed;
	MaviSlopeKind stairVerification = MAVI_SLOPE_NULL;

	t_analysisStart = millis();
	t_lastVerbalOutput = millis();
	t_lastVibrationOutput = millis();

	maviSendFeedback(MAVI_FEEDBACK_SYSTEM_READY);

	while (maviGetState() == MAVI_STATE_RUNNING)
	{
		t_elapsed = millis() - t_analysisStart;

		if (t_elapsed < MAVI_ANALYSIS_SAMPLE_PERIOD)
			delay(MAVI_ANALYSIS_SAMPLE_PERIOD - t_elapsed);

		t_analysisStart = millis();

		switch (maviNextStepScan())
		{
		case MAVI_NEXTSTEP_NOTHING:
			switch (maviSlopeScan())
			{
			case MAVI_SLOPE_FLAT:
				switch (maviMidRangeScan())
				{
				case MAVI_MIDRANGE_BOTH:
					maviSendFeedback(MAVI_FEEDBACK_VIBRATE_LR);
					break;

				case MAVI_MIDRANGE_LEFT:
					maviSendFeedback(MAVI_FEEDBACK_VIBRATE_LEFT);
					break;

				case MAVI_MIDRANGE_RIGHT:
					maviSendFeedback(MAVI_FEEDBACK_VIBRATE_RIGHT);
					break;

				case MAVI_MIDRANGE_NOTHING:
					break;

				default:
					cout << "Sensing and Analysis Error: Received invalid mid range scan data.";
					break;
				}
				stairVerification = MAVI_SLOPE_NULL;
				break;

			case MAVI_SLOPE_FLAT_STEP:
				switch (maviMidRangeScan())
				{
				case MAVI_MIDRANGE_BOTH:
					maviSendFeedback(MAVI_FEEDBACK_VIBRATE_ALL);
					break;

				case MAVI_MIDRANGE_LEFT:
					maviSendFeedback(MAVI_FEEDBACK_VIBRATE_CL);
					break;

				case MAVI_MIDRANGE_RIGHT:
					maviSendFeedback(MAVI_FEEDBACK_VIBRATE_CR);
					break;

				case MAVI_MIDRANGE_NOTHING:
					maviSendFeedback(MAVI_FEEDBACK_VIBRATE_CENTER);
					break;

				default:
					cout << "Sensing and Analysis Error: Received invalid mid range scan data.";
					break;
				}
				stairVerification = MAVI_SLOPE_NULL;
				break;

			case MAVI_SLOPE_OTHER:
				switch (maviMidRangeScan())
				{
				case MAVI_MIDRANGE_BOTH:
					maviSendFeedback(MAVI_FEEDBACK_VIBRATE_ALL);
					break;

				case MAVI_MIDRANGE_LEFT:
					maviSendFeedback(MAVI_FEEDBACK_VIBRATE_CL);
					break;

				case MAVI_MIDRANGE_RIGHT:
					maviSendFeedback(MAVI_FEEDBACK_VIBRATE_CR);
					break;

				case MAVI_MIDRANGE_NOTHING:
					maviSendFeedback(MAVI_FEEDBACK_VIBRATE_CENTER);
					break;

				default:
					cout << "Sensing and Analysis Error: Received invalid mid range scan data.";
					break;
				}
				stairVerification = MAVI_SLOPE_NULL;
				break;

			case MAVI_SLOPE_ASCENDING:
				if (stairVerification == MAVI_SLOPE_ASCENDING)
					maviSendFeedback(MAVI_FEEDBACK_STAIRS_ASC);
				else
					stairVerification = MAVI_SLOPE_ASCENDING;
				break;

			case MAVI_SLOPE_DESCENDING:
				if (stairVerification == MAVI_SLOPE_DECENDING)
					maviSendFeedback(MAVI_FEEDBACK_STAIRS_DESC);
				else
					stairVerification = MAVI_SLOPE_DESCENDING;
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
				maviSendFeedback(MAVI_FEEDBACK_STEP_FIRSTUP);
				cout << "STARTING STAIR ASSISTANCE" << endl;
				maviStairAssistance(MAVI_SLOPE_ASCENDING);
				break;

			case MAVI_SLOPE_FLAT_STEP:
				maviSendFeedback(MAVI_FEEDBACK_STEP_SINGLEUP);
				break;

			case MAVI_SLOPE_DESCENDING:
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_FLAT:
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_OTHER:
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			default:
				cout << "Sensing and Analysis Error: Received invalid slope scan data.";
				break;
			}

			stairVerification = MAVI_SLOPE_NULL;
			break;

		case MAVI_NEXTSTEP_STEP_DOWN:
			switch (maviSlopeScan())
			{
			case MAVI_SLOPE_DESCENDING:
				maviSendFeedback(MAVI_FEEDBACK_STEP_FIRSTDOWN);
				cout << "STARTING STAIR ASSISTANCE" << endl;
				maviStairAssistance(MAVI_SLOPE_DESCENDING);
				break;

			case MAVI_SLOPE_FLAT_STEP:
				maviSendFeedback(MAVI_FEEDBACK_STEP_SINGLEDOWN);
				break;

			case MAVI_SLOPE_ASCENDING:
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_FLAT:
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_OTHER:
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			default:
				cout << "Sensing and Analysis Error: Received invalid slope scan data.";
				break;
			}

			stairVerification = MAVI_SLOPE_NULL;
			break;

		case MAVI_NEXTSTEP_OBSTACLE:
			switch (maviSlopeScan())
			{
			case MAVI_SLOPE_ASCENDING:
				maviSendFeedback(MAVI_FEEDBACK_STEP_FIRSTUP);
				cout << "STARTING STAIR ASSISTANCE" << endl;
				maviStairAssistance(MAVI_SLOPE_ASCENDING);
				break;

			case MAVI_SLOPE_FLAT_STEP:
				maviSendFeedback(MAVI_FEEDBACK_STEP_SINGLEUP);
				break;

			case MAVI_SLOPE_DESCENDING:
				maviSendFeedback(MAVI_FEEDBACK_STEP_FIRSTDOWN);
				cout << "STARTING STAIR ASSISTANCE" << endl;
				maviStairAssistance(MAVI_SLOPE_DESCENDING);
				break;

			case MAVI_SLOPE_FLAT:
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_OTHER:
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			default:
				cout << "Sensing and Analysis Error: Received invalid slope scan data.";
				break;
			}

			stairVerification = MAVI_SLOPE_NULL;
			break;

		default:
			cout << "Sensing and Analysis Error: Received invalid next step data.";
			break;
		}
	}

	return;
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
				maviSendFeedback(MAVI_FEEDBACK_SYSTEM_PAUSED);
			}

			delay(MAVI_ANALYSIS_SAMPLE_PERIOD);
		}
		else
		{
			pauseFeedbackFlag = true;
			maviStartAllFilters();
			maviMobilityAssistance();
			maviStopAllFilters();
		}
	}

	return NULL;
}
