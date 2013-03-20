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

	/**
	 * Hands out fully discovered peers, i.e. peers which are candidates
	 *  matching for a connection, as they returned successfully from libwifip2p's
	 *  automated service discovery phase.
	 *
	 * @peer: Fully discovered potentially interesting peer for a P2P connection.
	 *
	 */
	virtual void peerFound(Peer peer) = 0;

	/**
	 * Hands out a fully discovered peer object to the respective interface
	 *  implementation. In turn, the concrete implementation becomes responsible
	 *  for checking back if a p2p group to this peer is already created or not.
	 *
	 * @peer: Peer, obviously interested in a holding a P2P connection to this
	 * 		   local station.
	 *
	 */
	virtual void connectionRequest(Peer peer) = 0;

	/**
	 * If libwifip2p may successfully establish a P2P connection, this connection will
	 *  be handed out for being externally managed.
	 *
	 * @conn: The just created connection for which a P2P Group Formation was
	 * 			successful.
	 *
	 */
	virtual void connectionEstablished(Connection conn) = 0;

	/**
	 * If libwifip2p registers a connection as lost, due to wpa_s' respectively
	 *  created event messages, the related connection data will be handed out to
	 *  the external software.
	 *
	 * @conn: The just lost or disconnected connection.
	 *
	 */
	virtual void connectionLost(Connection conn) = 0;

};

} /* namespace wifip2p */
#endif /* WIFIP2PINTERFACE_H_ */
