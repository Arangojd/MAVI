#inlcude "mavi-audio.hpp"
#include <stdlib.h>

int maviAudioPlay(MaviAudioID audioFile)
{
	std::string cmd = "omxplayer " + audioFile;
	system(cmd);
	return 0;
}
