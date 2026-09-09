/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __XPSOCKETURGENTCLASS_H__
#define __XPSOCKETURGENTCLASS_H__

#include "XPSocketClass.h"

namespace RNReplicaNet
{

/**
 * An extension to the normal XPSocket class that handles urgent but unreliable packets. This call also makes use of the function in the XPSocket base class
 * Most member functions return an XPSOCK_EERROR code.
 * XPSOCK_EOK indicates no error. XPSOCK_EWOULDBLOCK is a non-fatal error and means the socket operation would have blocked the current thread at this point.
 * If XPSOCK_EERROR is returned the user should use XPSocket::GetLastError() to find out the actual error code
 */
class XPSocketUrgent : public XPSocket
{
public:
	XPSocketUrgent() {};
	virtual ~XPSocketUrgent() {};

	/**
	 * Creates an urgent socket. Communication through this socket is faster but less reliable. A mechanism is supplied to enable data to be sent in a reliable manner but at the expense of CPU time and network bandwidth
	 * Returns XPSOCK_EERROR if an error occurred
	 * If port is zero (0) or XPSOCK_PORT_ANY then the network layer will pick a port number to be bound to
	 * Using a value of zero for the port when operating a client is advised since the any chosen port might already be in use
	 * \return returns a pointer to new socket or an XPSOCK_EERROR error code.
	 */
	int	Create(const int port = XPSOCK_PORT_ANY);

	/**
	 * Creates an urgent socket bound to a specific address. Please see Create(const int port) for more information.
	 */
	int	Create(const XPAddress &addr);

	/**
	 * Makes an urgent socket broadcast capable
	 * \return returns an XPSOCK error code.
	 */
	int MakeBroadcast(void);

	/**
	 * Sends a NULL terminated string to the peer (the connected machine) for this socket
	 * \param data The pointer to the NULL terminated string
	 * \return returns an XPSOCK_EERROR code.
	 */
	int Send(const char *data,const XPAddress &addr);

	/**
	 * Send an urgent packet from the socket to the specified address. The data sent is not guaranteed to arrive at the destination.
	 * This will return XPSOCK_EUDPSENDFAILED if this method doesn't exist. If so use a non-urgent socket
	 * \param data the pointer to the data to send
	 * \param len the length of the data to send
	 * \param the address and port number to send to
	 * \return returns an XPSOCK_EERROR error code.
	 */
	int Send(const char *data,const int len,const XPAddress &addr);

	/**
	 * Receives data from an urgent socket
	 * \param data pointer to a data buffer
	 * \param maximum length of the data buffer
	 * \param addr the address of the sending socket for any data received
	 * \return returns the length of the data received or 0 if no data pending or an XPSOCK_EERROR error code
	 */
	int Recv(char *const data,const int maxlen,XPAddress *const addr);

	/**
	 * Returns the maximum size for a packet
	 * \return the maximum size for a data packet for this socket
	 */
	static int GetMaxPacketSize(void)	{return XPSOCK_URGENTMAXPACKETSIZE;};

private:
};

} // namespace RNXPSockets

#endif
