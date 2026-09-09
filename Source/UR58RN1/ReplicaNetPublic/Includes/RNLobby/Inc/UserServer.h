/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _USERSERVER_H_
#define _USERSERVER_H_
#include "RNLobby/Inc/RNLobby.h"
#include "RNLobby/Inc/BaseTransportServer.h"
#include "RNLobby/Inc/UserClient.h"
#include "RNXPSockets/Inc/XPSocket.h"
#include "RNODBCSQL/Inc/ODBCSQL.h"
#include "RNODBCSQL/Inc/Prepare.h"
#include "RNLog/Inc/Log.h"
#include <map>
#include <set>

namespace RNReplicaNet
{

namespace RNLobby
{

/// This class uses the BaseTransportServer class to implement:
/// - User account create/authenticate.
/// - Persistent data storage for each user.
/// - Friends list.
/// - Game invites.
/// - Private messages.
/// - Chat rooms.
/// User information is stored in the following database tables:
/// - Users
/// - UserRelationship
/// - OfflineMessages
/// - Variables
/// - SuperRoomUsers
/// - AccountBlocked
/// - UserNonces
/// - UserNetworkAddressHistory
class UserServer : public BaseTransportServer
{
public:
	UserServer(const char *lobbyDSN = MRNLobbyDefaultDSN);
	virtual ~UserServer();

	enum
	{
		kDefaultPort = 4009
	};

	/// Because this uses a TransportUDP connection it will call XPURL::RegisterDefaultTransports()
	bool Start(const XPAddress &address = XPAddress(kDefaultPort));

	/// Returns the total number of users.
	size_t GetNumTotal(void);

	/// Verifies the input userNonce and returns the corresponding loginID or UserInfo::kAnyLoginID if the user nonce couldn't be verified.
	static int VerifyUserNonce(RNReplicaNet::RNODBCSQL::ODBCSQL &sql, const std::string &userNonce);
	static bool GetVariable(RNReplicaNet::RNODBCSQL::ODBCSQL &sql, const int titleID, const int loginID, const int section, const UserClient::VariableAccess access, const std::string &name, std::string &value);
	static bool GetVariable(RNReplicaNet::RNODBCSQL::ODBCSQL &sql, const int titleID, const int loginID, const int section, const UserClient::VariableAccess access, const std::string &name, int &value);
	static bool GetVariable(RNReplicaNet::RNODBCSQL::ODBCSQL &sql, const int titleID, const int loginID, const int section, const UserClient::VariableAccess access, const std::string &name, float &value);
	static bool SetVariable(RNReplicaNet::RNODBCSQL::ODBCSQL &sql, const int titleID, const int loginID, const int section, const UserClient::VariableAccess access, const std::string &name, const std::string &value);
	static bool SetVariable(RNReplicaNet::RNODBCSQL::ODBCSQL &sql, const int titleID, const int loginID, const int section, const UserClient::VariableAccess access, const std::string &name, const int value);
	static bool SetVariable(RNReplicaNet::RNODBCSQL::ODBCSQL &sql, const int titleID, const int loginID, const int section, const UserClient::VariableAccess access, const std::string &name, const float value);
	static bool DeleteVariable(RNReplicaNet::RNODBCSQL::ODBCSQL &sql, const int titleID, const int loginID, const int section, const UserClient::VariableAccess access, const std::string &name);

private:
	RNReplicaNet::RNLog::Log mLog;

	RNODBCSQL::ODBCSQL mWrite;
	RNODBCSQL::ODBCSQL mRead;
	struct SessionData
	{
		SessionData();
		int mDBIndex;	// This is also actually the loginID
		std::string mCachedName;	// This is the last known cached name for this connection
		int mClientNonce;
		int mInRoomID;
		bool mIsSuperRoomUser;
		int mLoggedInAsTitleID;
	};
	std::map<Transport *,SessionData> mMapSessionDataByTransport;
	std::map<int,Transport *> mMapTransportByIndex;	// As above, the index is the loginID
	SysTimeType mLastRelationshipUpdate;

	// By default newly created rooms are mCloseWhenNoAdmin and mCloseWhenEmpty
	struct RoomData
	{
		RoomData();
		// Also the logout (disconnection or otherwise) needs to remove the player from the room they might have been in.
		std::string mPasswordHash;
		std::set<int> mLoginIDs;
		std::set<int> mAdminLoginIDs;	// For future expansion we allow more than one room admin
		std::set<int> mBannedLoginIDs;	// When joining the room the loginID is checked against this
		bool mCloseWhenNoAdmin;
		bool mCloseWhenEmpty;
	};
	int mRoomID;	// A counter for user defined roomIDs
	std::map<int,RoomData> mRoomByID;

	bool CallbackPoll(void);

	bool CallbackParseValidMessage(Transport *transport,DynamicMessageHelper &message,const size_t length);

	/// Returns the loginID
	int InsertEntry(const int titleID,const char *name,const char *passwordHash,const char *emailAddress, const char *optionalID);

	bool RemoveEntry(const int loginID);

	void CallbackAccepted(Transport *transport);

	void CallbackDisconnected(Transport *transport);

