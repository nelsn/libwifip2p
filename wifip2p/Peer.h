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
	//Peer();
	Peer(const string &mac);
	Peer(const string &mac, const string &name);
	virtual ~Peer();

	string getMacAddr() const;
	string getName() const;
	void setName(string name);

	/**
	 * Matches the actual peer against the input parameter peer.
	 * Equality is explicitly defined as matching MAC addresses; as
	 *  a peer's name is not of any use regarding this issue.
	 *
	 * @peer:  peer to be checked for equality.
	 * Return: true or false, whether this = peer.
	 *
	 */
	bool operator==(const Peer &peer) const;

private:
	string mac_addr;
	string name;

};

} /* namespace wifip2p */
#endif /* PEER_H_ */
