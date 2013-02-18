/*
 * SupplicantHandle.cpp
 *
 *  Created on: 12.12.2012
 *      Author: morgenro
 *      		niels_w
 */

#include "wifip2p/SupplicantHandle.h"

//#include <stdlib.h>
//#include <stdint.h>
#include <cstdlib>
#include <iostream>
#include <unistd.h>	//recvReply FD operations accordingly to http://www.kernel.org/doc/man-pages/online/pages/man2/read.2.html
#include "common/wpa_ctrl.h"

using namespace std;

namespace wifip2p {

	const string SERVDISC_TYPE = "UPNP";
	const string SERVDISC_VERS = "10";

	SupplicantHandle::SupplicantHandle(bool monitor)
		: _handle(NULL),
		  monitor_mode(monitor) {

	}

	SupplicantHandle::~SupplicantHandle() {

		/*
		 * _handle is going to be dereferenced, i.e., the function
		 *  wpa_ctrl_close gets handed over the actual control_i/f
		 *  referenced by _handle.
		 */
		//if (monitor_mode)
		//	if (wpa_ctrl_detach((struct wpa_ctrl*)_handle) < 0)
		//		cerr << "Failed to detach monitor properly. Close will be forced." << endl;

		if (_handle != NULL)
			wpa_ctrl_close((struct wpa_ctrl*)_handle);

	}


	/*
	 * Take a look at hostap/src/common/wpa_ctrl.{h|c} for what methods
	 *  are actually provided. The needed maybe taken and used, by
	 *  handing over the _handle variable.
	 * The _handle variable represents the connection to the Linux
	 *  internal application socket towards WPA_SUPPLICANT as opened
	 *  through wpa_control_open(ctrl_path). Here, ctrl_path is the
	 *  actual socket address in the Linux file system, e.g.
	 *  /var/run/wpa_supplicant, which holds all the responsibility
	 *  of de-/multiplexing those connections.
	 */

	/*
	static void hostapd_cli_msg_cb(char *msg, size_t len)
	{
		cout << msg << endl;
	}
	*/

	/**
	 * Only for testing purposes.
	 *
	 */
	void SupplicantHandle::funcTest() throw (SupplicantHandleException) {

		char replybuf[64];

		// initialize the reply_len with the available buffer size
		size_t reply_len = sizeof(replybuf) - 1;

		// Send out a PING request. A wpa supplicant should answer with a PONG.
		int ret = wpa_ctrl_request(
				(struct wpa_ctrl*)_handle, "PING", 4, *&replybuf, &reply_len, NULL);

		// check for call errors
		if (ret != 0)
			throw SupplicantHandleException("PING request failed");

		// convert reply to a string
		string reply(replybuf, reply_len);

		if (reply.substr(0,4) != "PONG")
			throw SupplicantHandleException("WPASUPP did not reply to PING");

	}


	/**
	 * @*ctrl_path: Here, _handle is defined, representing the now opened control_i/f
	 * 				 of wpa_s as expected behind the *ctrl_path parameter.
	 * 				 E.g. when wpa_s is bound to hardware interface wlan0, fully qualified
	 * 				 *ctrl_path needs to match "/var/run/wpa_supplicant/wlan0"
	 */
	void SupplicantHandle::open(const char *ctrl_path) throw (SupplicantHandleException) {

		_handle = wpa_ctrl_open(ctrl_path);

		if (_handle != NULL) {
			if (this->monitor_mode) {
				if (this->setMonitorMode())
					cout << "WPASUP is now being monitored." << endl;
				else {
					monitor_mode = false;
					throw SupplicantHandleException("Failed to set monitor mode.");
				}
			} else
				cout << "Connection to WPASUP established." << endl;
		} else {
			throw SupplicantHandleException("Connection to WPASUP failed.");
		}
	}

