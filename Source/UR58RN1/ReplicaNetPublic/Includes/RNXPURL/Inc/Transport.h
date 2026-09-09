/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __TRANSPORT_H__
#define __TRANSPORT_H__

#include <string>

namespace RNReplicaNet
{

const int TransportAnyGameChannel = 0;
const int TransportMinimumPacketSize = 512;

/**
 * A virtual base class that defines a transport layer.
 * Each new Transport is a potential connection to the outside world or a method by which a machine can connect to another machine.
 * Each transport can use any means necessary to accomplish this connection.
 * Each new implementation of a transport class must support the operation of the base class pure virtual members.
 * Naturally a transport need not support the virtual members as default values are returned for these.<br>
 * The terms "Reliable", "Unreliable" and "Certain" are references to the three packet delivery types used by ReplicaNet, XPSession and XPURL Transport classes.<br>
 * "Reliable" guarantees delivery of data in the same order it was sent.<br>
 * "Certain" guarantees delivery of data but the order the data is received may not be preserved.<br>
 * "Unreliable" does not guarantee delivery of data and the order of the data when it is received may not be preserved.<br>
 * In general it is a good idea to not use too many "Reliable" updates because this method is quite expensive in terms of network and memory performance. If one packet gets lost it will delay the delivery of all other Reliable data until that packet is resent. The better choice would be to use "Certain" because this is less expensive in terms of memory and network performance. Also if one Certain packet is lost the delivery of other Certain packets is not delayed. Generally use Reliable for those events that absolutely must occur in the same order they were sent.<br>
 */
class Transport
{
public:

	/**
	 * Standard Transport errors
	 */
	enum Error
	{
		kTransport_EOK = 0,						/**< No error */
		kTransport_EERROR = -1,					/**< A fatal error happened, like the connection broke and cannot be recovered */
		kTransport_ECANTLISTENONCHANNEL = -2,	/**< Non-fatal, the transport couldn't listen on the channel. The usual response by the user is to then try to listen on AnyGameChannel */
		kTransport_EWAITING = -3				/**< Non-fatal, the transport is in the process of waiting to connect or for a connection or to listen */
	};

	enum Bandwidth
	{
		kBandwidth_Off = 0,
		kBandwidth_Connection = 1
	};

	/**
	 * The ctor performs some basic initialisation
	 */
	Transport();

	/**
	 * The dtor makes sure everything is tidy
	 */
	virtual ~Transport();

	/* These virtual functions have a base implementation but can also be replaced by a new transport if needed */

	/**
	 * Gets the total number of packets that were sent.
	 * \param reset The default parameter for this function is false. If true then the value is returned as normal but the internal total is reset to zero
	 * \return Returns the total value, since the last reset to zero
	 */
	virtual int GetPacketsSent(const bool reset = false);

	/**
	 * Gets the total number of merged packets that were sent.
	 * \param reset The default parameter for this function is false. If true then the value is returned as normal but the internal total is reset to zero
	 * \return Returns the total value, since the last reset to zero
	 */
	virtual int GetPacketsSentMerged(const bool reset = false);

	/**
	 * Gets the total size of packets that were sent.
	 * \param reset The default parameter for this function is false. If true then the value is returned as normal but the internal total is reset to zero
	 * \return Returns the total value, since the last reset to zero
	 */
	virtual int GetPacketsSentSize(const bool reset = false);

	/**
	 * Gets the total number of packets that were received.
	 * \param reset The default parameter for this function is false. If true then the value is returned as normal but the internal total is reset to zero
	 * \return Returns the total value, since the last reset to zero
	 */
	virtual int GetPacketsReceived(const bool reset = false);

	/**
	 * Gets the total number of merged packets that were received.
	 * \param reset The default parameter for this function is false. If true then the value is returned as normal but the internal total is reset to zero
	 * \return Returns the total value, since the last reset to zero
	 */
	virtual int GetPacketsReceivedMerged(const bool reset = false);

	/**
	 * Gets the total size of packets that were received.
	 * \param reset The default parameter for this function is false. If true then the value is returned as normal but the internal total is reset to zero
	 * \return Returns the total value, since the last reset to zero
	 */
	virtual int GetPacketsReceivedSize(const bool reset = false);

	/**
	 * Gets the total number of packets that were lost and had to be resent.
	 * \param reset The default parameter for this function is false. If true then the value is returned as normal but the internal total is reset to zero
	 * \return Returns the total value, since the last reset to zero
	 */
	virtual int GetPacketsLost(const bool reset = false);

