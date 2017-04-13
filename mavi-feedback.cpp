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
MaviFeedbackID lastInterrupt_id;
unsigned int t_lastVibrationOutput, t_lastVerbalOutput;
double irSDist_old = -1.0, irMDist_old = -1.0, irLDist_old = -1.0, srDist_old = -1.0, usLLDist_old = -1.0, usLRDist_old = -1.0, usULDist_old = -1.0, usURDist_old = -1.0, slope_old = 0;

void maviOutputDebugData(MaviFeedbackID id)
{
	//~ switch(id)
	//~ {
	//~ case MAVI_FEEDBACK_VIBRATE_CENTER:
	//~ case MAVI_FEEDBACK_VIBRATE_LEFT:
	//~ case MAVI_FEEDBACK_VIBRATE_RIGHT:
	//~ case MAVI_FEEDBACK_VIBRATE_LR:
	//~ case MAVI_FEEDBACK_VIBRATE_CL:
	//~ case MAVI_FEEDBACK_VIBRATE_CR:
	//~ case MAVI_FEEDBACK_VIBRATE_ALL:
	//~ case MAVI_FEEDBACK_IM_HAZARD:
	//~ case MAVI_FEEDBACK_LOW_HAZARD:
	//~ case MAVI_FEEDBACK_STEP_FINAL:
	//~ case MAVI_FEEDBACK_STEP_FIRSTDOWN:
	//~ case MAVI_FEEDBACK_STEP_FIRSTUP:
	//~ case MAVI_FEEDBACK_STEP_SINGLEDOWN:
	//~ case MAVI_FEEDBACK_STEP_SINGLEUP:
	//~ case MAVI_FEEDBACK_STAIRS_ASC:
	//~ case MAVI_FEEDBACK_STAIRS_DESC:
		//~ if (irSDist   == 	irSDist_old)  { cout << "IRS: 	OLD DATA"; } else { cout << "IRS: 	 " 	<< irSDist;  } cout << endl;
		//~ if (irMDist   == 	irMDist_old)  { cout << "IRM: 	OLD DATA"; } else { cout << "IRM: 	 " 	<< irMDist;  } cout << endl;
		//~ if (irLDist   == 	irLDist_old)  { cout << "IRL: 	OLD DATA"; } else { cout << "IRL: 	 " 	<< irLDist;  } cout << endl;
		//~ if (srDist    == 	srDist_old)   { cout << "SNR: 	OLD DATA"; } else { cout << "SNR: 	 " 	<< srDist;	 } cout << endl;
		//~ if (usLLDist  == 	usLLDist_old) { cout << "USLL: 	OLD DATA"; } else { cout << "USLL:  "	<< usLLDist; } cout << endl;
		//~ if (usLRDist  == 	usLRDist_old) { cout << "USLR: 	OLD DATA"; } else { cout << "USLR:  " 	<< usLRDist; } cout << endl;
		//~ if (slope     ==	slope_old)    { cout << "Slope: OLD DATA"; } else { cout << "Slope: "  	<< slope; 	 } cout << endl;
		//~ cout << endl;
		//~ break;
//~
	//~ default:
		//~ break;
	//~ }

	if (irSDist   == 	irSDist_old)  { cout << "IRS: 	OLD DATA"; } else { cout << "IRS: 	 " 	<< irSDist;  } cout << endl;
	if (irMDist   == 	irMDist_old)  { cout << "IRM: 	OLD DATA"; } else { cout << "IRM: 	 " 	<< irMDist;  } cout << endl;
	if (irLDist   == 	irLDist_old)  { cout << "IRL: 	OLD DATA"; } else { cout << "IRL: 	 " 	<< irLDist;  } cout << endl;
	if (srDist    == 	srDist_old)   { cout << "SNR: 	OLD DATA"; } else { cout << "SNR: 	 " 	<< srDist;	 } cout << endl;
	if (usLLDist  == 	usLLDist_old) { cout << "USLL: 	OLD DATA"; } else { cout << "USLL:  "	<< usLLDist; } cout << endl;
	if (usLRDist  == 	usLRDist_old) { cout << "USLR: 	OLD DATA"; } else { cout << "USLR:  " 	<< usLRDist; } cout << endl;
	if (slope     ==	slope_old)    { cout << "Slope: OLD DATA"; } else { cout << "Slope: "  	<< slope; 	 } cout << endl;
	cout << endl;

	irSDist_old  = irSDist;
	irMDist_old  = irMDist;
	irLDist_old	 = irLDist;
	srDist_old 	 = srDist;
	usLLDist_old = usLLDist;
	usLRDist_old = usLRDist;
	slope_old 	 = slope;
}

