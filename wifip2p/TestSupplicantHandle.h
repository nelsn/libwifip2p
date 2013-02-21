/*
 * TestSupplicantHandle.h
 *
 *  Created on: 19.02.2013
 *      Author: niels_w
 */

#ifndef TESTSUPPLICANTHANDLE_H_
#define TESTSUPPLICANTHANDLE_H_

#include "wifip2p/SupplicantHandle.h"

namespace wifip2p {

class TestSupplicantHandle: public SupplicantHandle {

public:
	TestSupplicantHandle(bool monitor);
	virtual ~TestSupplicantHandle();

	void functionsTest(const char *ctrl_path);

private:


};

} /* namespace wifip2p */
#endif /* TESTSUPPLICANTHANDLE_H_ */
