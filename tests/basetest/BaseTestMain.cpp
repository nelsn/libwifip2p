#include "wifip2p/SupplicantHandle.h"

int main() {
	wifip2p::SupplicantHandle handle("/var/run/wpa_supplicant");
	return 0;
}
