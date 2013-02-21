/*
 * SupplicantHandle.h
 *
 *  Created on: 12.12.2012
 *      Author: morgenro
 *      		niels_w
 */

#ifndef SUPPLICANTHANDLE_H_
#define SUPPLICANTHANDLE_H_

#include <list>
#include <string>

#include <iostream>
#include <sys/time.h> //FD_SET and functions...

#include "wifip2p/Peer.h"
#include "wifip2p/Connection.h"
#include "wifip2p/WifiP2PInterface.h"

using namespace std;

namespace wifip2p {

class SupplicantHandleException : public exception {
public:
	SupplicantHandleException(const string &what) : _what(what) {
	}

	~SupplicantHandleException() throw () {
	}

	string what() {
		return _what;
	}

private:
	const string _what;
};



class SupplicantHandle {

public:
	SupplicantHandle(bool monitor);
	virtual ~SupplicantHandle();

	void open(const char *ctrl_path) throw (SupplicantHandleException);
	void init(string name, list<string> services) throw (SupplicantHandleException);

	//----------

	//void* getHandle();
	/* Probably not needed, hence CoreEngine.listen() calls wpamon.listen() which then
	 *  does everything needed SupplicantHandle internally.
	 */
	//int getFDListen();

	char* recvReply(char *replybuf, size_t reply_len);
	char* recvReply();
	//----------/

	void findPeers() throw (SupplicantHandleException);
	void findPeersStop() throw (SupplicantHandleException);
	void listen() throw (SupplicantHandleException);
	void listen(WifiP2PInterface* ext_if) throw (SupplicantHandleException);

	void requestService(string service) throw (SupplicantHandleException);
	//void requestService(Peer peer, string service) throw (SupplicantHandleException);
	//void requestServiceCancel(Peer peer, string service) throw (SupplicantHandleException);
	void connectToPeer(Peer peer) throw (SupplicantHandleException);
	void disconnect(Connection conn) throw (SupplicantHandleException);

private:
	bool monitor_mode;
	void *_handle;
	int fd_listen;

	bool setMonitorMode() throw (SupplicantHandleException);

	bool setDeviceName(string name) throw (SupplicantHandleException);
	bool flushServices() throw (SupplicantHandleException);
	bool addService(string name, string service) throw (SupplicantHandleException);

	bool p2pCommand(string cmd) throw (SupplicantHandleException);

	string msgDecompose(char* buf);


};

} /* namespace wifip2p */
#endif /* SUPPLICANTHANDLE_H_ */
