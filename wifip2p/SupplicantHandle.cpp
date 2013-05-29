/*
 * SupplicantHandle.cpp
 *
 *  Created on: 12.12.2012
 *      Author: morgenro
 *      		niels_w
 */

#include "wifip2p/SupplicantHandle.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <common/wpa_ctrl.h>

using namespace std;

namespace wifip2p {

	const std::string SupplicantHandle::TAG = "SupplicantHandle";

	const string SERVDISC_TYPE = "upnp";
	const string SERVDISC_VERS = "10";
	const string BROADCAST = "00:00:00:00:00:00";

	SupplicantHandle::SupplicantHandle(bool monitor, Logger &logger)
		: _handle(NULL),
		  monitor_mode(monitor),
		  _logger(logger) {

	}

	SupplicantHandle::~SupplicantHandle() {

		if (_handle != NULL)
			wpa_ctrl_close((struct wpa_ctrl*)_handle);

	}


	void SupplicantHandle::open(const char *ctrl_path) throw (SupplicantHandleException) {

		_handle = wpa_ctrl_open(ctrl_path);

		if (_handle != NULL) {
			if (this->monitor_mode) {
				if (this->setMonitorMode())
					_logger.log_debug(40, TAG, "WPASUP is now being monitored.");
				else {
					monitor_mode = false;
					throw SupplicantHandleException("Failed to set monitor mode.");
				}
			} else
				_logger.log_debug(40, TAG, "Connection to WPASUP established.");
		} else {
			throw SupplicantHandleException("Connection to WPASUP failed.");
		}
	}


