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

// wpi:  8, 9
// bcm1: 0, 1
// bcm2: 2, 3
// These pins are reserved for the I2C interface,
// which is needed for the analog-digital converter,
// which is needed for the infrared sensors.
// i.e. DO NOT USE THEM HERE!!!

enum MaviDigitalPin : int
{
	MAVI_PIN_VL,
	MAVI_PIN_VC,
	MAVI_PIN_VR,

	MAVI_PIN_USL_TRIG,
	MAVI_PIN_USR_TRIG,
	MAVI_PIN_USL_ECHO,
	MAVI_PIN_USR_ECHO,

	MAVI_PIN_POWER,
	MAVI_PIN_PAUSE,
	MAVI_PIN_CALIB
};

#endif
