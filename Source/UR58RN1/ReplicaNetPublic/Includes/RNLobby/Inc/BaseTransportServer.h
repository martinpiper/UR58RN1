/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _BASETRANSPORTSERVER_H_
#define _BASETRANSPORTSERVER_H_
#include <map>
#include "RNPlatform/Inc/SysTime.h"
#include "RNPlatform/Inc/Encryption.h"
#include "RNPlatform/Inc/ThreadClass.h"
#include "RNPlatform/Inc/Thread.h"
#include "RNXPURL/Inc/Transport.h"
#include "RNPlatform/Inc/MessageHelper.h"
#include "RNLobby/Inc/CommonServer.h"

namespace RNReplicaNet
{

namespace RNLobby
{

/// A server base class that handles encrypted messages to and from a transport.
class BaseTransportServer : private CommonServer , public ThreadClass , public Thread
{
public:
	BaseTransportServer();
	virtual ~BaseTransportServer();

	/// Sets the encryption key to be used by all queries for this server. The same key must be used with any other RNLobby::BaseTransportServer querying this server.
	/// \param data The data to use as a seed for the key. This can be NULL to use the default key.
	/// \param length The length of the data to use for creating the key.
	void SetEncryptionKey(const void *data,const int length);

	/// Starts the thread. If this function is overridden then this base implementation must also be called.
	/// \param transport The transport to poll for this class.
	/// \param allowDelete When true this class is responsible for deleting the socket.
	/// \param isListen When true this class will listen for connections on the
	/// \return Success returns true, failure returns false.
	virtual bool StartWithTransport(Transport *transport,const bool allowDelete = true,const bool isListen = true);

	/// Stops the thread. If this function is overridden then this base implementation must also be called.
	/// This will not generate CallbackDisconnected() events for any existing connections.
	/// \return Success returns true, failure returns false.
	virtual bool Stop(void);

	static SysTimeType FloatTime(void);

	/// Returns the number of active connections.
	size_t GetNumConnections(void) const;

	/// Returns the total number of connections.
	size_t GetNumTotalSessions(void) const;

	/// This will put the connection into a list pending disconnection after a timeout period.
	bool SetAsDisconnecting(Transport *transport);

protected:
	struct Entry
	{
		Entry() : mLastPacketTime(0.0f) , mLastPacketsSent(0) , mWaitingForDisconnect(-1.0f) , mError(false) ,
			mClientACKConnectionNonce(false) , mCachedConnectionNonce(0) {}
		SysTimeType mLastPacketTime;
		int mLastPacketsSent;
		SysTimeType mWaitingForDisconnect;
		bool mError;
		bool mClientACKConnectionNonce;
		int mCachedConnectionNonce;
	};

	/// Called from the scope of the thread but without the class mutex being locked.
	/// If a MessageHelper::Exception() is caught by this server class the connection will be terminated.
	virtual bool CallbackParseValidMessage(Transport *transport,DynamicMessageHelper &message,const size_t length) { return true; }

	/// The class mutex is locked during the context of this function.
	virtual bool PackageAndSendData(Transport *transport,const DynamicMessageHelper &message);

	/// The class mutex is locked during the context of this function.
	/// The default operation is to decrypt the packet and then set result.SetGuardSize() with the size of the data and then call CallbackParseValidMessage(). This will cause a MessageHepler::Exception() exception to be thrown and may be caught by the user parser routines or will be caught by this server class.
	/// \param result The resultant data after decrypting and length will be updated with the new packet length.
	virtual bool CallbackParsePacketData(Transport *transport,void *data,size_t &length,DynamicMessageHelper &result,Entry &entry);

	/// Called when the server accepts a connection or when a non-listen class instance connection gets a first valid reply from the server.
	virtual void CallbackAccepted(Transport *transport) {}

	/// Called when the transport is disconnected. This is also called if there is a problem trying to connect. It is therefore possible for a non-listen class instance to receive this callback without a corresponding CallbackAccepted()
	virtual void CallbackDisconnected(Transport *transport) {}

	/// This callback may be used to alter data after it has been received and before the data received is processed.
	/// \param message The data held in a DynamicMessageHelper class.
	virtual void CallbackDataReceived(DynamicMessageHelper &message) {}

	/// This callback may be used to alter any data in a reply, just before it gets sent by the socket.
	/// \param message The data held in a DynamicMessageHelper class.
	virtual void CallbackDataSend(DynamicMessageHelper &message) {}

	/// This callback reports how long a query and reply took starting from when packet data was received until the packet data is sent.
	/// \param seconds The time in seconds.
	virtual void CallbackQueryReplyTime(const float seconds) {}

	/// Called from the scope of the thread but without the class mutex being locked.
	/// This can be called with or without a valid mTransport
	virtual bool CallbackPoll(void) { return true; }

	/// This callback may be used to tidy allocation of objects when the thread terminates.
	/// The class mutex is locked when executing this callback.
	virtual void CallbackTidy(void) {}

	int ThreadEntry(void);

	void GenerateNewNonce(void);

private:

	void ThreadExiting(const int returnCode);
	void Tidy(void);

	int RunThink(Transport *transport,Entry &entry);

protected:
	Transport *mTransport;
	bool mAllowDelete;

	int mNOnce;
	std::map<Transport*,Entry> mConnected;

	bool mIsListen;
	bool mClientGotAPacket;
	bool mClientIsStable;

	Entry mClientEntry;

	size_t mNumConnections;
	size_t mNumTotalConnections;

	/// Gets the connection's NOnce with respect to a Transport 
	int GetConnectionNOnce(Transport *transport = 0, const Entry *entry = 0);

	int CalculateConnectionNOnce(Transport *transport);

	const Entry *FindEntry(Transport *transport);
};

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
