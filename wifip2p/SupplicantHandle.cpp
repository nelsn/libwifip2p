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
#include <cstdlib>
#include <iostream>
#include <unistd.h>	//recvReply FD operations accordingly to http://www.kernel.org/doc/man-pages/online/pages/man2/read.2.html
#include <common/wpa_ctrl.h>

using namespace std;

namespace wifip2p {

	const string SERVDISC_TYPE = "upnp";
	const string SERVDISC_VERS = "10";
	const string BROADCAST = "00:00:00:00:00:00";

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
	 * @*ctrl_path: Here, _handle is defined, representing the now opened control_i/f
	 * 				 of wpa_s as expected behind the *ctrl_path parameter.
	 * 				 E.g. when wpa_s is bound to hardware interface wlan0, fully qualified
	 * 				 *ctrl_path needs to match "/var/run/wpa_supplicant/wlan0"
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
	void SupplicantHandle::listen(list<Peer> *peers, WifiP2PInterface *ext_if) throw (SupplicantHandleException) {
		if (this->monitor_mode) {

			cout << "BLABLABLABLA" << endl;

			int x = wpa_ctrl_pending((struct wpa_ctrl*)_handle);
			cout << "wpa_ctrl_pending results in: " << x << endl;

			char buf[256];
			size_t len;

			int k = 0;

			while (true) {

				if (wpa_ctrl_pending((struct wpa_ctrl*)_handle) > 0) {

					len = sizeof(buf);

					wpa_ctrl_recv((struct wpa_ctrl*)_handle, buf, &len);

					string buffer(buf, len);
					string bufcmd(buf, 23);
					//cout << buffer << "cut buffer --23 > " << bufcmd << endl;

					if (!(bufcmd == "<3>CTRL-EVENT-BSS-ADDED"
							&& bufcmd == "<3>CTRL-EVENT-BSS-REMOVED")) {
						cout << "Inner while-loop #" << k << endl;

						cout << buffer << endl;
					}

					vector<string> msg = msgDecompose(buf);

					//EVENT >> p2p_device_found
					if (msg.at(0) == P2P_EVENT_DEVICE_FOUND) {

						string mac(msg.at(1));
						string name = msg.at(4).substr(6, msg.at(4).length() - 7);

						Peer p(mac, name);

						list<Peer>::iterator jt = peers->begin();
						bool in_list = false;

						for (; jt != peers->end(); ++jt) {
							cout << jt->getName() << " :: " << jt->getMacAddr() << endl;
							if (jt->getName() == p.getName()) {
								if (jt->getMacAddr() == p.getMacAddr())
									in_list = true;
							}
						}

						if (!in_list) {
							peers->push_back(p);
							cout << " pushed to list" << endl;
						}

						++k;
					}

					//EVENT >> p2p_group_started (i.e. conn_established)
					if (msg.at(0) == P2P_EVENT_GROUP_STARTED) {
						;
					}

					//EVENT >> catch(group_formation_failure)


					//EVENT >> retrieved_service_request
					if (msg.at(0) == P2P_EVENT_SERV_DISC_REQ) {
						;
					}

					//EVENT >> retrieved_service_response
					if (msg.at(0) == P2P_EVENT_SERV_DISC_RESP) {
						;
					}


					if (k == 1)
						break;

				}
			}


			cout << "After while" << endl;

		} else {
			// TODO is an exception really needed here?
			throw SupplicantHandleException("SupplicantHandle needs to be in monitor mode for ::listen().");
		}
	}

	/**
	 * Initiates a fully broadcast upnp service request at wpa_s, i.e. its
	 * 	destination address is 00:00:00:00:00:00.
	 *
	 * @service   The string to be requested for; according to upnp the this
	 * 			   string represents the ST-Field (Search_Target) of its
	 * 			   respective M-SEARCH request.
	 * @*sdreq_id Pointer enabling ::requestService() to call back a list<string>
	 * 			   and ::push_back() the wpa_s returned service_request_id.
	 * 			   This id is later needed by wpa_s to cancel the request. If not
	 * 			   canceled, the request will be potentially broadcast for ever
	 * 			   -- though no more considered (and replied) by peers which were
	 * 			   able to handle it properly. But others would be penetrated.
	 */
	void SupplicantHandle::requestService(string service, list<string> *sdreq_id) throw (SupplicantHandleException) {
		try {
			if (sdreq_id != NULL) {
				string returned_id;
				this->p2pCommand("P2P_SERV_DISC_REQ "
						+ BROADCAST
						+ SERVDISC_TYPE
						+ SERVDISC_VERS
						+ service, &returned_id);
				sdreq_id->push_back(returned_id);
			} else {
				this->p2pCommand("P2P_SERV_DISC_REQ "
						+ BROADCAST
						+ SERVDISC_TYPE
						+ SERVDISC_VERS
						+ service, NULL);
			}
		} catch (SupplicantHandleException &ex) {
			throw SupplicantHandleException(ex.what());
		}
	}

