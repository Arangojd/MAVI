/*
 * mavi-feedback.cpp
 * -----------------
 *
 * ...
 */

#include <cstdlib>

#include "mavi-feedback.hpp"

unsigned int t_lastVibrationOutput, t_lastVerbalOutput;

void maviSendFeedback(MaviFeedbackID id)
{
	// system notifications (always output but never reset timer)
	switch(id)
	{
		case MAVI_FEEDBACK_SYSTEM_READY:
			cout << "System Ready" << endl << endl;
			maviAudioPlay(MAVI_AUDIO_SYSTEM_READY);
			return;

		case MAVI_FEEDBACK_SYSTEM_PAUSED:
			cout << "System Paused" << endl << endl;
			maviAudioPlay(MAVI_AUDIO_SYSTEM_PAUSED);
			return;

		case MAVI_FEEDBACK_SYSTEM_SHUTDOWN:
			cout << "System Shutting Down" << endl << endl;
			maviAudioPlay(MAVI_AUDIO_SYSTEM_SHUTDOWN);
			return;

		default:
			break;
	}

	unsigned int t_current = millis();

	if ((t_current - t_lastVibrationOutput) >= MAVI_VIBRATION_OUTPUT_PERIOD)
	{
		// Vibration notifications (output periodically)
		switch(id)
		{
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

			case MAVI_FEEDBACK_VIBRATE_LR:
				cout << "Vibrate Both" << endl;
				maviAudioPlay(MAVI_AUDIO_VIBRATE_LR);
				break;

			case MAVI_FEEDBACK_VIBRATE_CL:
				cout << "Vibrate Both" << endl;
				maviAudioPlay(MAVI_AUDIO_VIBRATE_CL);
				break;

			case MAVI_FEEDBACK_VIBRATE_CR:
				cout << "Vibrate Both" << endl;
				maviAudioPlay(MAVI_AUDIO_VIBRATE_CR);
				break;

			case MAVI_FEEDBACK_VIBRATE_ALL:
				cout << "Vibrate All" << endl;
				maviAudioPlay(MAVI_AUDIO_VIBRATE_ALL);
				break;

			default:
				cout << "No Feedback" << endl;
				return;
		}
		// Uncomment the line below when vibration motors are ready for use
		//t_lastVibrationOutput = millis();
	}
	else if ((t_current - t_lastVerbalOutput) >= MAVI_VERBAL_OUTPUT_PERIOD)
	{
		// normal notifications (only output periodically and reset timer)
		switch(id)
		{
			case MAVI_FEEDBACK_IM_HAZARD:
				cout << "Immediate Hazard" << endl;
				maviAudioPlay(MAVI_AUDIO_IM_HAZARD);
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

			case MAVI_FEEDBACK_VIBRATE_LR:
				cout << "Vibrate Both" << endl;
				maviAudioPlay(MAVI_AUDIO_VIBRATE_LR);
				break;

			case MAVI_FEEDBACK_VIBRATE_CL:
				cout << "Vibrate Both" << endl;
				maviAudioPlay(MAVI_AUDIO_VIBRATE_CL);
				break;

			case MAVI_FEEDBACK_VIBRATE_CR:
				cout << "Vibrate Both" << endl;
				maviAudioPlay(MAVI_AUDIO_VIBRATE_CR);
				break;

			case MAVI_FEEDBACK_VIBRATE_ALL:
				cout << "Vibrate All" << endl;
				maviAudioPlay(MAVI_AUDIO_VIBRATE_ALL);
				break;

			default:
				cout << "No Feedback" << endl;
				return;
		}

		t_lastVerbalOutput = millis();
	}
	else
	{
		// priority notifications (always output and reset timer)
		switch(id)
		{
			case MAVI_FEEDBACK_IM_HAZARD:
				cout << "Immediate Hazard" << endl;
				maviAudioPlay(MAVI_AUDIO_IM_HAZARD);
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
				cout << "No Feedback" << endl;
				return;
		}

		t_lastVerbalOutput = millis();
	}

	t_lastVibrationOutput = millis(); //delete this when vibration motors are ready for use
}