	/**
	 * Sets the bandwidth limit algorithm to be used for this Transport type.
	 * \param enable The default parameter is kBandwidth_Off which disables any bandwidth limit for this Transport type.
	 * kBandwidth_Connection will turn on bandwidth calculation for each instance and connection using this Transport.
	 * For example, if there are five instances of this Transport type and kBandwidth_Connection is used with the maximum outgoing bandwidth to be 1000 bytes per second then the maximum combined bandwidth will capped to five times 1000 bytes per second, which is 5000 bytes per second.
	 */
	virtual void SetBandwidthLimit(const Bandwidth enable = kBandwidth_Off);

	/**
	 * Reads the configuration value set by SetBandwidthLimit()
	 * \return the configured value
	 */
	virtual Bandwidth GetBandwidthLimit();

	/**
	 * Sets the maximum output bandwidth for emulation.
	 * \param bytesPerSecond the bandwidth in bytes per second. The default value is 0 meaning there is no limit.
	 */
	virtual void SetMaximumOutputBandwidth(const int bytesPerSecond = 0);

	/**
	 * Reads the configuration value set by SetMaximumOutputBandwidth()
	 * \return the configured value
	 */
	virtual int GetMaximumOutputBandwidth(void);

	/**
	 * Sets the maximum input bandwidth for emulation.
	 * \param bytesPerSecond the bandwidth in bytes per second. The default value is 0 meaning there is no limit.
	 */
	virtual void SetMaximumInputBandwidth(const int bytesPerSecond = 0);

	/**
	 * Reads the configuration value set by GetMaximumInputBandwidth().
	 * \return the configured value.
	 */
	virtual int GetMaximumInputBandwidth(void);

	/**
	 * If connected this gets the URL of the other transport. It is possible this function will return a null ("") string when connected if the transport provider does not support this functionality.
	 * \return the URL of the peer transport.
	 */
	virtual std::string GetPeerURL(void);

	/**
	 * If this returns false the transport was closed due to an error such as a timeout. If this returns true then the transport was gracefully closed without an error.
	 * \retrun The default is to return false.
	 */
	virtual bool GetTransportClosed(void);

	/**
	 * Enables or disables LAN discovery of this transport type.
	 * \param enable Set this to be true to enable LAN discovery. False will disable LAN discovery. The default is true.
	 */
	virtual void SetEnableLANDiscovery(const bool enable = true);

	/**
	 * Reads the configuration value set by SetEnableLANDiscovery().
	 * \return the configured value.
	 */
	virtual bool GetEnableLANDiscovery(void);

	/* These pure virtual functions are the minimum feature set the transport has to support */

	/**
	 * Enables a Transport to state which type of protocol it can receive.
	 * Each protocol type is separated by a comma.
	 * \return the comma separated list of protocols supported by the Transport
	 */
	virtual std::string CanAccept(void) = 0;

	/**
	 * Enables a Transport to state which type of protocol it can route.
	 * Each protocol type is separated by a comma.
	 * \return the comma separated list of protocols routed by the Transport
	 */
	virtual std::string CanRoute(void) = 0;

	/**
	 * Exports a URL that is used by this transport and can be used by other Transports to locate this transport.
	 * The format of the URL follows this convention: <TransportType>@<data>/
	 * \return the URL to locate this transport
	 */
	virtual std::string ExportURL(void) = 0;

	/**
	 * Used to allocate a transport of the relevant class type
	 * \return a Transport pointer
	 */
	virtual Transport *Allocate(void) = 0;

	/**
	 * Sets this Transport to listen on a certain channel. The default channel is defined as AnyGameChannel
	 * \param channel the channel to listen on, for example DefaultGameChannel. This parameter need not be passed in, in which case AnyGameChannel is used.
	 * \return A Transport::Error is returned indicating success or failure for the operation
	 */
	virtual Error Listen(const int channel = TransportAnyGameChannel) = 0;

	/**
	 * Sets this Transport to listen on a certain address. The default blank address is defined by the implemented Transport.
	 * \param address The address to listen on prefixed by the transport's CanAccept() string.
	 * \return A Transport::Error is returned indicating success or failure for the operation
	 */
	virtual Error Listen(const std::string &address) = 0;

	/**
	 * Tries to connect to a specified address
	 * \param address the address to connect to
	 * \return A Transport::Error is returned indicating success or failure for the operation
	 */
	virtual Error Connect(const std::string address) = 0;

	/**
	 * Accepts the first pending connection from another Transport
	 * \return returns NULL if there are no pending Transport connections or a new Transport pointer that can be used to communicate with
	 */
	virtual Transport *Accept(void) = 0;

	/**
	 * Tries to send data to the currently connected Transport via the "unreliable" method without using any form of packet loss correction.<br>
	 * This method of sending packets is very cheap since there is no packet loss correction.
	 * Before attempting to send any data using this transport instance wait for GetStatus() to return kTransport_EOK.
	 * \param data the data to send
	 * \param len the length of the data to send
	 * \return A Transport::Error is returned indicating success or failure for the operation
	 */
	virtual Error Send(const char *data,const int len) = 0;