	/// \return True if the DBID was connected to the transport
	bool RemoveTransportFromDBID(const int dbID,Transport *transport = 0);

	void SetLoginIDAsUpdated(const int loginID);

	void SetLoginIDAsLoggedOut(const int loginID);

	bool RemoveSessionDataFromRoom(SessionData &sessionData);

	bool IsSuperRoomUser(const int loginID);

	void AddUserAddressLog(const int loginID, const std::string text);


	RNODBCSQL::Prepare mPreparedUserRelationshipByID;
	RNODBCSQL::Prepare mPreparedUsersByID;
	RNODBCSQL::Prepare mPreparedDeleteUserRelationshipByID;
	RNODBCSQL::Prepare mPreparedUsers2ByID;
	RNODBCSQL::Prepare mPreparedUsersByName;
	RNODBCSQL::Prepare mPreparedUsersByNameMachineUID;
	RNODBCSQL::Prepare mPreparedUsers2ByName;
	RNODBCSQL::Prepare mPreparedCountAccountBlockedByID;
	RNODBCSQL::Prepare mPreparedUpdateUsersByID;
	RNODBCSQL::Prepare mPreparedUpdateUserRelationshipByID;
	RNODBCSQL::Prepare mPreparedOfflineMessagesByID;
	RNODBCSQL::Prepare mPreparedDeleteOfflineMessagesByID;
	RNODBCSQL::Prepare mPreparedUsers3ByID;
	RNODBCSQL::Prepare mPreparedInsertOfflineMessages;
	RNODBCSQL::Prepare mPreparedUsers4ByID;
	RNODBCSQL::Prepare mPreparedUserRelationshipByIDOtherID;
	RNODBCSQL::Prepare mPreparedUserRelationship2ByIDOtherID;
	RNODBCSQL::Prepare mPreparedInsertUserRelationship;
	RNODBCSQL::Prepare mPreparedUpdateUserRelationship2ByID;
	RNODBCSQL::Prepare mPreparedInsertUserRelationship2;
	RNODBCSQL::Prepare mPreparedUpdateUserRelationship3ByID;
	RNODBCSQL::Prepare mPreparedInsertUserRelationship3;
	RNODBCSQL::Prepare mPreparedUpdateUserRelationship4ByID;
	RNODBCSQL::Prepare mPreparedUserRelationship3ByIDOtherID;
	RNODBCSQL::Prepare mPreparedInsertUserRelationship4;
	RNODBCSQL::Prepare mPreparedUpdateUserRelationship5ByID;
	RNODBCSQL::Prepare mPreparedUpdateUserRelationship6ByID;
	RNODBCSQL::Prepare mPreparedUpdateUserRelationship7ByID;
	RNODBCSQL::Prepare mPreparedUpdateUserRelationship8ByID;
	RNODBCSQL::Prepare mPreparedUpdateUserRelationship9ByID;
	RNODBCSQL::Prepare mPreparedUpdateUserRelationship10ByID;
	RNODBCSQL::Prepare mPreparedInsertAccountBlocked;
	RNODBCSQL::Prepare mPreparedCannotChangePasswordByID;
	RNODBCSQL::Prepare mPreparedSetPasswordHashByID;
	RNODBCSQL::Prepare mPreparedCannotChangeEmailByID;
	RNODBCSQL::Prepare mPreparedSetEmailAddressByID;
	RNODBCSQL::Prepare mPreparedSetPlayingAGameByID;
	RNODBCSQL::Prepare mPreparedInsertUserNonces;
	RNODBCSQL::Prepare mPreparedUsersAccountBlockedByIDName;
	RNODBCSQL::Prepare mPreparedUsersByIDName;
	RNODBCSQL::Prepare mPreparedDeleteAccountBlockedByID;
	RNODBCSQL::Prepare mPreparedInsertUsers;
	RNODBCSQL::Prepare mPreparedDeleteUsersByID;
	RNODBCSQL::Prepare mPreparedSetUserRelationshipByOtherID;
	RNODBCSQL::Prepare mPreparedDeleteUserRelationship2ByID;
	RNODBCSQL::Prepare mPreparedDeleteOfflineMessages2ByID;
	RNODBCSQL::Prepare mPreparedDeleteVariablesByID;
	RNODBCSQL::Prepare mPreparedUpdateUserRelationshipByIDIsIgnored;
	RNODBCSQL::Prepare mPreparedUpdateUsers2ByID;
	RNODBCSQL::Prepare mPreparedUpdateUserRelationship11ByID;
	RNODBCSQL::Prepare mPreparedDeleteUserRelationshipByIDIsFriendIsIgnored;
	RNODBCSQL::Prepare mPreparedCountSuperRoomUsersByID;
	RNODBCSQL::Prepare mPreparedInsertUserNetworkAddressHistory;
	RNODBCSQL::Prepare mPreparedCountUserNetworkAddressHistoryByID;
	RNODBCSQL::Prepare mPreparedDeleteUserNetworkAddressHistoryByID;
	//RNODBCSQL::Prepare mPreparedUserNoncesByUserNonce;
};

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
