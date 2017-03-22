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
#include "mavi-vibration.hpp"

using namespace std;

int main(int argc, char ** argv)
{
	pthread_t saThread;

	maviInit();

	pthread_create(&saThread, NULL, maviSenseAndAnalyze, NULL);
	pthread_join(saThread, NULL);

	maviVibrate(MAVI_VIB_C | MAVI_VIB_L | MAVI_VIB_R, 0.5);
	return 0;
}
