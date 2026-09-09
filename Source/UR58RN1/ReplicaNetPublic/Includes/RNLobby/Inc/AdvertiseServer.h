/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _ADVERTISESERVER_H_
#define _ADVERTISESERVER_H_
#include "RNPlatform/Inc/ThreadClass.h"
#include "RNLobby/Inc/RNLobby.h"
#include "RNLobby/Inc/BaseTransportServer.h"
#include "RNXPSockets/Inc/XPSocket.h"
#include "RNODBCSQL/Inc/ODBCSQL.h"
#include "RNODBCSQL/Inc/Prepare.h"
#include "RNLog/Inc/Log.h"
#include <map>

namespace RNReplicaNet
{

namespace RNLobby
{

/// Maintains a list of advertised sessions.
/// Advertised session information is stored in the following database tables:
/// - GameListings
/// - ServerRules
/// - ServerPlayers
class AdvertiseServer : public BaseTransportServer
{
public:
	AdvertiseServer(const char *lobbyDSN = MRNLobbyDefaultDSN);
	virtual ~AdvertiseServer();

	enum
	{
		kDefaultPort = 4008
	};

	/// Because this uses a TransportUDP connection it will call XPURL::RegisterDefaultTransports()
	bool Start(const XPAddress &address = XPAddress(kDefaultPort));

	/// Returns the number of active advertised sessions.
	size_t GetNumActive(void) const;

	/// Returns the total number of advertised sessions.
	size_t GetNumTotal(void) const;

private:
	RNReplicaNet::RNLog::Log mLog;

	RNODBCSQL::ODBCSQL mWrite;
	RNODBCSQL::ODBCSQL mRead;
	struct RuleInfo
	{
		RuleInfo(const std::string &rule,const std::string &data) : mRule(rule),mData(data) {}
		std::string mRule;
		std::string mData;
	};
	struct PlayerInfo
	{
		PlayerInfo(const std::string &name,const std::string &data,int loginID) : mName(name),mData(data),mLoginID(loginID){}
		std::string mName;
		std::string mData;
		int mLoginID;
	};
	struct SessionData
	{
		SessionData();
		int mDBIndex;
		XPAddress mExternalIP;
		int mConnectionNOnce;

		// Temporary storage while an update is being received until the end packet then this gets stored into the DB
		std::list<RuleInfo> mRules;
		std::list<PlayerInfo> mPlayers;
	};
	std::map<Transport *,SessionData> mMapSessionDataByTransport;
	std::map<int,Transport *> mMapByIndex;

	size_t mNumActive;
	size_t mNumTotal;

	RNODBCSQL::Prepare mPreparedGamelistingsNoFilter;
	RNODBCSQL::Prepare mPreparedDeleteServerRules;
	RNODBCSQL::Prepare mPreparedInsertServerRules;
	RNODBCSQL::Prepare mPreparedGameListingsByID;
	RNODBCSQL::Prepare mPreparedServerRules;
	RNODBCSQL::Prepare mPreparedDeleteServerPlayers;
	RNODBCSQL::Prepare mPreparedInsertServerPlayers;
	RNODBCSQL::Prepare mPreparedServerPlayers;
	RNODBCSQL::Prepare mPreparedInsertGameListingsUrlUser;
	RNODBCSQL::Prepare mPreparedInsertGameListingsUrl;
	RNODBCSQL::Prepare mPreparedInsertGameListingsUser;
	RNODBCSQL::Prepare mPreparedInsertGameListings;
	RNODBCSQL::Prepare mPreparedUpdateGameListingsUrlUser;
	RNODBCSQL::Prepare mPreparedUpdateGameListingsUrl;
	RNODBCSQL::Prepare mPreparedUpdateGameListingsUser;
	RNODBCSQL::Prepare mPreparedDeleteGameListings;

	bool CallbackPoll(void);

	bool CallbackParseValidMessage(Transport *transport,DynamicMessageHelper &message,const size_t length);

	int InsertEntry(const int titleID,const int loginID,const XPAddress &externalIP,const char *url=0,const char *userBlock = 0);

	bool UpdateEntry(const int index,const char *url = 0,const char *userBlock = 0);

	bool RemoveEntry(const int index);

	void CallbackAccepted(Transport *transport);

	void CallbackDisconnected(Transport *transport);
};

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
