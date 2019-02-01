/*
 * Socket.h
 *
 *  Created on: 2011-9-16
 *      Author: dada
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define DEFAULT_MAX_TCP_CONN 100

class CSocket
{
public:
	CSocket();
	CSocket(int socket);
	~CSocket();

public:
	bool Create(unsigned int port = 0, int type = SOCK_STREAM, const char *address = NULL);
	bool Close();
	bool Socket(int type = SOCK_STREAM, int protocol = 0, int addressFormat = AF_INET);
	bool Bind(unsigned int port, const char *address = NULL);
	bool Bind(const sockaddr *sockAddr, int addressLength);
	bool Listen(int maxConnection = DEFAULT_MAX_TCP_CONN);
	bool Accept(CSocket *connectSocket, sockaddr *connectAddress, int *addressLength);
	int Accept(sockaddr *connectAddress, int *addressLength);
	bool Connect(unsigned int port, const char *address = NULL);
	bool Connect(const sockaddr*sockAddr, int addressLength);
	int Send(const void *buffer, int length, int flags = 0);
	int SendTo(const void *buffer, int length, unsigned int port, const char *address = NULL, int flags = 0);
	int SendTo(const void *buffer, int length, const sockaddr *sockAddr, int addressLength, int flags = 0);
	int Receive(void *buffer, int length, int flags = 0);
	int ReceiveFrom(void *buffer, int length, sockaddr *connectAddress, int *addressLength, int flags = 0);
	bool Shutdown(int how);
	bool SetSockOpt(int level, int optName, void *optVal, int optLen);
	bool GetSockOpt(int level, int optName, void *optVal, int *optLen);
	int GetSocket();
	int GetLastError();
	bool IsClosed();

	bool Attach(int socket);
	int Detach();

	bool SetReuseAddr(bool reuse);
	bool SetBlock(bool block);
	bool SetBufferSize(int size);

protected:
	bool SetSockAddress(sockaddr_in *sockAddr, const char *address, unsigned int port);

private:
	int m_socket;
};

#endif /* SOCKET_H_ */
