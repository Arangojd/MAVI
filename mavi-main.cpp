/*
 * mavi-main.cpp
 * -------------
 *
 * This is the entry point for the MAVI program.
 */

#include <ao/ao.h>
#include <wiringPi.h>

int main(int argc, char ** argv)
{
	// Initialization

	wiringPiSetupGpio();
	ao_initialize();

	// Main Loop
	while (1)
	{
		// Poll sensors

		// Process

		// Send feedback
	}
}
