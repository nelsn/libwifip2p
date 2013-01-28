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


namespace wifip2p {

	class SupplicantHandle {

		public:
			SupplicantHandle(const char *ctrl_path);
			virtual ~SupplicantHandle();

			//niewue
			void funcTest();


		private:
			/*
			 * void* _handle represents a generic pointer,
			 *  that is, a pointer which may point at memory
			 *  of any variables' type as long as it is not
			 *  defined as either 'const' or 'volatile'.
			 *  In that sense, one may call it an unspecified
			 *  pointer.
			 */
			void *_handle;

			//niewue
			//int wpa_ctrl_attach(_handle);

	};

} /* namespace wifip2p */
#endif /* SUPPLICANTHANDLE_H_ */
