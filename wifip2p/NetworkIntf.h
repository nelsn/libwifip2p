/*
 * NetworkIntf.h
 *
 *  Created on: 15.02.2013
 *      Author: niels_w
 */

#ifndef NETWORKINTF_H_
#define NETWORKINTF_H_

#include <string>

using namespace std;

namespace wifip2p {

class NetworkIntf {
public:
	NetworkIntf();
	NetworkIntf(string name);
	virtual ~NetworkIntf();

	string getName();

private:
	string name;
};

} /* namespace wifip2p */
#endif /* NETWORKINTF_H_ */
