/*
 * mavi-feedback.hpp
 * -----------------
 *
 * ...
 */

#ifndef MAVI_FEEDBACK_HPP
#define MAVI_FEEDBACK_HPP

extern unsigned int t_lastVerbalOutput, t_lastVibrationOutput;

const unsigned int MAVI_VERBAL_OUTPUT_PERIOD = 2000; // ms
const unsigned int MAVI_VIBRATION_OUTPUT_PERIOD = 750; // ms
const unsigned int MAVI_VIBRATION_DURATION = 1000; // ms

enum MaviFeedbackID
{
		MAVI_FEEDBACK_BTN_CALIB,
		MAVI_FEEDBACK_BTN_POWER,
		MAVI_FEEDBACK_BTN_SLEEP,
		MAVI_FEEDBACK_CALIB_FAILED,
		MAVI_FEEDBACK_CALIB_STARTED,
		MAVI_FEEDBACK_CALIB_SUCCESS,
		MAVI_FEEDBACK_IM_HAZARD,
		MAVI_FEEDBACK_STAIRS_ASC,
		MAVI_FEEDBACK_STAIRS_DESC,
		MAVI_FEEDBACK_STEP_FINAL,
		MAVI_FEEDBACK_STEP_FIRSTDOWN,
		MAVI_FEEDBACK_STEP_FIRSTUP,
		MAVI_FEEDBACK_STEP_SINGLEDOWN,
		MAVI_FEEDBACK_STEP_SINGLEUP,
		MAVI_FEEDBACK_SYSTEM_ON,
		MAVI_FEEDBACK_SYSTEM_PAUSED,
		MAVI_FEEDBACK_SYSTEM_READY,
		MAVI_FEEDBACK_SYSTEM_SHUTDOWN,
		MAVI_FEEDBACK_VIBRATE_CENTER,
		MAVI_FEEDBACK_VIBRATE_LEFT,
		MAVI_FEEDBACK_VIBRATE_RIGHT,
		MAVI_FEEDBACK_VIBRATE_LR,
		MAVI_FEEDBACK_VIBRATE_CL,
		MAVI_FEEDBACK_VIBRATE_CR,
		MAVI_FEEDBACK_VIBRATE_ALL,
		MAVI_FEEDBACK_WARNING_BATTERYLOW,
		MAVI_FEEDBACK_WARNING_BATTERYCRITICAL,
		MAVI_FEEDBACK_WARNING_NONOPERATIONAL,
		MAVI_FEEDBACK_WARNING_SENSORFAILURE
};

void maviOutputDebugData(MaviFeedbackID id);
void maviOutputVibrationFeedback(MaviFeedbackID id);
void maviOutputSystemFeedback(MaviFeedbackID id);
void maviOutputVerbalFeedback(MaviFeedbackID id);
void maviSendFeedback(MaviFeedbackID id);

#endif
