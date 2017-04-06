/*
 * mavi-main.cpp
 * -------------
 *
 * This is the entry point for the MAVI program.
 */

#include <cstdlib>
#include <wiringPi.h>
#include <pthread.h>

#include "mavi-analysis.hpp"
#include "mavi-init.hpp"
#include "mavi-feedback.hpp"

using namespace std;

int main(int argc, char ** argv)
{
	maviInit();
	maviSenseAndAnalyze(NULL);
	maviSendFeedback(MAVI_FEEDBACK_SYSTEM_SHUTDOWN);
	delay(MAVI_VIBRATION_DURATION);
	return 0;
}
