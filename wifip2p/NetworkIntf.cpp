/*
 * NetworkIntf.cpp
 *
 *  Created on: 14.02.2013
 *      Author: niels_w
 */

#include "NetworkIntf.h"

using namespace std;

namespace wifip2p {


NetworkIntf::NetworkIntf() {
	;
}

NetworkIntf::NetworkIntf(string ifname) {
	this->ifname = ifname;
}

NetworkIntf::~NetworkIntf() {
	// TODO Auto-generated destructor stub
}

const string& NetworkIntf::getIfname() const {
	return ifname;
}


} /* namespace wifip2p */
