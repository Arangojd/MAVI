/*
 * mavi-adc.hpp
 * ------------
 *
 * ...
 */

#ifndef MAVI_ADC_HPP
#define MAVI_ADC_HPP

extern int adc; // ADC file descriptor

enum MaviAnalogPin : int
{
	MAVI_PIN_IRS = 0,
	MAVI_PIN_IRM = 2,
	MAVI_PIN_IRL = 4
};

enum AD7997RegAddress : int
{
	AD7997_REG_RESULT = 0,
	AD7997_REG_ALERT,
	AD7997_REG_CONFIG,
	AD7997_REG_TIMER,
	AD7997_REG_DLOW_1,
	AD7997_REG_DHIGH_1,
	AD7997_REG_DHYST_1,
	AD7997_REG_DLOW_2,
	AD7997_REG_DHIGH_2,
	AD7997_REG_DHYST_2,
	AD7997_REG_DLOW_3,
	AD7997_REG_DHIGH_3,
	AD7997_REG_DHYST_3,
	AD7997_REG_DLOW_4,
	AD7997_REG_DHIGH_4,
	AD7997_REG_DHYST_4;
};

int maviADCRead(MaviAnalogPin apin);

void maviADCReadAll(int c, MaviAnalogPin *pins, int *values);

#endif
