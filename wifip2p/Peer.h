/*
 * Peer.h
 *
 *  Created on: 08.02.2013
 *      Author: niels_w
 */

#include <iostream>
#include <string>

#ifndef PEER_H_
#define PEER_H_

namespace wifip2p {

class PeerException : public std::exception {
public:
	PeerException(const std::string &what) : _what(what) {
	}

	~PeerException() throw () {
	}

	std::string what() {
		return _what;
	}

private:
	const std::string _what;
};


class Peer {

public:
	Peer(std::string) throw (PeerException); //Param std::string mac
	virtual ~Peer();

	std::string getMacAddr();

private:
	std::string mac_addr;

	bool setMacAddr(std::string mac);
	bool validMacAddr(std::string mac);

};

} /* namespace wifip2p */
#endif /* PEER_H_ */
