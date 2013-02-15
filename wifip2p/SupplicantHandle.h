/*
 * SupplicantHandle.h
 *
 *  Created on: 12.12.2012
 *      Author: morgenro
 *      		niels_w
 */

#ifndef SUPPLICANTHANDLE_H_
#define SUPPLICANTHANDLE_H_

#include <iostream>
#include <sys/time.h> //FD_SET and functions...

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
	SupplicantHandle();
	SupplicantHandle(const char *ctrl_path, bool monitor)
		throw (SupplicantHandleException);
	virtual ~SupplicantHandle();

	void funcTest() throw (SupplicantHandleException);

	void* getHandle();
	int getFDListen();

	char* recvReply(char *replybuf, size_t reply_len);
	char* recvReply();


	void setDeviceName(string name);



private:
	bool monitor_mode;
	void *_handle;
	string name;
	int fd_listen;


	bool setMonitorMode() throw (SupplicantHandleException) ;

	void p2p_find() throw (SupplicantHandleException);

};

} /* namespace wifip2p */
#endif /* SUPPLICANTHANDLE_H_ */
