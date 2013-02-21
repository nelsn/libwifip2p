/*
 * Timer.h
 *
 *  Created on: 15.02.2013
 *      Author: niels_w
 */

#ifndef TIMER_H_
#define TIMER_H_

namespace wifip2p {

class Timer {
public:
	Timer();
	virtual ~Timer();

	bool isRunning();
	void setTimer(int m, int s);
	bool timeout();

private:
	int minutes;
	int seconds;

};

} /* namespace wifip2p */
#endif /* TIMER_H_ */
