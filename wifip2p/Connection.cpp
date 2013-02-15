/*
 * Connection.cpp
 *
 *  Created on: 14.02.2013
 *      Author: niels_w
 */

#include "Connection.h"

namespace wifip2p {

Connection::Connection(NetworkIntf nic, Peer peer) {
	this->nic = nic;
	this->peer = peer;
}

Connection::~Connection() {
	// TODO Auto-generated destructor stub
}

} /* namespace wifip2p */

const wifip2p::NetworkIntf& Connection::getNic() const {
	return nic;
}

const wifip2p::Peer& Connection::getPeer() const {
	return peer;
}
