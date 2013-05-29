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
#include "wifip2p/Connection.h"
#include "wifip2p/Peer.h"
#include "wifip2p/NetworkIntf.h"
#include "wifip2p/Logger.h"

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

	/**
	 * Constructs a CoreEngine object initializing all variables to proper values.
	 * 	The state timer are initialized to (10, 10, 20) seconds for (st_idle,
	 * 	st_scan, st_sreq). Each of the time values may be changed independently
	 * 	of the others by using the respective setter method.
	 *
	 * @ctrl_path: string representing the path to the wpa_s ctrl_i/f domain socket.
	 * @name:	   The string representation of this devices name. This one  will
	 * 				be used to register services accordingly to the general naming
	 * 				conventions at the wpa_s. Furthermore this name will be set at the
	 * 				devices SoftMAC, if the driver allows for.
	 * @&ext_if:   Reference to the external interface implementation used for call
	 * 				backs from SupplicantHandle objects.
	 *
	 */
	CoreEngine(string ctrl_path, string name, WifiP2PInterface &ext_if, Logger &logger);

	virtual ~CoreEngine();


	SupplicantHandle wpasup;
	SupplicantHandle wpamon;

	/**
	 * Makes a CoreEngine setting up all SupplicantHandle connections, initializing
	 *  and running in the state machine's loop.
	 *
	 */
	void run();

	/**
	 * Stops a running CoreEngine.
	 *
	 */
	void stop();

	/**
	 * Enables to connect to a specifically known peer.
	 *
	 * @peer: The Peer object to connect to.
	 *
	 */
	void connect(Peer peer);

	/**
	 * Cancel a specifically known connection per respective Connection object.
	 *
	 * @connection: The Connection objects data required for canceling the connection.
	 *
	 */
	void disconnect(Connection connection);

	/**
	 * Iterates over all the yet listed connections searching for one with the respective
	 * 	(virtual) network interface name. If found: disconnect this, else do nothing.
	 *
	 * @nic: The to be removed network interface.
	 *
	 */
	void disconnect(NetworkIntf nic);

	/**
	 * Iterates over all the yet listed connections searching for one with the respective
	 * 	peer's MAC address. If found: disconnect this, else do nothing.
	 *
	 * @peer: The peer a running connection to should be quit.
	 *
	 */
	void disconnect(Peer peer);

	/**
	 * TODO Verify that calling ::initialize()::open() on already opened _handle
	 *  connections is not leading to any error.
	 *  Maybe to define and implement a method *terminate()*, closing all the connection
	 *  of the respective wpasup and wpamon _handles.
	 */
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
				 ST_SREQ };


private:
	static const std::string TAG;

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

	list<Connection> connections;

	WifiP2PInterface &ext_if;
	Logger &logger;

	void initialize() throw (CoreEngineException);

	/**
	 * This method observes a fixed set of file descriptors per blocking ::select
	 * 	in order to catch event messages created at wpa_s and escalated to the
	 * 	respectively attached SupplicantHandle.
	 * 	The method will remain in ::select for a to be defined number of seconds.
	 *
	 * @&wpa:	 Reference to the SupplicantHandle whichs wpa_s control_i/f connection
	 * 			  file descriptor will be monitored by blocking ::select for incoming
	 * 			  event messages.
	 * @seconds: Time value for how long the method will remain in blocking ::select.
	 * @next:	 The next CoreEngine state to be entered after leaving ::select.
	 *
	 */
	void triggeredEvents(const SupplicantHandle &wpa, list<Peer> &peers, int seconds, state next);

};

} /* namespace wifip2p */

#endif /* COREENGINE_H_ */
