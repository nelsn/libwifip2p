/*
 * CoreEngine.cpp
 *
 *  Created on: 06.02.2013
 *      Author: niels_w
 */

#include <list>
#include "wifip2p/CoreEngine.h"

using namespace std;

namespace wifip2p {

CoreEngine::CoreEngine(string ctrl_path, string name) throw (CoreEngineException) {
	this->name = name;
	this->initialize(ctrl_path.c_str());
}

CoreEngine::~CoreEngine() {
	// TODO Auto-generated destructor stub
}

/* Methods/Member functions >>
 *
 */
void CoreEngine::run() {
	;
}

void CoreEngine::connect(wifip2p::Peer peer) {
	;
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
	this->reinitialize();
}

void CoreEngine::reinitialize() {
	this->initialize(this->ctrl_path.c_str());
}

void CoreEngine::reinitialize(const char* ctrl_path, list<string> services) {
	;
}

bool CoreEngine::addService(string service) {
	this->services.push_back(service);
}

bool CoreEngine::addService(list<string> services) {
	;
}

void CoreEngine::initialize(const char* ctrl_path) throw (CoreEngineException) {
	this->actual_state = ST_IDLE;

	try {
		this->wpasup = wifip2p::SupplicantHandle(ctrl_path, false);
		this->wpamon = wifip2p::SupplicantHandle(ctrl_path, true);
	} catch (SupplicantHandleException &ex) {
		throw CoreEngineException("Error initializing handles: " + ex.what());
	}

	this->wpasup.init(name, services);

}


} /* namespace wifip2p */
