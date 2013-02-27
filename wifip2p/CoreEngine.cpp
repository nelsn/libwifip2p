/*
 * CoreEngine.cpp
 *
 *  Created on: 06.02.2013
 *      Author: niels_w
 */

#include "wifip2p/CoreEngine.h"

using namespace std;

namespace wifip2p {

CoreEngine::CoreEngine(string ctrl_path, string name, WifiP2PInterface &ext_if)
		: wpasup(false),
		  wpamon(true),
		  actual_state(ST_IDLE),
		  ext_if(ext_if),
		  name(name),
		  ctrl_path(ctrl_path),
		  timer(0, 0) {

}

CoreEngine::~CoreEngine() {
	// TODO Auto-generated destructor stub
}

/* Methods/Member functions >>
 *
 */
void CoreEngine::run() {
	wpasup.open(this->ctrl_path.c_str());
	wpamon.open(this->ctrl_path.c_str());
	wpasup.init(this->name, this->services);

	bool running = true;

	while (running) {

		switch(actual_state) {

		case ST_IDLE: {

			do {

				if (!timer.isRunning())
					timer.setTimer(2, 0);

				wpamon.listen(peers, connections, services, sdreq_id, ext_if);

			} while(!timer.timeout());

			actual_state = ST_SCAN;

			break;
		}

		case ST_SCAN: {

			wpasup.findPeers();

			do {

				if (!timer.isRunning())
					timer.setTimer(1, 0);

				//wpamon.listen(&peers, ext_if);

			} while(!timer.timeout());

			wpasup.findPeersStop();

			if (!peers.empty()) {
				actual_state = ST_SREQ;
			} else {
				actual_state = ST_IDLE;
			}

			break;
		}

		case ST_SREQ: {

			wpasup.findPeers();

			list<string>::iterator it = services.begin();
			for (; it != services.end(); ++it)
				wpasup.requestService(*it, &sdreq_id);

			do {
				if (!timer.isRunning())
					timer.setTimer(1, 0);

				//wpamon.listen(&peers, ext_if);

			} while(!timer.timeout());

			list<string>::iterator jt = sdreq_id.begin();
			for (; jt != sdreq_id.end(); ++jt)
				wpasup.requestServiceCancel(*jt);

			wpasup.findPeersStop();

			break;
		}

		case ST_CONN: {

			while (!connections.empty()) {

				//wpamon.listen(&peers, ext_if);

			}

			break;
		}

		default:

			break;

		}
	}
}

void CoreEngine::connect(wifip2p::Peer peer) {

	wpasup.connectToPeer(peer);

	actual_state = ST_CONN;

	while (true) {

		//wpamon.listen();

	}
}

void CoreEngine::disconnect(wifip2p::Connection connection) {
	;
}

void CoreEngine::disconnect(wifip2p::NetworkIntf nic) {
	;
}

void CoreEngine::disconnect(wifip2p::Peer peer) {
	;
}

void CoreEngine::setName(string name) {
	this->name = name;
	this->initialize();
}

/**
 * TODO Verify that calling ::initialize()::open() on already opened _handle
 *  connections is not leading to any error.
 *  Maybe to define and implement a method *terminate()*, closing all the connection
 *  of the respective wpasup and wpamon _handles.
 */
void CoreEngine::reinitialize(string ctrl_path, list<string> services) throw (CoreEngineException) {
	this->ctrl_path = ctrl_path;
	this->services = services;
	try {
		this->initialize();
	} catch (CoreEngineException &ex) {
		throw CoreEngineException(ex.what());
	}
}

bool CoreEngine::addService(string service) {
	this->services.push_back(service);
}

bool CoreEngine::addService(list<string> services) {
	;
}

void CoreEngine::initialize() throw (CoreEngineException) {

	try {
		this->wpasup.open(this->ctrl_path.c_str());
		this->wpamon.open(this->ctrl_path.c_str());
	} catch (SupplicantHandleException &ex) {
		throw CoreEngineException("Error initializing handles: " + ex.what());
	}

	this->actual_state = ST_IDLE;

	//this->wpasup.init(name, services);

}


} /* namespace wifip2p */
