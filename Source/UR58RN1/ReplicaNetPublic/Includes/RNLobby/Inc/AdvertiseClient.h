/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _ADVERTISECLIENT_H_
#define _ADVERTISECLIENT_H_
#include "RNLobby/Inc/BaseTransportServer.h"
#include "RNXPSockets/Inc/XPSocket.h"
#include "RNLobby/Inc/UserClient.h"
#include <list>

namespace RNReplicaNet
{

namespace RNLobby
{

/// Maintains a connection to the AdvertiseServer.
class AdvertiseClient : public BaseTransportServer
{
public:
	AdvertiseClient();
	virtual ~AdvertiseClient();

	/// Because this uses a TransportUDP connection it will call XPURL::RegisterDefaultTransports()
	bool Start(void);

	/// This starts this class to only advertise and find sessions on the LAN
	bool StartLAN(void);

	/// Sets the address and port of the server. The default for the instance of the class is localhost and port AdvertiseServer::kDefaultPort or the address resolved by a successful BackendClient::Start().
	/// \param address The new address and port number to use for the server
	void SetServer(const XPAddress &address);

	/// One advertised session for each instance of this class. This can be called with any updated details as often as required.
	/// Typically this will be used to advertise the exported URL from a NATUDP transport or a ReplicaNet/XPSession session using NATUDP.
	/// If using ReplicaNet or XPSession and the instance is a session master (ReplicaNet::IsMaster() == true)
	/// or using a NATUDP transport it is a good idea to regularly export the url and call this function with the details.
	/// This is because the url can change as the NAT resolution processes does its work. Please see Example11 for details.
	/// This starts the "getinfo" advertise using TransportConfig::SetInfoString(url + userData).
	bool AdvertiseSession(const char *url,const char *userData = "");

	/// The user may call this notify the server that the session rules have changed. This then causes a new rules list to be requested by CallbackWantRules.
	bool RulesChanged(void);

	/// The user may call this notify the server that the session players list has changed. This then causes a new players list to be requested by CallbackWantPlayers.
	bool PlayersChanged(void);

	/// Stop advertising the session
	/// This also stops the "getinfo" advertise using TransportConfig::SetInfoString().
	bool StopAdvertiseSession(void);

	/// Gets listings with an optional filter. Using a filter query increases the load on the database depending on how many filters are used.
	/// \param maxResults The maximum number of results returned by this query.
	/// \param offset The offset into the information stored by the AdvertiseServer that will be returned by this query.
	/// \param filter An optional filter to use that is applied at the server before sending data to the client. The filter consists of logical expressions separated by a new-line or semi-colon. For example:
	/// Setting a filter to be "rule.needPass = 1;rule.dedicated = 0;" will only return servers that have the rules called needPass and dedicated which are equal to 1 and 0 respectively.
	/// The characters allowed for the rule conditional operand are "=><!"
	bool GetListings(const unsigned int maxResults = 10,const unsigned int offset = 0,const char *filter = 0);

	/// Stop the current request for listings.
	bool StopListings(void);

	/// Request the rules for a specific server. CallbackRulesBegin etc will be called when the rules have been fetched.
	bool RequestRules(const int serverIndex);

	/// Request the players for a specific server. CallbackPlayersBegin etc will be called when the players have been fetched.
	bool RequestPlayers(const int serverIndex);

	/// Requests the latency to a specific server to be calculated. CallbackLatencyTo will be called when the latency has been calculated.
	/// <br>Depending on how the server is configured and the firewall/NAT used this operation may take a long time to complete or may never complete indicating the server couldn't be reached or there is significant packet loss.
	bool RequestLatency(const int serverIndex);

	/// Returns true if there are any pings active.
	bool GetAnyActivePings(void) const;

	/// This must only be called during CallbackWantRules()
	/// \param rule The rule name as a text string.
	/// \param userData The data attached to this rule name;
	bool AddRule(const char *rule,const char *userData = "");

	/// Adds a player to an advertised server.
	/// This must only be called during CallbackWantPlayers()
	/// \param name The name of the player. This need not always be the name, it could be any user defined data in the form of a text string.
	/// \param userData Any user defined data as a text string.
	/// \param loginID This loginID may be the same as the loginID from UserClient::GetLoginID(), this is left to the user to implement if required.
	bool AddPlayer(const char *name,const char *userData = "",const int loginID = UserInfo::kAnyLoginID);

	/// Sends a reliable message to a server index. The message arrives via CallbackMessageReceived(). Zero length message are not sent. The maximum size of the message that can be sent is 512 bytes.
	/// \return false indicates there was an error sending the message, for example the server index may be unknown.
	bool SendReliableMessage(const int serverIndex,const void *data,const int length);

protected:

	/// User extensible callbacks.

	/// This callback is not triggered during a LAN session query.
	virtual void CallbackConnectionEstablished(Transport *transport) {}

	/// This is called by AdvertiseClient when the user needs to supply rules for the currently advertised server.
	virtual void CallbackWantRules(void) {}

	/// This is called by AdvertiseClient when the user needs to supply players for the currently advertised server.
	virtual void CallbackWantPlayers(void) {}

	/// This is called when the server index is known for the currently advertised server.
	virtual void CallbackGotServerIndex(const int serverIndex) {}

	/// This is called at the start when the listings are refreshed.
	virtual void CallbackListingsBegin(void) {}

