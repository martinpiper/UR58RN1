/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
/* XPSocket.h */
#ifndef _XPSOCK_H_
#define _XPSOCK_H_

#include <string>
#include "RNPlatform/Inc/MessageHelper.h"

namespace RNReplicaNet
{

#define	_XPSOCK_VERSION 1700


/* Some error return defines */
/**
 * \def XPSOCK_EOK No error, everything is OK
 */
#define XPSOCK_EOK					(0)		  				/**< Not an error but defined for completeness. */
#define XPSOCK_EERROR				(-1)					/**< An undefined error. Look in asocket_lasterror for a fuller description. */
#define XPSOCK_EWOULDBLOCK			(-2)					/**< Not a serious error, just informational. */
#define XPSOCK_ECLOSED				(-3)					/**< The other end has closed the connection. */
#define XPSOCK_ENOEXIST				(-4)					/**< There is not a machine there to connect to. */
#define XPSOCK_ENOMEMORY			(-5)					/**< The sockets ran out of memory. */
#define XPSOCK_ENOTASOCKET			(-6)					/**< The socket pointer passed into a function was not a socket pointer. */
#define XPSOCK_ECANTLISTEN			(-7)					/**< Couldn't make this socket listen. */
#define XPSOCK_ENOTALISTEN			(-8)					/**< This socket isn't a listen socket. */
#define XPSOCK_ENOTCONNECTED		(-9)					/**< This socket isn't connected to another socket. */
#define XPSOCK_EBUFFEROVERFLOW		(-10)					/**< The local host buffer has been over flowed, increase the standard amount in this header file. */
#define XPSOCK_EUDPSENDFAILED		(-11)					/**< Sending a UDP packet failed. As a last resort to sending a slow packet instead. */
#define XPSOCK_ECANTBIND			(-12)					/**< Can't bind the socket to an address. */
#define XPSOCK_ECANTRESOLVE			(-13)					/**< Can't resolve the supplied string to a number. */
#define XPSOCK_EURGENTTOOBIG		(-14)					/**< The size of the urgent data is too big. */
#define XPSOCK_BUFFERTOOSMALL		(-15)					/**< The size of the destination buffer is too small. */
#define XPSOCK_ILLEGALOPERATION		(-16)					/**< The requested operation cannot be performed on this socket due to type differences between urgent and normal sockets. */
#define XPSOCK_ECONNINPROGRESS		(-17)					/**< A connection is in progress. */
#define XPSOCK_ESTACKSTARTFAILURE	(-18)					/**< The network stack failed to start. */
#define XPSOCK_ESTACKNOTSTARTED		(-19)					/**< The opration couldn't be completed because the stack has not been started. */

#define pXPSOCK_EERROR  ((RNReplicaNet::t_XPSocket *) XPSOCK_EERROR)	/**< Define a pointer type to check with for an error return */

/* Probable XPSocket types */
#define XPSOCK_TLOCALHOST	(0)
#define XPSOCK_TINET		(1)

/* XPSocket status */
#define XPSOCK_SNULL		(0)
#define XPSOCK_SLISTEN		(1)
#define XPSOCK_SCONNECT		(2)
#define XPSOCK_SCLOSE		(3)

/* Flags to pass to XPSock_Recv */
#define XPSOCK_FNULL			(0)					/**< NULL flag to pass to XPSock_Recv */
#define XPSOCK_FPEEK			(1)					/**< PEEK flag to pass to XPSock_Recv. This returns the message but leaves it in the queue */


#define XPSOCK_PORT_ANY			(0)					/**< Port number define to allow any port to be chosen */
#define XPSOCK_PORT_LOCAL		(-1)				/**< Port number to force local loopback operation */

#define XPSOCK_MAXPACKETSIZE	(8192)				/**< Maximum size that a packet can be */

#define XPSOCK_URGENTMAXPACKETSIZE	(2048)			/**< Maximum size that a packet can be for urgent sockets */

/* Some constants for the local loop back sockets */
#define XPSOCK_LOOPBACKBUFFER	20480

/* Flags returned by XPSock_GetHardwareStatus */
#define XPSOCK_HARDWARE_AVAILABLE			(1<<0)
#define XPSOCK_HARDWARE_CABLE_CONNECTED		(1<<1)
#define XPSOCK_HARDWARE_10MBS				(1<<2)
#define XPSOCK_HARDWARE_100MBS				(1<<3)
#define XPSOCK_HARDWARE_FULL_DUPLEX			(1<<4)
#define XPSOCK_HARDWARE_HALF_DUPLEX			(1<<5)


/* Structs and their type defs */

/**
 * This is an address that the sockets use
 */
struct XPAddress
{
	/**
	 * ctor for XPAddress that allows the structure to be constructed in a C++ fashion
	 */
	XPAddress(const int port,unsigned char addr1,unsigned char addr2,unsigned char addr3,unsigned char addr4);

	/**
	 * ctor for XPAddress that allows the structure to be constructed in a C++ fashion. The address component is initialised to be any incoming address.
	 */
	XPAddress(const int port);