	/**
	 * @name: 	  The hardware interface's name, as to be set by wpa_s.
	 * @services: The services, as to be registered by wpa_s for being able to
	 * 			   respond, if any regarding requests come in.
	 */
	void SupplicantHandle::init(string name, list<string> services) throw (SupplicantHandleException) {

		try {
			this->setDeviceName(name);
			this->flushServices();

			list<string>::iterator it = services.begin();
			if (!services.empty()) {
				for (; it != services.end(); ++it) {
					this->addService(name, *it);
				}
			}
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
	}


	/**
	 * @name: 	The name as to be set for the device, controlled by this wpa_s.
	 * Returns: true, if device name is set; false, else. Throws exception on error.
	 *
	 */
	bool SupplicantHandle::setDeviceName(string name) throw (SupplicantHandleException) {
		try {
			this->p2pCommand("SET DEVICE_NAME " + name);
			return true;
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
		return false;
	}

	bool SupplicantHandle::flushServices() throw (SupplicantHandleException) {
		try {
			this->p2pCommand("P2P_SERVICE_FLUSH");
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
	}

	bool SupplicantHandle::addService(string name, string service) throw (SupplicantHandleException) {
		try {
			string cmd = "P2P_SERVICE_ADD "
							+ SERVDISC_TYPE + " "
							+ SERVDISC_VERS + " "
							+ "name:" + name + "::"
							+ "service:" + service;
			this->p2pCommand(cmd);
			return true;
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
		return false;
	}

	/**
	 * Initiates P2P_FIND command at wpa_s. Escalates exception on failure.
	 *
	 */
	void SupplicantHandle::findPeers() throw (SupplicantHandleException) {
		try {
			this->p2pCommand("P2P_FIND");
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
	}

	/**
	 * Stops P2P_FIND command at wpa_s.
	 *
	 */
	void SupplicantHandle::findPeersStop() throw (SupplicantHandleException) {
		try {
			this->p2pCommand("P2P_STOP_FIND");
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
	}

	void listen() throw (SupplicantHandleException);

	void requestService(Peer peer, string service) throw (SupplicantHandleException);

	void requestService(string service) throw (SupplicantHandleException);

	/**
	 * Creates a peering between this local and wpa_s controlled WNIC and Peer peer.
	 * The method uses WPS Push Button Configuration (PBC). wpa_s will create a
	 *   virtual WNIC per each connection which is going to be established. The virtual
	 *   interface will be called "p2p-<HW-IF-name>-<#count>".
	 * @Peer the peer to which wpa_s should initiate a connection.
	 *
	 */
	void SupplicantHandle::connectToPeer(Peer peer) throw (SupplicantHandleException) {
		try {
			this->p2pCommand("P2P_CONNECT " + peer.getMacAddr() + " PBC");
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
	}

	/**
	 * By now, this method only handles to remove wpa_s' created virtual WNICs, thus
	 *  disconnecting both of the group's peers.
	 * This is sufficient, as SupplicantHandle::connectToPeer(Peer) does not deal with
	 *  joining a P2P group, whether available, but just always creates exactly one
	 *  peering between each two devices in a separate group.
	 *
	 */
	void SupplicantHandle::disconnect(Connection conn) throw (SupplicantHandleException) {
		try {
			this->p2pCommand("P2P_GROUP_REMOVE " + conn.getNetworkIntf().getName());
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
	}



	/**
	 * TODO Look carefully whether the command requires all UPPERCASE writing
	 * 	or not.
	 *
	 * @cmd: 	Command string, to be transmitted to wpa_s. See wpa_s documentation
	 * 				for possible commands.
	 * Returns: true or false, whether the command may be initiated successfully
	 * 				or not.
	 */
	bool SupplicantHandle::p2pCommand(string cmd) throw (SupplicantHandleException) {

		const char *cmd_p;
		cmd_p = cmd.c_str();

		char 	reply_buf[64];
		size_t 	reply_len = sizeof(reply_buf) - 1;

		int ret = wpa_ctrl_request((struct wpa_ctrl*) _handle,
				cmd_p, cmd.length(),
				*&reply_buf, &reply_len, NULL);

		if (ret == -1) {
			throw SupplicantHandleException("wpa_s ctrl_i/f; send or receive failed.");
			return false;
		}
		if (ret == -2) {
			throw SupplicantHandleException("wpa_s ctrl_i/f; communication timed out.");
			return false;
		}

		string reply(reply_buf, reply_len);

		if (reply.substr(0, reply_len -1) == "FAIL") {
			throw SupplicantHandleException("wpa_s was not able to initiate " + cmd + " successfully.");
			return false;
		} else {
			return true;
		}

	}


	/*
	 * Utility functions >>
	 *
	 */


	void* SupplicantHandle::getHandle() {
		return _handle;
	}

	int SupplicantHandle::getFDListen() {
		return fd_listen;
	}

	char* SupplicantHandle::recvReply(char *replybuf, size_t reply_len) {

		//int res;
		//struct timeval Timeout;

		fd_set readfs;

		//set testing for source 1
		FD_SET(fd_listen, &readfs);
		//FD_SET(fd2, &readfs);

		//set testing for source 2
		//block until input becomes available

		//select(maxfd, &readfs, NULL, NULL, NULL);

		if (FD_ISSET(fd_listen, &readfs)) {	//input from source 1 available

			wpa_ctrl_recv((struct wpa_ctrl *)_handle, *&replybuf, &reply_len);

			return replybuf;

			//if (FD_ISSET(fd2))         // input from source 2 available
			//  handle_input_from_source2();
		}
	}

	char* SupplicantHandle::recvReply() {

		char *buf[128];
		size_t buf_len = sizeof(buf) - 1;
		ssize_t buf_read;

		buf_read = read(fd_listen, *buf, buf_len);

		if (buf_read >= 0) {
			return *buf;
		} else
			return "FD read() was not successful.";

	}


	bool SupplicantHandle::setMonitorMode() throw (SupplicantHandleException) {

		if (wpa_ctrl_attach((struct wpa_ctrl*) (_handle)) == 0) {
			/* TODO:
			 *
			 *  FileDescriptor socket registration using
			 *	wpa_ctrl_get_fd((struct wpa_ctrl*)_wpactrl_mon);
			 *
			 */
			fd_listen = wpa_ctrl_get_fd((struct wpa_ctrl*)_handle);

			cout << "fd_listen state: " << fd_listen << endl;

			if (fd_listen < 0) {
				throw SupplicantHandleException("FileDescr. problem occured.");
				return false;
			}
			return true;
		} else
			return false;

	}

} /* namespace wifip2p */
