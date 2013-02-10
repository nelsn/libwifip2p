#include "wifip2p/SupplicantHandle.h"
#include "wifip2p/Peer.h"


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

		std::string s("blabl");
		//wifip2p::Peer peer("aa");
		//std::cout << peer_a.getMacAddr() << std::endl;
		//wifip2p::Peer peer_b("ab;cd:01:23:45:78");
		//wifip2p::Peer peer_c("ab:cd:01:23:45:7");
//
		wifip2p::SupplicantHandle wpa_out("/var/run/wpa_supplicant/wlan1", 0);
//		wifip2p::SupplicantHandle wpa_mon("/var/run/wpa_supplicant/wlan1", 1);
//
//		/*
//		 * To test implemented functions enter the code here. The actual
//		 *  testing should take place by running Johannes' make check script,
//		 *  as he has described in his e-mail regarding libwifip2p.
//		 */
//
//		wpa_out.funcTest();
//
//		char *reply[128];
//		size_t reply_len = sizeof(reply) - 1;
//
//		bool loop = true;
//
//		int res;
//		struct timeval Timeout;
//
//		while (loop) {
////			Timeout.tv_usec = 0;
////			Timeout.tv_sec = 1;
////
////			select(NULL, &)
////
////			res
//
//			//wpa_mon.recvReply();
//
////			std::string rpl(wpa_mon.recvReply(*reply, reply_len), reply_len);
////			if (rpl.substr(7,22) == "P2P-DEVICE-FOUND") {
////				std::cout << wpa_mon.recvReply(*reply, reply_len) << std::endl;
////				loop = false;
////			}
//
//
//		}

	} catch (exception &ex) {
		std::cerr << "Error: " << ex.what() << std::endl;
		return -1;
	}

	return 0;
}
