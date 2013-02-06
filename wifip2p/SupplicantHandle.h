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


namespace wifip2p {

	class SupplicantHandleException : public std::exception {
	public:
		SupplicantHandleException(const std::string &what) : _what(what) {
		}

		~SupplicantHandleException() throw () {
		}

		std::string what() {
			return _what;
		}

	private:
		const std::string _what;
	};



	class SupplicantHandle {

		public:
			SupplicantHandle(const char *ctrl_path, bool monitor)
					throw (SupplicantHandleException);
			virtual ~SupplicantHandle();

			void funcTest() throw (SupplicantHandleException);

			void* getHandle();
			int getFDListen();

			char* recvReply(char *replybuf, size_t reply_len);
			char* recvReply();




		private:
			bool monitor_mode;
			void *_handle;
			int fd_listen;


			bool setMonitorMode();

			void p2p_find() throw (SupplicantHandleException);

};

} /* namespace wifip2p */
#endif /* SUPPLICANTHANDLE_H_ */
