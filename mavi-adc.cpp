/*
 * mavi-adc.cpp
 * ------------
 *
 * ...
 */

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "mavi-adc.hpp"

int maviADCRead(MaviAnalogPin apin)
{
	// NOTE: If we need to run the ADC in fast mode, this
	// code will need to be revised to avoid timing issues.

	// Tell the ADC "I want the signal on this pin converted."
	wiringPiI2CWrite(adc, 0x80 | (apin << 4));

	// Read the converted value
	return wiringPiI2CRead(adc);
}

int maviADCReadAll(void)
{
	// The AD7997 supports conversion for multiple signals
	// in sequence, which is faster than querying one-by-one.

	// TODO: Implement this protocol

	return 0;
}
