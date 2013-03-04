/*
 * Timer.cpp
 *
 *  Created on: 15.02.2013
 *      Author: niels_w
 */

#include "wifip2p/Timer.h"

#include <iostream>

using namespace std;

namespace wifip2p {

Timer::Timer()
	: minutes(0),
	  seconds(0),
	  running(false),
	  stoptime(time(NULL)) {

}

Timer::~Timer() {

}

bool Timer::isRunning() {
	return running;
}


void Timer::setTimer(int m, int s) {
	time(&stoptime + (m*60) + s);
	running = true;
}

/**
 * <Description>
 *
 * Returns: true, if the stoptime  has elapsed is;
 * 			 false else.
 *
 */
bool Timer::timeout() {

	if (difftime(time(NULL), stoptime) < 0) {
		running = false;
		cout << "Time is up now." << endl;
		return true;
	} else {
		cout << difftime(stoptime, time(NULL)) << endl;
		return false;
	}
}

} /* namespace wifip2p */
