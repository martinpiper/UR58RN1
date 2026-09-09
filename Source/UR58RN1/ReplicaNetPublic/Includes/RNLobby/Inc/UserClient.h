/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _USERCLIENT_H_
#define _USERCLIENT_H_
#include "RNLobby/Inc/BaseTransportServer.h"
#include "RNXPSockets/Inc/XPSocket.h"
#include <set>
#include <map>

namespace RNReplicaNet
{

namespace RNLobby
{

/// This class is used to communicate with the UserServer class.
class UserInfo
{
public:
	UserInfo();
	virtual ~UserInfo();

	int GetLoginID(void) const;
	std::string GetName(void) const;

	enum
	{
		kAnyLoginID = 0
	};

	enum
	{
		kState_Friend			= (1<<0),
		kState_FriendRequest	= (1<<2),
		kState_Online			= (1<<3),
		kState_GameInvite		= (1<<5),
		kState_Speaking			= (1<<6),
		kState_Ignored			= (1<<7),
		kState_InARoom			= (1<<8),	// MPi: TODO: Implement
		kState_PlayingAGame		= (1<<9)
	};
	/// \return A combination of kState_*
	int GetState(void) const;

	std::string GetGameInvite(void) const;

	std::string GetLastOnlineTime(void) const;

	std::string GetPlayingAGameURL(void) const;

	int mID;
	int mLoginID;
	std::string mName;
	int mState;
	std::string mGameInvite;
	std::string mLastOnlineTime;
	std::string mPlayingAGameURL;
};

/// Maintains a connection to the UserServer.
/// This allows access to user accounts, friends and user persistent data for each title.
class UserClient : public BaseTransportServer
{
public:

	UserClient();
	virtual ~UserClient();

	/// Because this uses a TransportUDP connection it will call XPURL::RegisterDefaultTransports()
	bool Start(void);

	bool Stop(void);

	/// Sets the address and port of the server. The default for the instance of the class is localhost and port UserServer::kDefaultPort or the address resolved by a successful BackendClient::Start().
	/// \param address The new address and port number to use for the server
	void SetServer(const XPAddress &address);

	/// Will try to create an account and then login if it was created.
	/// If the account already exists or the optionalID check finds a duplicate ID then CallbackUserAlreadyExists() is called and the user should try to use LoginUser() instead.
	/// When creating a login the duplicate name check is not case sensitive so that if someone has the name "Fred" then someone else will not be able to create the name "fred".
	/// \param name The name to use for the account.
	/// \param password The password to use for the account.
	/// \param emailAddress The email address to use for the account. The email address can be used to request a password reset using the ASP web lobby interface.
	/// \param The optional ID to use for the account. If the optional ID is not an empty string then it is used to detect for more than one account being created by a person or machine. For example the string could be a hash of the user's credit card information, or the MAC address of the machine.
	bool CreateLoginUser(const char *name,const char *password,const char *emailAddress = "",const char *optionalID = "");

	/// Will try to login an existing account. CallbackUserLogin() will be called if the name and password are correct. CallbackUserLoginFailed() will be called if the name or password were incorrect.
	/// \param name The name of the user. This is not case sensitive. However the original case of the name supplied to CreateLoginUser will be displayed.
	/// \param password The password of the user. This is case sensitive.
	bool LoginUser(const char *name,const char *password);

	bool ChangePassword(const char *password = "");

	bool ChangeEmail(const char *emailAddress = "");


	/// Returns the login ID when the user is logged in or UserInfo::kAnyLoginID when the user is not logged in.
	int GetLoginID(void);

	/// Returns the login name as stored on the server when the user is logged in.
	std::string GetLoginName(void);

	/// Logs out a user.
	bool LogoutUser(void);

	/// Deletes a user that is currently logged in.
	bool DeleteUser(void);

	/// Sends a message from a logged in user to a login ID.
	bool SendMessageTo(const int loginID,const char *message = "");

	/// Request to add someone as a friend.
	bool AddFriend(const int friendLoginID);

	/// Authorise a friend request from a UserInfo that has the bit kState_FriendRequest set in mState.
	bool AuthoriseFriendRequest(const int friendLoginID);

	/// Deny a friend request from a UserInfo that has the bit kState_FriendRequest set in mState.
	bool DenyFriendRequest(const int friendLoginID);

	/// Remove a user from the friend list. The user info changes will stop tracking.
	bool RemoveFriend(const int friendLoginID);

	/// Call this to clear the state for a received message.
	void AnswerMessage(const int fromLoginID = UserInfo::kAnyLoginID);

