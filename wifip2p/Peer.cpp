/*
 * Peer.cpp
 *
 *  Created on: 08.02.2013
 *      Author: niels_w
 */

#include "wifip2p/Peer.h"

namespace wifip2p {

Peer::Peer(std::string mac) throw (PeerException) {

	this->mac_addr = mac;

	/*
	if (!this->setMacAddr(mac))
		throw PeerException("No valid MAC address. Note: "
				"A valid MAC address is a 48bit hexdump with "
				"every two byte pairs separated by a colon.");
	*/
}

Peer::~Peer() {
	// TODO Auto-generated destructor stub

}


std::string Peer::getMacAddr() {
	return this->mac_addr;
}

bool Peer::setMacAddr(std::string mac) {
	if (this->validMacAddr(mac)) {
		this->mac_addr = mac;
		return true;
	} else {
		return false;
	}
}


/**
 * ATTENTION! Method is erroneous!!
 * This method is called by setMacAddr(), which in turn is no more used, so far.
 * ATTENTION! Method is erroneous!!
 *
 */
bool Peer::validMacAddr(std::string mac) {
	bool ret = true;
	int x = mac.length();
	std::cout << "mac: " << mac << "; mac_len: " << x << std::endl;
	if (mac.length() == 17) {
		for (int i=0; i<17; i++) {
			std::cout << "LOOP: " << (char) mac[i] << (int) mac[i] << std::endl;
			int s = mac[i];
			if (i == 2 || i == 5 || i == 8 || i == 11 || i == 14) {
				if (s != 58){
					std::cout << "INNERPART_KOTZ_i:" << (int) mac[i] << (char) mac[i] << i << std::endl;
					return false;
				}
			} else {
				std::cout << s << std::endl;
				if ((48 <= s <= 57) || (65 <= s <= 70) || (97 <= s <= 102)) {
					std::cout << "INNERPART" << std::endl;
					continue;
				}
				else {
					std::cout << "RAUS" << std::endl;
					return false;
					}
			}
		}
		return true;
	} else {
		return false;
	}
	return false;
}

} /* namespace wifip2p */
