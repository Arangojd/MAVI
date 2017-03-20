/*
 * mavi-main.cpp
 * -------------
 *
 * This is the entry point for the MAVI program.
 */

#include <cstdlib>
#include <pthread.h>

#include "mavi-analysis.hpp"
#include "mavi-init.hpp"

using namespace std;

int main(int argc, char ** argv)
{
	pthread_t saThread;

	maviInit();

	pthread_create(&saThread, NULL, maviSenseAndAnalyze, NULL);
	pthread_join(saThread, NULL);

	return 0;
}