	/**
	 * ctor that accepts a std::string in the format as Import().
	 */
	XPAddress(std::string addr);

	/**
	 * default ctor, no address types set.
	 */
	XPAddress();

	/**
	 * Exports the XPAddress as a std::string
	 * \return returns the std::string version of this XPAddress
	 */
	std::string Export(void) const;

	/**
	 * Imports an address string in IPv4 or IPv6 formats. For example:
	 * "127.0.0.1:80-[1000:a123:0000:0000:0000:0000:12ab:12ab]:80" or "127.0.0.1:80" or "[1234::1bcd:4321:1234:43bc]:80"
	 * \param address The address in x.x.x.x:x format.
	 * \return true is a successful import, false indicates there was a formatting error within the string.
	 */
	bool Import(const std::string address);

	/**
	 * Sets this address to be for any incoming address with an optional port number.
	 */
	void MakeAny(const int port = XPSOCK_PORT_ANY);

	/**
	 *	\return Returns XPAddress(0,0,0,0,0)
	 */
	static XPAddress Null(void);

	/**
	 * \return Returns true if this XPAddress is localhost for IPv6 or IPv6
	 */
	bool IsLocalhost(void) const;

	int port;										/**< The IPv4 port number for this address */
	unsigned char addr[4];							/**< The IPv4 addr for this structure */

	int port6;										/**< The IPv6 port number for this address */
	unsigned char addr6[16];						/**< The IPv6 addr for this structure */

	bool mIPv4;
	bool mIPv6;

	// Useful operators to compare addresses
	bool operator== (const XPAddress& b) const;
	bool operator!= (const XPAddress& b) const;
	// Can be used with STL less<> tests
	bool operator< (const XPAddress& b) const;

};

typedef struct XPAddress t_XPAddress;
typedef struct XPAddress XPAddress;

void operator<< (RNReplicaNet::DynamicMessageHelper &message,const XPAddress &a);
void operator>> (RNReplicaNet::DynamicMessageHelper &message,XPAddress &a);

void SetAddrV4(DynamicMessageHelper &message,const XPAddress &addr);
void GetAddrV4(DynamicMessageHelper &message,XPAddress &addr);

typedef struct s_XPSocket t_XPSocket;

typedef struct s_XPSocket
{
	int type;										/**< Type of the socket */
	int state;										/**< State of the socket */
	int total_sent;									/**< Total amount of data sent in bytes from this socket */
	int total_received;								/**< Total amount of data received in bytes to this socket */
	int total_accepts;								/**< Total number of accepts this socket has processed if it is a listen socket */
	int total_connects;								/**< Total number of connects for this socket */
	int urgent;										/**< Flagged with TRUE if it is an urgent socket */
	t_XPAddress addr;								/**< Address of external connection for this socket */
#ifdef _WIN64
	unsigned __int64 wins_socket;					/**< For OS socket identifier/handle */
	unsigned __int64 wins_socket6;					/**< For OS socket6 identifier/handle */
#else
	int wins_socket;								/**< For OS socket identifier/handle */
	int wins_socket6;								/**< For OS socket6 identifier/handle */
#endif
	t_XPSocket *loopback_socket;					/**< Pointer to the internal loop-back socket if the socket is in local loopback mode */
	char *loopback_recvbuffer;						/**< Pointer to the receive buffer */
	int loopback_recvpos;							/**< Write position in the receive buffer */
	int	loopback_readpos;							/**< Read position in the receive buffer */
	int loopback_pending;							/**< Pending */
	int port;										/**< The port number */
	int port6;										/**< The port number */
	int mCounter;									/**< Incremented every receive, regardless if there was an error or not */
	t_XPAddress mCachedAddress;
	bool mCachedAddressGot;
} t_XPSocket;									/**< The socket structure */

typedef struct s_XPErrorTranslate
{
	int error;										/**< Error number */
	const char *text;										/**< Short error text */
	const char *longtext;									/**< Long error text */
} t_XPErrorTranslate;							/**< A structure to use for storing errors */

/* Macros to make life easier */
#define XPSOCK_PARANOID(x)	if ((x) == NULL || (x) == pXPSOCK_EERROR)\
	{\
		asocket_lasterror = XPSOCK_ENOTASOCKET;\
		return XPSOCK_EERROR;\
	}


#define XPSOCK_S_PARANOID(x)	if ((x) == NULL || (x) == pXPSOCK_EERROR)\
	{\
		asocket_lasterror = XPSOCK_ENOTASOCKET;\
		return (t_XPSocket *) XPSOCK_EERROR;\
	}

} // namespace RNXPSockets

// Return error values are either pXPSOCK_EERROR, XPSOCK_EERROR or pXPSOCK_EOK