	enum Reason
	{
		kReason_ClientForcedLogout = 0,
		kReason_ServerForcedLogout,
		kReason_ChatRoomFull,
		kReason_ChatRoomUserLeft,
		kReason_ChatRoomClosed,
		kReason_ChatRoomKickedByAdmin,
		kReason_ChatRoomWrongPassword,
		kReason_ChatRoomDoesNotExist,
		kReason_ChatRoomBanned,
		kReason_IncorrectLoginDetails,
		kReason_AccountBlocked
	};

	enum
	{
		kState_Online			= (1<<0),
		kState_FriendRequest	= (1<<1),
		kState_GameInvite		= (1<<2),
		kState_Message			= (1<<3)
	};

	/// Returns a state for this instance that can be displayed as an icon on the screen.
	/// \return A combination of kState_*
	int GetState(void);

	/// Sends a game invite to a friend. Only friends can receive game invites from a user.
	bool SendGameInvite(const int friendLoginID,const char *invite = "");

	/// Cancel a game invite to a friend.
	bool CancelGameInvite(const int friendLoginID);

	/// Call this to clear the state for a received game invite from a specific friend or by default all game invites.
	bool AnswerGameInvite(const int friendLoginID = UserInfo::kAnyLoginID);

	enum VariableAccess
	{
		kPrivateRead					= 0,		// Values set by the statistics server.
		kPrivateReadWrite				= 1,		// Values set by the owner or the statistics server.
		kPublicReadPrivateReadWrite		= 2,		// Values set by the owner or the statistics server.
		kPublicReadPrivateRead			= 3			// Values set by the statistics server.
	};

	/// Only allow write access to our own variable space. Setting a null value will erase that value if there is access to do so.
	/// The name and value string is limited to 128 chars. (StatServerPrivate::kMaximumPairValueSize)
	bool SetVariable(const int section,const char *name,const char *value = 0,const VariableAccess access = kPrivateReadWrite);

	/// Invokes CallbackGotVariable() when the variable is retrieved from the server.
	/// The name string is limited to 128 chars. (StatServerPrivate::kMaximumPairValueSize)
	bool GetVariable(const int section,const char *name,const VariableAccess access = kPrivateReadWrite);

	/// Invokes CallbackGotVariable() when the variable is retrieved from the server.
	/// The name string is limited to 128 chars. (StatServerPrivate::kMaximumPairValueSize)
	bool GetVariable(const int loginID,const int section,const char *name,const VariableAccess access = kPublicReadPrivateRead);

	/// Requests UserInfo from a loginID. CallbackUserInfo() is triggered when a request succeeds and CallbackUserInfoFailed() on failure.  This is useful for maintaining a list of recent players the user has encountered.
	/// \param loginID The loginID to request information for.
	/// \param useCache When true if possible use the cached information.
	/// \param trackChanges When true this asks the server to send any changes of the loginID to this user. The server will expire old requests when the user logs out to avoid too much server load. RemoveUserInfo() can be used to forget tracking user changes for a loginID.
	bool AddUserInfo(const int loginID,const bool useCache = true,const bool trackChanges = false);

	/// Stops tracking a loginID for changes that have been requested from using AddUserInfo(). If the user is a friend this will not remove the user from the friend list.
	bool RemoveUserInfo(const int loginID);

	/// Using the currently known list of users and friends this will try to instantly return the most up to date UserInfo.
	/// \param loginID The loginID to search for.
	/// \param result If not null this is a pointer to the resultant UserInfo.
	/// \return Returns true if the loginID was found. If false is returned the contents of result is unchanged.
	bool GetUserInfo(const int loginID,UserInfo *result);

	/// Create a new chat room and if successful makes this user join the room as the room admin. The user will be notified of the chatRoomID by CallbackChatRoomCreated()
	bool CreateChatRoom(const char *password = 0);

	/// Leave a chat room. If the user created the room and the room has no admins left then the room will be closed. All users in the room will be receive CallbackChatRoomLeft() with a reason of kReason_ChatRoomClosed.
	bool LeaveChatRoom(void);

	/// Attempts to join a room. CallbackChatRoomEntered() will be called if successful and CallbackChatRoomEnterFailed() will be called if not successful.
	/// Super admins can enter rooms without needing to know the password.
	bool EnterChatRoom(const int chatRoomID,const char *password = 0);

	/// Returns the current chatRoomID.
	int GetCurrentRoom(void);

