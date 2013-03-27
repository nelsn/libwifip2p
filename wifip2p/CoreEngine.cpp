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
		  st_idle_time(10),
		  st_scan_time(10),
		  st_sreq_time(20),
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


void CoreEngine::stop() {
	running = false;
	char buf[1] = { '\0' };
	::write(pipe_fds[1], buf, 1);

	try {
		wpasup.flushServices();
	} catch (SupplicantHandleException &ex) {
		cerr << "Services could not have been unregistered due to some "
				"exception raised: " << ex.what() << endl;
	}

}


void CoreEngine::run() {
	wpasup.open(this->ctrl_path.c_str());
	wpamon.open(this->ctrl_path.c_str());
	wpasup.init(this->name, this->services);

	list<Peer> peers;

	running = true;

	while (running) {

		switch(actual_state) {

		case ST_IDLE: {

			cout << "[ENTERED_STATE := IDLE_STATE]" << endl;

			triggeredEvents(wpamon, peers, st_idle_time, ST_SCAN);

			break;
		}

		case ST_SCAN: {

			cout << "[ENTERED_STATE := SCAN_STATE]" << endl;

			wpasup.findPeers();

			triggeredEvents(wpamon, peers, st_scan_time, ST_SCAN);

			wpasup.findPeersStop();

			if (!peers.empty()) {

				list<Peer>::iterator peer_it = peers.begin();
				bool all_known = true;

				for (; peer_it != peers.end(); ++peer_it) {
					if (peer_it->getName() == "") {
						all_known = false;
						break;
					} else {
						continue;
					}
				}

				if (all_known) {
					actual_state = ST_IDLE;
				} else {
					actual_state = ST_SREQ;
				}

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

			triggeredEvents(wpamon, peers, st_sreq_time, ST_SREQ);

			set<string>::iterator jt = sdreq_id.begin();
			for (; jt != sdreq_id.end(); ++jt)
				wpasup.requestServiceCancel(*jt);
			sdreq_id.clear();
			wpasup.findPeersStop();

			actual_state = ST_IDLE;

			break;
		}

		default:

			break;

		}
	}
}


void CoreEngine::connect(wifip2p::Peer peer) {

	// TODO
	// Rearrange try-catch. Write any catch exception to ::cerr
//	try {
		wpasup.connectToPeer(peer);
//
//	} catch (SupplicantHandleException &ex) {
//
//	}

}


void CoreEngine::disconnect(wifip2p::Connection connection) {

	wpasup.disconnect(connection);

}


void CoreEngine::disconnect(wifip2p::NetworkIntf nic) {
	list<Connection>::iterator conn_it = connections.begin();

	for (; conn_it != connections.end(); ++conn_it) {
		if (conn_it->getNetworkIntf() == nic) {
			disconnect(*conn_it);
		}
	}
}


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


void CoreEngine::reinitialize(string ctrl_path, list<string> services) throw (CoreEngineException) {
	this->ctrl_path = ctrl_path;
	this->services = services;
	try {
		this->initialize();
	} catch (CoreEngineException &ex) {
		throw CoreEngineException(ex.what());
	}
}


void CoreEngine::triggeredEvents(const SupplicantHandle &wpa, list<Peer> &peers, int seconds, state next) {

	fd_set fds;
	int wpa_fd = wpa.getFD();

	if (wpa_fd <= 0) return;

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

	while (::select(high_fd + 1, &fds, NULL, NULL, &tv) != 0)
	{
		if (FD_ISSET(wpa_fd, &fds)) {
			wpamon.listen(peers, connections, services, sdreq_id, ext_if);
		}

		if (FD_ISSET(pipe_fds[0], &fds)) break;

		FD_SET(wpa_fd, &fds);
		FD_SET(pipe_fds[0], &fds);
	}

	actual_state = next;
}


void CoreEngine::setName(string name) {
	this->name = name;
	this->initialize();
}


const string CoreEngine::getName() const {
	return this->name;
}


void CoreEngine::setTime_ST_IDLE(int s) {
	this->st_idle_time = s;
}


const int CoreEngine::getTime_ST_IDLE() const {
	return this->st_idle_time;
}


void CoreEngine::setTime_ST_SCAN(int s) {
	this->st_scan_time = s;
}


const int CoreEngine::getTime_ST_SCAN() const {
	return this->st_scan_time;
}


void CoreEngine::setTime_ST_SREQ(int s) {
	this->st_sreq_time = s;
}


const int CoreEngine::getTime_ST_SREQ() const {
	return this->st_sreq_time;
}


void CoreEngine::addService(string service) {
	this->services.push_back(service);
}



} /* namespace wifip2p */
