/*
 * Connection.h
 *
 *  Created on: 15.02.2013
 *      Author: niels_w
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "wifip2p/Peer.h"
#include "wifip2p/NetworkIntf.h"

namespace wifip2p {

class Connection {
public:
	Connection();
	Connection(Peer peer, NetworkIntf nic);
	virtual ~Connection();

	Peer getPeer();
	NetworkIntf getNetworkIntf();

private:
	Peer 		peer;
	NetworkIntf nic;

};

} /* namespace wifip2p */
#endif /* CONNECTION_H_ */
