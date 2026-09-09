/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
/* A class implementation of the C style XPSocket library */
#ifndef __XPSOCKETCLASS_H__
#define __XPSOCKETCLASS_H__

#include "XPSocket.h"

namespace RNReplicaNet
{

/**
 * This class is a socket which is a potential connection or actual connection to a machine.
 * This uses the underlying XPSocket library which in turn uses the winsock or berkley sockets API.
 * Most member functions return an XPSOCK_EERROR code.
 * XPSOCK_EOK indicates no error. XPSOCK_EWOULDBLOCK is a non-fatal error and means the socket operation would have blocked the current thread at this point.
 * If XPSOCK_EERROR is returned the user should use XPSocket::GetLastError() to find out the actual error code
 */
class XPSocket
{
public:
friend class XPSocketUrgent;
//friend class XPStream;
	/**
	 * The ctor for an XPSocket
	 * This checks to make sure sockets are active and sets some initial states. At this point no connection is made for the socket
	 */
	XPSocket();

	/**
	 * The dtor for an XPSocket
	 * This cleans up everything for this socket
	 */
	virtual ~XPSocket();

	/**
	 * Gets the last error reported by the XPSocket API
	 * \return returns an XPSOCK_EERROR code.
	 */
	static int GetLastError(void);

	/**
	 * Creates a TCP style socket. This is the first stage to creating a connection
	 * \return returns an XPSOCK_EERROR code.
	 */
	virtual int Create(void);

	/**
	 * Tries to connect this socket to a network address
	 * \param addr the address to try to connect to
	 * \return returns an XPSOCK_EERROR code.
	 */
	int Connect(const XPAddress &addr);

	/**
	 * Sends a NULL terminated string to the peer (the connected machine) for this socket
	 * \param data The pointer to the NULL terminated string
	 * \return returns an XPSOCK_EERROR code.
	 */
	int Send(const char *data);

	/**
	 * Sends data to the peer (the connected machine) for this socket
	 * \param data the pointer to the data to send
	 * \param len the length of the data to send
	 * \return returns an XPSOCK_EERROR code.
	 */
	int Send(const char *data,const int len);

	/**
	 * Tries to receive data that is waiting for this socket
	 * \param data the pointer to the buffer to receive the data
	 * \param maxlen the maximum length of the data buffer
	 * \return returns the length of the data received or 0 for no data pending or an XPSOCK_EERROR code.
	 */
	int Recv(char *const data,const int maxlen);

	/**
	 * Closes the socket but does not delete the memory for the XPSocket class
	 * \return returns an XPSOCK_EERROR code.
	 */
	int Close(void);

	/**
	 * Makes the socket listen on a certain port number
	 * \param port the port number to listen on for incoming connections
	 * \return returns an XPSOCK_EERROR code.
	 */
	int Listen(const int port = XPSOCK_PORT_ANY);

	/**
	 * Makes the socket listen on a certain address
	 * \param addr the address to listen on for incoming connections
	 * \return returns an XPSOCK_EERROR code.
	 */
	int Listen(const XPAddress &addr);

	/**
	 * Accepts any pending connections for this socket. This should be used with the Listen() member function
	 * \return returns NULL if no connection was pending or returns the pointer to the new connected XPSocket or pcXPSOCK_EERROR if an error occurred
	 */
	XPSocket *Accept(void);

	/**
	 * Sets the size of the send buffer for the socket
	 * \param size The size to set the send buffer to or -1 to only return the value
	 * \return the new size of the send buffer
	 */
	int SetSendBuffer(const int size);

	/**
	 * Sets the size of the recv buffer for the socket
	 * \param size The size to set the recv buffer to or -1 to only return the value
	 * \return the new size of the recv buffer
	 */
	int SetRecvBuffer(const int size);

	/**
	 * Fills in the internet address and port of the socket to the address structure.
	 * \param addr the address structure to fill in
	 * \return returns XPSOCK_EOK or an XPSOCK_EERROR error code.
	 */
	int GetAddress(XPAddress *const addr);

	/**
	 * Fills in the internet address of the peer of the socket to the address structure.
	 * \param addr the address structure to fill in
	 * \return returns XPSOCK_EOK or an XPSOCK_EERROR error code.
	 */
	int GetPeerAddress(XPAddress *const addr);

	/**
	 * Returns the maximum size for a packet
	 * \return the maximum size for a data packet for this socket
	 */
	static int GetMaxPacketSize(void)	{return XPSOCK_MAXPACKETSIZE;};

	/**
	 * Mutates this socket class to use a socket
	 * \param socket the socket to mutate to
	 * \return returns XPSOCK_EOK or an XPSOCK_EERROR error code.
	 */
	int MutateSocket(XPSocket *socket);

	/**
	 * Tests the alive state of the socket
	 * \return returns true if the socket is alive or false if the socket is dead
	 */
	bool IsAlive(void);

	/**
	 * Access for the t_XPSocket pointer
	 * \return  The t_XPSocket pointer associated with this class
	 */
	t_XPSocket *GetSocketPointer(void);

private:

	t_XPSocket *mSocket;			/**< The t_XPSocket pointer returned from the C style API */

	enum SocketState
	{
		TypeUndefined = 0,
		TypeTCP = 1,
		TypeUDP = 2
	};								/**< A private enumerate for the state of the socket */

	SocketState mSocketState;		/**< The state of this socket, to avoid using incorrect member function calls on the wrong type of socket */
};

/* A macro to make our life easier */
#define SOCKET_ASSERT_TYPE(x) \
	if (mSocketState == (x))	\
	{	\
		assert(0 && "XPSOCK_ILLEGALOPERATION");	\
		asocket_lasterror = XPSOCK_ILLEGALOPERATION;	\
		return XPSOCK_EERROR;	\
	}	\
	if ((x) == TypeUDP)	\
	{	\
		mSocketState = TypeTCP;	\
	}	\
	else	\
	{	\
		mSocketState = TypeUDP;	\
	}

#define SOCKET_ASSERT_TYPEP(x) \
	if (mSocketState == (x))	\
	{	\
		assert(0 && "XPSOCK_ILLEGALOPERATION");	\
		asocket_lasterror = XPSOCK_ILLEGALOPERATION;	\
		return pcXPSOCK_EERROR;	\
	}	\
	if ((x) == TypeUDP)	\
	{	\
		mSocketState = TypeTCP;	\
	}	\
	else	\
	{	\
		mSocketState = TypeUDP;	\
	}

#define pcXPSOCK_EERROR  ((RNReplicaNet::XPSocket *) XPSOCK_EERROR)	/**< Define a pointer type to check with for an error return */

} // namespace RNXPSockets

#endif
