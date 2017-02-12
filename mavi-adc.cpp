/*
 * mavi-adc.cpp
 * ------------
 *
 * ...
 */

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "mavi-adc.hpp"

inline int decruft_AD7997_result(int msB, int lsB)
{
	return (msB & 0xF) << 6 | lsB >> 2;
}

int maviADCRead(MaviAnalogPin apin)
{
	// NOTE: If we need to run the ADC in fast mode, this
	// code will need to be revised to avoid timing issues.

	int msB, lsB;

	// Tell the ADC "I want the signal on this pin converted."
	wiringPiI2CWrite(adc, 0x80 | (apin << 4));

	// Read from the conversion register
	msB = wiringPiI2CRead(adc);
	lsB = wiringPiI2CRead(adc);

	//~ assert(msB >> 4 & 0b111 == apin);

	// Return the converted value (minus cruft)
	return decruft_AD7997_result(msB, lsB);
}

void maviADCReadAll(int c, MaviAnalogPin *pins, int *values)
{
	// The AD7997 supports conversion for multiple signals
	// in sequence, which is faster than querying one-by-one.

	int i, msB, lsB, pinMask = 0;

	for (i = 0; i < c; i++)
		pinMask |= 1 << pins[i];

	wiringPiI2CWriteReg16(adc, AD7997_REG_CONFIG, pinMask << 4 | 0x8);
	wiringPiI2CWrite(adc, 0x70);

	for (i = 0; i < c; i++)
	{
		msB = wiringPiI2CRead(adc);
		lsB = wiringPiI2CRead(adc);
		//~ assert(msB >> 4 & 0b111 == pins[i]);
		values[i] = decruft_AD7997_result(msB, lsB);
	}
}
