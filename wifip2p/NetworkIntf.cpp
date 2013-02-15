/*
 * NetworkIntf.cpp
 *
 *  Created on: 14.02.2013
 *      Author: niels_w
 */

#include "NetworkIntf.h"

namespace wifip2p {

NetworkIntf::NetworkIntf(std::string ifname) {
	this->ifname = ifname;
}

NetworkIntf::~NetworkIntf() {
	// TODO Auto-generated destructor stub
}

} /* namespace wifip2p */

const string& NetworkIntf::getIfname() const {
	return ifname;
}
