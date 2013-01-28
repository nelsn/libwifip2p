/*
 * InterfacePowerSaving.h
 *
 *  Created on: 28.01.2013
 *      Author: niels_w
 */

#ifndef INTERFACEPOWERSAVING_H_
#define INTERFACEPOWERSAVING_H_

/* InterfacePowerSaving - Interface class; Actual
 * 		implementations of arbitrary complexity needs to
 * 		being placed in this i/f's extending classes.
 */
class InterfacePowerSaving {

public:
	/* pwrSavingLoop - no 802.11 scanning/p2p_find while
	 * 			remaining in this loop.
	 * Returns: false, if the loop is completely gone through,
	 * 			true, otherwise.
	 */
	virtual ~InterfacePowerSaving() {
	}
	;
	virtual bool pwrSavingLoop() const = 0;

private:

};

class PowerSaving: public InterfacePowerSaving {
public:
	PowerSaving();
	virtual ~PowerSaving();
};
