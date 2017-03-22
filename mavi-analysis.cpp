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

	relativeDif_IRS = refDistIRS - irSDist;

	if (abs(relativeDif_IRS) < MAVI_ERROR_MARGIN_IRS)
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

	relativeDif_IRM = refDistIRM - irMDist;
	relativeDif_IRL = refDistIRL - irLDist;

	if (relativeDif_IRM < -MAVI_ERROR_MARGIN_IRM && relativeDif_IRL < -MAVI_ERROR_MARGIN_IRL)
		return MAVI_SLOPE_DESCENDING;

	if (abs(slope) < MAVI_ERROR_MARGIN_SLOPE)
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

	if (usLDist > MAVI_ERROR_MARGIN_USL && usLDist <= refDistUSL) scanResult |= MAVI_MIDRANGE_LEFT;
	if (usRDist > MAVI_ERROR_MARGIN_USR && usRDist <= refDistUSR) scanResult |= MAVI_MIDRANGE_RIGHT;

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
			switch (maviSlopeScan())
			{
			case MAVI_SLOPE_FLAT:
				cout << "Next Step: Safe 	Slope: Flat		Mid Range: NULL" << endl;
				break;

			case MAVI_SLOPE_FLAT_STEP:
				cout << "Next Step: Safe 	Slope: Flat Step	Mid Range: NULL" << endl;
				maviSendFeedback(MAVI_FEEDBACK_VIBRATE_CENTER);
				break;

			case MAVI_SLOPE_OTHER:
				cout << "Next Step: Safe 	Slope: Other	Mid Range: NULL" << endl;
				maviSendFeedback(MAVI_FEEDBACK_VIBRATE_CENTER);
				break;

			case MAVI_SLOPE_ASCENDING:
				cout << "Next Step: Nothing		Slope: Ascending	Mid Range: NULL" << endl;
				maviSendFeedback(MAVI_FEEDBACK_STAIRS_ASC);
				break;

			case MAVI_SLOPE_DESCENDING:
				cout << "Next Step: Nothing		Slope: Descending	Mid Range: NULL" << endl;
				maviSendFeedback(MAVI_FEEDBACK_STAIRS_DESC);
				break;

			default:
				cout << "Sensing and Analysis Error: Received invalid slope scan data.";
				break;
			}

			cout << endl;
			return;

		case MAVI_NEXTSTEP_STEP_UP:
			switch (maviSlopeScan())
			{
			case MAVI_SLOPE_ASCENDING:
				if (stair_slope == MAVI_SLOPE_ASCENDING)
				{
					cout << "Next Step: Up		Slope: Ascending	Mid Range: NULL" << endl;
				}
				else
				{
					cout << "Next Step: Up		Slope: Ascending	Mid Range: NULL" << endl;
					maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				}

				break;

			case MAVI_SLOPE_FLAT_STEP:
				cout << "Next Step: Up	Slope: Flat Step	Mid Range: NULL" << endl;
				maviSendFeedback(MAVI_FEEDBACK_STEP_FINAL);
				cout << "ENDING STAIR ASSISTANCE" << endl << endl;
				return;

			case MAVI_SLOPE_DESCENDING:
				cout << "Next Step: Up	Slope: Descending	Mid Range: NULL" << endl;
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_FLAT:
				cout << "Next Step: Up 	Slope: Flat		Mid Range: NULL" << endl;
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_OTHER:
				cout << "Next Step: Up	Slope: Other	Mid Range: NULL" << endl;
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
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
				if (stair_slope == MAVI_SLOPE_DESCENDING)
					cout << "Next Step: Down 	Slope: Descending	Mid Range: NULL" << endl;
				else
				{
					cout << "Next Step: Down 	Slope: Descending	Mid Range: NULL" << endl;
					maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				}
				break;

			case MAVI_SLOPE_FLAT_STEP:
				cout << "Next Step: Down 	Slope: Flat Step	Mid Range: NULL" << endl;
				maviSendFeedback(MAVI_FEEDBACK_STEP_FINAL);
				cout << "ENDING STAIR ASSISTANCE" << endl << endl;
				return;

			case MAVI_SLOPE_ASCENDING:
				cout << "Next Step: Down 	Slope: Ascending	Mid Range: NULL" << endl;
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_FLAT:
				cout << "Next Step: Down 	Slope: Flat		Mid Range: NULL" << endl;
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_OTHER:
				cout << "Next Step: Down 	Slope: Other	Mid Range: NULL" << endl;
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			default:
				cout << "Sensing and Analysis Error: Received invalid slope scan data.";
				break;
			}

			break;

		case MAVI_NEXTSTEP_OBSTACLE:
			cout << "Next step: Obstacle	Slope: NULL		Mid Range: NULL" << endl;
			maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
			break;

		default:
			cout << "Sensing and Analysis Error: Received invalid next step data.";
			break;
		}

		cout << endl;
	}

	return;
}

