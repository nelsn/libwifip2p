/*
 * SupplicantHandle.cpp
 *
 *  Created on: 12.12.2012
 *      Author: morgenro
 */

#include "wifip2p/SupplicantHandle.h"

#include <stdlib.h>
#include <stdint.h>
#include "common/wpa_ctrl.h"

namespace wifip2p {

	SupplicantHandle::SupplicantHandle(const char *ctrl_path) {
		_handle = wpa_ctrl_open(ctrl_path);
	}

	SupplicantHandle::~SupplicantHandle() {
		wpa_ctrl_close((struct wpa_ctrl*)_handle);
	}

} /* namespace wifip2p */
