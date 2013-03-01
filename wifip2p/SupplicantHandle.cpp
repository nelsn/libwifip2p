/*
 * SupplicantHandle.cpp
 *
 *  Created on: 12.12.2012
 *      Author: morgenro
 *      		niels_w
 */

#include "wifip2p/SupplicantHandle.h"

//#include <stdlib.h>
//#include <stdint.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <unistd.h>	//recvReply FD operations accordingly to http://www.kernel.org/doc/man-pages/online/pages/man2/read.2.html
#include <common/wpa_ctrl.h>
#include <string.h>

using namespace std;

namespace wifip2p {

	const string SERVDISC_TYPE = "upnp";
	const string SERVDISC_VERS = "10";
	const string BROADCAST = "00:00:00:00:00:00";
	const string REQ_ALL_UPNP_SERVICES = "02000201";

	SupplicantHandle::SupplicantHandle(bool monitor)
		: _handle(NULL),
		  monitor_mode(monitor) {

	}

	SupplicantHandle::~SupplicantHandle() {

		/*
		 * _handle is going to be dereferenced, i.e., the function
		 *  wpa_ctrl_close gets handed over the actual control_i/f
		 *  referenced by _handle.
		 */
		//if (monitor_mode)
		//	if (wpa_ctrl_detach((struct wpa_ctrl*)_handle) < 0)
		//		cerr << "Failed to detach monitor properly. Close will be forced." << endl;

		if (_handle != NULL)
			wpa_ctrl_close((struct wpa_ctrl*)_handle);

	}


	/*
	 * Take a look at hostap/src/common/wpa_ctrl.{h|c} for what methods
	 *  are actually provided. The needed maybe taken and used, by
	 *  handing over the _handle variable.
	 * The _handle variable represents the connection to the Linux
	 *  internal application socket towards WPA_SUPPLICANT as opened
	 *  through wpa_control_open(ctrl_path). Here, ctrl_path is the
	 *  actual socket address in the Linux file system, e.g.
	 *  /var/run/wpa_supplicant, which holds all the responsibility
	 *  of de-/multiplexing those connections.
	 */

	/*
	static void hostapd_cli_msg_cb(char *msg, size_t len)
	{
		cout << msg << endl;
	}
	*/

	/**
	 * @*ctrl_path: Here, _handle is defined, representing the now opened
	 * 				 control_i/f of wpa_s as expected behind the *ctrl_path
	 * 				 parameter.
	 * 				 E.g. when wpa_s is bound to hardware interface wlan0,
	 * 				 fully qualified *ctrl_path needs to match
	 * 				 "/var/run/wpa_supplicant/wlan0"
	 */
	void SupplicantHandle::open(const char *ctrl_path) throw (SupplicantHandleException) {

		_handle = wpa_ctrl_open(ctrl_path);

		if (_handle != NULL) {
			if (this->monitor_mode) {
				if (this->setMonitorMode())
					cout << "WPASUP is now being monitored." << endl;
				else {
					monitor_mode = false;
					throw SupplicantHandleException("Failed to set monitor mode.");
				}
			} else
				cout << "Connection to WPASUP established." << endl;
		} else {
			throw SupplicantHandleException("Connection to WPASUP failed.");
		}
	}

	/**
	 * Initializes wpa_s beyond *_handle, i.e. sets the respective device name
	 * 	through cfg/mac80211, flushes all yet registered services, adds all services
	 * 	as handed over with list<string> services.
	 *
	 * @name: 	  The hardware interface's name, as to be set by wpa_s.
	 * @services: The services, as to be registered at wpa_s for being able to
	 * 			   respond, in case of any regarding requests.
	 */
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


