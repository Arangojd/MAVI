#inlcude "mavi-audio.hpp"

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>

#include <stdlib.h>
#include <assert.h>
#include <ao/ao.h>

static const int BUF_SIZE = 4096;

struct WavHeader 
{
    char id[4]; //should contain RIFF
    int32_t totalLength;
    char wavefmt[8];
    int32_t format; // 16 for PCM
    int16_t pcm; // 1 for PCM
    int16_t channels;
    int32_t frequency;
    int32_t bytesPerSecond;
    int16_t bytesByCapture;
    int16_t bitsPerSample;
    char data[4]; // "data"
    int32_t bytesInData;
};

int maviAudioPlay(MaviAudioID audioFile) 
{
	/* Open file and process header */

    	ao_device* device;
	ao_sample_format format;
    	int defaultDriver;
    	WavHeader header;

    	std::ifstream file;
    	file.open(audioFile, std::ios::binary | std::ios::in);

    	file.read(header.id, sizeof(header.id));
    	assert(!std::memcmp(header.id, "RIFF", 4)); //is it a WAV file?
    	file.read((char*)&header.totalLength, sizeof(header.totalLength));
    	file.read(header.wavefmt, sizeof(header.wavefmt)); //is it the right format?
    	assert(!std::memcmp(header.wavefmt, "WAVEfmt ", 8));
    	file.read((char*)&header.format, sizeof(header.format));
    	file.read((char*)&header.pcm, sizeof(header.pcm));
    	file.read((char*)&header.channels, sizeof(header.channels));
    	file.read((char*)&header.frequency, sizeof(header.frequency));
    	file.read((char*)&header.bytesPerSecond, sizeof(header.bytesPerSecond));
    	file.read((char*)&header.bytesByCapture, sizeof(header.bytesByCapture));
    	file.read((char*)&header.bitsPerSample, sizeof(header.bitsPerSample));
    	file.read(header.data, sizeof(header.data));
    	file.read((char*)&header.bytesInData, sizeof(header.bytesInData));

	/* Initialize libao */

    	ao_initialize();

    	defaultDriver = ao_default_driver_id();

    	memset(&format, 0, sizeof(format));
    	format.bits = header.format;
    	format.channels = header.channels;
    	format.rate = header.frequency;
    	format.byte_format = AO_FMT_LITTLE;

    	device = ao_open_live(defaultDriver, &format, NULL);
    	if (device == NULL) 
	{
        	cout << "Error: Unable to open audio driver." << endl << endl;
        	return 1;
    	}
	
	/* Load and play audio file  */

    	char* buffer = (char*)malloc(BUF_SIZE * sizeof(char));

    	int fSize = header.bytesInData;
    	int bCount = fSize / BUF_SIZE;

    	for (int i = 0; i < bCount; ++i) 
	{
        	file.read(buffer, BUF_SIZE);
        	ao_play(device, buffer, BUF_SIZE);
    	}

    	int leftoverBytes = fSize % BUF_SIZE;
    	file.read(buffer, leftoverBytes);
    	memset(buffer + leftoverBytes, 0, BUF_SIZE - leftoverBytes);
    	ao_play(device, buffer, BUF_SIZE);
	
	/* Clean up and shutdown */

    	ao_close(device);
    	ao_shutdown();
	file.close();
}
