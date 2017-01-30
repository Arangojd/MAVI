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

enum maviPin : int
{
	MAVI_PIN_VL = 2,
	MAVI_PIN_VC = 3,
	MAVI_PIN_VR = 4,

	MAVI_PIN_IRS = 17,
	MAVI_PIN_IRM = 27,
	MAVI_PIN_IRL = 22,

	MAVI_PIN_USL_TRIG =  9,
	MAVI_PIN_USR_TRIG = 11,
	MAVI_PIN_USL_ECHO = 25,
	MAVI_PIN_USR_ECHO =  8,

	MAVI_PIN_POWER = 14,
	MAVI_PIN_PAUSE = 15,
	MAVI_PIN_CALIB = 18
};

#endif
