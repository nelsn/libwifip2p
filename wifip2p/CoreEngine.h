/*
 * CoreEngine.h
 *
 *  Created on: 06.02.2013
 *      Author: niels_w
 */

#ifndef COREENGINE_H_
#define COREENGINE_H_

#include <list>
#include <set>
#include <string>

#include "wifip2p/SupplicantHandle.h"
#include "wifip2p/WifiP2PInterface.h"
#include "wifip2p/Timer.h"
#include "wifip2p/Connection.h"
#include "wifip2p/Peer.h"
#include "wifip2p/NetworkIntf.h"

using namespace std;

namespace wifip2p {

class CoreEngineException : public exception {
public:
	CoreEngineException(const string &what) : _what(what) {
	}

	~CoreEngineException() throw () {
	}

	string what() {
		return _what;
	}

private:
	const string _what;
};


class CoreEngine {

public:
	CoreEngine(string ctrl_path, string name, WifiP2PInterface &ext_if);
	virtual ~CoreEngine();


	SupplicantHandle wpasup;
	SupplicantHandle wpamon;


	void run();
	void stop();

	void connect(Peer peer);

	void disconnect(Connection connection);
	void disconnect(NetworkIntf nic);
	void disconnect(Peer peer);

	void reinitialize(string ctrl_path, list<string> services) throw (CoreEngineException);

	void addService(string service);

	void setName(string name);
	const string getName() const;

	void setTime_ST_IDLE(int s);
	const int getTime_ST_IDLE() const;
	void setTime_ST_SCAN(int s);
	const int getTime_ST_SCAN() const;
	void setTime_ST_SREQ(int s);
	const int getTime_ST_SREQ() const;


	enum state { ST_IDLE,
				 ST_SCAN,
				 ST_SREQ,
				 ST_CONN };


private:
	string ctrl_path;
	string name;
	list<string> services;
	set<string> sdreq_id;

	state actual_state;
	int st_idle_time,
		st_scan_time,
		st_sreq_time;

	bool running;

	int pipe_fds[2];

	list<Peer> peers;
	list<Connection> connections;

	WifiP2PInterface &ext_if;


	void initialize() throw (CoreEngineException);

	bool triggeredEvents(const SupplicantHandle &wpa, int seconds, state next);

};

} /* namespace wifip2p */

#endif /* COREENGINE_H_ */