void maviOutputVibrationFeedback(MaviFeedbackID id)
{
	// Vibration notifications (output periodically)
	switch (id)
	{
	case MAVI_FEEDBACK_VIBRATE_CENTER:
		cout << "Vibrate Center ";
		if (irMDist <= MAVI_V_THRESH_HI || srDist <= MAVI_V_THRESH_HI)
		{
			maviVibrate(MAVI_VIB_C, MAVI_VCOUNT_HI);
			cout << "1" << endl;
		}
		else if (srDist <= MAVI_V_THRESH_MD)
		{
			maviVibrate(MAVI_VIB_C, MAVI_VCOUNT_MD);
			cout << "2" << endl;
		}
		else
		{
			maviVibrate(MAVI_VIB_C, MAVI_VCOUNT_LO);
			cout << "3" << endl;
		}
		break;

	case MAVI_FEEDBACK_VIBRATE_LEFT:
		cout << "Vibrate Left" << endl;
		if (usLLDist <= MAVI_V_THRESH_HI)
			maviVibrate(MAVI_VIB_L, MAVI_VCOUNT_HI);
		else if (usLLDist <= MAVI_V_THRESH_MD)
			maviVibrate(MAVI_VIB_L, MAVI_VCOUNT_MD);
		else
			maviVibrate(MAVI_VIB_L, MAVI_VCOUNT_LO);
		break;

	case MAVI_FEEDBACK_VIBRATE_RIGHT:
		cout << "Vibrate Right" << endl;
		if (usLRDist <= MAVI_V_THRESH_HI)
			maviVibrate(MAVI_VIB_R, MAVI_VCOUNT_HI);
		else if (usLRDist <= MAVI_V_THRESH_MD)
			maviVibrate(MAVI_VIB_R, MAVI_VCOUNT_MD);
		else
			maviVibrate(MAVI_VIB_R, MAVI_VCOUNT_LO);
		break;

	case MAVI_FEEDBACK_VIBRATE_LR:
		cout << "Vibrate Left & Right" << endl;
		if (usLLDist <= MAVI_V_THRESH_HI || usLRDist <= MAVI_V_THRESH_HI)
			maviVibrate(MAVI_VIB_L | MAVI_VIB_R, MAVI_VCOUNT_HI);
		else if (usLLDist <= MAVI_V_THRESH_MD || usLRDist <= MAVI_V_THRESH_MD)
			maviVibrate(MAVI_VIB_L | MAVI_VIB_R, MAVI_VCOUNT_MD);
		else
			maviVibrate(MAVI_VIB_L | MAVI_VIB_R, MAVI_VCOUNT_LO);
		break;

	case MAVI_FEEDBACK_VIBRATE_CL:
		cout << "Vibrate Left & Center" << endl;
		if (irMDist <= MAVI_V_THRESH_HI || srDist <= MAVI_V_THRESH_MD)
			maviVibrate(MAVI_VIB_C | MAVI_VIB_L, MAVI_VCOUNT_HI);
		else if (srDist <= MAVI_V_THRESH_MD)
			maviVibrate(MAVI_VIB_C | MAVI_VIB_L, MAVI_VCOUNT_MD);
		else
			maviVibrate(MAVI_VIB_C | MAVI_VIB_L, MAVI_VCOUNT_LO);
		break;

	case MAVI_FEEDBACK_VIBRATE_CR:
		cout << "Vibrate Right & Center" << endl;
		if (irMDist <= MAVI_V_THRESH_HI || srDist <= MAVI_V_THRESH_MD)
			maviVibrate(MAVI_VIB_C | MAVI_VIB_R, MAVI_VCOUNT_HI);
		else if (srDist <= MAVI_V_THRESH_MD)
			maviVibrate(MAVI_VIB_C | MAVI_VIB_R, MAVI_VCOUNT_MD);
		else
			maviVibrate(MAVI_VIB_C | MAVI_VIB_R, MAVI_VCOUNT_LO);
		break;

	case MAVI_FEEDBACK_VIBRATE_ALL:
		cout << "Vibrate All" << endl;
		if (irMDist <= MAVI_V_THRESH_HI || srDist <= MAVI_V_THRESH_MD)
			maviVibrate(MAVI_VIB_C | MAVI_VIB_L | MAVI_VIB_R, MAVI_VCOUNT_HI);
		else if (srDist <= MAVI_V_THRESH_MD)
			maviVibrate(MAVI_VIB_C | MAVI_VIB_L | MAVI_VIB_R, MAVI_VCOUNT_MD);
		else
			maviVibrate(MAVI_VIB_C | MAVI_VIB_L | MAVI_VIB_R, MAVI_VCOUNT_LO);
		break;

	default:
		resetTimer = false;
		break;
	}
}

