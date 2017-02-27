#ifndef MAVI_AUDIO_HPP
#define MAVI_AUDIO_HPP

typedef MaviAudioID char*;

MaviAudioID MAVI_AUDIO_BTN_CALIB = "mavi-button-calibration.wav";
MaviAudioID MAVI_AUDIO_BTN_POWER = "mavi-button-power.wav";
MaviAudioID MAVI_AUDIO_BTN_SLEEP = "mavi-button-sleep.wav";
MaviAudioID MAVI_AUDIO_CALIB_FAILED = "mavi-calibration-failed.wav"
MaviAudioID MAVI_AUDIO_CALIB_STARTED = "mavi-calibration-started.wav"
MaviAudioID MAVI_AUDIO_CALIB_SUCCESS = "mavi-calibration-sucessful.wav"
MaviAudioID MAVI_AUDIO_IM_HAZARD = "mavi-immediate-hazard.wav"
MaviAudioID MAVI_AUDIO_STAIRS_ASC = "mavi-stairs-ascending.wav"
MaviAudioID MAVI_AUDIO_STAIRS_DESC = "mavi-stairs-descending.wav"
MaviAudioID MAVI_AUDIO_STEP_FINAL = "mavi-step-final.wav"
MaviAudioID MAVI_AUDIO_STEP_FIRSTDOWN = "mavi-step-first-down.wav"
MaviAudioID MAVI_AUDIO_STEP_FIRSTUP = "mavi-step-first-up.wav"
MaviAudioID MAVI_AUDIO_STEP_SINGLEDOWN = "mavi-step-single-down.wav"
MaviAudioID MAVI_AUDIO_STEP_SINGLEUP = "mavi-step-single-up.wav"
MaviAudioID MAVI_AUDIO_SYSTEM_ON = "mavi-system-on.wav"
MaviAudioID MAVI_AUDIO_SYSTEM_PAUSED = "mavi-system-paused.wav"
MaviAudioID MAVI_AUDIO_SYSTEM_READY = "mavi-system-ready.wav"
MaviAudioID MAVI_AUDIO_SYSTEM_SHUTDOWN = "mavi-system-shutdown.wav"
MaviAudioID MAVI_AUDIO_VIBRATE_CENTER = "mavi-vibrate-center.wav"
MaviAudioID MAVI_AUDIO_VIBRATE_LEFT = "mavi-vibrate-left.wav"
MaviAudioID MAVI_AUDIO_VIBRATE_RIGHT = "mavi-vibrate-right.wav"
MaviAudioID MAVI_AUDIO_WARNING_BATTERYLOW = "mavi-warning-battery-low.wav"
MaviAudioID MAVI_AUDIO_WARNING_BATTERYCRITICAL = "mavi-warning-battery-critical.wav"
MaviAudioID MAVI_AUDIO_WARNING_NONOPERATIONAL = "mavi-warning-nonoperational.wav"
MaviAudioID MAVI_AUDIO_WARNING_SENSORFAILURE = "mavi-warning-sensor-failure.wav"

int maviAudiPlay(MaviAudioID audioFile);

#endif