/* All the global functions */
extern int XPSock_Init(void);
extern int XPSock_Quit(void);
extern RNReplicaNet::t_XPSocket *XPSock_Create(void);
extern int XPSock_SetSendBuffer(RNReplicaNet::t_XPSocket * /*socket*/,int /*size*/);
extern int XPSock_SetRecvBuffer(RNReplicaNet::t_XPSocket * /*socket*/,int /*size*/);
extern int XPSock_Connect(RNReplicaNet::t_XPSocket * /*socket*/,const RNReplicaNet::t_XPAddress * /*addr*/);
extern int XPSock_Send(RNReplicaNet::t_XPSocket * /*socket*/,const char * /*data*/,const int /*len*/);
extern int XPSock_Recv(RNReplicaNet::t_XPSocket * /*socket*/,char * /*data*/,int /*maxlen*/,int /*flag*/);
extern int XPSock_Close(RNReplicaNet::t_XPSocket * /*socket*/);
extern int XPSock_Listen(RNReplicaNet::t_XPSocket * /*socket*/,int /*port*/);
extern int XPSock_Listen(RNReplicaNet::t_XPSocket * /*socket*/,const RNReplicaNet::t_XPAddress * /*addr*/);
extern RNReplicaNet::t_XPSocket *XPSock_Accept(RNReplicaNet::t_XPSocket * /*socket*/);

extern int XPSock_GetAddress(RNReplicaNet::t_XPSocket * /*socket*/,RNReplicaNet::t_XPAddress * /*addr*/);
extern int XPSock_GetPeerAddress(RNReplicaNet::t_XPSocket * /*socket*/,RNReplicaNet::t_XPAddress * /*addr*/);

extern RNReplicaNet::t_XPSocket *XPSock_UrgentCreate(int /*port*/);
extern RNReplicaNet::t_XPSocket *XPSock_UrgentCreate(const RNReplicaNet::t_XPAddress * /*addr*/);
extern int XPSock_UrgentMakeBroadcast(RNReplicaNet::t_XPSocket * /*socket*/);
extern int XPSock_UrgentSend(RNReplicaNet::t_XPSocket * /*socket*/,const char * /*data*/,int /*len*/,const RNReplicaNet::t_XPAddress * /*addr*/);
extern int XPSock_UrgentRecv(RNReplicaNet::t_XPSocket * /*socket*/,char *const /*data*/,int /*maxlen*/,int /*flag*/,RNReplicaNet::t_XPAddress *const /*addr*/);

extern RNReplicaNet::t_XPSocket *XPSock_ICMPCreate(void);
extern int XPSock_ICMPSendPing(RNReplicaNet::t_XPSocket * /*socket*/,const RNReplicaNet::t_XPAddress * /*addr*/,unsigned short *retID,unsigned short *retSeq,unsigned int *retTimeSent);
extern int XPSock_ICMPRecvPing(RNReplicaNet::t_XPSocket * /*socket*/,RNReplicaNet::t_XPAddress *const /*addr*/,unsigned short *retID,unsigned short *retSeq,unsigned int *retTimeElapsed);
extern int XPSock_ICMPWaitForPing(RNReplicaNet::t_XPSocket * socket,const float timeoutSeconds = 1.0f);

extern int XPSock_GetSocketSelectMaxSize(void);
extern int XPSock_SocketSelect(RNReplicaNet::t_XPSocket **sockets,const int numSockets, const int milliseconds);

extern const char *XPSock_TranslateError(int /*errno*/);
extern const char *XPSock_TranslateErrorLong(int /*errno*/);

extern int XPSock_GetHostName(char * /*name*/,int /*name_len*/);
extern int XPSock_GetHostAddress(RNReplicaNet::XPAddress * /*addr*/);
extern int XPSock_Resolve(RNReplicaNet::t_XPAddress * /*resolve*/,const char * /*input*/);

extern void XPSock_Poll(void);

extern void XPSock_SentHistogram(unsigned int * /*classes*/,int /*numClasses*/);
extern void XPSock_RecvHistogram(unsigned int * /*classes*/,int /*numClasses*/);

extern int XPSock_GetHardwareStatus(void);

// The global variables
extern int asocket_lasterror;						/**< This contains the last error value returned from any of these functions */

/* Extern the counters */
extern int	asocket_packets_got;					/**< Total number of packets received */
extern int	asocket_packets_sent;					/**< Total number of packets sent */
extern int	asocket_packets_got_size;				/**< Total byte size of packets received */
extern int	asocket_packets_sent_size;				/**< Total byte size of packets sent */

extern void XPSock_UpdatePerSecondMetrics(void);	/**< This is automatically called by other ReplicaNet modules using the XPURL Transport system */
extern float	asocket_packets_got_per_second;		/**< Total number of packets received per second */
extern float	asocket_packets_sent_per_second;	/**< Total number of packets sent per second */
extern float	asocket_packets_got_size_per_second;	/**< Total number of packets size received per second */
extern float	asocket_packets_sent_size_per_second;	/**< Total number of packets size sent per second */

// Only use these if you know exactly what you are doing
extern bool gXPSock_PacketLossEmulation;
extern float gXPSock_PacketLossEmulationPercent;
extern bool gXPSock_PacketLatencyEmulation;
extern float gXPSock_PacketLatencyEmulationBase;
extern float gXPSock_PacketLatencyEmulationJitter;
extern int gXPSock_UDPProtoParameter;

#endif

