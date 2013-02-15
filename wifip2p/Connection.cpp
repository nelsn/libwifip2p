/*
 * Connection.cpp
 *
 *  Created on: 14.02.2013
 *      Author: niels_w
 */

#include "wifip2p/Connection.h"

namespace wifip2p {

Connection::Connection(NetworkIntf nic, Peer peer) {
	this->nic = nic;
	this->peer = peer;
}

Connection::~Connection() {
	// TODO Auto-generated destructor stub
}

const wifip2p::NetworkIntf& Connection::getNic() {
	return nic;
}

const Peer& Connection::getPeer() {
	return peer;
}


} /* namespace wifip2p */