	/**
	 * TODO <Description>
	 *
	 * @name: 	The name as to be set for the device, controlled by this wpa_s.
	 * Returns: true, if device name is set; false, else. Throws exception on error.
	 *
	 */
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
								+ "name:" + name + "::"
								+ "service:" + service,	NULL);
			return true;
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
		return false;
	}

	/**
	 * Initiates P2P_FIND command at wpa_s. Escalates exception on failure.
	 *
	 */
	void SupplicantHandle::findPeers() throw (SupplicantHandleException) {
		try {
			this->p2pCommand("P2P_FIND", NULL);
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
	}

	/**
	 * Stops P2P_FIND command at wpa_s.
	 *
	 */
	void SupplicantHandle::findPeersStop() throw (SupplicantHandleException) {
		try {
			this->p2pCommand("P2P_STOP_FIND", NULL);
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
	}

	/**
	 * TODO <Description>
	 *
	 * TODO <Signature-Description>
	 *
	 * 	Return: string of the fetched wpa_s reply. The handling, whether the reply
	 * 			is of interest in the overall state machine and setting or not, will
	 * 			then be handled in the explicit state of the respective CoreEngine.
	 * 		Another possibility would be to >>
	 *  Return: void and handling all the decision whether to call back any of the
	 *  		external application's implementation of the WifiP2PInterface.
	 *  		Here, it would be of interest if then the program jumps back into
	 *  		the originating SupplicantHandle::listen() method, when, e.g. lead to a
	 *  		WifiP2PInterface::peerFound(Peer p) --> CoreEngine::connect(Peer p).
	 *  		(!) In turn, this procedure needs any SupplicantHandle to hold a
	 *  			variable referring to the respective WifiP2PInterface implementation.
	 */
	void SupplicantHandle::listen(list<Peer> &peers, list<Connection> &connections,
			list<string> &services, list<string> &sdreq_id,
			WifiP2PInterface &ext_if) throw (SupplicantHandleException) {

		if (this->monitor_mode) {

			int x = wpa_ctrl_pending((struct wpa_ctrl*)_handle);

//			if (x!=0)
//				cout << "Pending? " << x << endl;

			char buf[256];
			size_t len = 256;

			if (wpa_ctrl_pending((struct wpa_ctrl*)_handle) > 0) {

				wpa_ctrl_recv((struct wpa_ctrl*)_handle, &buf[0], &len);

				//cout << "buffer generating" << endl;

				string buffer(buf+3, len-3);
				vector<string> msg = msgDecompose(buffer);

				//cout << "RECV: " << len << " bytes, " << std::string(buf, len) << endl;

				/* EVENT >> [DEVICE_FOUND]
				 *
				 */
				if (msg.at(0) == P2P_EVENT_DEVICE_FOUND) {

					cout << "EVENT >> [DEVICE_FOUND]" << endl;

					/* With msg.at(1) one may get the MAC immediately, but this
					 *  place especially not stores the hardware interface's MAC,
					 *  but the one of a may or may not have been created virtual
					 *  interface.
					 * So here we cope with msg.at(2).substr(13), resulting in
					 *  msg.at(2): 			  "p2p_dev_addr=<MAC-ADDR>"
					 *  msg.at(2).substr(13): "<MAC-ADDR>"
					 */
					//cout << "Occurence (1)  : " << msg.at(2) << endl;

					//if (msg.at(2).length() <=13) return;

					string mac(msg.at(2).substr(13));
					cout << "Peer found " << mac << endl;
					Peer p(mac);

					list<Peer>::const_iterator peer_it = find(peers.begin(), peers.end(), p);

					if (peer_it == peers.end()) {
					    peers.push_back(p);
					    cout << "Peer pushed back at peers, as yet not contained" << endl;
					} else {
						//Check: p in list? => peer_it ~ p in list
					    if (peer_it->getName() != "") {
					        /* If peer_it has name other than ""
					         * 	=> peer_it is already fully_discovered
					         * 	=> call back ext_if with *peer_it
					         * 		dereferenced
					         */
					    	cout << "call back ext_if with fully discovered peer" << endl;
					        ext_if.peerFound(*peer_it);
					    } //else {
					    	//
					    	//TODO [!!!!]
					    	//
					    	// DIESEN BLOCK AUSLAGERN IN DIE CORE_ENGINE!
					    	// Dort sollen alle lokal registrierten Services per broadcast
					    	//	an all Peers gehen.
					    	// Der Request wie in diesem Block, ist nötig für eingehende
					    	//	service_requests, sofern der anfragende Peer nicht bereits
					    	//	fully_discovered (~ bekannt) ist.
					    	//
					    	/* peer_it is not fully_discovered yet
					    	 * 	=> start service_discovery@peer_it
					    	 */
					    //	cout << "peer not fully discovered. request services" << endl;
					    //    list<string>::iterator it = services.begin();
					    //    for (; it != services.end(); ++it)
					    //       requestService(*peer_it, *it, &sdreq_id);
					    //}
					}
				}

				/* EVENT >> [GROUP_STARTED]
				 *
				 * That is, a connection is established.
				 * For a connection being established, the peer to be
				 *  connected is required to be fully_discovered -
				 *  as only those peers are handed out to ext_if eventually
				 *  being called for a connection.
				 *
				 */
				if (msg.at(0) == P2P_EVENT_GROUP_STARTED) {

					cout << "EVENT >> [GROUP_STARTED]" << endl;

					if (msg.at(2) == "GO") {
						NetworkIntf go_nic(msg.at(1));
						Connection new_conn(go_nic);
						connections.push_front(new_conn);
					} else {
						/*
						 */
						if (msg.at(2) == "client") {

							//cout << "Occurence (2)" << endl;
							Peer temp_p(msg.at(6).substr(12));
							NetworkIntf nic(msg.at(1));

							list<Peer>::const_iterator peer_it = find(peers.begin(), peers.end(), temp_p);

							if (peer_it != peers.end()) {
								Connection conn(*peer_it, nic);
								connections.push_front(conn);
								ext_if.connectionEstablished(conn);
							}
						}
					}
				}

				/* EVENT >> [AP_STA_CONNECTED]
				 *
				 * As the process is some kind of probabilistic, whether
				 * 	frames cause wpa_s to generate whatever EVENT next, it
				 * 	is assumed here, that the up front Connection within
				 * 	connections is one with no Peer assigned yet.
				 * 	The respective station receiving this EVENT is
				 * 	assumed to be the GO of this Connection. The Peer
				 * 	who lead to the generating this EVENT is likely to
				 * 	be the one related to this connection.
				 * Because only fully discovered Peers may be selected for
				 * 	begin connected, the specific one must be contained
				 * 	within list<Peer> peers.
				 *
				 */
				if (msg.at(0) == AP_STA_CONNECTED) {

					cout << "EVENT >> [AP_STA_CONNECTED]" << endl;

					//cout << "Occurence (3)" << endl;
					Peer connected_peer(msg.at(2).substr(13));
					list<Peer>::const_iterator peer_it = find(peers.begin(), peers.end(), connected_peer);

					if (peer_it != peers.end()) {
						connections.front().setPeer(*peer_it);
						ext_if.connectionEstablished(connections.front());
					}
				}


				/* EVENT >> [AP_STA_DISCONNECTED]
				 *
				 * Maybe the GO had removed the p2p_group or, after a
				 * 	longer timeout, a connected non-GO station had
				 * 	removed its p2p_group_connection, i.e. the corresponding
				 * 	virtual interface.
				 *
				 */
				if (msg.at(0) == AP_STA_DISCONNECTED) {

					cout << "EVENT >> [AP_STA_DISCONNECTED]" << endl;

					//cout << "Occurence (4)" << endl;
					string mac_lost = msg.at(2).substr(13);

					list<Connection>::iterator it = connections.begin();

					for (; it != connections.end(); ++it) {
						if (it->getPeer().getMacAddr() == mac_lost) {
							disconnect((*it));
							ext_if.connectionLost(*it);
						}
					}
				}

				/* EVENT >> [GROUP_NEG_REQUEST]
				 *
				 * That is a connection request.
				 * Checks whether requesting peer is fully discovered
				 *  or not. If so, the the connect command will automatically
				 *  be called at wpa_s. Else if, ext_if will be called.
				 *
				 */
				if (msg.at(0) == P2P_EVENT_GO_NEG_REQUEST) {

					cout << "EVENT >> [GROUP_NEG_REQUEST]" << endl;

					Peer temp_p(msg.at(1));

					list<Peer>::const_iterator peer_it = find(peers.begin(), peers.end(), temp_p);

					if (peer_it != peers.end()) {
						connectToPeer(*peer_it);
					} else {
						ext_if.peerFound(*peer_it);
					}
				}

				// EVENT >> p2p_device_lost (device or connection lost for reasons unknown)
				// TODO Very likely not needed. CHECK!
				/*
				if (msg.at(0) == P2P_EVENT_DEVICE_LOST) {
					string mac_lost = msg.at(1).substr(13);
					list<Connection>::iterator it = connections.begin();
					for (; it != connections.end(); ++it) {
						if (it->getPeer().getMacAddr() == mac_lost)
							disconnect(*it);
						else
							continue;
					}
				}
				*/

				/* EVENT >> [RECEIVED_SERVICE_REQUEST]
				 *
				 */
				if (msg.at(0) == P2P_EVENT_SERV_DISC_REQ) {

					cout << "EVENT >> [RECEIVED_SERVICE_REQUEST]" << endl;

					Peer p(msg.at(2));

					list<Peer>::const_iterator peer_it = find(peers.begin(),
																peers.end(), p);

					if (peer_it != peers.end()) { //true if p in peers
						if (peer_it->getName() != "") //true if p~peer_it fully_discovered
							ext_if.peerFound(*peer_it);
						else { //p~peer_it not fully_discovered => DISCOVER!
							list<string>::iterator it = services.begin();
							for (; it != services.end(); ++it)
								requestService(*peer_it, *it, &sdreq_id);
						}
					} else {
						peers.push_back(*peer_it);
					}
				}

				/* EVENT >> [RECEIVED_SERVICE_RESPONSE]
				 *
				 * The return of not only the upnp sd_resp header with empty
				 * 	data implies the specific peer to host at least on
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

					cout << "EVENT >> [RECEIVED_SERVICE_RESPONSE]" << endl;

					if (msg.at(3) != "0300020101") {
						Peer p(msg.at(1));
						list<Peer>::const_iterator peer_it = find(peers.begin(), peers.end(), p);

						if (peer_it != peers.end()) {
							if (peer_it->getName() != "") {
								ext_if.peerFound(*peer_it);
							} else {
								list<Peer>::iterator it = peers.begin();
								for (; it != peers.end(); ++it) {
									/* TODO
									 * /bin/sh: Zeile 5: 14029 Speicherzugriffsfehler
									 * 		(Speicherabzug geschrieben) ${dir}$tst
									 * FAIL: basetest
									 *
									 * >>
									 *
									 * probably done, got the hint to change
									 * 	peers.erase(it)
									 * 	  into
									 * 	  it= peers.erase(it)
									 *
									 */
									if (*it == p){
										it = peers.erase(it);
									}
								}
								//cout << "Occurence (4b)" << endl;
								cout << "Message " << msg.at(3) << "; length = " <<
										msg.at(3).length() << endl;
								if (msg.at(3).length() > 11) {
									p.setName(getPeerNameFromSDResp(msg.at(3).substr(12))); //.substr(12)
									peers.push_back(p);
								} else {
									cout << "Non-confirm service registered at " << p.getMacAddr() << endl;
								}
							}
						} else {
							p.setName(getPeerNameFromSDResp(msg.at(3))); //.substr(12)
							peers.push_back(p);
						}
							// TODO IMPLEMENT ::getPeerNameFromSDResp(string tlv)
							/*
							 * This case is rather unrealistic, as it is likely be
							 * 	assumed that any peer is discovered with it's name
							 * 	stored beside the MAC address in a proper Peer object
							 * 	right before any sd_resp will be received.
							 * 	Following this, the very most of the peers, as being
							 * 	candidates for potentially connections, will have been
							 * 	discovered already and are stored locally, with proper
							 * 	MAC and peer names.
							 * Implementing the above method would solve this issue
							 * 	completely.
							 *
							 */
					} else {
						list<Peer>::iterator it = peers.begin();
						for (; it != peers.end(); ++it) {
							if (it->getMacAddr() == msg.at(1)) {
								it = peers.erase(it);
							}
						}
					}
				}
			}
		} else {
			// TODO is an exception really needed here?
			throw SupplicantHandleException("SupplicantHandle needs to be in monitor mode for ::listen().");
		}
	}

	/**
	 * Initiates a upnp service request at wpa_s as fully broadcast, i.e. its
	 * 	destination address is 00:00:00:00:00:00.
	 *
	 * @service   The string to be requested for; according to upnp the this
	 * 			   string represents the ST-Field (Search_Target) of its
	 * 			   respective M-SEARCH request.
	 * @*sdreq_id Pointer enabling ::requestService() to call back a list<string>
	 * 			   and ::push_back() the wpa_s' returned service_request_id.
	 * 			   This id is later needed by wpa_s to cancel the request. If not
	 * 			   canceled, the request will be broadcast potentially for ever
	 * 			   -- though no more considered (and replied) by peers, which were
	 * 			   able to handle it properly; others would be penetrated.
	 */
	void SupplicantHandle::requestService(string service, list<string> *sdreq_id)
			throw (SupplicantHandleException) {
		try {
			if (sdreq_id != NULL) {
				string returned_id;
				this->p2pCommand("P2P_SERV_DISC_REQ "
						+ BROADCAST + " "
						+ SERVDISC_TYPE + " "
						+ SERVDISC_VERS + " "
						+ service, &returned_id);
				sdreq_id->push_back(returned_id);
			} else {
				this->p2pCommand("P2P_SERV_DISC_REQ "
						+ BROADCAST + " "
						+ SERVDISC_TYPE + " "
						+ SERVDISC_VERS + " "
						+ service, NULL);
			}
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
	}

	/**
	 * Requests a upnp service, at a specific destination peer.
	 * This method is especially meant to request the destination peer's name,
	 *  as registered per service.
	 *  Furthermore, one will get a service response whether the service is
	 *  registered at that peer's wpa_s or not.
	 * The whole procedure turns a peer (only MAC known) into a 'fully discovered'
	 * 	peer, with MAC and name known, in the case it holds the requested service.
	 * 	The name must be derived from the destination peer's automatically
	 * 	generated sd_response.
	 *
	 * @peer:	   The peer destined for the service request.
	 * @service:   The service in request.
	 * @*sdreq_id: Pointer at a list, where the per request corresponding
	 * 				will be pushed at sdreq_id. Required to cancel any requested
	 * 				service later.
	 *
	 */
	void SupplicantHandle::requestService(Peer peer, string service, list<string> *sdreq_id)
			throw (SupplicantHandleException) {
		try {
			if (sdreq_id != NULL) {
				string returned_id;
				this->p2pCommand("P2P_SERV_DISC_REQ "
						+ peer.getMacAddr() + " "
						+ SERVDISC_TYPE + " "
						+ SERVDISC_VERS + " "
						+ service, &returned_id);
				sdreq_id->push_back(returned_id);
			} else {
				this->p2pCommand("P2P_SERV_DISC_REQ "
						+ peer.getMacAddr() + " "
						+ SERVDISC_TYPE + " "
						+ SERVDISC_VERS + " "
						+ service, NULL);
			}
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
	}

	/**
	 * Cancels a pending service request, according to its id at wpa_s.
	 *
	 * @sdreq_id: The corresponding ID required to cancel a pending service request.
	 *
	 */
	void SupplicantHandle::requestServiceCancel(string sdreq_id) throw (SupplicantHandleException) {
		try {
			this->p2pCommand("P2P_SERV_DISC_CANCEL_REQ " + sdreq_id, NULL);
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
	}



	/**
	 * Creates a peering between this local and wpa_s controlled WNIC and Peer
	 *  peer. The method uses WPS Push Button Configuration (PBC). wpa_s will
	 *  create a virtual WNIC per each connection which is going to be established.
	 *  The virtual interface will be called "p2p-<HW-IF-name>-<#count>".
	 *
	 * @peer: The peer to which wpa_s should initiate a connection.
	 *
	 */
	void SupplicantHandle::connectToPeer(Peer peer) throw (SupplicantHandleException) {
		try {
			this->p2pCommand("P2P_CONNECT " + peer.getMacAddr() + " pbc", NULL);
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
	}

	/**
	 * By now, this method only handles to remove wpa_s' created virtual WNICs,
	 *  thus disconnecting both of a group's peers.
	 * This is sufficient, as SupplicantHandle::connectToPeer(Peer) does not deal
	 * 	with joining a P2P group, whether available, but just always creates
	 * 	exactly one peering between each two devices in a separate group.
	 *
	 * @conn: The Connection which stores the interface to be removed.
	 *
	 */
	void SupplicantHandle::disconnect(Connection conn) throw (SupplicantHandleException) {
		try {
			this->p2pCommand("P2P_GROUP_REMOVE " + conn.getNetworkIntf().getName(), NULL);
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
	}



	/**
	 * TODO <Description>
	 *
	 * @cmd: 			  Command string, to be transmitted to wpa_s. See wpa_s
	 * 					   documentation for possible commands.
	 * 					  IT IS OF HIGHEST IMPORTANCE, to handing over everything
	 * 					   except the actual command - which is only the very
	 * 					   first part of each such statement - and freely to be
	 * 					   defined names, e.g. an actual device name, in all-lower
	 * 					   case letters! Otherwise, wpa_s will fail executing
	 * 					   the command.
	 * @*direct_feedback: TODO <Description>
	 * Returns: 		  true or false, whether the command may be initiated
	 * 					   successfully or not.
	 */
	bool SupplicantHandle::p2pCommand(string cmd, string *direct_feedback) throw (SupplicantHandleException) {

		const char *cmd_p;
		cmd_p = cmd.c_str();

		char 	reply_buf[64];
		size_t 	reply_len = sizeof(reply_buf) - 1;

		int ret = wpa_ctrl_request((struct wpa_ctrl*) _handle,
					cmd_p, cmd.length(),
					*&reply_buf, &reply_len, NULL);

		if (ret == -1)
			throw SupplicantHandleException("wpa_s ctrl_i/f; send or receive failed.");
		if (ret == -2)
			throw SupplicantHandleException("wpa_s ctrl_i/f; communication timed out.");

		/* May be uncommented for testing purposes. Enables getting
		 * 	insight into actual command and variable statuses, which else
		 * 	would have been hidden by flying exceptions, in case of
		 * 	errors.
		 *
		 */
		//cout << cmd << endl;
		//cout << "reply_buf: " << reply_buf << endl;

		string reply(reply_buf, reply_len);

		//cout << "Occurence (5)" << endl;
		if (reply.substr(0, reply_len -1) == "FAIL") {
			throw SupplicantHandleException("wpa_s was not able to initiate <" + cmd + "> successfully.");
		} else {
			if (direct_feedback != NULL)
				direct_feedback = &reply;
			return true;
		}

		return false;

	}


	/**
	 * Decomposed the wpa_s message into an array of strings, with
	 * 	each array element representing one of the message's values.
	 * 	The first value within the returned string array represents
	 * 	the event type while the following depend on that type.
	 *
	 * @char: 	Awaits a wpa_s event message for input.
	 * Returns: string array, [0] = msg type, [..]
	 */
	vector<string> SupplicantHandle::msgDecompose(string buffer) {

		//string buffer(buf+3);
		vector<string> ret = tokenize(" ", buffer, -1);
		ret[0] += " ";
		return ret;
//
//		int k=1;
//		for (int i=0; i<buffer.length(); i++) {
//			if (buffer[i] == ' ' && i != buffer.length()-1)
//				++k;
//		}
//
//		vector<string> ret_vec;
//		int i=0, dif;
//		size_t pos, npos;
//		npos = buffer.find(' ');
//
//		/** [+1] in order to additionally fetch the ending-blank			*
//		 * 	[-3] in order to equalize the event msgs' introductory "<3>" 	*/
//		dif = (npos + 1) - 3;
//
//		//cout << "Occurence (6)" << endl;
//		ret_vec.push_back(buffer.substr(3, dif)); //substr(startpos, steps)
//		++i;
//
//		while (i<k) {
//			pos = npos;
//			npos = buffer.find(' ', pos+1);
//			dif = npos - pos - 1;
//			//cout << "Occurence (7)" << endl;
//			ret_vec.push_back(buffer.substr(pos+1, dif));
//			++i;
//		}
//
//		/* Only for testing purposes. Uncomment to get insight on vector's
//		 * 	values.
//		 *
//		 */
//		//vector<string>::iterator it = ret_vec.begin();
//		//for (; it != ret_vec.end(); ++it)
//		//	cout << *it << endl;
//
//		return ret_vec;
//
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


	/**
	 * Converts hexadecimal input data to its proper ASCII coded
	 * 	string representation.
	 * Needs helper function SupplicantHandle::hexlookup().
	 *
	 * @string: The TLV received from a service discovery response.
	 * 			 That is only response frame's data, not its header.
	 * 			 Independently of wpa_s' specification, this method
	 * 			 may actually only deal with service entries at
	 * 			 opposite stations, which do not overflow the total
	 * 			 size of 65535 Byte, which equals FFFF as length.
	 * Return:	A string representation of the TLV input data.
	 *
	 */
	string SupplicantHandle::getPeerNameFromSDResp(string tlv) {

		cout << "TLV  : " << tlv << endl;

		string result;

		bool x = true;
		short secnd, first;

		for (unsigned int i=0; i<tlv.length(); i++) {

			if (x) {
				secnd = hexlookup(tlv[i]) * 16;
				//cout << "The second " << secnd << endl;
				x = !x;
			} else {
				first = hexlookup(tlv[i]);
				//cout << "The first " << first << endl;
				x = !x;

				int want = secnd + first;
				cout << (char) want;
				result.push_back((char) want);
			}
		}

		cout << endl;

		return result;
	}

	/**
	 * Sets SupplicantHandle in monitor mode by attaching it to wpa_s' domain
	 * 	socket.
	 * Initializes the attached domain sockets FileDescriptor as returned by
	 * 	the proper wpa_s control_i/f's function.
	 *
	 * Returns: true, if the monitor may be set. false, otherwise.
	 *
	 */
	bool SupplicantHandle::setMonitorMode() throw (SupplicantHandleException) {
		if (wpa_ctrl_attach((struct wpa_ctrl*) (_handle)) == 0)
			return true;
		else
			return false;
	}

	/**
	 * Table lookup returning a short (decimal representation)
	 * 	accordingly to the input character (hex representation).
	 *
	 * @c:	   hex input character [0-9,a-f]
	 * Return: decimal equivalent [0-15]
	 *
	 */
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