	/// This is called when there is a listings entry.
	/// \param serverIndex The server index that can be used to identify this entry with other callbacks or with RequestRules() for example.
	/// \param hostLoginID The loginID used to create this advertised entry.
	/// \param url The URL passed to AdvertiseSession()
	/// \param userData The user data passed to AdvertiseSession()
	/// \param age In seconds, how long ago this entry was first advertised.
	/// \param address The external address of the machine doing the advertise from the point of view of the AdvertiseServer. If AdvertiseSession() is used on a multihomed machine this external address may not be the same address the advertised game server is bound to as indicated in the URL.
	virtual void CallbackListingsEntry(const int serverIndex,const int hostLoginID,const char *url, const char *userData, const float age, const XPAddress &address) {}

	/// This is called when there are no more listings.
	virtual void CallbackListingsComplete(void) {}

	/// This is called the start when the rules for a server index are updated.
	virtual void CallbackRulesBegin(const int serverIndex) {}

	/// This is called when there is a rule for a server index.
	/// \param serverIndex The server index from CallbackListingsEntry().
	/// \param rule The rule name passed to AddRule().
	/// \param userdata The rule user data passed to AddRule().
	virtual void CallbackRule(const int serverIndex,const char *rule, const char *userData) {}

	/// This is called when there are no more rules for a server index.
	/// \param serverIndex The server index from CallbackListingsEntry().
	virtual void CallbackRulesComplete(const int serverIndex) {}

	/// This is called the start when the players for a server index are updated.
	/// \param serverIndex The server index from CallbackListingsEntry().
	virtual void CallbackPlayersBegin(const int serverIndex) {}

	/// This is called when there is a player for a server index.
	/// \param serverIndex The server index from CallbackListingsEntry().
	/// \param name The name of the player passed to AddPlayer().
	/// \param userData The user data of the player passed to AddPlayer().
	/// \param loginID The loginID of the player passed to AddPlayer().
	virtual void CallbackPlayer(const int serverIndex,const char *name, const char *userData, const int loginID) {}

	/// This is called when there are no more players for a server index.
	/// \param serverIndex The server index from CallbackListingsEntry().
	virtual void CallbackPlayersComplete(const int serverIndex) {}

	/// This is called when the latency to a server index has been calculated.
	/// \param serverIndex The server index from CallbackListingsEntry().
	/// \param latency This is the time taken in seconds for a packet to reach the machine that is advertising the server index. This is half round trip ping time.
	/// \param packetLossPercent The packet loss percentage.
	virtual void CallbackLatencyTo(const int serverIndex,const float latency,const float packetLossPercent) {}

	/// This is called when a user defined message is received.
	/// \param sourceLoginID The source loginID of the packet.
	/// \param data The message data that was sent.
	/// \param length The message data length that was sent.
	virtual void CallbackMessageReceived(const int sourceLoginID,const void *data,const int length) {}

protected:
	// These callbacks from BaseTransportServer are implemented by this class. If they are implemented by the user this implementation must be called.
	void CallbackTidy(void);

	bool CallbackPoll(void);

	bool CallbackParseValidMessage(Transport *transport,DynamicMessageHelper &message,const size_t length);

	XPAddress mServerAddress;

	bool mIsAdvertising;

	std::string mCurrentURL;
	std::string mCurrentUserData;

	bool mDoingListing;

	bool mWantRules;
	bool mDoingRules;

	bool mWantPlayers;
	bool mDoingPlayers;

	XPSocketUrgent *mBroadcast;
//	int mSerial;
	std::string mBroadcastURL;						// Protected by the mutex
	std::string mBroadcastData;						// Protected by the mutex
	std::map<int,XPAddress> mGotReplyFrom;			// Protected by the mutex
	int mBroadcastServerID;
	XPAddress mCurrentReply;
	int mCurrentNOnce;
	SysTimeType mRequestListingTime;
	std::list<std::pair<SysTimeType,int> > mRequestRulesTimes;	// Protected by the mutex
	std::list<std::pair<SysTimeType,int> > mRequestPlayersTimes;	// Protected by the mutex

	struct ServerInfo
	{
		ServerInfo();

		void StartLatencyCalculate(void);

		// If the latency has not been calculated then it returns a value less than 0.0f
		float GetLatency(void);

		float GetPacketLoss(void);

		std::string mURL;
		std::string mData;
		int mLoginID;
		XPAddress mExternalIP;
		bool mWantPing;
		int mPingTries;
		SysTimeType mLastPingSentTime;
		size_t mPacketsLost;
		int mReplies;
		SysTimeType mTotalRoundTripTime;
		bool mPingAcked;
	};

	class PingServer : public ThreadClass , public Thread
	{
	public:
		PingServer();
		~PingServer();

		std::map<int,ServerInfo> mLocalServerInfo;		// Protected by the mutex
		std::multimap<XPAddress,int> mServerIndexByAddress;	// Protected by the mutex
		std::list<int> mServerIDsPingFinished;			// Protected by the mutex
		bool mAnyActivePings;

		int ThreadEntry(void);

	private:
		void ThreadExiting(const int returnCode);

		t_XPSocket *mPingSocket;
		bool mAPingSent;
	};

	PingServer mPingServer;

	bool mServerAddressSet;
};

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