void maviMobilityAssistance(void)
{
	unsigned int t_elapsed;

	t_analysisStart = 0;
	t_lastVerbalOutput = 0;
	t_lastVibrationOutput = 0;

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
					cout << "Next Step: Nothing		Slope: Flat	Mid Range: Left & Right" << endl;
					maviSendFeedback(MAVI_FEEDBACK_VIBRATE_LR);
					break;

				case MAVI_MIDRANGE_LEFT:
					cout << "Next Step: Nothing		Slope: Flat	Mid Range: Left" << endl;
					maviSendFeedback(MAVI_FEEDBACK_VIBRATE_LEFT);
					break;

				case MAVI_MIDRANGE_RIGHT:
					cout << "Next Step: Nothing		Slope: Flat	Mid Range: Right" << endl;
					maviSendFeedback(MAVI_FEEDBACK_VIBRATE_RIGHT);
					break;

				case MAVI_MIDRANGE_NOTHING:
					cout << "Next Step: Nothing		Slope: Flat	Mid Range: Nothing" << endl;
					break;

				default:
					cout << "Sensing and Analysis Error: Received invalid mid range scan data.";
					break;
				}
				break;

			case MAVI_SLOPE_FLAT_STEP:
				switch (maviMidRangeScan())
				{
				case MAVI_MIDRANGE_BOTH:
					cout << "Next Step: Safe 	Slope: Flat Step	Mid Range: Left & Right" << endl;
					maviSendFeedback(MAVI_FEEDBACK_VIBRATE_ALL);
					break;

				case MAVI_MIDRANGE_LEFT:
					cout << "Next Step: Safe 	Slope: Flat Step	Mid Range: Left" << endl;
					maviSendFeedback(MAVI_FEEDBACK_VIBRATE_CL);
					break;

				case MAVI_MIDRANGE_RIGHT:
					cout << "Next Step: Safe 	Slope: Flat Step	Mid Range: Right" << endl;
					maviSendFeedback(MAVI_FEEDBACK_VIBRATE_CR);
					break;

				case MAVI_MIDRANGE_NOTHING:
					cout << "Next Step: Safe 	Slope: Flat Step	Mid Range: Nothing" << endl;
					maviSendFeedback(MAVI_FEEDBACK_VIBRATE_CENTER);
					break;

				default:
					cout << "Sensing and Analysis Error: Received invalid mid range scan data.";
					break;
				}
				break;

			case MAVI_SLOPE_OTHER:
				switch (maviMidRangeScan())
				{
				case MAVI_MIDRANGE_BOTH:
					cout << "Next Step: Safe 	Slope: Other	Mid Range: Left & Right" << endl;
					maviSendFeedback(MAVI_FEEDBACK_VIBRATE_ALL);
					break;

				case MAVI_MIDRANGE_LEFT:
					cout << "Next Step: Safe 	Slope: Other	Mid Range: Left" << endl;
					maviSendFeedback(MAVI_FEEDBACK_VIBRATE_CL);
					break;

				case MAVI_MIDRANGE_RIGHT:
					cout << "Next Step: Safe 	Slope: Other	Mid Range: Right" << endl;
					maviSendFeedback(MAVI_FEEDBACK_VIBRATE_CR);
					break;

				case MAVI_MIDRANGE_NOTHING:
					cout << "Next Step: Safe 	Slope: Other	Mid Range: Nothing" << endl;
					maviSendFeedback(MAVI_FEEDBACK_VIBRATE_CENTER);
					break;

				default:
					cout << "Sensing and Analysis Error: Received invalid mid range scan data.";
					break;
				}
				break;

			case MAVI_SLOPE_ASCENDING:
				cout << "Next Step: Nothing		Slope: Ascending	Mid Range: NULL" << endl;
				maviSendFeedback(MAVI_FEEDBACK_STAIRS_ASC);
				break;

			case MAVI_SLOPE_DESCENDING:
				cout << "Next Step: Nothing		Slope: Descending	Mid Range: NULL" << endl;
				maviSendFeedback(MAVI_FEEDBACK_STAIRS_DESC);
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
				cout << "Next Step: Up	Slope: Ascending	Mid Range: NULL" << endl;
				maviSendFeedback(MAVI_FEEDBACK_STEP_FIRSTUP);
				cout << "STARTING STAIR ASSISTANCE" << endl;
				maviStairAssistance(MAVI_SLOPE_ASCENDING);
				break;

			case MAVI_SLOPE_FLAT_STEP:
				cout << "Next Step: Up	Slope: Flat Step	Mid Range: NULL" << endl;
				maviSendFeedback(MAVI_FEEDBACK_STEP_SINGLEUP);
				break;

			case MAVI_SLOPE_DESCENDING:
				cout << "Next Step: Up	Slope: Descending	Mid Range: NULL" << endl;
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_FLAT:
				cout << "Next Step: Up 	Slope: Flat		Mid Range: NULL" << endl;
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_OTHER:
				cout << "Next Step: Up	Slope: Other	Mid Range: NULL" << endl;
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
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
				cout << "Next Step: Down 	Slope: Descending	Mid Range: NULL" << endl;
				maviSendFeedback(MAVI_FEEDBACK_STEP_FIRSTDOWN);
				cout << "STARTING STAIR ASSISTANCE" << endl;
				maviStairAssistance(MAVI_SLOPE_DESCENDING);
				break;

			case MAVI_SLOPE_FLAT_STEP:
				cout << "Next Step: Down 	Slope: Flat Step	Mid Range: NULL" << endl;
				maviSendFeedback(MAVI_FEEDBACK_STEP_SINGLEDOWN);
				break;

			case MAVI_SLOPE_ASCENDING:
				cout << "Next Step: Down 	Slope: Ascending	Mid Range: NULL" << endl;
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_FLAT:
				cout << "Next Step: Down 	Slope: Flat		Mid Range: NULL" << endl;
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_OTHER:
				cout << "Next Step: Down 	Slope: Other	Mid Range: NULL" << endl;
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			default:
				cout << "Sensing and Analysis Error: Received invalid slope scan data.";
				break;
			}
			break;

		case MAVI_NEXTSTEP_OBSTACLE:
			cout << "Next step: Obstacle	Slope: NULL		Mid Range: NULL" << endl;
			maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
			break;

		default:
			cout << "Sensing and Analysis Error: Received invalid next step data.";
			break;
		}

		cout << endl;
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
			maviSendFeedback(MAVI_FEEDBACK_SYSTEM_READY);
			maviMobilityAssistance();
			maviStopAllFilters();
		}
	}

	maviSendFeedback(MAVI_FEEDBACK_SYSTEM_SHUTDOWN);

	return NULL;
}