	/**
	 * Tries to send data to the currently connected Transport via the "unreliable ordered" method without using any form of packet loss correction.<br>
	 * This method of sending packets is very cheap since there is no packet loss correction. Packets will be received at the destination in order.
	 * Before attempting to send any data using this transport instance wait for GetStatus() to return kTransport_EOK.
	 * \param data the data to send
	 * \param len the length of the data to send
	 * \param band This optional parameter allows the user to send ordered packets and ensure they are in original order grouped by band. This parameter is optional for a Transport to support and if supported the Transport may only support a limited number of bands.
	 * \return A Transport::Error is returned indicating success or failure for the operation
	 */
	virtual Error SendOrdered(const char *data,const int len,const unsigned int band = 0) = 0;

	/**
	 * Tries to send data to the currently connected Transport via the "reliable" method that guarantees packets are received in the same order they are sent.<br>
	 * This method of sending packets is the most expensive in terms of CPU time and memory buffers
	 * Before attempting to send any data using this transport instance wait for GetStatus() to return kTransport_EOK.
	 * \param data the data to send
	 * \param len the length of the data to send
	 * \param band This optional parameter allows the user to send reliable packets and ensure they are in original order grouped by band. This parameter is optional for a Transport to support and if supported the Transport may only support a limited number of bands.
	 * \return A Transport::Error is returned indicating success or failure for the operation
	 */
	virtual Error SendReliable(const char *data,const int len,const unsigned int band = 0) = 0;

	/**
	 * Tries to send data to the currently connected Transport via the "certain" method that guarantees delivery of packets but does not gaurantee the packet receive order is maintained.<br>
	 * This method is less expensive than the "reliable" method because send and receive order is not preserved.
	 * Before attempting to send any data using this transport instance wait for GetStatus() to return kTransport_EOK.
	 * \param data the data to send
	 * \param len the length of the data to send
	 * \return A Transport::Error is returned indicating success or failure for the operation
	 */
	virtual Error SendCertain(const char *data,const int len) = 0;

	/**
	 * Tries to receive data pending at this Transport. Data packets received by this method are not merged together.
	 * \param data the data buffer to use
	 * \param maxlen the maximum length of this data buffer
	 * \return the length of the data received or zero for no data pending. If maxlen is too small the size of the next packet is still returned but the packet data is not returned. The next function call should use an increased maxlen size for the packet.
	 */
	virtual int Recv(char *const data,const int maxlen) = 0;

	/**
	 * Gets the maximum packet size that can be transmitted over this Transport. The base spec requires a Transport to cope with packets of at least the number of bytes defined by TransportMinimumPacketSize
	 * \return the maximum packet size that can be transmitted
	 */
	virtual int GetMaxPacketSize(void) = 0;

	/**
	 * If a discovery is pending this will return the URL of the source machine that made a discovery request
	 * \return returns the URL of the machine attempting a discovery or a "" string
	 */
	virtual std::string HandleDiscovery(void) = 0;

	/**
	 * This function handles the reply URL that is passed back to the previous calling machine of HandleDiscovery()
	 * \param reply the reply string to send
	 */
	virtual void HandleDiscoveryReply(const std::string reply) = 0;

	/**
	 * This starts the discovery cycle to try to find other transports
	 */
	virtual void BeginDiscovery(void) = 0;

	/**
	 * This is the function that can be used to discover other similar transports that can be connected to.
	 * \return Returns a URL, which is the URL passed in by HandleDiscoveryReply()
	 */
	virtual std::string GetNextDiscovery(void) = 0;

	/**
	 * Gets the status of this transport
	 * \return the status
	 */
	virtual Error GetStatus(void) = 0;

	/**
	 * Gets the current instance of this Transport compared to all Transports allocated in this session.
	 */
	int GetInstance(void);

	/**
	 * Allows transparent packet compression to be enabled or disabled. Setting this on a Transport listener will enable accepted Transports to inherit this value.
	 * This will only have an effect before a Connect() or Accept() call.
	 * \param enable The default is 0 which disables compression. A number between 1 and 10 will enable the extra packet compression where 1 is the fastest compression and 10 is the slowest and best compression. See XPCompression for details. Compression level 5 is a good trade-off between speed and compression.
	 */
	virtual void SetTransparentPacketCompression(const int level = 0)
	{
	}

	/**
	 * Allows the setting for SetTransparentPacketCompression() to be read. Transports that do not support this functioanlity will always return 0 regardless of any calls to SetTransparentPacketCompression().
	 * \return The setting for SetTransparentPacketCompression()
	 */
	virtual int GetTransparentPacketCompression(void)
	{
		return 0;
	}

private:
	int mTransportInstance;
};

} // namespace RNReplicaNet

#endif
