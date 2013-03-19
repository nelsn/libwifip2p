/*
 * Connection.cpp
 *
 *  Created on: 15.02.2013
 *      Author: niels_w
 */

#include "wifip2p/Connection.h"

namespace wifip2p {

Connection::Connection(NetworkIntf nic)
	: peer(""), nic(nic) {

}

Connection::Connection(Peer peer, NetworkIntf nic)
	: peer(peer),
	  nic(nic) {

}

Connection::~Connection() {
	// TODO Auto-generated destructor stub
}

Peer Connection::getPeer() const {
	return this->peer;
}

void Connection::setPeer(Peer peer) {
	this->peer = peer;
}

NetworkIntf Connection::getNetworkIntf() const {
	return this->nic;
}

} /* namespace wifip2p */
