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
	MAVI_DPIN_INVALID = -1,

	MAVI_DPIN_VL,
	MAVI_DPIN_VC,
	MAVI_DPIN_VR,

	MAVI_DPIN_USL_TRIG,
	MAVI_DPIN_USR_TRIG,
	MAVI_DPIN_USL_ECHO,
	MAVI_DPIN_USR_ECHO,

	MAVI_DPIN_POWER,
	MAVI_DPIN_PAUSE,
	MAVI_DPIN_CALIB
};

enum MaviAnalogPin : int
{
	MAVI_APIN_INVALID = -1,
	MAVI_APIN_IRS,
	MAVI_APIN_IRM,
	MAVI_APIN_IRL
};

#endif
