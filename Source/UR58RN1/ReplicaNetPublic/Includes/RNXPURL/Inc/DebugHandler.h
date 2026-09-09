/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _DEBUGHANDLER_H_
#define _DEBUGHANDLER_H_

namespace RNReplicaNet
{

/**
 * DebugHandler is a base class for handling debug messages output by RNXPURL, RNXPSession and RNReplicaNet.
 * The implementation of DebugHandler::DebugPrint() can be extended by the application. The default implementation of DebugPrint() is to call OutputToSocket() with the debug text.
\verbatim
// Define the class
class ApplicationDebugHandler : public RNReplicaNet::DebugHandler
{
public:
	ApplicationDebugHandler() {};
	virtual ~ApplicationDebugHandler() {};

	void DebugPrint(const char *text)
	{
		// TODO: Add applications specific code here

		// And optionally call the base class function
		RNReplicaNet::DebugHandler::DebugPrint(text);
	}
};

#ifdef REPLICANET_VISUALDEBUGGER
// In the application code, to enable the debug output
RNReplicaNet::XPURL::RegisterDebugHandler(new ApplicationDebugHandler());
RNReplicaNet::XPURL::GetDebugHandler()->EnableVisualDebuggerSocket();
#endif

\endverbatim
 * The class also contains a simple mechanism for starting a listen socket for a single connection from the ReplicaNet Visual Debugger application.
 */
class DebugHandler
{
public:
	DebugHandler();

	virtual ~DebugHandler();

	/**
	 * A virtual function that an application can implement to enable debug output. The default implementation calls OutputToSocket()
	 */
	virtual void DebugPrint(const char *);

	/**
	 * Outputs some text to the debug socket.
	 * \param text The text
	 * \return Indicates an error occurred if true while sending the text, false indicates no error.
	 */
	virtual bool OutputToSocket(const char *text);

	/**
	 * This enables a TCP listen socket that can be used for connections from the Visual Debugger to this debug session. This function always calls DisableVisualDebuggerSocket() first.
	 * \param port The port number to listen on. The default is 8000. If the port cannot be used listening, i.e. it is used, a random port number will be chosen.
	 * \return Indicates an error occurred if true, false means no error.
	 */
	virtual bool EnableVisualDebuggerSocket(const int port = 8000);

	/**
	 * This frees the socket used for the Visual Debugger connection. If connected to the Visual Debugger, this terminates the connection. If there is no socket allocated or connected this function does nothing.
	 * A PlatformHeap::ForceFree() will also disconnect any active Visual Debugger connection.
	 */
	virtual void DisableVisualDebuggerSocket(void);

	/**
	 * A variable argument printf style function that uses DebugPrint() to output the result
	 * \param format The format, like printf.
	 */
	virtual void Printf(const char *format, ...);

	/**
	 * Called regularly by the library hosting the debug connection. Can also be called by the user.
	 */
	virtual void Poll(void);
	
	/**
	 * Returns the state of the socket connection.
	 * \return Returns true if there is an accepted connection, false if not.
	 */
	virtual bool GotConnection(void) const;

	/**
	 * This virtual function is called when there is a connection error.
	 * \param error This contains a transport specific error. Typically negative values indicate a fatal error while positive errors indicate non-fatal errors.
	 */
	virtual void CallbackConnectionError(const int error);
};

} // namespace RNReplicaNet


#endif
