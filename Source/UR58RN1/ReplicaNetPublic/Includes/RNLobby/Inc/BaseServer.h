/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _BASESERVER_H_
#define _BASESERVER_H_
#include "RNLobby/Inc/CommonServer.h"
#include "RNPlatform/Inc/SysTime.h"

namespace RNReplicaNet
{

namespace RNLobby
{

/// A server base class that handles encrypted messages to and from a socket.
class BaseServer : private CommonServer , public ThreadClass , public Thread
{
public:
	BaseServer();
	virtual ~BaseServer();

	/// Starts the thread. If this function is overridden then this base implementation must also be called.
	/// \param socket The socket to poll for this class.
	/// \param allowDelete When true this class is responsible for deleting the socket.
	/// \return Success returns true, failure returns false.
	virtual bool StartWithSocket(t_XPSocket *socket,const bool allowDelete = true);

	/// Stops the thread. If this function is overridden then this base implementation must also be called.
	/// \return Success returns true, failure returns false.
	virtual bool Stop(void);

	/// Sets the encryption key to be used by all queries for this server. The same key must be used with any other RNLobby::BaseServer querying this server.
	/// \param data The data to use as a seed for the key. This can be NULL to use the default key.
	/// \param length The length of the data to use for creating the key.
	void SetEncryptionKey(const void *data,const int length);

	/// This callback may be used to alter data after it has been received and before the data received is processed.
	/// \param message The data held in a DynamicMessageHelper class.
	/// \param address The network address for this data packet. The contents of the address may be changed if required.
	virtual void CallbackDataReceived(DynamicMessageHelper &message,XPAddress &address) {}

	/// This callback may be used to alter any data in a reply, just before it gets sent by the socket.
	/// \param message The data held in a DynamicMessageHelper class.
	/// \param address The network address for this data packet. The contents of the address may be changed if required.
	virtual void CallbackDataSend(DynamicMessageHelper &message,XPAddress &address) {}

	/// This callback reports how long an query and reply took starting from when packet data was received until the packet data is sent.
	/// \param seconds The time in seconds.
	virtual void CallbackQueryReplyTime(const float seconds) {}

	/// Called from the scope of the thread but without the class mutex being locked.
	/// \return Return true to indicate this service class should continue to run. If false is returned this will stop the receiving thread and shutdown this server class.
	virtual bool CallbackPoll(void) { return true; }

	static SysTimeType FloatTime(void);

	/// This callback may be used to tidy allocation of objects when the thread terminates.
	/// The class mutex is locked when executing this callback.
	virtual void CallbackTidy(void) {}

protected:
	/// Called from the scope of the thread but without the class mutex being locked.
	/// \return Return true to indicate this service class should continue to run. If false is returned this will stop the receiving thread and shutdown this server class.
	virtual bool CallbackParseValidMessage(DynamicMessageHelper &message,const size_t length,const XPAddress &address) { return true; }

	/// The class mutex is locked during the context of this function.
	/// \return Return true to indicate this service class should continue to run. If false is returned this will stop the receiving thread and shutdown this server class.
	virtual bool PackageAndSendData(t_XPSocket *socket,const DynamicMessageHelper &message,const XPAddress &address);

	/// The class mutex is locked during the context of this function.
	/// \param result The resultant data after decrypting and length will be updated with the new packet length.
	virtual bool CallbackParsePacketData(t_XPSocket *socket,const XPAddress &addr,void *data,size_t &length,DynamicMessageHelper &result);

	int ThreadEntry(void);

private:

	void ThreadExiting(const int returnCode);
	void Tidy(void);

protected:
	t_XPSocket *mSocket;
	bool mAllowDelete;

	int mNOnce;
};

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
