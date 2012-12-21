#include "wifip2p/SupplicantHandle.h"

int main() {
	wifip2p::SupplicantHandle handle("/var/run/wpa_supplicant");

	handle.ctrlAttach("/var/run/wpa_supplicant");
/*
 * TODO qredqe
 */
	std::cout << "blabla" << std::endl;

	return 0;
}
