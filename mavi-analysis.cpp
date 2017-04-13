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
double irSDist = -1.0, irMDist = -1.0, irLDist = -1.0, srDist = -1.0, usLLDist = -1.0, usLRDist = -1.0, usULDist = -1.0, usURDist = -1.0, slope = 0;

MaviNextStepKind maviNextStepScan(void)
{
	double relativeDif_IRS;

	irSDist = maviIRFilter.poll(MAVI_SENSOR_IRS);

	if (irSDist == MAVI_BAD_SENSOR_READING)
	{
		cout << "IRS: Bad sensor reading" << endl;
		return MAVI_NEXTSTEP_ERROR;
	}

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
	slope = maviGetSlope(irMDist, irLDist);

	if (irMDist == MAVI_BAD_SENSOR_READING || irLDist == MAVI_BAD_SENSOR_READING)
		return MAVI_SLOPE_ERROR;

	relativeDif_IRM = MAVI_REF_DIST_IRM - irMDist;
	relativeDif_IRL = MAVI_REF_DIST_IRL - irLDist;

	if (abs(slope) < MAVI_ERROR_SLOPE)
	{
		if (relativeDif_IRM >= 1.2 * MAVI_STAIR_HEIGHT_MIN && relativeDif_IRM <= 1.2 * MAVI_STAIR_HEIGHT_MAX)
			return MAVI_SLOPE_FLAT_STEP;
		else if (relativeDif_IRM <= MAVI_ERROR_IRM && relativeDif_IRL <= MAVI_ERROR_IRL)
			return MAVI_SLOPE_FLAT;

		return MAVI_SLOPE_OTHER;
	}
	else if (relativeDif_IRM >= 1.2 * MAVI_STAIR_HEIGHT_MIN && relativeDif_IRL >= 1.2 * MAVI_STAIR_HEIGHT_MIN
			&& abs(slope) >= MAVI_STAIR_SLOPE_MIN && abs(slope) <= MAVI_STAIR_SLOPE_MAX)
	{
		return slope > 0 ? MAVI_SLOPE_ASCENDING : MAVI_SLOPE_DESCENDING;
	}
	else if (relativeDif_IRL < -MAVI_ERROR_IRL)
	{
		return MAVI_SLOPE_DESCENDING;
	}
	else
	{
		return MAVI_SLOPE_OTHER;
	}
}

MaviLongRangeKind maviLongRangeScan(void)
{
	srDist = maviSRFilter.poll(MAVI_SENSOR_SR);

	if (srDist == MAVI_BAD_SENSOR_READING)
		return MAVI_LONGRANGE_ERROR;

	if (srDist >= MAVI_MIN_DIST_SR && srDist <= MAVI_MAX_DIST_SR)
		return MAVI_LONGRANGE_OBSTACLE;
	else
		return MAVI_LONGRANGE_NOTHING;
}

MaviMidRangeKind maviMidRangeScan(void)
{
	usLLDist = maviUSLFilter.poll(MAVI_SENSOR_USLL);
	usLRDist = maviUSLFilter.poll(MAVI_SENSOR_USLR);

	if (usLLDist == MAVI_BAD_SENSOR_READING || usLRDist == MAVI_BAD_SENSOR_READING)
		return MAVI_MIDRANGE_ERROR;

	MaviMidRangeKind scanResult = MAVI_MIDRANGE_NOTHING;

	if (usLLDist > MAVI_MIN_DIST_USLL && usLLDist <= MAVI_MAX_DIST_USLL) scanResult |= MAVI_MIDRANGE_LEFT;
	if (usLRDist > MAVI_MIN_DIST_USLR && usLRDist <= MAVI_MAX_DIST_USLR) scanResult |= MAVI_MIDRANGE_RIGHT;

	return scanResult;
}

