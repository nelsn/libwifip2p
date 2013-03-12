/*
 * WifiP2PInterface.h
 *
 *  Created on: 15.02.2013
 *      Author: niels_w
 */

#ifndef WIFIP2PINTERFACE_H_
#define WIFIP2PINTERFACE_H_

#include "wifip2p/Peer.h"
#include "wifip2p/Connection.h"

namespace wifip2p {

class WifiP2PInterface {

public:
	virtual ~WifiP2PInterface() { };

	virtual void peerFound(Peer peer) = 0;
	virtual void connectionEstablished(Connection conn) = 0;
	virtual void connectionLost(Connection conn) = 0;

};

} /* namespace wifip2p */
#endif /* WIFIP2PINTERFACE_H_ */
