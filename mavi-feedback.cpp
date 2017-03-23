/*
 * mavi-feedback.cpp
 * -----------------
 *
 * ...
 */

#include <cstdlib>
#include <iostream>
#include <wiringPi.h>

#include "mavi-analysis.hpp"
#include "mavi-feedback.hpp"
#include "mavi-audio.hpp"
#include "mavi-vibration.hpp"
#include "mavi-calib.hpp"

using namespace std;

bool resetTimer = true;
MaviFeedbackID last_interrupt_id;
unsigned int t_lastVibrationOutput, t_lastVerbalOutput;

void maviDebugDataOutput(MaviFeedbackID id)
{
	switch(id)
	{
	case MAVI_FEEDBACK_VIBRATE_CENTER:
		cout << "IRS: " << irSDist << endl;
		cout << "IRM: " << irMDist << endl;
		cout << "IRL: " << irLDist << endl;
		cout << "USL: " << usLDist << endl;
		cout << "USR: " << usRDist << endl;
		cout << "Slope: " << slope << endl;
		break;

	case MAVI_FEEDBACK_VIBRATE_LEFT:
		cout << "IRS: " << irSDist << endl;
		cout << "IRM: " << irMDist << endl;
		cout << "IRL: " << irLDist << endl;
		cout << "USL: " << usLDist << endl;
		cout << "USR: " << usRDist << endl;
		cout << "Slope: " << slope << endl;
		break;

	case MAVI_FEEDBACK_VIBRATE_RIGHT:
		cout << "IRS: " << irSDist << endl;
		cout << "IRM: " << irMDist << endl;
		cout << "IRL: " << irLDist << endl;
		cout << "USL: " << usLDist << endl;
		cout << "USR: " << usRDist << endl;
		cout << "Slope: " << slope << endl;
		break;

	case MAVI_FEEDBACK_VIBRATE_LR:
		cout << "IRS: " << irSDist << endl;
		cout << "IRM: " << irMDist << endl;
		cout << "IRL: " << irLDist << endl;
		cout << "USL: " << usLDist << endl;
		cout << "USR: " << usRDist << endl;
		cout << "Slope: " << slope << endl;
		break;

	case MAVI_FEEDBACK_VIBRATE_CL:
		cout << "IRS: " << irSDist << endl;
		cout << "IRM: " << irMDist << endl;
		cout << "IRL: " << irLDist << endl;
		cout << "USL: " << usLDist << endl;
		cout << "USR: " << usRDist << endl;
		cout << "Slope: " << slope << endl;
		break;

	case MAVI_FEEDBACK_VIBRATE_CR:
		cout << "IRS: " << irSDist << endl;
		cout << "IRM: " << irMDist << endl;
		cout << "IRL: " << irLDist << endl;
		cout << "USL: " << usLDist << endl;
		cout << "USR: " << usRDist << endl;
		cout << "Slope: " << slope << endl;
		break;

	case MAVI_FEEDBACK_VIBRATE_ALL:
		cout << "IRS: " << irSDist << endl;
		cout << "IRM: " << irMDist << endl;
		cout << "IRL: " << irLDist << endl;
		cout << "USL: " << usLDist << endl;
		cout << "USR: " << usRDist << endl;
		cout << "Slope: " << slope << endl;
		break;

	case MAVI_FEEDBACK_IM_HAZARD:
		cout << "IRS: " << irSDist << endl;
		break;

	case MAVI_FEEDBACK_STEP_FINAL:
		cout << "IRS: " << irSDist << endl;
		cout << "IRM: " << irMDist << endl;
		cout << "IRL: " << irLDist << endl;
		cout << "Slope: " << slope << endl;
		break;

	case MAVI_FEEDBACK_STEP_FIRSTDOWN:
		cout << "IRS: " << irSDist << endl;
		cout << "IRM: " << irMDist << endl;
		cout << "IRL: " << irLDist << endl;
		cout << "Slope: " << slope << endl;
		break;

	case MAVI_FEEDBACK_STEP_FIRSTUP:
		cout << "IRS: " << irSDist << endl;
		cout << "IRM: " << irMDist << endl;
		cout << "IRL: " << irLDist << endl;
		cout << "Slope: " << slope << endl;
		break;

	case MAVI_FEEDBACK_STEP_SINGLEDOWN:
		cout << "IRS: " << irSDist << endl;
		cout << "IRM: " << irMDist << endl;
		cout << "IRL: " << irLDist << endl;
		cout << "Slope: " << slope << endl;
		break;

	case MAVI_FEEDBACK_STEP_SINGLEUP:
		cout << "IRS: " << irSDist << endl;
		cout << "IRM: " << irMDist << endl;
		cout << "IRL: " << irLDist << endl;
		cout << "Slope: " << slope << endl;
		break;

	case MAVI_FEEDBACK_STAIRS_ASC:
		cout << "IRS: " << irSDist << endl;
		cout << "IRM: " << irMDist << endl;
		cout << "IRL: " << irLDist << endl;
		cout << "Slope: " << slope << endl;
		break;

	case MAVI_FEEDBACK_STAIRS_DESC:
		cout << "IRS: " << irSDist << endl;
		cout << "IRM: " << irMDist << endl;
		cout << "IRL: " << irLDist << endl;
		cout << "Slope: " << slope << endl;
		break;

	default:
		break;
	}

	cout << endl;
}