	// TODO
	void SupplicantHandle::requestService(Peer peer, string service) throw (SupplicantHandleException) {
		;
	}

	// TODO
	void SupplicantHandle::requestServiceCancel(string sdreq_id) throw (SupplicantHandleException) {
		;
	}



	/**
	 * Creates a peering between this local and wpa_s controlled WNIC and Peer peer.
	 * 	The method uses WPS Push Button Configuration (PBC). wpa_s will create a
	 *  virtual WNIC per each connection which is going to be established. The virtual
	 *  interface will be called "p2p-<HW-IF-name>-<#count>".
	 *
	 * @peer the peer to which wpa_s should initiate a connection.
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
	 * By now, this method only handles to remove wpa_s' created virtual WNICs, thus
	 *  disconnecting both of the group's peers.
	 * This is sufficient, as SupplicantHandle::connectToPeer(Peer) does not deal with
	 *  joining a P2P group, whether available, but just always creates exactly one
	 *  peering between each two devices in a separate group.
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
	 * @cmd: 	Command string, to be transmitted to wpa_s. See wpa_s documentation
	 * 				for possible commands.
	 * 			IT IS OF HIGHEST IMPORTANCE, to handing over everything except the actual
	 * 				command - which is only the very first part of each such statement -
	 * 				and freely to be defined names, e.g. an actual device name, in all-lower
	 * 				case letters! Otherwise, wpa_s will fail executing the command.
	 * Returns: true or false, whether the command may be initiated successfully
	 * 				or not.
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

		/**	May be uncommented for testing purposes, to enable getting insight into actual
		 * 	 command and variable statuses, which else would have been hidden by flying
		 * 	 exceptions, in case of errors.
		 */
		//cout << cmd << endl;
		//cout << "reply_buf: " << reply_buf << endl;

		string reply(reply_buf, reply_len);

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
	vector<string> SupplicantHandle::msgDecompose(char* buf) {

		string buffer(buf);

		int k=1;
		for (int i=0; i<buffer.length(); i++) {
			if (buffer[i] == ' ' && i != buffer.length()-1) 
				++k;
		}
		
		vector<string> ret_vec;
		int i=0, dif;
		size_t pos, npos;
		npos = buffer.find(' ');

		/** [+1] in order to additionally fetch the ending-blank			*
		 * 	[-3] in order to equalize the event msgs' introductory "<3>" 	*/
		dif = (npos + 1) - 3;

		ret_vec.push_back(buffer.substr(3, dif)); //substr(startpos, steps)
		++i;

		while (i<k) {
			pos = npos;
			npos = buffer.find(' ', pos+1);
			dif = npos - pos - 1;
			ret_vec.push_back(buffer.substr(pos+1, dif));
			++i;
		}

		/** Only for testing purposes. Uncomment to get insight on vector's
		 * 	 values.
		 */
		//vector<string>::iterator it = ret_vec.begin();
		//for (; it != ret_vec.end(); ++it)
		//	cout << *it << endl;

		return ret_vec;

	}

/*
	 * Utility functions >>
	 *
	 */

	/**
	 * Sets SupplicantHandle in monitor mode by attaching it to wpa_s' domain socket.
	 * Initializes the attached domain sockets FileDescriptor as returned by the
	 * 	proper wpa_s control_i/f's function.
	 *
	 * Returns: true, if the monitor may be set. false, otherwise.
	 *
	 */
	bool SupplicantHandle::setMonitorMode() throw (SupplicantHandleException) {
		if (wpa_ctrl_attach((struct wpa_ctrl*) (_handle)) == 0) {

			//Get monitoring socket's FD
			fd_listen = wpa_ctrl_get_fd((struct wpa_ctrl*)_handle);

			cout << "fd_listen state: " << fd_listen << endl;

			if (fd_listen < 0) {
				throw SupplicantHandleException("FileDescr. problem occured.");
			}
			return true;
		} else {
			return false;
		}
	}

} /* namespace wifip2p */
