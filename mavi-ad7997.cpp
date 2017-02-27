/*
 * mavi-ad7997.cpp
 * ---------------
 *
 * ...
 */

#include <cassert>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "mavi-ad7997.hpp"
#include "mavi-pins.hpp"

inline int decruft_AD7997_result(int outcode)
{
	return (outcode >> 2) & ((1 << 10) - 1);
}

int maviADCRead(MaviAnalogPin apin)
{
	int outcode;

	// Write to the Address Pointer Register
	// Tell the ADC "I want the signal on this pin converted."
	wiringPiI2CWrite(adc, 0x80 | (apin << 4));

	// IMPORTANT: If the ADC is in Fast mode, insert a delay here to ensure
	// that the conversion is finished before we attempt to read it.

	// Read from the conversion register
	outcode = wiringPiI2CReadReg16(adc, AD7997_REG_RESULT);

	assert(outcode >> 12 & 0x7 == apin);

	// Return the converted value (minus cruft)
	return decruft_AD7997_result(outcode);
}