void maviOutputVibrationFeedback(MaviFeedbackID id)
{
	// Vibration notifications (output periodically)
	switch(id)
	{
	case MAVI_FEEDBACK_VIBRATE_CENTER:
		cout << "Vibrate Center" << endl;
		if (irMDist <= MAVI_REF_DIST_IRM * 0.33)
			maviVibrate(MAVI_VIB_C, MAVI_VIBRATION_DURATION, MAVI_VSPEED_HI);
		else if (irMDist <= MAVI_REF_DIST_IRM * 0.66)
			maviVibrate(MAVI_VIB_C, MAVI_VIBRATION_DURATION, MAVI_VSPEED_MD);
		else
			maviVibrate(MAVI_VIB_C, MAVI_VIBRATION_DURATION, MAVI_VSPEED_LO);
		break;

	case MAVI_FEEDBACK_VIBRATE_LEFT:
		cout << "Vibrate Left" << endl;
		if (usLDist <= MAVI_MAX_DIST_USL * 0.33)
			maviVibrate(MAVI_VIB_L, MAVI_VIBRATION_DURATION, MAVI_VSPEED_HI);
		else if (usLDist <= MAVI_MAX_DIST_USL * 0.66)
			maviVibrate(MAVI_VIB_L, MAVI_VIBRATION_DURATION, MAVI_VSPEED_MD);
		else
			maviVibrate(MAVI_VIB_L, MAVI_VIBRATION_DURATION, MAVI_VSPEED_LO);
		break;

	case MAVI_FEEDBACK_VIBRATE_RIGHT:
		cout << "Vibrate Right" << endl;
		if (usRDist <= MAVI_MAX_DIST_USR * 0.33)
			maviVibrate(MAVI_VIB_R, MAVI_VIBRATION_DURATION, MAVI_VSPEED_HI);
		else if (usRDist <= MAVI_MAX_DIST_USR * 0.66)
			maviVibrate(MAVI_VIB_R, MAVI_VIBRATION_DURATION, MAVI_VSPEED_MD);
		else
			maviVibrate(MAVI_VIB_R, MAVI_VIBRATION_DURATION, MAVI_VSPEED_LO);
		break;

	case MAVI_FEEDBACK_VIBRATE_LR:
		cout << "Vibrate Both" << endl;
		if (usLDist <= MAVI_MAX_DIST_USL * 0.33 || usRDist <= MAVI_MAX_DIST_USR * 0.33)
			maviVibrate(MAVI_VIB_L | MAVI_VIB_R, MAVI_VIBRATION_DURATION, MAVI_VSPEED_HI);
		else if (usList <= MAVI_MAX_DIST_USL * 0.66 || usRDist <= MAVI_MAX_DIST_USR * 0.66)
			maviVibrate(MAVI_VIB_L | MAVI_VIB_R, MAVI_VIBRATION_DURATION, MAVI_VSPEED_MD);
		else
			maviVibrate(MAVI_VIB_L | MAVI_VIB_R, MAVI_VIBRATION_DURATION, MAVI_VSPEED_LO);
		break;

	case MAVI_FEEDBACK_VIBRATE_CL:
		cout << "Vibrate Both" << endl;
		if (irMDist <= MAVI_REF_DIST_IRM * 0.33)
			maviVibrate(MAVI_VIB_C | MAVI_VIB_L, MAVI_VIBRATION_DURATION, MAVI_VSPEED_HI);
		else if (irMDist <= MAVI_REF_DIST_IRM * 0.66)
			maviVibrate(MAVI_VIB_C | MAVI_VIB_L, MAVI_VIBRATION_DURATION, MAVI_VSPEED_MD);
		else
			maviVibrate(MAVI_VIB_C | MAVI_VIB_L, MAVI_VIBRATION_DURATION, MAVI_VSPEED_LO);
		break;

	case MAVI_FEEDBACK_VIBRATE_CR:
		cout << "Vibrate Both" << endl;
		if (irMDist <= MAVI_REF_DIST_IRM * 0.33)
			maviVibrate(MAVI_VIB_C | MAVI_VIB_R, MAVI_VIBRATION_DURATION, MAVI_VSPEED_HI);
		else if (irMDist <= MAVI_REF_DIST_IRM * 0.66)
			maviVibrate(MAVI_VIB_C | MAVI_VIB_R, MAVI_VIBRATION_DURATION, MAVI_VSPEED_MD);
		else
			maviVibrate(MAVI_VIB_C | MAVI_VIB_R, MAVI_VIBRATION_DURATION, MAVI_VSPEED_LO);
		break;

	case MAVI_FEEDBACK_VIBRATE_ALL:
		cout << "Vibrate All" << endl;
		if (irMDist <= MAVI_REF_DIST_IRM * 0.33)
			maviVibrate(MAVI_VIB_C | MAVI_VIB_L | MAVI_VIB_R, MAVI_VIBRATION_DURATION, MAVI_VSPEED_HI);
		else if (irMDist <= MAVI_REF_DIST_IRM * 0.66)
			maviVibrate(MAVI_VIB_C | MAVI_VIB_L | MAVI_VIB_R, MAVI_VIBRATION_DURATION, MAVI_VSPEED_MD);
		else
			maviVibrate(MAVI_VIB_C | MAVI_VIB_L | MAVI_VIB_R, MAVI_VIBRATION_DURATION, MAVI_VSPEED_LO);
		break;

	default:
		resetTimer = false;
		break;
	}
}

