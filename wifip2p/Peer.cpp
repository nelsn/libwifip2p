/*
 * Peer.cpp
 *
 *  Created on: 08.02.2013
 *      Author: niels_w
 */

#include "wifip2p/Peer.h"
//TODO include may probably be removed from here.
#include <algorithm>

using namespace std;

namespace wifip2p {

Peer::Peer() {
	;
}

Peer::Peer(string mac)
	: mac_addr(mac) {

}

Peer::Peer(string mac, string name)
	: mac_addr(mac),
	  name(name) {

}

Peer::~Peer() {

}


bool Peer::operator==(const Peer &peer) const {
	return this->mac_addr == peer.mac_addr;
}


string Peer::getMacAddr() const {
	return this->mac_addr;
}


string Peer::getName() const {
	return this->name;
}


void Peer::setName(string name) {
	this->name = name;
}


} /* namespace wifip2p */