	/// Only chat room admins or super admins can use this function.
	/// \param text The text string is limited to 384 chars. (UserServerPrivate::kMaximumMessageSize)
	/// \param flags User defined flags that will be sent to all recipients.
	bool ChatRoomAdminMessage(const char *text,const unsigned int flags = 0);

	/// Say some text that is publicly viewable in the current room.
	/// \param text The text string is limited to 384 chars. (UserServerPrivate::kMaximumMessageSize)
	/// \param flags User defined flags that will be sent to all recipients.
	bool ChatRoomSay(const char *text,const unsigned int flags = 0);

	/// Whisper a message to someone in the chat room.
	/// \param text The text string is limited to 384 chars. (UserServerPrivate::kMaximumMessageSize)
	/// \param flags User defined flags that will be sent to all recipients.
	bool ChatRoomWhisper(const int loginID,const char *text,const unsigned int flags = 0);

	/// Kicks and optionally bans the user loginID from the currently joined chat room, until the room is destroyed and then created again.
	/// Only chat room admins or super admins can use this function.
	/// \param reason The text string is limited to 384 chars. (UserServerPrivate::kMaximumMessageSize)
	bool ChatRoomKickUser(const int loginID,const bool ban = false,const char *reason = "");

	/// Kicks globally across all rooms and optionally bans the user loginID account. Only super admins have the ability for this function to succeed.
	/// If the user is banned the user will not be able to login again until the block is lifted in the database.
	/// \param reason The text string is limited to 384 chars. (UserServerPrivate::kMaximumMessageSize)
	bool GlobalKickUser(const int loginID,const bool ban = false,const char *reason = "");

	/// If a user account has been globally banned this will restore the account to active service.
	bool GlobalRestoreUser(const int loginID);

	/// If the user is a room admin this will return true.
	bool IsRoomAdmin(void);

	/// If the user is a super admin this will return true.
	bool IsSuperAdmin(void);

	/// Flags if this user is playing a game. This causes the UserInfo friend icons to update.
	/// \param playing Use true to indicate the user is playing a game, the optional url will be used. When playing is false the optional url will not be used.
	/// \param url An optional url to indicate what game the user is playing. The text string is limited to 384 chars. (UserServerPrivate::kMaximumMessageSize)
	bool PlayingAGame(const bool playing = false,const char *url = "");

	/// Gets the state set by PlayingAGame()
	bool GetPlayingAGame(void);
	/// Gets the url state set by PlayingAGame()
	std::string GetPlayingAGameURL(void);

	/// Gets the last time this user account logged in.
	std::string GetLastOnlineTime(void);

	/// Requests a user nonce from the server that can be used to securely identify this user to any other RNLobby supported service.
	/// When the user nonce is ready CallbackGotUserNonce() is called. The user nonce will be valid for a number of requested seconds of time. The user nonce can only be used once before it becomes invalid.
	bool RequestUserNonce(const int seconds);

	/// Requests names that are like the supplied parameter. This causes CallbackNamesLikeBegin,CallbackNamesLike and CallbackNamesLikeEnd to be called once the server replies.
	/// By default active accounts are searched. Only super admins are able to search banned accounts.
	bool RequestNamesLike(const char *match = "",const bool banned = false);

	int GetPendingGetVariableRequests(void);

protected:
	/// User extensible callbacks.

	/// A virtual callback that is triggered when the UserClient connects the the UserServer.
	virtual void CallbackConnectionEstablished(Transport *transport) {}

	/// A virtual callback that is triggered when the user successfully logs in.
	virtual void CallbackUserLogin(const int loginID) {}

	/// A virtual callback that is triggered when the user login attempt fails.
	virtual void CallbackUserLoginFailed(const Reason reason) {}

	/// A virtual callback that is triggered when the user logs out.
	virtual void CallbackUserLogout(const Reason reason) {}

	/// A virtual callback that is triggered when the user account already exists.
	virtual void CallbackUserAlreadyExists(const int loginID) {}

	/// A virtual callback that is triggered when the user is logged out due to the same account being logged in on another instance.
	virtual void CallbackUserLoginFromOtherClient(void) {}

	/// A virtual callback that is triggered when the user receives message from another user.
	/// \param message The pointer to the message will be freed as soon as this callback returns so do not store the pointer, store the contents of the pointer instead.
	virtual void CallbackMessageReceived(const int fromLoginID,const char *message) {}

	/// A virtual callback that is triggered when the user deletes a friend.
	virtual void CallbackFriendDeleted(const int loginID) {}

