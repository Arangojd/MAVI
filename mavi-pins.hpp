/*
 * mavi-pins.hpp
 * -------------
 *
 * Contains the pin # definitions for MAVI's component connections on the RPi GPIO array.
 * These are Broadcom Rev. 2 pin #'s. See http://wiringpi.com/pins for more details.
 */

#ifndef MAVI_PINS_HPP
#define MAVI_PINS_HPP

#define MAVI_PINTYPE_BCM

enum maviPin
{
	MAVI_PIN_IRS = 2,
	MAVI_PIN_IRM = 3,
	MAVI_PIN_IRL = 4,

	MAVI_PIN_USL = 22,
	MAVI_PIN_USR = 27,

	MAVI_PIN_VL = 14,
	MAVI_PIN_VC = 15,
	MAVI_PIN_VR = 18,

	MAVI_PIN_POWER = 23,
	MAVI_PIN_PAUSE = 24,
	MAVI_PIN_CALIB = 25
};

#endif
