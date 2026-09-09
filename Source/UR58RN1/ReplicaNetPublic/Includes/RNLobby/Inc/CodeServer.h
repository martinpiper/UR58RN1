/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _CODESERVER_H_
#define _CODESERVER_H_
#include "RNPlatform/Inc/SysTime.h"
#include "RNPlatform/Inc/RegistryManagerMap.h"
#include "RNPlatform/Inc/Encryption.h"
#include "RNPlatform/Inc/ThreadClass.h"
#include "RNPlatform/Inc/Thread.h"
#include "RNXPSockets/Inc/XPSockets.h"
#include <string.h>

namespace RNReplicaNet
{

namespace RNLobby
{

/// This code server uses a UDP socket to respond to queries for the validity of codes. The codes are stored in a file with one code per line.
/// Use the RNLobby::CodeClient class to make queries to this server.
class CodeServer : private ThreadClass , private Thread
{
public:
	class CodeEntry
	{
	public:
		struct ThisLess
		{
			bool operator()(const std::string &s1, const std::string &s2) const;
		};
		CodeEntry();

		// ctor variables
		XPAddress mLastAddress;
		SysTimeType mLastTime;
		int mUserSuppliedReply;
		int mLastNOnce1,mLastNOnce2;

		// ctor body variables
		std::string mTheCode;
	};

	enum Reply
	{
		kReply_ValidKey = 0,
		kReply_UnspecifiedError,
		kReply_InvalidKey,
		kReply_KeyInUse,
		kReply_Timeout,
		kReply_CorruptReply
	};
	enum
	{
		kDefaultPort = 4003
	};


	CodeServer();
	virtual ~CodeServer();

	/// Reads codes from a file and stores the codes in a database in memory. Each code must be in a separate line.
	/// This function may be used when the code server has been started. Each call to this function will add codes to the database held in memory. Each code must be no longer than 255 characters.
	/// \param filename The filename to read.
	/// \param duplicateCodeCheck If this is true then each code is checked to make sure it is not a duplicate code already stored in the database. The default is false, which disables the duplicate code check and makes adding codes slightly faster.
	/// \return Success returns true, failure returns false.
	bool ReadCodeFile(const char *filename, const bool duplicateCodeCheck = false);

	/// Adds a code to the database in memory. Each code must be no longer than 255 characters.
	/// This function may be used when the code server has been started.
	/// \param code The code to add.
	/// \param duplicateCodeCheck If this is true then the code is checked to make sure it is not a duplicate code already stored in the database. The default is false, which disables the duplicate code check and makes adding codes slightly faster.
	/// \param userSuppliedReply This is returned from CodeClient::GetUserSuppliedReply(). This is useful if the key server wishes to communicate that the key has billing problems, been suspended etc. To update userReply for an existing key you may enable duplicateCodeCheck or use RemoveCode() and AddCode().
	/// \return Success returns true, failure returns false.
	bool AddCode(const char *code, const bool duplicateCodeCheck = false,const int userSuppliedReply = 0);

	/// Removes codes from the database in memory.
	/// This function may be used when the code server has been started.
	/// \param code If the code is supplied the code is removed from the database in memory. If there is no code supplied then all codes are removed from the database in memory.
	/// \return Success returns true, failure returns false.
	bool RemoveCode(const char *code = 0);

	/// Starts the code server thread and processes code queries. If not already started the network hardware is started.
	/// \return Success returns true, failure returns false.
	bool Start(const XPAddress &address = XPAddress(kDefaultPort));

	/// Stops the code server thread and stops processing code queries.
	/// \param hardware If true this stops the network hardware. The default is false.
	/// \return Success returns true, failure returns false.
	bool Stop(bool hardware = false);

	/// Sets the encryption key to be used by all queries for this server. The same key must be used with any RNLobby::CodeClient querying this server.
	/// The default encryption key is the same for RNLobby::CodeServer and RNLobby::CodeClient.
	/// \param data The data to use as a seed for the key. This can be NULL to use the default key.
	/// \param length The length of the data to use for creating the key.
	void SetEncryptionKey(const void *data,const int length);

	/// This sets the time period that a code will be flagged as being used and kReply_KeyInUse will be returned from CodeClient::GetReply().
	/// After this time period has elapsed a request to validate the same code will be allowed.
	/// \param time The time in seconds. The default value is (60.0f * 30.0f) which is 30 minutes.
	void SetCodeTimeout(const float time = (60.0f * 30.0f));

	/// Gets the value configured by SetCodeTimeout()
	/// \return The value configured by SetCodeTimeout()
	float GetCodeTimeout(void) const;

	/// This callback may be used to alter data after it has been received and before the data is processed.
	/// \param data The pointer to the data.
	/// \param length The length of the data. If the output length changes then this variable can be used to pass the value back by reference. A returned length of zero discards the packet.
	/// \param maxLength The maximum length of the data buffer. Parameter length must not be greater than this value.
	/// \param address The network address for this data packet. The contents of the address may be changed if required.
	virtual void CallbackDataReceived(void *data,int &length,const int maxLength,XPAddress &address) {}

	/// This callback may be used to ascertain the version of the request made.
	/// \param version The version of the request.
	virtual void CallbackVersion(const int version) {}

	/// This callback may be used to alter the actual key queried before it is matched with the database.
	/// \param keyData The pointer to a null terminated char string.
	/// \param maxLength The maximum length of the data buffer, including the null termination character.
	/// \param nOnce1 The first nonce.
	/// \param nOnce2 The second nonce.
	/// \param updateDatabase This defaults to true, meaning that the query will update the database with the results of the search. If this is changed to be false the query will only be read only.
	/// A read only query does not require correct nonce values to be able to return kReply_ValidKey.
	virtual void CallbackKeyData(char *keyData,const int maxLength,int &nOnce1,int &nOnce2,bool &updateDatabase) {}

	/// This callback may be used to alter the CodeServer::Reply.
	/// \param reply The reply from the key query.
	/// \param userSuppliedReply The user supplied reply.
	/// \param nOnce1 The first reply nonce.
	/// \param nOnce2 The second reply nonce.
	virtual void CallbackQueryReply(Reply &reply,int &userSuppliedReply,int &nOnce1,int &nOnce2) {}

	/// This callback may be used to alter any data in a reply, just before it gets sent.
	/// \param data The pointer to the data.
	/// \param length The length of the data. If the output length changes then this variable can be used to pass the value back by reference. A returned length of zero discards the packet.
	/// \param maxLength The maximum length of the data buffer. Parameter length must not be greater than this value.
	/// \param address The network address for this data packet. The contents of the address may be changed if required.
	virtual void CallbackDataSend(void *data,int &length,const int maxLength,XPAddress &address) {}

	/// This callback reports how long an query and reply took starting from when packet data was received until the packet data is sent.
	/// \param seconds The time in seconds.
	virtual void CallbackQueryReplyTime(const float seconds) {}

	/// Returns the total number of codes queries.
	int GetNumRequests(void) const;

	/// Returns the total number of codes queries that were returned as kReply_InvalidKey.
	int GetNumFailed(void) const;

private:
	int ThreadEntry(void);

protected:
	RegistryManagerMap<std::string,CodeEntry,CodeEntry::ThisLess> mCodes;
	MutexClass mCodesMutex;
private:
	XPSocketUrgent *mListen;

	Encryption::Key mCryptoKey;
	float mCodeTimeout;
	int mNumRequests;
	int mNumFailed;
};

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
