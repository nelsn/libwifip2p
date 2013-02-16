/*
 * Connection.cpp
 *
 *  Created on: 15.02.2013
 *      Author: niels_w
 */

#include "wifip2p/Connection.h"

namespace wifip2p {

Connection::Connection(Peer peer, NetworkIntf nic) {
	this->peer = peer;
	this->nic = nic;
}

Connection::~Connection() {
	// TODO Auto-generated destructor stub
}

Peer Connection::getPeer() {
	return this->peer;
}

NetworkIntf Connection::getNetworkIntf() {
	return this->nic;
}

} /* namespace wifip2p */
