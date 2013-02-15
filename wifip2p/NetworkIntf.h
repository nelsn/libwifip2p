/*
 * NetworkIntf.h
 *
 *  Created on: 14.02.2013
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
	NetworkIntf(string ifname);
	virtual ~NetworkIntf();
	const string& getIfname() const;

private:
	string ifname;

};

} /* namespace wifip2p */
#endif /* NETWORKINTF_H_ */
