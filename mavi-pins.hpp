/*
 * mavi-pins.hpp
 * -------------
 *
 * Contains the pin # definitions for MAVI's component
 * connections on the RPi GPIO array and the ADC channels.
 */

#ifndef MAVI_PINS_HPP
#define MAVI_PINS_HPP

// If uncommented, use Broadcom Rev. 2 pin #'s for digital IO pins.
// See http://wiringpi.com/pins for more details.
#define MAVI_PINTYPE_BCM

// I2C Pins (needed for AD7997)
// wpi: 8, 9
// bcm: 2, 3

// SPI Pins (needed for MCP3008)
// wpi: 10~14
// bcm: 7~11

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