void maviOutputSystemFeedback(MaviFeedbackID id)
{
	// system notifications (always output but never reset timer)
	switch(id)
	{
	case MAVI_FEEDBACK_SYSTEM_READY:
		cout << "System Ready" << endl << endl;
		maviAudioPlay(MAVI_AUDIO_SYSTEM_READY, MAVI_APRI_HI);
		return;

	case MAVI_FEEDBACK_SYSTEM_PAUSED:
		cout << "System Paused" << endl << endl;
		maviAudioPlay(MAVI_AUDIO_SYSTEM_PAUSED);
		return;

	case MAVI_FEEDBACK_SYSTEM_SHUTDOWN:
		cout << "System Shutting Down" << endl << endl;
		maviAudioPlay(MAVI_AUDIO_SYSTEM_SHUTDOWN);
		maviVibrate(MAVI_VIB_C | MAVI_VIB_L | MAVI_VIB_R, MAVI_VIBRATION_DURATION);
		return;

	default:
		break;
	}	
}	
	
void maviOutputVerbalFeedback(MaviFeedbackID id, bool interrupt)
{
	// normal notifications (only output periodically and reset timer)
	switch(id)
	{
	case MAVI_FEEDBACK_IM_HAZARD:
		cout << "Immediate Hazard" << endl;
		maviAudioPlay(MAVI_AUDIO_IM_HAZARD, MAVI_APRI_HI);
		break;

	case MAVI_FEEDBACK_STEP_FINAL:
		cout << "Final Step" << endl;
		maviAudioPlay(MAVI_AUDIO_STEP_FINAL, MAVI_APRI_MD);
		break;

	case MAVI_FEEDBACK_STEP_FIRSTDOWN:
		cout << "First Step Down" << endl;
		maviAudioPlay(MAVI_AUDIO_STEP_FIRSTDOWN, MAVI_APRI_HI);
		break;

	case MAVI_FEEDBACK_STEP_FIRSTUP:
		cout << "First Step Up" << endl;
		maviAudioPlay(MAVI_AUDIO_STEP_FIRSTUP, MAVI_APRI_HI);
		break;

	case MAVI_FEEDBACK_STEP_SINGLEDOWN:
		cout << "Single Step Down" << endl;
		maviAudioPlay(MAVI_AUDIO_STEP_SINGLEDOWN, MAVI_APRI_MD);
		break;

	case MAVI_FEEDBACK_STEP_SINGLEUP:
		cout << "Single Step Up" << endl;
		maviAudioPlay(MAVI_AUDIO_STEP_SINGLEUP, MAVI_APRI_MD);
		break;

	case MAVI_FEEDBACK_STAIRS_ASC:
		if(!interrupt)
		{
			cout << "Stairs Ahead: Ascending" << endl;
			maviAudioPlay(MAVI_AUDIO_STAIRS_ASC);
		}
		break;

	case MAVI_FEEDBACK_STAIRS_DESC:
		if (!interrupt)
		{
			cout << "Stairs Ahead: Descending" << endl;
			maviAudioPlay(MAVI_AUDIO_STAIRS_DESC);
		}
		break;

	default:
		resetTimer = false;
		break;
	}	
}

void maviSendFeedback(MaviFeedbackID id)
{

	maviOutputSystemFeedback(id);
	
	unsigned int t_current = millis();

	if ((t_current - t_lastVibrationOutput) >= MAVI_VIBRATION_OUTPUT_PERIOD)
	{
		maviOutputVibrationFeedback(id);

		if (resetTimer)
		{
			maviDebugDataOutput(id);
			t_lastVibrationOutput = millis();	
		}
		else
		{
			resetTimer = true;
		}
	}

	if ((t_current - t_lastVerbalOutput) >= MAVI_VERBAL_OUTPUT_PERIOD)
	{
		maviOutputVerbalFeedback(id, false);

		if (resetTimer)
		{
			maviDebugDataOutput(id);
			t_lastVerbalOutput = millis();
			lastInterrupt_id = MAVI_FEEDBACK_NULL;
		}
		else
		{
			resetTimer = true;
		}
	}
	else if (lastInterrupt_id != id)
	{
		maviOutputVerbalFeedback(id, true);
		
		if (resetTimer)
		{
			maviOutputDebugData(id);
			lastInterrupt_id = id;
		}
		else
		{
			resetTimer = true;
		}
	}
}

