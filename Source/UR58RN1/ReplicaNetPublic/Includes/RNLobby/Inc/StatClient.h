/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _STATCLIENT_H_
#define _STATCLIENT_H_
#include "RNLobby/Inc/BaseTransportServer.h"
#include "RNXPSockets/Inc/XPSocket.h"
#include <list>

namespace RNReplicaNet
{

namespace RNLobby
{

/// Maintains a connection to the StatServer.
class StatClient : public BaseTransportServer
{
public:
	StatClient();
	virtual ~StatClient();

	/// Because this uses a TransportUDP connection it will call XPURL::RegisterDefaultTransports()
	bool Start(void);

	/// Sets the address and port of the server. The default for the instance of the class is localhost and port StatServer::kDefaultPort or the address resolved by a successful BackendClient::Start().
	/// \param address The new address and port number to use for the server
	void SetServer(const XPAddress &address);

	/// Using a user nonce, returned from UserClient::RequestUserNonce(), request to upload a script with the corresponding ID.
	/// CallbackUploadAuthenticationFailure() is called if the userNonce authentication fails.
	/// CallbackUploadAuthenticationSuccess() is called if the userNonce authentication succeeds.
	bool WantToUpload(const char *userNonce, const int scriptID);

	/// Following a successful request to WantToUpload() this function must be used to transfer script data to the server.
	/// \param data The data to send.
	/// \param size The size of the data to send. Currently there is a limit of 64 kilobytes (65536 bytes) (StatServerPrivate::kMaximumScriptSize).
	bool UploadData(const void *data,const int size);

	/// Begins a statistics tracking session.
	bool BeginSession(const int scriptID);

	/// For a supplied user nonce begin updating value pairs.
	bool UpdatePairsBegin(const char *userNonce);
	/// Sends a value pair to the server. The name and value string is limited to 128 chars. (StatServerPrivate::kMaximumPairValueSize)
	bool UpdatePairsValue(const char *name,const char *value = 0);
	/// Sends a value pair to the server. The name is limited to 128 chars. (StatServerPrivate::kMaximumPairValueSize)
	bool UpdatePairsValue(const char *name,const int value);
	/// Sends a value pair to the server. The name is limited to 128 chars. (StatServerPrivate::kMaximumPairValueSize)
	bool UpdatePairsValue(const char *name,const float value);
	/// End updating the current batch of value pairs.
	bool UpdatePairsEnd(void);

	/** End the statistics tracking session and start the script processing on the StatServer.
	 * On the StatServer for each authenticated player this calls the script function ProcessPlayer with the loginID in its own thread. The script function ProcessGame is also called once in its own thread.
	 * Each player and tracking session thread has an allocated 2000 script cycles to complete the work and update stats.
	 * It is a good idea to use the RNSimpleScriptDebugger to measure how many cycles each function takes.
	 * An example script, note the standard include header BootStat of the AsmBranch commands (each 5 bytes) for the script entry point. This BootStat file can instead be included first in the input file list.
	\verbatim
		#include <BootStat>

		DefProc ProcessPlayer(loginID)
		EndProc

		DefProc ProcessGame()
		EndProc
	\endverbatim
	 * Using this standard script it is possible to use the debugger to call the second branch using the command line, using 5 (5 bytes for the AsmBranch) as the start address:
	 * \verbatim RNSimpleScriptDebugger.exe script.bin 5 script.bnd \endverbatim
	 */
	bool EndSession(void);

protected:

	/// User extensible callbacks.

	/// A virtual callback that is triggered when the StatClient connects the the StatServer.
	virtual void CallbackConnectionEstablished(Transport *transport) {}

	/// A virtual callback that is triggered when the userNonce from WantToUpload() fails the authentication check.
	virtual void CallbackUploadAuthenticationFailure(void) {}

	/// A virtual callback that is triggered when the userNonce from WantToUpload() passes the authentication check.
	virtual void CallbackUploadAuthenticationSuccess(void) {}

	/// A virtual callback that is triggered when UploadData() succeeds.
	virtual void CallbackUploadACK(const int scriptID) {}

	/// A virtual callback that is triggered when the StatServer processes the BeginSession() request.
	virtual void CallbackBeginSession() {}

	/// A virtual callback that is triggered when the StatServer has finished processing the session.
	/// Even if there is an error processing the session this callback is called.
	virtual void CallbackEndSession() {}

	/// A virtual callback that is triggered when the StatServer is unable to find the script.
	/// This is called before CallbackEndSession()
	virtual void CallbackEndSessionScriptNotFound() {}

	/// A virtual callback that is triggered when the StatServer is unable to complete a script. Most likely due to an exception or the script taking too long to execute.
	/// This is called before CallbackEndSession()
	virtual void CallbackEndSessionScriptNotCompleted() {}

protected:
	// These callbacks from BaseTransportServer are implemented by this class. If they are implemented by the user this implementation must be called.
	void CallbackTidy(void);

	bool CallbackPoll(void);

	bool CallbackParseValidMessage(Transport *transport,DynamicMessageHelper &message,const size_t length);

	void InitInternalState(void);

	XPAddress mServerAddress;
	bool mServerAddressSet;

	bool mCanUpload;

	bool mInASession;
	bool mUpdatingPairs;
};

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
