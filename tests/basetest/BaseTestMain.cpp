#include "wifip2p/SupplicantHandle.h"


using namespace std;

int main() {

	/* >> wifip2p::SupplicantHandle handle("/var/run/wpa_supplicant");
	 *
	 * Was genau geschieht hier? Der Scope-Operator macht Inhalte des
	 *  Namensraumes 'wifip2p' zugreifbar. Hier das Objekt SupplicantHandle.
	 *  Das Objekt wird dann aber _nicht_ über den Konstruktor instanziiert,
	 *  sondern einfach benannt und nach einem Leerzeichen die Objekt-
	 *  variable _handle -- ohne vorangestelltem Unterstrich -- wie eine
	 *  Methode, mit Parameter-Übergabe in Klammern, "aufgerufen".
	 *
	 *  Was ist das für ein Konstrukt?
	 *
	 * Sinnigerweise nimmt man das offenbar syntaktische Equivalent, das
	 *  gemäß den üblichen Gepflogenheiten erscheint. >>
	 */

	try {
		wifip2p::SupplicantHandle wpasup("/var/run/wpa_supplicant/wlan0");

		/*
		 * To test implemented functions enter the code here. The actual
		 *  testing should take place by running Johannes' make -check script,
		 *  as he has described in his e-mail regarding libwifip2p.
		 */
		wpasup.funcTest();

	} catch (wifip2p::SupplicantHandleException &ex) {
		std::cerr << "Error: " << ex.what() << std::endl;
		return -1;
	}

	return 0;
}
