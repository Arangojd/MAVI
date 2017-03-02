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

#include <wiringPi.h>
#include <wiringPiSPI.h>

#include "mavi-mcp3008.hpp"
#include "mavi-pins.hpp"

void maviMCP3008Init(void)
{
	wiringPiSPISetup(0, MCP3008_CLOCK_SPEED);
}

inline int decruft_MCP3008_result(char buffer[3])
{
	return ((buffer[1] << 8) | buffer[2]) & 0x3FF;
}

int maviMCP3008ReadRaw(MaviAnalogPin apin)
{
	unsigned char buffer[3];

	buffer[0] = 0x01;
	buffer[1] = 0x80 | (apin << 4);
	buffer[2] = 0x00;

	wiringPiSPIDataRW(0, buffer, 3);

	return decruft_MCP3008_result(buffer);
}

double maviMCP3008Read(MaviAnalogPin apin)
{
	return maviMCP3008ReadRaw(apin) * MCP3008_VREF / 1024.0;
}
