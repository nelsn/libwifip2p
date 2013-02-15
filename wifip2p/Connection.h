/*
 * Connection.h
 *
 *  Created on: 14.02.2013
 *      Author: niels_w
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <wifip2p/NetworkIntf.h>
#include <wifip2p/Peer.h>


namespace wifip2p {

class Connection {
public:
	Connection(NetworkIntf nic, Peer peer);
	virtual ~Connection();
	const NetworkIntf& getNic();
	const Peer& getPeer();

private:
	NetworkIntf nic;
	Peer peer;

};

} /* namespace wifip2p */
#endif /* CONNECTION_H_ */
