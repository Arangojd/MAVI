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

	MAVI_DPIN_VL = 23,
	MAVI_DPIN_VC = 25,
	MAVI_DPIN_VR =  5,

	MAVI_DPIN_USL_TRIG = 20,
	MAVI_DPIN_USR_TRIG = 21,
	MAVI_DPIN_USL_ECHO = 16,
	MAVI_DPIN_USR_ECHO = 12,

	MAVI_DPIN_POWER =  6,
	MAVI_DPIN_PAUSE = 19,
	MAVI_DPIN_CALIB = 13
};

enum MaviAnalogPin : int
{
	MAVI_APIN_INVALID = -1,
	MAVI_APIN_IRS = 0,
	MAVI_APIN_IRM = 1,
	MAVI_APIN_IRL = 2
};

#endif
