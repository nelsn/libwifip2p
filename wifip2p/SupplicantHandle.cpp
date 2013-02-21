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

namespace wifip2p {

	SupplicantHandle::SupplicantHandle(const char *ctrl_path, bool monitor)
			throw (SupplicantHandleException) {

		monitor_mode = monitor;

		/*
		 * Here, _handle is defined to represent the now opened
		 *  control_i/f expected from the ctrl_path parameter.
		 *  E.g. when instantiated with the parameter /var/run/
		 *  wpa_supplicant the WPASUP ctrl_i/f is now referenced
		 *  by _handle.
		 */
		_handle = wpa_ctrl_open(ctrl_path);

		if (_handle != NULL) {
			if (monitor_mode) {
				if (this->setMonitorMode())
					std::cout << "WPASUP is now being monitored." << std::endl;
				else {
					monitor_mode = false;
					throw SupplicantHandleException("Failed to set monitor mode.");
				}
			} else
				std::cout << "Connection to WPASUP established." << std::endl;
		} else {
			throw SupplicantHandleException("Connection to WPASUP failed.");
		}

	}

	SupplicantHandle::~SupplicantHandle() {

		/*
		 * _handle is going to be dereferenced, i.e., the function
		 *  wpa_ctrl_close gets handed over the actual control_i/f
		 *  referenced by _handle.
		 */
		if (monitor_mode)
			if (wpa_ctrl_detach((struct wpa_ctrl*)_handle) < 0)
				std::cerr << "Failed to detach monitor properly. Close will be forced." << std::endl;

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
		std::cout << msg << std::endl;
	}
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

		// convert reply to a std::string
		std::string reply(replybuf, reply_len);

		if (reply.substr(0,4) != "PONG")
			throw SupplicantHandleException("WPASUPP did not reply to PING");

		this->p2p_find();
	}

	/*
	 * p2p_find(Timer core_engines_timer_object)
	 * 	one could also think of. By that, the Timer object is instantiated and maintained
	 * 	by the respective CoreEngine while at the same time may be used with all its
	 * 	timing functionality within SupplicantHandle methods which are in need of some
	 * 	function of time, determining whether to stop and internally call another method,
	 * 	e.g. stop p2p_find as determined by the timer and in return within the _find method
	 * 	starting p2p_get_peers.
	 * 	By that, one is able to orchestrate several methods and hide their inherent complexities
	 * 	from the CoreEngine's statemachine/running-loop.
	 */
	void SupplicantHandle::p2p_find() throw (SupplicantHandleException) {

		char replybuf[64];
		size_t reply_len = sizeof(replybuf) - 1;

		int ret = wpa_ctrl_request(
					(struct wpa_ctrl*)_handle, "P2P_FIND", 8, *&replybuf, &reply_len, NULL);

		if (ret == -1)
			throw SupplicantHandleException("wpa_s ctrl_i/f; send or receive failed.");
		if (ret == -2)
			throw SupplicantHandleException("wpa_s ctrl_i/f; communication timed out.");

		std::string reply(replybuf, reply_len);

		if (reply.substr(0, reply_len -1) == "FAIL")
			throw SupplicantHandleException("WPASUP was not able to initiate P2P_FIND.");
		else {
//			std::cout << "läuft durch?" << std::endl;
//			void *_handle_in;
//			_handle_in = &_handle;
//			_handle_in = wpa_ctrl_attach((struct wpa_ctrl*)_handle_in);
		}


		/* TODO wait for period of time for having the supplicant in p2p_find mode
		 *  Then needs to call p2p_peers whether there are any found peers ore not;
		 *  this method also needs to parse the peers-list returned in the reply_buf
		 *  into a linked list or somewhat, accessible in the sense that one CoreEngine
		 *  may initiate serv_disc and connect actions per peer...
		 */

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

			std::cout << "fd_listen state: " << fd_listen << std::endl;

			if (fd_listen < 0) {
				throw SupplicantHandleException("FileDescr. problem occured.");
				return false;
			}
			return true;
		} else
			return false;

	}

} /* namespace wifip2p */
