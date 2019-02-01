/*
 * BattleSocket.cpp
 *
 *  Created on: 2016-9-17
 *      Author: Ralf
 */


#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "BattleSocket.h"
#include "ErrorUtil.h"

map<unsigned, BattleSocket*> BattleSocket::m_map;

int BattleSocket::Send(unsigned server, CSG17Packet* packet)
{
	server = MainConfig::GetMergedDomain(server);
	if(!m_map.count(server))
		m_map[server] = new BattleSocket(server);
	if(m_map[server]->IsClosed() && m_map[server]->Connect() != 0)
		return R_ERROR;
	return m_map[server]->Send(packet);
}
int BattleSocket::Receive(unsigned server, CSG17Packet* packet)
{
	server = MainConfig::GetMergedDomain(server);
	if(!m_map.count(server) || m_map[server]->IsClosed())
		return R_ERROR;
	return m_map[server]->Receive(packet);
}

int BattleSocket::Connect()
{
	if (!socket.Socket(SOCK_STREAM, 0, AF_INET))
	{
		error_log("create socket error");
		return R_ERROR;
	}

	Config::SelectDomain(serverid, serverid);
	string tcpserver = Config::GetValue("server_listen");
	size_t pos = tcpserver.find(':');
	string host = tcpserver.substr(0, pos);
	string port = tcpserver.substr(pos + 1, tcpserver.length() - pos - 1);

	struct sockaddr_in server;
	server.sin_addr.s_addr = inet_addr(host.c_str());
	server.sin_port = htons(atoi(port.c_str()));
	server.sin_family = AF_INET;
	memset(server.sin_zero, 0x00, sizeof(server.sin_zero));

	if (!socket.Connect((struct sockaddr *) &server,sizeof(struct sockaddr)))
	{
		error_log("connect socket error,addr = %s, port=%s,errno = %d, errmsg = %s", host.c_str(),port.c_str(),errno, strerror(errno));
		socket.Close();
		return R_ERR_REFUSE;
	}

	struct timeval timeout;
	timeout.tv_sec = 3;
	timeout.tv_usec = 0;
	if(!socket.SetSockOpt(SOL_SOCKET, SO_SNDTIMEO, (char *) &timeout, sizeof(struct timeval))
	|| !socket.SetSockOpt(SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(struct timeval))
	|| !socket.SetBlock(true))
	{
		error_log("set socket wrong");
		socket.Close();
		return R_ERR_REFUSE;
	}

	return 0;
}
int BattleSocket::Send(CSG17Packet* packet)
{
	CBuffer buffer(SG17_MAX_PACKET_SIZE);
	if(!packet->Encode(&buffer))
	{
		error_log(" packet encode failed");
		return R_ERR_DATA;
	}
	if(!(socket.Send(buffer.GetConstBuffer(), buffer.GetSize(), 0)))
	{
		error_log("send socket error");
		socket.Close();
		return R_ERR_REFUSE;
	}
	return 0;
}
int BattleSocket::Receive(CSG17Packet* packet)
{
	CBuffer buffer(SG17_MAX_PACKET_SIZE);
	unsigned int recvsize = 0;
	recvsize = socket.Receive(buffer.GetNativeBuffer(), buffer.GetFreeCapacity());
	if (recvsize < 0)
	{
		error_log("receive socket error");
		socket.Close();
		return R_ERR_REFUSE;
	}
	buffer.SetSize(recvsize);

	if(!packet->Decode(&buffer))
	{
		error_log("cpacket decode error");
		socket.Close();
		return R_ERR_DATA;
	}

	return 0;
}
