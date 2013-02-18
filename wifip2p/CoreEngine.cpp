/*
 * CoreEngine.cpp
 *
 *  Created on: 06.02.2013
 *      Author: niels_w
 */

#include "wifip2p/CoreEngine.h"

using namespace std;

namespace wifip2p {

CoreEngine::CoreEngine(string ctrl_path, string name)
		: wpasup(false),
		  wpamon(true),
		  actual_state(ST_IDLE),
		  name(name),
		  ctrl_path(ctrl_path) {

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
