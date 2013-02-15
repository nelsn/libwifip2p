/*
 * CoreEngine.cpp
 *
 *  Created on: 06.02.2013
 *      Author: niels_w
 */

#include <list>
#include "wifip2p/CoreEngine.h"

namespace wifip2p {

CoreEngine::CoreEngine(const char *ctrl_path, string name) throw (CoreEngineException) {
	this->name = name;
	this->initialize(ctrl_path);
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

void CoreEngine::setName(std::string name) {
	this->name = name;
	this->reinitialze();
}

void CoreEngine::reinitialize(const char* ctrl_path) {
	this->initialize();
}

void CoreEngine::reinitialize(const char* ctrl_path, std::list<string> services) {
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

	this->wpasup = wifip2p::SupplicantHandle(ctrl_path, false);
	this->wpamon = wifip2p::SupplicantHandle(ctrl_path, true);

	if (this->wpasup == NULL || this->wpamon == NULL)
		throw CoreEngineException("Error initializing SupplicantHandles.");

	this->wpasup.setDeviceName(name);
}


} /* namespace wifip2p */