	void SupplicantHandle::init(string name, list<string> services) throw (SupplicantHandleException) {

		try {
			this->setDeviceName(name);
			this->flushServices();

			list<string>::iterator it = services.begin();
			if (!services.empty()) {
				for (; it != services.end(); ++it) {
					this->addService(name, *it);
				}
			}
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
	}


	bool SupplicantHandle::setDeviceName(string name) throw (SupplicantHandleException) {
		try {
			this->p2pCommand("SET device_name " + name, NULL);
			return true;
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
		return false;
	}

	bool SupplicantHandle::flushServices() throw (SupplicantHandleException) {
		try {
			this->p2pCommand("P2P_SERVICE_FLUSH", NULL);
			return true;
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
		return false;
	}


	bool SupplicantHandle::addService(string name, string service) throw (SupplicantHandleException) {
		try {
			this->p2pCommand("P2P_SERVICE_ADD "
								+ SERVDISC_TYPE + " "
								+ SERVDISC_VERS + " "
								+ service + "$" + name,	NULL);
			return true;
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
		return false;
	}


	void SupplicantHandle::findPeers() throw (SupplicantHandleException) {
		try {
			this->p2pCommand("P2P_FIND", NULL);
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
	}


	void SupplicantHandle::findPeers(int seconds) throw (SupplicantHandleException) {
		try {
			this->p2pCommand("P2P_FIND " + seconds, NULL);
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
	}


	void SupplicantHandle::findPeersStop() throw (SupplicantHandleException) {
		try {
			this->p2pCommand("P2P_STOP_FIND", NULL);
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
	}


	void SupplicantHandle::listen(list<Peer> &peers, list<Connection> &connections,
			list<string> &services, set<string> &sdreq_id,
			WifiP2PInterface &ext_if) throw (SupplicantHandleException) {

		if (this->monitor_mode) {

			int x = wpa_ctrl_pending((struct wpa_ctrl*)_handle);

			char buf[256];
			size_t len = 256;

			if (wpa_ctrl_pending((struct wpa_ctrl*)_handle) > 0) {

				wpa_ctrl_recv((struct wpa_ctrl*)_handle, &buf[0], &len);

				string buffer(buf+3, len-3);
				vector<string> msg = msgDecompose(buffer, " ");

				/* EVENT >> [DEVICE_FOUND]
				 *
				 */
				if (msg.at(0) == P2P_EVENT_DEVICE_FOUND) {

					_logger.log_debug(40, TAG, "EVENT >> [DEVICE_FOUND]");

					/* With msg.at(1) one may get the MAC immediately, but this
					 *  place especially not stores the hardware interface's MAC,
					 *  but the one of a may or may not have been created virtual
					 *  interface.
					 * So here we cope with msg.at(2).substr(13), resulting in
					 *  msg.at(2): 			  "p2p_dev_addr=<MAC-ADDR>"
					 *  msg.at(2).substr(13): "<MAC-ADDR>"
					 */

					string mac(msg.at(2).substr(13));
					Peer p(mac);

					list<Peer>::const_iterator peer_it = find(peers.begin(), peers.end(), p);

					if (peer_it == peers.end()) {
						peers.push_back(p);
						_logger.log_debug(50, TAG, "Peer pushed back at peers, as yet not contained");
					} else {
						//Check: p in list? Then peer_it ~ p in list
					    if (peer_it->getName() != "") {
					        /* If peer_it has name other than ""
					         * 	=> peer_it is already fully_discovered
					         * 	=> call back ext_if with *peer_it
					         * 		dereferenced
					         */
					    	_logger.log_debug(50, TAG, "call back ext_if with fully discovered peer");
					        ext_if.peerFound(*peer_it);
					    }
					}

					_logger.log_debug(40, TAG, "[DEVICE_FOUND] << EVENT");
				}

				/* EVENT >> [GROUP_STARTED]
				 *
				 * That is, a connection is established.
				 * For a connection being established, the peer to be
				 *  connected is required to be fully discovered -
				 *  as only those peers are handed out to ext_if for
				 *  eventually being called for a connection.
				 *
				 */
				if (msg.at(0) == P2P_EVENT_GROUP_STARTED) {

					_logger.log_debug(40, TAG, "EVENT >> [GROUP_STARTED]");

					//local device is supposed being group owner >>
					if (msg.at(2) == "GO") {
						NetworkIntf go_nic(msg.at(1));
						Connection new_conn(go_nic);
						//requires now listening for AP_STA_CONNECTED event
						connections.push_front(new_conn);
					} else {
						//local device is supposed being group client >>
						if (msg.at(2) == "client") {

							Peer temp_p(msg.at(6).substr(12));
							NetworkIntf nic(msg.at(1));

							//find peer within fully discovered peers >>
							list<Peer>::const_iterator peer_it =
									find(peers.begin(), peers.end(), temp_p);

							//create connection and push_back to list >>
							if (peer_it != peers.end()) {
								Connection conn(*peer_it, nic);
								connections.push_back(conn);
								ext_if.connectionEstablished(conn);
							}
						}
					}

					_logger.log_debug(45, TAG, "[GROUP_STARTED] << EVENT");
				}

				/* EVENT >> [AP_STA_CONNECTED]
				 *
				 * As the process is some kind of probabilistic, whether
				 * 	frames cause wpa_s to generate whatever EVENT next, it
				 * 	is assumed here, that the up front Connection within
				 * 	connections is one with no Peer assigned yet.
				 * 	The respective station receiving this EVENT is
				 * 	assumed to be the GO of this Connection. The Peer
				 * 	who lead to this EVENT being generated is likely to
				 * 	be the one related to this up front connection.
				 * Because only fully discovered Peers may be selected for
				 * 	being connected, the specific one must be contained
				 * 	within list<Peer> peers.
				 *
				 */
				if (msg.at(0) == AP_STA_CONNECTED) {

					_logger.log_debug(40, TAG, "EVENT >> [AP_STA_CONNECTED]");

					Peer connected_peer(msg.at(2).substr(13));
					_logger.log_debug(50, TAG, "The connection-peer should be " + connected_peer.getMacAddr());
					list<Peer>::const_iterator peer_it =
							find(peers.begin(), peers.end(), connected_peer);
					_logger.log_debug(60, TAG, "The connection-peer is " + peer_it->getMacAddr() + "; " + peer_it->getName());

					if (peer_it != peers.end()) {
						connections.front().setPeer(*peer_it);
						ext_if.connectionEstablished(connections.front());
					}

					_logger.log_debug(45, TAG, "[AP_STA_CONNECTED] << EVENT");
				}


				/* EVENT >> [AP_STA_DISCONNECTED]
				 *
				 * A client-peer connected to a P2P GO triggers this event
				 *  at the GO wpa_s by removing its respective virtual
				 *  interface, thus disconnecting.
				 *  Such an event needs to be intercepted and in turn used
				 *  to search for the respective connection from within the
				 *  connections list and then remove the respective's
				 *  group virtual interface as well as reporting the connection
				 *  to being quit to the ext_if.
				 *
				 */
				if (msg.at(0) == AP_STA_DISCONNECTED) {

					_logger.log_debug(40, TAG, "EVENT >> [AP_STA_DISCONNECTED]");

					string mac_lost = msg.at(2).substr(13);

					list<Connection>::iterator it = connections.begin();

					for (; it != connections.end(); ++it) {
						if (it->getPeer().getMacAddr() == mac_lost) {
							disconnect(*it);
							Connection conn = *it;
							it = connections.erase(it);
							ext_if.connectionLost(conn);
						}
					}

					_logger.log_debug(45, TAG, "[AP_STA_DISCONNECTED] << EVENT");
				}

				/* EVENT >> [GROUP_REMOVED]
				 *
				 * This event is triggered when a virtual (group) interface
				 * 	is removed due to several reasons. E.g. if the connection
				 * 	on that i/f is timed out, requested by GO and so on.
				 *
				 */
				if (msg.at(0) == P2P_EVENT_GROUP_REMOVED) {

					_logger.log_debug(40, TAG, "EVENT >> [GROUP_REMOVED]");

					list<Connection>::iterator it = connections.begin();

					for (; it != connections.end(); ++it) {
						if (it->getNetworkIntf().getName() == msg.at(1)) {
							disconnect(*it);
							Connection conn = *it;
							it = connections.erase(it);
							ext_if.connectionLost(conn);
						}

					}


					_logger.log_debug(45, TAG, "[GROUP_REMOVED] << EVENT");
				}

				/* EVENT >> [GROUP_NEG_REQUEST]
				 *
				 * That is a connection request.
				 * Checks whether requesting peer is fully discovered
				 *  or not. If so, the specific fully discovered peer will
				 *  be handed out. Else, the non-fully discovered peer
				 *  temp_p will be handed out to ext_if.
				 *
				 */
				if (msg.at(0) == P2P_EVENT_GO_NEG_REQUEST) {

					_logger.log_debug(40, TAG, "EVENT >> [GROUP_NEG_REQUEST]");

					Peer temp_p(msg.at(1));
					list<Peer>::const_iterator peer_it = find(peers.begin(), peers.end(), temp_p);

					//peer is contained within list >>
					if (peer_it != peers.end()) {
						// peer is fully discovered >>
						if (peer_it->getName() != "") {

							list<Connection>::iterator conn_it = connections.begin();
							bool contained = false;

							for (; conn_it != connections.end(); ++conn_it) {
								if (conn_it->getPeer() == *peer_it) {
									contained = true;
									break;
								} else {
									continue;
								}
							}

							//peer is not already connected
							if (!contained)
								ext_if.connectionRequest(*peer_it);

						// hand over to ext_if for being checked externally >>
						} else {
							ext_if.connectionRequest(temp_p);
						}
					// hand over to ext_if for being checked externally >>
					} else {
						ext_if.connectionRequest(temp_p);
					}

					_logger.log_debug(45, TAG, "[GROUP_NEG_REQUEST] << EVENT");
				}

				/* EVENT >> [SERVICE_REQUEST_RECEIVED]
				 *
				 */
				if (msg.at(0) == P2P_EVENT_SERV_DISC_REQ) {

					_logger.log_debug(40, TAG, "EVENT >> [RECEIVED_SERVICE_REQUEST]");

					Peer p(msg.at(2));

					const list<Peer>::const_iterator peer_it = find(peers.begin(), peers.end(), p);

					// peer is already contained in peers >>
					if (peer_it != peers.end()) {
						_logger.log_debug(45, TAG, "peer is already contained in peers >>");
						// peer is fully discovered >>
						if (peer_it->getName() != "") {
							_logger.log_debug(45, TAG, "peer is fully discovered >>");
							// prüfe: ist der angefragte service teil meiner eigenen gelisteten services?

							// service request likely to contain a proper request
							if (msg.at(5).size() > 10) {

								string frag_tlv(msg.at(5).substr(10));
								_logger.log_debug(45, TAG, "TLV data fragment: " + frag_tlv);

								string req(getStringFromHexTLV(frag_tlv));
								_logger.log_debug(45, TAG, "Requested Service Data from TLV: " + req);

								list<string>::iterator serv_it = services.begin();
								for (; serv_it != services.end(); ++serv_it) {
									// one locally registered/own service requested >>
									if (matchingService(*serv_it, req)) {
 										ext_if.peerFound(*peer_it);
									}
								}
							}
						// peer is not yet fully discovered >>
						} else {
							list<string>::iterator it = services.begin();
							for (; it != services.end(); ++it)
								requestService(*peer_it, *it, sdreq_id);
						}
					} else {
						peers.push_back(p);
					}

					_logger.log_debug(45, TAG, "[RECEIVED_SERVICE_REQUEST] << EVENT");
				}

				/* EVENT >> [SERVICE_RESPONSE_RECEIVED]
				 *
				 * The return of not only the upnp sd_resp header with empty
				 * 	data implies the specific peer to host at least one
				 * 	requested service. An Iterator checks, if that peer is
				 * 	already contained in the peers list. If not, that peer
				 * 	is pushed to the list; else, the list remains unchanged
				 * 	and ext_if.peerFound(Peer) gets called.
				 * If only the upnp sd_resp header with empty data is returned,
				 *  which implies the service not being hosted at that peer,
				 *  the specific peer will be deleted from the peers list.
				 *
				 */
				if (msg.at(0) == P2P_EVENT_SERV_DISC_RESP) {

					_logger.log_debug(40, TAG, "EVENT >> [RECEIVED_SERVICE_RESPONSE]");

					// non-empty service response received >>
					if (msg.at(3) != "0300020101" && msg.at(3) != "0300020102") {
						Peer p(msg.at(1));
						list<Peer>::iterator peer_it = find(peers.begin(), peers.end(), p);

						/*
						 * Peer found within peers >>
						 *
						 */
						if (peer_it != peers.end()) {
							// peer fully discovered already contained in list >>
							if (peer_it->getName() != "") {
								ext_if.peerFound(*peer_it);
							// peer is in list, but _not_ fully discovered >>
							} else {
								Peer &peer = (*peer_it);
								// TLV data assumed to contain proper information
								if (msg.at(3).length() > 10) {
									/*
									 * Convention: Services are registered at
									 * 	peers wpa_s' like
									 *
									 * 	<ServiceName_string>$<DeviceName_string>
									 *
									 * 	so a typical string representation of
									 * 	a TLV would look like
									 * 	"SomeService$SomeArbitraryDeviceName"
									 * Otherwise, a message will be written to
									 * 	std::cerr.
									 *
									 */

									string tlv = getStringFromHexTLV(msg.at(3).substr(12));
									string name;
									unsigned int pos = tlv.find_first_of('$');

									if (pos < tlv.length()) {
										name = tlv.substr(pos+1);
									} else {
										_logger.log_err(TAG, ">> RECEIVED_SERVICE_RESPONSE: No conventionally-confirm DeviceName!");
									}

									if (name != "") {
										peer.setName(name);
										//peers.push_back(p);
										ext_if.peerFound(peer);
									} else {
										_logger.log_err(TAG, ">> RECEIVED_SERVICE_RESPONSE: No conventionally-confirm DeviceName!");
									}
								}
							}
						/*
						 * Peer not found within peers. Will now try to get
						 * 	a proper name from the TLV and push the new peer to
						 * 	peers.
						 *
						 */
						} else {
							// TLV data assumed to contain proper information
							if (msg.at(3).length() > 10) {
								string name(msgDecompose(getStringFromHexTLV(msg.at(3).substr(12)), "$")[1]);
								if (name != "") {
									p.setName(name);
									peers.push_back(p);
									ext_if.peerFound(p);
								} else {
									_logger.log_err(TAG, ">> RECEIVED_SERVICE_RESPONSE: No conventionally-confirm DeviceName!");
								}
							}
						}
					}

					_logger.log_debug(45, TAG, "[RECEIVED_SERVICE_RESPONSE] << EVENT");
				}
			}
		} else {
			throw SupplicantHandleException("SupplicantHandle needs to be in monitor mode for ::listen().");
		}
	}


	void SupplicantHandle::requestService(string service, set<string> &sdreq_id)
			throw (SupplicantHandleException) {
		try {
			string returned_id;
			this->p2pCommand("P2P_SERV_DISC_REQ "
					+ BROADCAST + " "
					+ SERVDISC_TYPE + " "
					+ SERVDISC_VERS + " "
					+ service, &returned_id);
			sdreq_id.insert(returned_id);
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
	}


	void SupplicantHandle::requestService(Peer peer, string service, set<string> &sdreq_id)
			throw (SupplicantHandleException) {
		try {
			string returned_id;
			this->p2pCommand("P2P_SERV_DISC_REQ "
					+ peer.getMacAddr() + " "
					+ SERVDISC_TYPE + " "
					+ SERVDISC_VERS + " "
					+ service, &returned_id);
			sdreq_id.insert(returned_id);
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
	}


	void SupplicantHandle::requestServiceCancel(string sdreq_id) throw (SupplicantHandleException) {
		try {
			_logger.log_debug(45, TAG, "ServiceRquest_ID : " + sdreq_id);
			this->p2pCommand("P2P_SERV_DISC_CANCEL_REQ " + sdreq_id, NULL);
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
	}


	void SupplicantHandle::connectToPeer(Peer peer) throw (SupplicantHandleException) {
		try {
			this->p2pCommand("P2P_CONNECT " + peer.getMacAddr() + " pbc", NULL);
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
	}


	void SupplicantHandle::disconnect(Connection conn) throw (SupplicantHandleException) {
		try {
			this->p2pCommand("P2P_GROUP_REMOVE " + conn.getNetworkIntf().getName(), NULL);
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
	}


	bool SupplicantHandle::p2pCommand(string cmd, string *direct_feedback) throw (SupplicantHandleException) {

		vector<char> reply_buf(64);
		size_t buf_len = reply_buf.size();

		int ret = wpa_ctrl_request((struct wpa_ctrl*) _handle,
					cmd.c_str(), cmd.length(),
					&reply_buf[0], &buf_len, NULL);

		if (ret == -1)
			throw SupplicantHandleException("wpa_s ctrl_i/f; send or receive failed.");
		if (ret == -2)
			throw SupplicantHandleException("wpa_s ctrl_i/f; communication timed out.");

		string reply(&reply_buf[0], buf_len);

		if (reply == "FAIL") {
			throw SupplicantHandleException("wpa_s was not able to initiate <" + cmd + "> successfully.");
		} else {
			if (direct_feedback != NULL)
				*direct_feedback = reply;
			return true;
		}

		return false;

	}


	vector<string> SupplicantHandle::msgDecompose(string buffer, string tok) {

		//string buffer(buf+3);
		vector<string> ret = tokenize(tok, buffer, -1);
		ret[0] += " ";
		return ret;
	}


	vector<string> SupplicantHandle::tokenize(string token, string data, size_t max) const
	{
		vector<string> l;
		string value;

		// Skip delimiters at beginning.
		string::size_type pos = data.find_first_not_of(token, 0);

		while (pos != string::npos)
		{
			// Find first "non-delimiter".
			string::size_type tokenPos = data.find_first_of(token, pos);

			// Found a token, add it to the vector.
			if(tokenPos == string::npos){
				value = data.substr(pos);
				l.push_back(value);
				break;
			} else {
				value = data.substr(pos, tokenPos - pos);
				l.push_back(value);
			}
			// Skip delimiters.  Note the "not_of"
			pos = data.find_first_not_of(token, tokenPos);
			// Find next "non-delimiter"
			tokenPos = data.find_first_of(token, pos);

			// if maximum reached
			if (l.size() >= max && pos != string::npos)
			{
				// add the remaining part to the vector as last element
				l.push_back(data.substr(pos, data.length() - pos));

				// and break the search loop
				break;
			}
		}

		return l;
	}


	string SupplicantHandle::getStringFromHexTLV(string tlv) {

		string result;

		bool x = true;
		short secnd, first;

		for (unsigned int i=0; i<tlv.length(); i++) {

			if (x) {
				secnd = hexlookup(tlv[i]) * 16;
				x = !x;
			} else {
				first = hexlookup(tlv[i]);
				x = !x;

				int want = secnd + first;
				result.push_back((char) want);
			}
		}

		return result;
	}


	bool SupplicantHandle::setMonitorMode() throw (SupplicantHandleException) {
		if (wpa_ctrl_attach((struct wpa_ctrl*) (_handle)) == 0)
			return true;
		else
			return false;
	}


	int SupplicantHandle::getFD() const throw (SupplicantHandleException) {
		if (monitor_mode)
			return wpa_ctrl_get_fd((struct wpa_ctrl*) (_handle));
		else
			throw SupplicantHandleException("Unable to get file descriptor. wpa_s not in monitor mode.");
	}


	bool SupplicantHandle::matchingService(string own, string foreign) {

		if (own.size() < foreign.size() || foreign.size() == 0) {
			return false;
		} else {
			for (unsigned int i=0; i<own.size(); i++) {
				if (own.size()-i >= foreign.size()) {
					int k = i;
					for (unsigned int j=0; j <foreign.size(); j++) {
						if (own[k] == foreign[j]) {
							k++;
							if (j == foreign.size()-1) {
								return true;
							}
						} else {
							break;
						}
					}
				} else {
					return false;
				}
			}
		}

		return false;
	}


	short SupplicantHandle::hexlookup(char c) {
		short ret;
		switch (c) {
			case 'a':
				ret = 10;
				break;
			case 'A':
				ret = 10;
				break;
			case 'b':
				ret = 11;
				break;
			case 'B':
				ret = 11;
				break;
			case 'c':
				ret = 12;
				break;
			case 'C':
				ret = 12;
				break;
			case 'd':
				ret = 13;
				break;
			case 'D':
				ret = 13;
				break;
			case 'e':
				ret = 14;
				break;
			case 'E':
				ret = 14;
				break;
			case 'f':
				ret = 15;
				break;
			case 'F':
				ret = 15;
				break;
			default:
				ret = c - '0';
				break;
		}
		return ret;
	}

} /* namespace wifip2p */
