/*
 * mavi-mcp3008.hpp
 * ----------------
 *
 * ...
 */

#ifndef MAVI_MCP3008_HPP
#define MAVI_MCP3008_HPP

#include "mavi-pins.hpp"

extern int adc; // ADC file descriptor

const unsigned long MCP3008_CLOCK_SPEED = 32000000; // 32 million; counting 0's is hard
const double MCP3008_VREF = 5.0; // Volts

void maviMCP3008Init(void);

int maviMCP3008ReadRaw(MaviAnalogPin apin);

double maviMCP3008Read(MaviAnalogPin apin);

#endif
