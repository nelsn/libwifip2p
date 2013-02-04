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
#include "common/wpa_ctrl.h"

namespace wifip2p {

	SupplicantHandle::SupplicantHandle(const char *ctrl_path) throw (SupplicantHandleException) {

		/*
		 * Here, _handle is defined to represent the now opened
		 *  control_i/f expected from the ctrl_path parameter.
		 *  E.g. when instantiated with the parameter /var/run/
		 *  wpa_supplicant the WPASUP ctrl_i/f is now referenced
		 *  by _handle.
		 */
		_handle = wpa_ctrl_open(ctrl_path);

		if (_handle == NULL)
			throw SupplicantHandleException("connection to wpa supplicant failed");
	}

	SupplicantHandle::~SupplicantHandle() {

		/*
		 * _handle is going to be dereferenced, i.e., the function
		 *  wpa_ctrl_close gets handed over the actual control_i/f
		 *  referenced by _handle.
		 */
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
	 * One first example of the previously stated: Here the exact function
	 *  wpa_ctrl_attach(), as defined in hostap/src/common/wpa_ctrl.h, is
	 *  used by handing over the _handle, which represents the address
	 *  of the socket connection.
	 */
	/*int wpa_ctrl_attach(SupplicantHandle::_handle) {
		return 1;
	}*/


	static void hostapd_cli_msg_cb(char *msg, size_t len)
	{
		std::cout << msg << std::endl;
	}

	bool SupplicantHandle::findPeer() {

		char *resp = NULL;
		size_t resp_len = 0;

		try {
			resp = new char[64];

			wpa_ctrl_request((struct wpa_ctrl*)_handle, "p2p_find", 8, resp, &resp_len, NULL);

			wpa_ctrl_request((struct wpa_ctrl*)_handle, "p2p_find", 8, resp, &resp_len, hostapd_cli_msg_cb);
					//How to handle this last msg-callback thing?

			delete[] resp;

			return true;

		} catch (std::bad_alloc &e) {
			//TODO: Exception handling
		}

		return true;

	}

	void SupplicantHandle::funcTest() throw (SupplicantHandleException) {
		char replybuf[64];

		// initialize the reply_len with the available buffer size
		size_t reply_len = sizeof(replybuf) - 1;

		// Send out a PING request. A wpa supplicant should answer with a PONG.
		int ret = wpa_ctrl_request((struct wpa_ctrl*)_handle, "PING", 4, *&replybuf, &reply_len, NULL);

		// check for call errors
		if (ret != 0)
			throw SupplicantHandleException("PING request failed");

		// convert reply to a std::string
		std::string reply(replybuf, reply_len);

		if (reply.substr(0,4) != "PONG")
			throw SupplicantHandleException("WPASUPP did not reply to PING");
	}


} /* namespace wifip2p */
