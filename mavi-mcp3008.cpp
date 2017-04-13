/*
 * mavi-mcp3008.cpp
 * ----------------
 *
 * ...
 *
 * Contains copy-paste-modified code from the mcp3004 extension to the wiringPi library.
 * That code was written by Gordon Henderson and is available at git.drogon.net.
 * Used without explicit permission, but I think is permitted under GNU LGPL?
 * I dunno. I'm not a lawyer.
 *
 *   - Emmanuel
 */

#include <pthread.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

#include "mavi-mcp3008.hpp"
#include "mavi-pins.hpp"

void maviMCP3008Init(void)
{
	wiringPiSPISetup(0, MCP3008_CLOCK_SPEED);
}

inline int decruft_MCP3008_result(unsigned char buffer[3])
{
	return ((buffer[1] << 8) | buffer[2]) & 0x3FF;
}

int maviMCP3008ReadRaw(MaviAnalogPin apin)
{
	static pthread_mutex_t mut_adc = PTHREAD_MUTEX_INITIALIZER;

	unsigned char buffer[3] =
	{
		0x01,
		0x80 | (apin << 4);
		0x00
	};

	pthread_mutex_lock(&mut_adc);
	wiringPiSPIDataRW(0, buffer, 3);
	pthread_mutex_unlock(&mut_adc);

	return decruft_MCP3008_result(buffer);
}
