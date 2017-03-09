/*
 * mavi-main.cpp
 * -------------
 *
 * This is the entry point for the MAVI program.
 */

#include <cstdlib>
#include <pthread.h>

#include "mavi-analysis.hpp"
#include "mavi-feedback.hpp"
#include "mavi-init.hpp"

using namespace std;

int main(int argc, char ** argv)
{
	pthread_t saThread, fbThread;

	maviInit();

	pthread_create(&saThread, NULL, maviSenseAndAnalyze, NULL);
	pthread_create(&fbThread, NULL, maviProvideFeedback, NULL);

	pthread_join(saThread, NULL);
	pthread_join(fbThread, NULL);

	return 0;
}
