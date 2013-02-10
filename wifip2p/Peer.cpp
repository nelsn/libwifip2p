/*
 * Peer.cpp
 *
 *  Created on: 08.02.2013
 *      Author: niels_w
 */

#include "wifip2p/Peer.h"

namespace wifip2p {

Peer::Peer(const char *macc) throw (PeerException) {
	std::string mac(macc, sizeof(macc) - 1);
	if (!this->setMacAddr(mac))
		throw PeerException("No valid MAC address. Note: ");
				//+ "A valid MAC address is a 48bit hexdump with "
				//+ "every two byte pairs separated by a colon.");
}

Peer::~Peer() {
	// TODO Auto-generated destructor stub

}


std::string Peer::getMacAddr() throw (PeerException) {
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

bool Peer::validMacAddr(std::string mac) {
	if (mac.length() == 17) {
		for (int i=0; i<17; i++) {
			char *pt;
			*pt = mac;
			if (i == 2 || i == 5 || i == 8 || i == 11 || i == 14) {
				if (&pt[i] != ":")
					return false;
			} else {
				if ( (&pt[i]+=2 >= 30 && &pt[i]+=2 <= 39)
						|| (&pt[i]+=2 >= 41 && &pt[i]+=2 <= 46)
						|| (&pt[i]+=2 >= 61 && &pt[i]+=2 <= 66) )
					return true;
				else
					return false;
			}
		}
	} else {
		return false;
	}

	return false;
}

} /* namespace wifip2p */