MaviLowHangKind maviLowHangScan(void)
{
	usULDist = maviUSUFilter.poll(MAVI_SENSOR_USUL);
	usURDist = maviUSUFilter.poll(MAVI_SENSOR_USUR);

	if (usULDist == MAVI_BAD_SENSOR_READING || usURDist == MAVI_BAD_SENSOR_READING)
		return MAVI_LOWHANG_ERROR;

	MaviMidRangeKind scanResult = MAVI_LOWHANG_NOTHING;

	if (usULDist > MAVI_MIN_DIST_USUL && usULDist <= MAVI_MAX_DIST_USUL) scanResult |= MAVI_LOWHANG_LEFT;
	if (usURDist > MAVI_MIN_DIST_USUR && usURDist <= MAVI_MAX_DIST_USUR) scanResult |= MAVI_LOWHANG_RIGHT;

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
			maviSendFeedback(MAVI_FEEDBACK_STEP_FINAL);
			cout << endl << "ENDING STAIR ASSISTANCE (301)" << endl << endl;
			delay(2 * MAVI_ANALYSIS_SAMPLE_PERIOD);
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
				cout << endl << "ENDING STAIR ASSISTANCE (302)" << endl << endl;
				delay(2 * MAVI_ANALYSIS_SAMPLE_PERIOD);
				return;

			case MAVI_SLOPE_DESCENDING:
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_FLAT:
				//~ maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_OTHER:
				//~ maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_ERROR:
				maviSendFeedback(MAVI_FEEDBACK_WARNING_SENSORFAILURE);
				break;

			default:
				cout << "Sensing and Analysis Error (221): Received invalid slope scan data.";
				break;
			}

			break;

		case MAVI_NEXTSTEP_STEP_DOWN:
			switch (maviSlopeScan())
			{
			case MAVI_SLOPE_DESCENDING:
				if (stair_slope != MAVI_SLOPE_DESCENDING)
				{
					maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				}
				break;

			case MAVI_SLOPE_FLAT_STEP:
				maviSendFeedback(MAVI_FEEDBACK_STEP_FINAL);
				cout << "ENDING STAIR ASSISTANCE (303)" << endl << endl;
				delay(2 * MAVI_ANALYSIS_SAMPLE_PERIOD);
				return;

			case MAVI_SLOPE_ASCENDING:
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_FLAT:
				//maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_OTHER:
				//~ maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_ERROR:
				maviSendFeedback(MAVI_FEEDBACK_WARNING_SENSORFAILURE);
				break;

			default:
				cout << "Sensing and Analysis Error (222): Received invalid slope scan data.";
				break;
			}

			break;

		case MAVI_NEXTSTEP_OBSTACLE:
			switch (maviSlopeScan())
			{
			case MAVI_SLOPE_DESCENDING:
				if (stair_slope != MAVI_SLOPE_DESCENDING)
				{
					maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				}
				break;

			case MAVI_SLOPE_FLAT_STEP:
				maviSendFeedback(MAVI_FEEDBACK_STEP_FINAL);
				cout << "ENDING STAIR ASSISTANCE (304)" << endl << endl;
				delay(2 * MAVI_ANALYSIS_SAMPLE_PERIOD);
				return;

			case MAVI_SLOPE_ASCENDING:
				if (stair_slope != MAVI_SLOPE_ASCENDING)
				{
					maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				}
				break;

			case MAVI_SLOPE_FLAT:
				//~ maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_OTHER:
				//~ maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				break;

			case MAVI_SLOPE_ERROR:
				maviSendFeedback(MAVI_FEEDBACK_WARNING_SENSORFAILURE);
				break;

			default:
				cout << "Sensing and Analysis Error (223): Received invalid slope scan data.";
				break;
			}

			break;

		case MAVI_NEXTSTEP_ERROR:
			maviSendFeedback(MAVI_FEEDBACK_WARNING_SENSORFAILURE);
			break;

		default:
			cout << "Sensing and Analysis Error (211): Received invalid next step data.";
			break;
		}
	}

	cout << "ENDING STAIR ASSISTANCE (300)" << endl << endl;
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
				switch (maviLongRangeScan())
				{
				case MAVI_LONGRANGE_NOTHING:
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
						switch(maviLowHangScan())
						{
						case MAVI_LOWHANG_BOTH:
						case MAVI_LOWHANG_LEFT:
						case MAVI_LOWHANG_RIGHT:
							maviSendFeedback(MAVI_FEEDBACK_LOW_HAZARD);
							break;

						case MAVI_LOWHANG_NOTHING:
							break;

						case MAVI_LOWHANG_ERROR:
							maviSendFeedback(MAVI_FEEDBACK_WARNING_SENSORFAILURE);
							break;

						default:
							break;
						}
						break;

					case MAVI_MIDRANGE_ERROR:
						maviSendFeedback(MAVI_FEEDBACK_WARNING_SENSORFAILURE);
						break;

					default:
						cout << "Sensing and Analysis Error (131): Received invalid mid range scan data.";
						break;
					}
					break;

				case MAVI_LONGRANGE_OBSTACLE:
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

					case MAVI_MIDRANGE_ERROR:
						maviSendFeedback(MAVI_FEEDBACK_WARNING_SENSORFAILURE);
						break;

					default:
						cout << "Sensing and Analysis Error (132): Received invalid mid range scan data.";
						break;
					}
					break;

				case MAVI_LONGRANGE_ERROR:
					maviSendFeedback(MAVI_FEEDBACK_WARNING_SENSORFAILURE);
					break;

				default:
					cout << "Sensing and Analysis Error (141): Received invalid long range scan data.";
					break;
				}

				stairVerification = MAVI_SLOPE_NULL;
				break;

			case MAVI_SLOPE_FLAT_STEP:
				maviSendFeedback(MAVI_FEEDBACK_VIBRATE_CENTER);
				stairVerification = MAVI_SLOPE_NULL;
				break;

			case MAVI_SLOPE_OTHER:
				maviSendFeedback(MAVI_FEEDBACK_VIBRATE_CENTER);
				stairVerification = MAVI_SLOPE_NULL;
				break;

			case MAVI_SLOPE_ASCENDING:
				if (stairVerification == MAVI_SLOPE_ASCENDING)
					maviSendFeedback(MAVI_FEEDBACK_STAIRS_ASC);
				else
					stairVerification = MAVI_SLOPE_ASCENDING;
				break;

			case MAVI_SLOPE_DESCENDING:
				if (stairVerification == MAVI_SLOPE_DESCENDING)
					maviSendFeedback(MAVI_FEEDBACK_STAIRS_DESC);
				else
					stairVerification = MAVI_SLOPE_DESCENDING;
				break;

			case MAVI_SLOPE_ERROR:
				maviSendFeedback(MAVI_FEEDBACK_WARNING_SENSORFAILURE);
				break;

			default:
				cout << "Sensing and Analysis Error (121): Received invalid slope scan data.";
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

			case MAVI_SLOPE_ERROR:
				maviSendFeedback(MAVI_FEEDBACK_WARNING_SENSORFAILURE);
				break;

			default:
				cout << "Sensing and Analysis Error (122): Received invalid slope scan data.";
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

			case MAVI_SLOPE_ERROR:
				maviSendFeedback(MAVI_FEEDBACK_WARNING_SENSORFAILURE);
				break;

			default:
				cout << "Sensing and Analysis Error (123): Received invalid slope scan data.";
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
				maviSendFeedback(MAVI_FEEDBACK_IM_HAZARD);
				//~ maviSendFeedback(MAVI_FEEDBACK_STEP_SINGLEUP);
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

			case MAVI_SLOPE_ERROR:
				maviSendFeedback(MAVI_FEEDBACK_WARNING_SENSORFAILURE);
				break;

			default:
				cout << "Sensing and Analysis Error (124): Received invalid slope scan data.";
				break;
			}

			stairVerification = MAVI_SLOPE_NULL;
			break;

		case MAVI_NEXTSTEP_ERROR:
			maviSendFeedback(MAVI_FEEDBACK_WARNING_SENSORFAILURE);
			break;

		default:
			cout << "Sensing and Analysis Error (111): Received invalid next step data.";
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
