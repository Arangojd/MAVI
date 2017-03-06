/*
 * mavi-audio.cpp
 * --------------
 *
 * ...
 */

#include <cstdlib>
#include <cstring>

#include <unistd.h>

#include "mavi-audio.hpp"

const MaviAudioID
	MAVI_AUDIO_BTN_CALIB = "audio/mavi-button-calibration.wav",
	MAVI_AUDIO_BTN_POWER = "audio/mavi-button-power.wav",
	MAVI_AUDIO_BTN_SLEEP = "audio/mavi-button-sleep.wav",
	MAVI_AUDIO_CALIB_FAILED = "audio/mavi-calibration-failed.wav",
	MAVI_AUDIO_CALIB_STARTED = "audio/mavi-calibration-started.wav",
	MAVI_AUDIO_CALIB_SUCCESS = "audio/mavi-calibration-sucessful.wav",
	MAVI_AUDIO_IM_HAZARD = "audio/mavi-immediate-hazard.wav",
	MAVI_AUDIO_STAIRS_ASC = "audio/mavi-stairs-ascending.wav",
	MAVI_AUDIO_STAIRS_DESC = "audio/mavi-stairs-descending.wav",
	MAVI_AUDIO_STEP_FINAL = "audio/mavi-step-final.wav",
	MAVI_AUDIO_STEP_FIRSTDOWN = "audio/mavi-step-first-down.wav",
	MAVI_AUDIO_STEP_FIRSTUP = "audio/mavi-step-first-up.wav",
	MAVI_AUDIO_STEP_SINGLEDOWN = "audio/mavi-step-single-down.wav",
	MAVI_AUDIO_STEP_SINGLEUP = "audio/mavi-step-single-up.wav",
	MAVI_AUDIO_SYSTEM_ON = "audio/mavi-system-on.wav",
	MAVI_AUDIO_SYSTEM_PAUSED = "audio/mavi-system-paused.wav",
	MAVI_AUDIO_SYSTEM_READY = "audio/mavi-system-ready.wav",
	MAVI_AUDIO_SYSTEM_SHUTDOWN = "audio/mavi-system-shutdown.wav",
	MAVI_AUDIO_VIBRATE_CENTER = "audio/mavi-vibrate-center.wav",
	MAVI_AUDIO_VIBRATE_LEFT = "audio/mavi-vibrate-left.wav",
	MAVI_AUDIO_VIBRATE_RIGHT = "audio/mavi-vibrate-right.wav",
	MAVI_AUDIO_WARNING_BATTERYLOW = "audio/mavi-warning-battery-low.wav",
	MAVI_AUDIO_WARNING_BATTERYCRITICAL = "audio/mavi-warning-battery-critical.wav",
	MAVI_AUDIO_WARNING_NONOPERATIONAL = "audio/mavi-warning-nonoperational.wav",
	MAVI_AUDIO_WARNING_SENSORFAILURE = "audio/mavi-warning-sensor-failure.wav";

int maviAudioPlay(MaviAudioID audioFile)
{
	if (vfork() == 0)
		execlp("aplay", "aplay", audioFile, NULL);

	return 0;
}
