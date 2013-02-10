/*
 * CoreEngine.h
 *
 *  Created on: 06.02.2013
 *      Author: niels_w
 */

#ifndef COREENGINE_H_
#define COREENGINE_H_

#include "wifip2p/SupplicantHandle.h"
#include "wifip2p/Peer.h"

namespace wifip2p {

class CoreEngineException : public std::exception {
public:
	CoreEngineException(const std::string &what) : _what(what) {
	}

	~CoreEngineException() throw () {
	}

	std::string what() {
		return _what;
	}

private:
	const std::string _what;
};


class CoreEngine {

public:
	CoreEngine();
	virtual ~CoreEngine();

	void run(); //really void? Or int:={-1=err, 0=okay}
	bool modInterests(std::list<std::string> interests);

	enum state { ST_CONN_NO,
				 ST_SCAN,
				 ST_SDREQ,
				 ST_CONNECT,
				 ST_CONN_EST };


private:
	std::list<std::string> peer_interests;

	wifip2p::SupplicantHandle wpa_in;
	wifip2p::SupplicantHandle wpa_monitor;

	std::list<wifip2p::Peer> peers;

	state actual_state;



};

} /* namespace wifip2p */

#endif /* COREENGINE_H_ */
