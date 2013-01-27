/*
 * SupplicantHandle.h
 *
 *  Created on: 12.12.2012
 *      Author: morgenro
 */

#ifndef SUPPLICANTHANDLE_H_
#define SUPPLICANTHANDLE_H_

//niewue
#include <iostream>
//#include <string>


namespace wifip2p {

	class SupplicantHandle {

	private:
		void *_handle;

	public:
		SupplicantHandle(const char *ctrl_path);
		virtual ~SupplicantHandle();

		//niewue
		//int wpa_ctrl_attach(*_handle);

	};

} /* namespace wifip2p */
#endif /* SUPPLICANTHANDLE_H_ */
