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
		services.push_back("IBRDTN");
		//services.push_back("Whatever");
		this->init("DevName", services);

		set<string> sdreq_id;

//		Peer p1("aa:bb:cc:dd:ee:ff", "PeersName");
//		Peer p2("30:39:26:00:e9:9d", "Android_9cd");
		Peer p3("00:0c:42:6c:a6:e8", "dell_latitude");

		list<Peer> peers;
//		peers.push_back(p1);
//		peers.push_back(p2);
		peers.push_back(p3);

		list<Connection> connections;

//		NetworkIntf nic("p2p-wlan1-18");
//		Connection conn(p1, nic);

		TestExternalWifiP2P ext_if_dummy;

		list<string>::iterator sdr_it = services.begin();
		for (; sdr_it != services.end(); ++sdr_it) {
			this->requestService(*sdr_it, sdreq_id);
		}
		this->findPeers();

		/** Must-fails as tested with non-P2P compatible device, i.e.
		 *	 (1) p2p_find not possible
		 *	 (2) whether peers where to be found, a peer with mac aa:bb:..
		 *	 		is not existant.
		 *	 (3) as no connection to any peer is established, there simply
		 *	 		is no virtual P2P group interface which may be removed
		 */
		//this->connectToPeer(p1);
		//this->disconnect(conn);


		for (int i=10000000; i!=0; i--) {
			this->listen(peers, connections, services, sdreq_id, ext_if_dummy);
		}

//		list<Peer>::iterator it = peers.begin();
//		for (; it != peers.end(); ++it) {
//			if (it->getName() != "")
//				this->connectToPeer(*it);
//			cout << "Peer: " << it->getMacAddr() << "_" << it->getName() << endl;
//		}

		for (int i=10000000; i!=0; i--) {
			this->listen(peers, connections, services, sdreq_id, ext_if_dummy);
		}


		cout << "# of sd_req's on air:  " << sdreq_id.size() << endl;

		set<string>::iterator id_it = sdreq_id.begin();
		for (; id_it != sdreq_id.end(); ++id_it) {
			requestServiceCancel(*id_it);
		}
		sdreq_id.clear();

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

		list<Connection>::iterator conn_it = connections.begin();

		for (; conn_it != connections.end(); ++conn_it) {
			cout << "Connection: " << conn_it->getNetworkIntf().getName()
					<< " with Peer " << conn_it->getPeer().getMacAddr()
					<< "; " << conn_it->getPeer().getName() << endl;
			disconnect(*conn_it);
		}


		this->findPeersStop();



	} catch (SupplicantHandleException &ex) {
		std::cerr << ex.what() << std::endl;
	}
}


} /* namespace wifip2p */
