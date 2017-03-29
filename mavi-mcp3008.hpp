/*
 * mavi-mcp3008.hpp
 * ----------------
 *
 * ...
 */

#ifndef MAVI_MCP3008_HPP
#define MAVI_MCP3008_HPP

#include "mavi-pins.hpp"

const unsigned long MCP3008_CLOCK_SPEED = 500000; // 500 KHz
const double MCP3008_VREF = 5.0; // Volts

void maviMCP3008Init(void);

int maviMCP3008ReadRaw(MaviAnalogPin apin);

inline double maviMCP3008Read(MaviAnalogPin apin)
{
	return maviMCP3008ReadRaw(apin) * MCP3008_VREF / 1024.0;
}

#endif
