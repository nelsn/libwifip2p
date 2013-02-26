/*
 * TestExternalWifiP2P.cpp
 *
 *  Created on: 24.02.2013
 *      Author: niels_w
 */

#include <wifip2p/WifiP2PInterface.h>

using namespace std;

namespace wifip2p {

class TestExternalWifiP2P : public WifiP2PInterface {
public:
	TestExternalWifiP2P() {
		;
	}

	void peerFound(Peer peer) {
		cout << "Yeah, a Peer " << peer.getName() << " is found!!" << endl;
		//cout << "Yeah, a Peer is found!!" << endl;
	}


	void connectionEstablished(Connection conn) {
		;
	}

	void connectionLost(Connection conn) {
		;
	}


};

}