	/// A virtual callback that is triggered when a friend UserInfo is added. A UserInfo can be added when UserInfo::GetState() has kState_FriendRequest set for a pending friend request.
	virtual void CallbackFriendAdded(const UserInfo &user) {}

	/// A virtual callback that is triggered when a UserInfo is changed.
	virtual void CallbackFriendUpdated(const UserInfo &user) {}

	/// A virtual callback that is triggered when the state changes for this instance.
	virtual void CallbackStateChange(const int previousState,const int newState) {}

	/// A virtual callback that is triggered when GetVariable() has read a variable from the server.
	virtual void CallbackGotVariable(const int loginID,const int section,const char *name,const char *value,const VariableAccess access) {}

	/// A virtual callback that is triggered when GetVariable() has failed to read a variable from the server.
	virtual void CallbackVariableNotFound(const int loginID,const int section,const char *name,const VariableAccess access) {}

	/// A virtual callback that is triggered when the user deletes a user info request.
	virtual void CallbackUserInfoDeleted(const int loginID) {}

	/// A virtual callback that is triggered when a AddUserInfo request has succeeded.
	virtual void CallbackUserInfoAdded(const UserInfo &user) {}

	/// A virtual callback that is triggered when a UserInfo is changed.
	virtual void CallbackUserInfoUpdated(const UserInfo &user) {}

	virtual void CallbackChatRoomCreated(const int chatRoomID) {}

	virtual void CallbackChatRoomEntered(const int chatRoomID) {}

	virtual void CallbackChatRoomEnterFailed(const int chatRoomID,const Reason reason) {}

	virtual void CallbackChatRoomLeft(const Reason reason) {}

	// When entering a room the first callback will be CallbackChatRoomEntered() followed by any number of CallbackChatRoomUserEntered().
	// The last callback will be CallbackChatRoomUserEntered() with the current user ID to signify the end of the list.
	virtual void CallbackChatRoomUserEntered(const UserInfo &user) {}

	virtual void CallbackChatRoomUserLeft(const RNReplicaNet::RNLobby::UserInfo &user,const Reason reason) {}

	virtual void CallbackChatRoomMessage(const char *text,const UserInfo &user,const bool whispered,const bool isAdminMessage,const unsigned int flags) {}

	virtual void CallbackChatRoomKicked(const char *reason,bool banned,const UserInfo &user) {}

	virtual void CallbackGlobalKicked(const char *reason,bool banned,const UserInfo &user) {}

	virtual void CallbackPasswordChanged(void) {}

	virtual void CallbackCannotChangePassword(void) {}

	virtual void CallbackEmailChanged(void) {}

	virtual void CallbackCannotChangeEmail(void) {}

	virtual void CallbackGotUserNonce(const char *userNonce,const int seconds) {}

	/// A virtual callback that is triggered when the StatServer updates any variables for this user.
	virtual void CallbackNewStatsAvailable(const int lastUpdatedTitleID) {}

	/// A virtual callback that is triggered when RequestNamesLike is used.
	/// \param match The corresponding match parameter that was used for RequestNamesLike().
	/// \param match The corresponding banned parameter that was used for RequestNamesLike().
	virtual void CallbackNamesLikeBegin(const char *match,const bool banned) {}

	/// A virtual callback that is triggered when RequestNamesLike is used.
	virtual void CallbackNamesLike(const UserInfo &user) {}

	/// A virtual callback that is triggered when RequestNamesLike is used.
	virtual void CallbackNamesLikeEnd(void) {}

protected:
	// These callbacks from BaseTransportServer are implemented by this class. If they are implemented by the user this implementation must be called.
	void CallbackTidy(void);

	bool CallbackPoll(void);

	bool CallbackParseValidMessage(Transport *transport,DynamicMessageHelper &message,const size_t length);

private:

	// Called after a logout or other similar event
	void InitInternalState(void);

	XPAddress mServerAddress;

	bool mLoginTried;
	int mMyLoginID;

	std::set<int> mPendingFriendIDs;

	MutexClass mInternalLists;

	std::map<int,UserInfo> mUsers;
	int mPreviousState;
	int mCurrentState;
	bool mMessagePending;
	bool mGameInvitePending;

	void DoStateChangeCalc(void);
	int mInCurrentRoom;
	bool mRoomMode;
	bool mIsRoomAdmin;
	bool mIsGlobalAdmin;
	std::string mMyLoginName;
	bool mServerAddressSet;
	bool mLastPlayingAGame;
	std::string mLastOnlineTime;
	std::string mLastPlayingAGameURL;

	int mPendingGetVariables;
};

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
