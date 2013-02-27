/*
 * Peer.h
 *
 *  Created on: 08.02.2013
 *      Author: niels_w
 */

#include <iostream>
#include <list>
#include <string>

#ifndef PEER_H_
#define PEER_H_

using namespace std;

namespace wifip2p {

class PeerException : public exception {
public:
	PeerException(const string &what) : _what(what) {
	}

	~PeerException() throw () {
	}

	string what() {
		return _what;
	}

private:
	const string _what;
};


class Peer {

public:
	Peer(string mac);
	Peer(string mac, string name);
	virtual ~Peer();

	string getMacAddr();
	string getName();

	bool equals(Peer peer);
	bool inList(const list<Peer> &peers, Peer *peer);

private:
	string mac_addr;
	string name;

	bool setMacAddr(string mac);
	bool validMacAddr(string mac);

};

} /* namespace wifip2p */
#endif /* PEER_H_ */
