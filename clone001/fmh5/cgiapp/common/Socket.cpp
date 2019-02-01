/*
 * Socket.cpp
 *
 *  Created on: 2011-9-16
 *      Author: dada
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "Socket.h"

CSocket::CSocket()
{
	m_socket = -1;
}

CSocket::CSocket(int socket)
{
	m_socket = socket;
}

CSocket::~CSocket()
{
	Close();
}

bool CSocket::Create(unsigned int port, int type, const char *address)
{
	if(!Socket(type))
	{
		return false;
	}
	if(!Bind(port, address))
	{
		Close();
		return false;
	}
	return true;
}

bool CSocket::Close()
{
	if(m_socket == -1)
	{
		return true;
	}
	int result = close(m_socket);
	m_socket = -1;
	return result != -1;
}

bool CSocket::Socket(int type, int protocol, int addressFormat)
{
	m_socket = socket(addressFormat, type, protocol);
	return m_socket != -1;
}

bool CSocket::Bind(unsigned int port, const char *address)
{
	sockaddr_in sockAddr;
	if(!SetSockAddress(&sockAddr, address, port))
	{
		return false;
	}
	return Bind((sockaddr *)&sockAddr, sizeof(sockaddr));
}

bool CSocket::Bind(const sockaddr *sockAddr, int addressLength)
{
	return bind(m_socket, sockAddr, addressLength) != -1;
}

bool CSocket::Listen(int maxConnection)
{
	return listen(m_socket, maxConnection) != -1;
}

bool CSocket::Accept(CSocket *connectSocket, sockaddr *connectAddress, int *addressLength)
{
	int sockTemp = Accept(connectAddress, addressLength);
	connectSocket->Attach(sockTemp);
	return sockTemp != -1;
}

int CSocket::Accept(sockaddr *connectAddress, int *addressLength)
{
	socklen_t lengthTemp = (socklen_t)*addressLength;
	int sock = accept(m_socket, connectAddress, &lengthTemp);
	*addressLength = (int)lengthTemp;
	return sock;
}

bool CSocket::Connect(unsigned int port, const char *address)
{
	sockaddr_in sockAddr;
	if(!SetSockAddress(&sockAddr, address, port))
	{
		return false;
	}
	return Connect((sockaddr *)&sockAddr, sizeof(sockaddr));
}

bool CSocket::Connect(const sockaddr*sockAddr, int addressLength)
{
	return connect(m_socket, sockAddr, addressLength) != -1;
}

int CSocket::Send(const void *buffer, int length, int flags)
{
	return send(m_socket, buffer, length, flags);
}

int CSocket::SendTo(const void *buffer, int length, unsigned int port, const char *address, int flags)
{
	sockaddr_in sockAddr;
	if(!SetSockAddress(&sockAddr, address, port))
	{
		return false;
	}
	return SendTo(buffer, length, (sockaddr *)&sockAddr, sizeof(sockaddr), flags);
}

int CSocket::SendTo(const void *buffer, int length, const sockaddr *sockAddr, int addressLength, int flags)
{
	return sendto(m_socket, buffer, length, flags, sockAddr, addressLength);
}

int CSocket::Receive(void *buffer, int length, int flags)
{
	return recv(m_socket, buffer, length, flags);
}

int CSocket::ReceiveFrom(void *buffer, int length, sockaddr *connectAddress, int *addressLength, int flags)
{
	socklen_t lengthTemp = (socklen_t)*addressLength;
	length = recvfrom(m_socket, buffer, length, flags, connectAddress, &lengthTemp);
	*addressLength = (int)lengthTemp;
	return length;
}

bool CSocket::Shutdown(int how)
{
	return shutdown(m_socket, how);
}

bool CSocket::SetSockOpt(int level, int optName, void *optVal, int optLen)
{
	return setsockopt(m_socket, level, optName, optVal, (socklen_t)optLen) != -1;
}

bool CSocket::GetSockOpt(int level, int optName, void *optVal, int *optLen)
{
	socklen_t len = (socklen_t)*optLen;
	int nResult = getsockopt(m_socket, level, optName, optVal, &len);
	*optLen = (int)len;
	return nResult != -1;
}

int CSocket::GetSocket()
{
	return m_socket;
}

int CSocket::GetLastError()
{
	return errno;
}

bool CSocket::IsClosed()
{
	return m_socket == -1;
}

bool CSocket::Attach(int socket)
{
	if(!Close())
	{
		return false;
	}
	m_socket = socket;
	return true;
}

int CSocket::Detach()
{
	int socket = m_socket;
	m_socket = -1;
	return socket;
}

bool CSocket::SetReuseAddr(bool reuse)
{
	int valReuse = reuse ? 1 : 0;
	return SetSockOpt(SOL_SOCKET, SO_REUSEADDR, &valReuse, sizeof(valReuse));
}

bool CSocket::SetBlock(bool block)
{
	int flag = fcntl(m_socket, F_GETFL, 0);
	if(block)
	{
		flag &= (~O_NONBLOCK);
		flag &= (~O_NDELAY);
	}
	else
	{
		flag |= O_NONBLOCK;
		flag |= O_NDELAY;
	}
	return fcntl(m_socket, F_SETFL, flag) != -1;
}

bool CSocket::SetBufferSize(int size)
{
	if(!SetSockOpt(SOL_SOCKET, SO_RCVBUF, &size, sizeof(size)))
	{
		return false;
	}
	if(!SetSockOpt(SOL_SOCKET, SO_SNDBUF, &size, sizeof(size)))
	{
		return false;
	}
	return true;
}

bool CSocket::SetSockAddress(sockaddr_in *sockAddr, const char *address, unsigned int port)
{
	if(address == NULL)
	{
		sockAddr->sin_addr.s_addr = htonl(INADDR_ANY);	//Linux support. In Windows use INADDR_LOOPBACK
	}
	else
	{
		sockAddr->sin_addr.s_addr = inet_addr(address);
		if (sockAddr->sin_addr.s_addr == INADDR_NONE)
		{
			hostent *phost;
			phost = gethostbyname(address);
			if (phost != NULL)
			{
				sockAddr->sin_addr.s_addr = ((in_addr *)phost->h_addr)->s_addr;
			}
			else
			{
				errno = EINVAL;
				return false;
			}
		}
	}
	sockAddr->sin_family = AF_INET;
	sockAddr->sin_port = htons((unsigned short)port);
	return true;
}

