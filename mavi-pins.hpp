/*
 * mavi-pins.hpp
 * -------------
 *
 * Contains the pin # definitions for MAVI's component
 * connections on the RPi GPIO array and the AD7997 channels.
 */

#ifndef MAVI_PINS_HPP
#define MAVI_PINS_HPP

// If uncommented, use Broadcom Rev. 2 pin #'s for digital IO pins.
// See http://wiringpi.com/pins for more details.
#define MAVI_PINTYPE_BCM

// wpi: 8, 9
// bcm: 2, 3
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

enum MaviAnalogPin : int
{
	MAVI_PIN_IRS,
	MAVI_PIN_IRM,
	MAVI_PIN_IRL
};

#endif
