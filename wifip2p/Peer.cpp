/*
 * Peer.cpp
 *
 *  Created on: 08.02.2013
 *      Author: niels_w
 */

#include "wifip2p/Peer.h"
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

	/*
	 * Yet taken out, because this address validating is not
	 * 	necessarily needed.
	 * 	The address gets assigned, but not by calling the
	 * 	specifically defined private method.
	 *
	 * 	TODO (Either fixing and making this process work or deleting it.)
	 *
	 *	if (!this->setMacAddr(mac))
	 *		throw PeerException("No valid MAC address. Note: "
	 *				"A valid MAC address is a 48bit hexdump with "
	 *				"every two byte pairs separated by a colon.");
	 *
	 */
}

Peer::~Peer() {
	// TODO Auto-generated destructor stub

}

/**
 * Matches the actual peer against the input parameter peer.
 * Equality is explicitly defined as matching MAC addresses, as
 *  a peer's name is not of any use regarding this issue.
 *
 * @peer:  peer to be checked for equality.
 * Return: true or false, whether this = peer.
 *
 */
bool Peer::operator==(const Peer &peer) const {
	if (this->mac_addr == peer.mac_addr)
		return true;
	else
		return false;
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

/**
 * TODO (Deletion when never being used)
 */
bool Peer::setMacAddr(string mac) {
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
 * TODO (Fixing and testing or deletion.)
 *
 */
bool Peer::validMacAddr(string mac) {
	bool ret = true;
	int x = mac.length();
	cout << "mac: " << mac << "; mac_len: " << x << endl;
	if (mac.length() == 17) {
		for (int i=0; i<17; i++) {
			cout << "LOOP: " << (char) mac[i] << (int) mac[i] << endl;
			int s = mac[i];
			if (i == 2 || i == 5 || i == 8 || i == 11 || i == 14) {
				if (s != 58){
					cout << "INNERPART_KOTZ_i:" << (int) mac[i] << (char) mac[i] << i << endl;
					return false;
				}
			} else {
				cout << s << endl;
				if ((48 <= s <= 57) || (65 <= s <= 70) || (97 <= s <= 102)) {
					cout << "INNERPART" << endl;
					continue;
				}
				else {
					cout << "RAUS" << endl;
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
