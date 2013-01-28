/*
 * SupplicantHandle.cpp
 *
 *  Created on: 12.12.2012
 *      Author: morgenro
 */

#include "wifip2p/SupplicantHandle.h"

//#include <stdlib.h>
//#include <stdint.h>
#include "common/wpa_ctrl.h"

namespace wifip2p {

	SupplicantHandle::SupplicantHandle(const char *ctrl_path) {

		/*
		 * Here, _handle is defined to represent the now opened
		 *  control_i/f expected from the ctrl_path parameter.
		 *  E.g. when instantiated with the parameter /var/run/
		 *  wpa_supplicant the WPASUP ctrl_i/f is now referenced
		 *  by _handle.
		 */
		_handle = wpa_ctrl_open(ctrl_path);

	}

	SupplicantHandle::~SupplicantHandle() {

		/*
		 * _handle is going to be dereferenced, i.e., the function
		 *  wpa_ctrl_close gets handed over the actual control_i/f
		 *  refeenced by _handle.
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

	void SupplicantHandle::funcTest() {
		;
	}


} /* namespace wifip2p */
