/*
 *
 */

#include <ao/ao.h>
#include <wiringPi.h>

int main(int argc, char ** argv)
{
	// Initialization

	wiringPiSetup();
	ao_initialize();

	// Main Loop
	while (1)
	{
		// Poll sensors

		// Process

		// Send feedback
	}
}
