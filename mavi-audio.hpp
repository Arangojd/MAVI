#ifndef MAVI_AUDIO_HPP
#define MAVI_AUDIO_HPP

typedef MaviAudioID char*;

MaviAudioID MAVI_AUDIO_BTN_CALIB = "audio/mavi-button-calibration.wav";
MaviAudioID MAVI_AUDIO_BTN_POWER = "audio/mavi-button-power.wav";
MaviAudioID MAVI_AUDIO_BTN_SLEEP = "audio/mavi-button-sleep.wav";
MaviAudioID MAVI_AUDIO_CALIB_FAILED = "audio/mavi-calibration-failed.wav"
MaviAudioID MAVI_AUDIO_CALIB_STARTED = "audio/mavi-calibration-started.wav"
MaviAudioID MAVI_AUDIO_CALIB_SUCCESS = "audio/mavi-calibration-sucessful.wav"
MaviAudioID MAVI_AUDIO_IM_HAZARD = "audio/mavi-immediate-hazard.wav"
MaviAudioID MAVI_AUDIO_STAIRS_ASC = "audio/mavi-stairs-ascending.wav"
MaviAudioID MAVI_AUDIO_STAIRS_DESC = "audio/mavi-stairs-descending.wav"
MaviAudioID MAVI_AUDIO_STEP_FINAL = "audio/mavi-step-final.wav"
MaviAudioID MAVI_AUDIO_STEP_FIRSTDOWN = "audio/mavi-step-first-down.wav"
MaviAudioID MAVI_AUDIO_STEP_FIRSTUP = "audio/mavi-step-first-up.wav"
MaviAudioID MAVI_AUDIO_STEP_SINGLEDOWN = "audio/mavi-step-single-down.wav"
MaviAudioID MAVI_AUDIO_STEP_SINGLEUP = "audio/mavi-step-single-up.wav"
MaviAudioID MAVI_AUDIO_SYSTEM_ON = "audio/mavi-system-on.wav"
MaviAudioID MAVI_AUDIO_SYSTEM_PAUSED = "audio/mavi-system-paused.wav"
MaviAudioID MAVI_AUDIO_SYSTEM_READY = "audio/mavi-system-ready.wav"
MaviAudioID MAVI_AUDIO_SYSTEM_SHUTDOWN = "audio/mavi-system-shutdown.wav"
MaviAudioID MAVI_AUDIO_VIBRATE_CENTER = "audio/mavi-vibrate-center.wav"
MaviAudioID MAVI_AUDIO_VIBRATE_LEFT = "audio/mavi-vibrate-left.wav"
MaviAudioID MAVI_AUDIO_VIBRATE_RIGHT = "audio/mavi-vibrate-right.wav"
MaviAudioID MAVI_AUDIO_WARNING_BATTERYLOW = "audio/mavi-warning-battery-low.wav"
MaviAudioID MAVI_AUDIO_WARNING_BATTERYCRITICAL = "audio/mavi-warning-battery-critical.wav"
MaviAudioID MAVI_AUDIO_WARNING_NONOPERATIONAL = "audio/mavi-warning-nonoperational.wav"
MaviAudioID MAVI_AUDIO_WARNING_SENSORFAILURE = "audio/mavi-warning-sensor-failure.wav"

int maviAudiPlay(MaviAudioID audioFile);

#endif
