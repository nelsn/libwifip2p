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

		list<Connection> connections;

		NetworkIntf nic("p2p-wlan1-18");
		Connection conn(p1, nic);

		TestExternalWifiP2P ext_if_dummy;

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

		this->listen(peers, connections, ext_if_dummy);


		/** Testing SupplicantHandle::msgDecompose(*char)
		 * 	Method needs to be in SupplicantHandle's public section,
		 * 		whether it's supposed to be a private method.
		 */
		/*
		char pseudo_buf[256] = "<3>P2P-DEVICE-FOUND 30:39:26:00:e9:9d p2p_dev_addr=30:39:26:00:e9:9d pri_dev_type=10-0050F204-5 name='Android_9cdb' config_methods=0x188 dev_capab=0x21 group_capab=0x0";

		vector<string> msgvec;
		msgvec = msgDecompose(pseudo_buf);

		vector<string>::iterator it = msgvec.begin();
		for (; it != msgvec.end(); ++it) {
			cout << *it << endl;
		}
		// Testing SupplicantHandle::msgDecompose(*char) */


		this->findPeersStop();



	} catch (SupplicantHandleException &ex) {
		std::cerr << ex.what() << std::endl;
	}
}


} /* namespace wifip2p */
