/*
 * mavi-audio.cpp
 * --------------
 *
 * ...
 */

#include <cstdlib>
#include <cstring>
#include "mavi-audio.hpp"

int maviAudioPlay(MaviAudioID audioFile)
{
	char cmd[strlen(audioFile) + 10];
	strcpy(cmd, "omxplayer ");
	strcat(cmd, audioFile);
	system(cmd);
	return 0;
}
