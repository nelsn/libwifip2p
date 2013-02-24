/*
 * TestSupplicantHandle.cpp
 *
 *  Created on: 19.02.2013
 *      Author: niels_w
 */

#include "wifip2p/TestSupplicantHandle.h"
#include "wifip2p/TestExternalWifiP2P.cpp"

namespace wifip2p {

TestSupplicantHandle::TestSupplicantHandle(bool monitor)
	: SupplicantHandle(monitor) {

}

TestSupplicantHandle::~TestSupplicantHandle() {
	// TODO Auto-generated destructor stub
}

void TestSupplicantHandle::functionsTest(const char *ctrl_path) {

	try {
		this->open(ctrl_path);

		list<string> services;
		services.push_back("Serv1");
		services.push_back("Serv2");
		this->init("DevName", services);

		Peer p1("aa:bb:cc:dd:ee:ff", "PeersName");
		Peer p2("30:39:26:00:e9:9d", "Android_9cd");

		list<Peer> peers;
		peers.push_back(p1);
		peers.push_back(p2);

		NetworkIntf nic("p2p-wlan1-18");
		Connection conn(p1, nic);

		TestExternalWifiP2P ext_if_dummy();
		WifiP2PInterface wipi();

		/** Must-fails as tested with non-P2P compatible device, i.e.
		 *	 (1) p2p_find not possible
		 *	 (2) whether peers where to be found, a peer with mac aa:bb:..
		 *	 		is not existant.
		 *	 (3) as no connection to any peer is established, there simply
		 *	 		is no virtual P2P group interface which may be removed
		 */
		this->findPeers();
		//this->connectToPeer(p1);
		//this->disconnect(conn);

		this->listen(&peers, &ext_if_dummy);

		this->findPeersStop();



	} catch (SupplicantHandleException &ex) {
		std::cerr << ex.what() << std::endl;
	}
}


} /* namespace wifip2p */
