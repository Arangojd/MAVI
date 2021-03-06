/*
 * mavi-ad7997.hpp
 * ---------------
 *
 * ...
 */

#ifndef MAVI_AD7997_HPP
#define MAVI_AD7997_HPP

#include "mavi-pins.hpp"

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

const double AD7997_VREF = 3.3; // Volts

void maviAD7997Init(void);

int maviAD7997ReadRaw(MaviAnalogPin apin);

double maviAD7997Read(MaviAnalogPin apin);

#endif