void maviOutputSystemFeedback(MaviFeedbackID id)
{
	// system notifications (always output but never reset timer)
	switch (id)
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
		maviVibrate(MAVI_VIB_C | MAVI_VIB_L | MAVI_VIB_R, MAVI_VCOUNT_SY);
		return;

	case MAVI_FEEDBACK_CALIB_STARTED:
		cout << "Calibration Started" << endl << endl;
		maviAudioPlay(MAVI_AUDIO_CALIB_STARTED);
		return;

	case MAVI_FEEDBACK_CALIB_SUCCESS:
		cout << "Calibration Successful" << endl << endl;
		maviAudioPlay(MAVI_AUDIO_CALIB_SUCCESS);
		return;

	case MAVI_FEEDBACK_CALIB_FAILED:
		cout << "Calibration Failed" << endl << endl;
		maviAudioPlay(MAVI_AUDIO_CALIB_FAILED);
		return;

	default:
		break;
	}
}

void maviOutputVerbalFeedback(MaviFeedbackID id, bool interrupt)
{
	switch (id)
	{
	case MAVI_FEEDBACK_IM_HAZARD:
		cout << "Immediate Hazard" << endl;
		maviAudioPlay(MAVI_AUDIO_IM_HAZARD, MAVI_APRI_HI);
		break;

	case MAVI_FEEDBACK_LOW_HAZARD:
		cout << "Low Hanging Hazard" << endl;
		maviAudioPlay(MAVI_AUDIO_LOW_HAZARD, MAVI_APRI_HI);
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
		if (!interrupt)
		{
			cout << "Stairs Ahead: Ascending" << endl;
			maviAudioPlay(MAVI_AUDIO_STAIRS_ASC);
		}
		else
		{
			resetTimer = false;
		}
		break;

	case MAVI_FEEDBACK_STAIRS_DESC:
		if (!interrupt)
		{
			cout << "Stairs Ahead: Descending" << endl;
			maviAudioPlay(MAVI_AUDIO_STAIRS_DESC);
		}
		else
		{
			resetTimer = false;
		}
		break;

	case MAVI_FEEDBACK_WARNING_SENSORFAILURE:
		cout << "MAVI Sensor Failed" << endl;
		maviAudioPlay(MAVI_AUDIO_WARNING_SENSORFAILURE);
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
			maviOutputDebugData(id);
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
			maviOutputDebugData(id);
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
