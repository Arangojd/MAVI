/*
 * mavi-adc.hpp
 * ------------
 *
 * ...
 */

#ifndef MAVI_ADC_HPP
#define MAVI_ADC_HPP

extern long adc; // ADC device #

enum MaviAnalogPin
{
	MAVI_PIN_IRS = 0,
	MAVI_PIN_IRM = 2,
	MAVI_PIN_IRL = 4
};

int maviADCRead(MaviAnalogPin apin);

#endif
