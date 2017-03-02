/*
 * mavi-main.cpp
 * -------------
 *
 * This is the entry point for the MAVI program.
 */

#include <wiringPi.h>
#include <cstdlib>
#include <csignal>
#include <iostream>

#include "mavi-control.hpp"
#include "mavi-state.hpp"
#include "mavi-calib.hpp"

using namespace std;

pthread_t saThread, fbThread;

void onInterrupt(int s)
{
	cout << "SIGINT received; exiting" << endl;
	maviShutdown(&saThread, &fbThread);
}

int main(int argc, char ** argv)
{
	signal(SIGINT, onInterrupt);

	maviInit(&saThread, &fbThread);
	pthread_join(&saThread, NULL);
	pthread_join(&fbThread, NULL);

	return 0;
}
