#include "wifip2p/CoreEngine.h"
#include "wifip2p/SupplicantHandle.h"
#include "wifip2p/TestSupplicantHandle.h"
#include "wifip2p/Peer.h"

#include <list>

using namespace std;

int main() {

//	/* Constructed at STACK >>
//	 *
//	 *	wifip2p::SupplicantHandle wpasup("/var/run/wpa_supplicant/wlan0");
//	 *
//	 */
//
//	/* Constructed at HEAP >>
//	 *
//	 *	wifip2p::SupplicantHandle *wpasup;
//	 *	wpasup = new wifip2p::SupplicantHandle("/var/run/wpa_supplicant");
//	 *
//	 * Requires deletion >>
//	 *
//	 *	delete wpasup[];
//	 *
//	 */

	try {

		//wifip2p::CoreEngine ce("/var/run/wpa_supplicant/wlan1", "dell_studio");

		list<string> s;

		s.push_front("SERV1");

		//ce.reinitialize("/var/run/wpa_supplicant/wlan1", s);

		wifip2p::Peer peer("Ab:Cf:09:23:45:7g", "STA_NAME");

		wifip2p::TestSupplicantHandle supp_cmd(false);
		wifip2p::TestSupplicantHandle supp_mon(true);

		//true for SupplicantHandle testing
		if (true) {
			supp_cmd.functionsTest("/var/run/wpa_supplicant/wlan1");
			supp_mon.functionsTest("/var/run/wpa_supplicant/wlan1");
		}

	} catch (wifip2p::SupplicantHandleException &ex) {
		std::cerr << "Error: " << ex.what() << std::endl;
		return -1;
	} catch (wifip2p::PeerException &ex) {
		std::cerr << "Error: " << ex.what() << std::endl;
		return -1;
	}

	return 0;
}
