/*
 * CoreEngine.cpp
 *
 *  Created on: 06.02.2013
 *      Author: niels_w
 */

#include "wifip2p/CoreEngine.h"
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

namespace wifip2p {

CoreEngine::CoreEngine(string ctrl_path, string name, WifiP2PInterface &ext_if)
		: wpasup(false),
		  wpamon(true),
		  actual_state(ST_IDLE),
		  running(false),
		  ext_if(ext_if),
		  name(name),
		  ctrl_path(ctrl_path) {

	::pipe(pipe_fds);
}

CoreEngine::~CoreEngine() {
	::close(pipe_fds[0]);
	::close(pipe_fds[1]);
}

/* Methods/Member functions >>
 *
 */

void CoreEngine::stop() {
	running = false;
	char buf[1] = { '\0' };
	::write(pipe_fds[1], buf, 1);
}

void CoreEngine::run() {
	wpasup.open(this->ctrl_path.c_str());
	wpamon.open(this->ctrl_path.c_str());
	wpasup.init(this->name, this->services);

	running = true;

	while (running) {

		switch(actual_state) {

		case ST_IDLE: {

			cout << "[ENTERED_STATE := IDLE_STATE]" << endl;

			if (triggeredEvents(wpamon, 10, ST_SCAN)) {
				wpamon.listen(peers, connections, services, sdreq_id, ext_if);
			}

			break;
		}

		case ST_SCAN: {

			cout << "[ENTERED_STATE := SCAN_STATE]" << endl;

			wpasup.findPeers();

			if (triggeredEvents(wpamon, 10, ST_SCAN)) {
				wpamon.listen(peers, connections, services, sdreq_id, ext_if);
			}

			wpasup.findPeersStop();

			if (!peers.empty()) {
				actual_state = ST_SREQ;
			} else {
				actual_state = ST_IDLE;
			}

			break;
		}

		case ST_SREQ: {

			cout << "[ENTERED_STATE := SD_REQUEST_STATE]" << endl;

			list<string>::iterator it = services.begin();
			for (; it != services.end(); ++it)
				wpasup.requestService(*it, sdreq_id);

			wpasup.findPeers();

			do {

				wpamon.listen(peers, connections, services, sdreq_id, ext_if);

			} while(triggeredEvents(wpamon, 20, ST_SREQ));

			set<string>::iterator jt = sdreq_id.begin();
			for (; jt != sdreq_id.end(); ++jt)
				wpasup.requestServiceCancel(*jt);
			sdreq_id.clear();
			wpasup.findPeersStop();

			if (!connections.empty()) {
				actual_state = ST_CONN;
			} else {
				if (peers.empty()) {
					actual_state = ST_SCAN;
				}
			}

			break;
		}

		case ST_CONN: {

			cout << "[ENTERED_STATE := CONNECTED_STATE]" << endl;

			while (!connections.empty()) {

				if (triggeredEvents(wpamon, 5, ST_CONN)) {
					wpamon.listen(peers, connections, services, sdreq_id, ext_if);
				}

			}

			actual_state = ST_IDLE;

			break;
		}

		default:

			break;

		}
	}
}

/**
 * Enables to connect to a specifically known peer. After calling the respective
 * 	method at wpasup, a state change is needed. If the connection my not be
 * 	established, for whatever reason, the connections-list may be evaluated to
 * 	empty and in turn another state change will take place.
 *
 * @peer: The Peer object to connect to.
 *
 */
void CoreEngine::connect(wifip2p::Peer peer) {

	wpasup.connectToPeer(peer);

	actual_state = ST_CONN;

}

/**
 * Cancel a specifically known connection per respective Connection object.
 *
 * @connection: The Connection objects data required for canceling the connection.
 *
 */
void CoreEngine::disconnect(wifip2p::Connection connection) {

	wpasup.disconnect(connection);

}

/**
 * Iterates over all the yet listed connections searching for one with the respective
 * 	(virtual) network interface name. If found: disconnect this, else do nothing.
 *
 * @nic: The to be removed network interface.
 *
 */
void CoreEngine::disconnect(wifip2p::NetworkIntf nic) {
	list<Connection>::iterator conn_it = connections.begin();

	for (; conn_it != connections.end(); ++conn_it) {
		if (conn_it->getNetworkIntf().getName() == nic.getName()) {
			disconnect(*conn_it);
		}
	}
}

/**
 * Iterates over all the yet listed connections searching for one with the respective
 * 	peer's MAC address. If found: disconnect this, else do nothing.
 *
 * @peer: The peer a running connection to should be quit.
 *
 */
void CoreEngine::disconnect(wifip2p::Peer peer) {
	list<Connection>::iterator conn_it = connections.begin();

	for (; conn_it != connections.end(); ++conn_it) {
		if (conn_it->getPeer() == peer) {
			disconnect(*conn_it);
		}
	}
}

void CoreEngine::initialize() throw (CoreEngineException) {

	try {
		this->wpasup.open(this->ctrl_path.c_str());
		this->wpamon.open(this->ctrl_path.c_str());
	} catch (SupplicantHandleException &ex) {
		throw CoreEngineException("Error initializing handles: " + ex.what());
	}

	this->actual_state = ST_IDLE;

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

bool CoreEngine::triggeredEvents(const SupplicantHandle &wpa, int seconds, state next) {

	fd_set fds;
	int wpa_fd = wpa.getFD();

	if (wpa_fd <= 0) return false;

	FD_ZERO(&fds);
	FD_SET(wpa_fd, &fds);
	FD_SET(pipe_fds[0], &fds);

	int high_fd = wpa_fd;

	struct timeval tv;
	tv.tv_sec = seconds;
	tv.tv_usec = 0;

	if (high_fd < pipe_fds[0]) {
		high_fd = pipe_fds[0];
	}

	int res = ::select(high_fd + 1, &fds, NULL, NULL, &tv);

	if (res == 0) {
		actual_state = next;
	}

	return FD_ISSET(wpa_fd, &fds);
}

void CoreEngine::setName(string name) {
	this->name = name;
	this->initialize();
}

void CoreEngine::addService(string service) {
	this->services.push_back(service);
}

void CoreEngine::addService(list<string> services) {
	;
}


} /* namespace wifip2p */
