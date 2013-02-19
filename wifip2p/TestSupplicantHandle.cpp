/*
 * TestSupplicantHandle.cpp
 *
 *  Created on: 19.02.2013
 *      Author: niels_w
 */

#include "wifip2p/TestSupplicantHandle.h"

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
		NetworkIntf nic("wlan0");
		Connection conn(p1, nic);

		/** Must-fails as tested with non-P2P compatible device, i.e.
		 *	 (1) p2p_find not possible
		 *	 (2) whether peers where to be found, a peer with mac aa:bb:..
		 *	 		is not existant.
		 *	 (3) as no connection to any peer is established, there simply
		 *	 		is no virtual P2P group interface which may be removed
		 */
		//this->findPeers();
		//this->connectToPeer(p1);
		//this->disconnect(conn);


		this->findPeersStop();

		this->listen();

	} catch (SupplicantHandleException &ex) {
		std::cerr << ex.what() << std::endl;
	}
}


} /* namespace wifip2p */
