/*
 * mavi-main.cpp
 * -------------
 *
 * This is the entry point for the MAVI program.
 */

#include <iostream>
#include <cstdlib>
#include <csignal>
#include <pthread.h>

#include "mavi-analysis.hpp"
#include "mavi-feedback.hpp"
#include "mavi-init.hpp"
#include "mavi-state.hpp"

using namespace std;

void onInterrupt(int s)
{
	cout << endl << "SIGINT received" << endl;
	maviSetState(MAVI_STATE_SHUTDOWN);
}

int main(int argc, char ** argv)
{
	pthread_t saThread, fbThread;

	maviInit();

	signal(SIGINT, onInterrupt);

	pthread_create(&saThread, NULL, maviSenseAndAnalyze, NULL);
	pthread_create(&fbThread, NULL, maviProvideFeedback, NULL);

	pthread_join(saThread, NULL);
	pthread_join(fbThread, NULL);

	return 0;
}
