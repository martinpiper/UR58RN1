/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __XPURL_H__
#define __XPURL_H__

#define	_XPURL_VERSION 7300

#include "RNXPURL/Inc/Transport.h"
#include <vector>

namespace RNReplicaNet
{

class DebugHandler;
struct URLBlock;

/**
 * XPURL decodes and encodes URLs and acts as a common method for interfacing various networking protocols.
 */
class XPURL
{
public:
	/**
	 * The ctor for an XPURL
	 */
	XPURL();

	/**
	 * The dtor for an XPURL
	 * This cleans up everything for this class
	 */
	virtual ~XPURL();

	/**
	 * Registers a Transport pointer with the URL decoder
	 * \param transport the Transport to register
	 */
	static void RegisterTransport(Transport &transport);

	/**
	 * This static member function registers all of the default transports such as reliable UDP.
	 */
	static void RegisterDefaultTransports(void);

	/**
	 * Starts enumerating the current number of Transport types
	 */
	void BeginEnumerateTransports(void);

	/**
	 * Enumerates the Transport list, BeginEnumerateTransports() must be called before this function
	 * Call this function repeatedly in a loop to enumerate through the list
	 * \return the next Transport pointer in the list
	 */
	Transport *EnumerateTransports(void);

	/**
	 * Parses a URL and after each iteration returns a tokenised URL. The first time this function is called pass in the whole URL and on subsequent calls pass in ""
	 * \return Each iteration returns a single URL or "" to denote an end to the list of URLs
	 */
	std::string ParseURLs(const std::string urls);

	/**
	 * Finds the matching Transport for the URL
	 * \return The new Transport pointer for which Allocate() should be called to create a usable Transport pointer
	 */
	static Transport *FindTransport(const std::string url);

	/**
	 * Frees any resources for a Transport* returned by FindTransport()->Allocate();
	 */
	static bool FreeTransport(Transport *transport);

	/**
	 * This polls XPURL. Normally an application will yield enough time during threads for XPURL to function.
	 * Sometimes an application that uses a lot of 3D graphics can lock out other threads for quite some time.
	 * In these cases you may call XPURL::Poll() to yield some time to XPURL to perform housekeeping. If a reliable UDP transport is being used this function will also poll the reliable UDP manager.<br>
	 * Single threaded considerations:<br>
	 * When using the single threaded libraries this method should be called regularly. Unless XPSession::Poll() or ReplicaNet::Poll() is called in which case the default action is that this Poll() method is called.
	 */
	void Poll(void);

	/**
	 * Sets the manual Poll() method for this class and the reliable UDP manager.<br>
	 * For multi-threaded libraries this turns off the automatic polling method.<br>
	 * This allows an application to time exactly when polls are done.<br>
	 * The application must call Poll() on a regular basis. A good place is just before or after the frame update is done.
	 */
	void SetManualPoll(void);

	/**
	 * Sets the automatic Poll() method for this class and the reliable UDP manager.<br>
	 * In multi-threading libraries this restarts the automatic polling thread so that an application does not need the call Poll()<br>
	 * In single-threading libraries this method has no effect and does not start a thread.<br>
	 * The default multi-threading library behaviour is to enable multi-threading automatic polls,<br>
	 * The default single-threading library behaviour is to use manual polls,<br>
	 */
	void SetAutomaticPoll(void);

	/**
	 * Causes the network layer to start. This can include all system level hardware on the target platform.<br>
	 * \param hardware Set this to be true to initialise all network hardware. The default is false.
	 * It is not necessary to manually call this since the network will start automatically when the first network related function is used.
	 */
	static void InitialiseNetwork(const bool hardware = false);

	/**
	 * Forces the network layer to shutdown if it is started. This can include all system level hardware on the target platform. This should only be used after users of XPURL classes, such as XPSession and ReplicaNet, have been freed.
	 * This does not free all internal allocated memory blocks, an example would be for registered factory classes.
	 * To force all internal allocated memory to be released RNReplicaNet::PlatformHeap::ForceFree() should be called.
	 * A PlatformHeap::ForceFree() will call ShutdownNetwork(true).
	 * \param hardware Set this to be true to shutdown all network hardware. The default is false.
	 */
	static void ShutdownNetwork(const bool hardware = false);


	/**
	 * This registers an extensible debug handler for use by XPURL, XPSession, ReplicaNet and the Visual Debugger.
	 * A PlatformHeap::ForceFree() will delete the active currently registered DebugHandler class. The application can use RegisterDebugHandler() without a parameter to unregister the DebugHandler and avoid the handler being automatically freed by PlatformHeap::ForceFree().
	 * \param debugHandler A pointer to an instance of the debug handler class. The default value is null which disables the debugger output.
	 * \return The previous debug handler pointer or null if no debugger handler was registered.
	 */
	static DebugHandler *RegisterDebugHandler(DebugHandler *debugHandler = 0);

	/**
	 * Returns the debug handler set by RegisterDebugHandler() or null.
	 * \return The debug handler or null if no debugger handler is registered.
	 */
	static DebugHandler *GetDebugHandler(void);

private:

	std::vector<URLBlock *>::iterator mst,men;

	std::string mParseState;
};

} // namespace RNReplicaNet

#endif
