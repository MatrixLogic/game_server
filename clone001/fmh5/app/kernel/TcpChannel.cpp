/*
 * TcpChannel.cpp
 *
 *  Created on: 2012-2-10
 *      Author: dada
 */

#include "TcpChannel.h"
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "TcpServer.h"

#define TCP_SERVER_LISTEN_CONNCETIONS 100
#define TCP_CHANNEL_ACTIVE_TIMEOUT	60

CInternetAddress::CInternetAddress() :
	Port(0)
{
}

CInternetAddress::CInternetAddress(const char *ip, uint16_t port) :
	IP(ip),
	Port(port)
{
}

CInternetAddress &CInternetAddress::operator=(const CInternetAddress &address)
{
	IP = address.IP;
	Port = address.Port;
	return *this;
}

void CInternetAddress::Clear()
{
	IP.clear();
	Port = 0;
}

bool CInternetAddress::FromString(const string &sAddress)
{
	vector<string> vecAddress;
	CBasic::StringSplitTrim(sAddress, ":", vecAddress);
	if(vecAddress.size() != 2)
	{
		return false;
	}
	unsigned port;
	if(!Convert::StringToUInt(port, vecAddress[1]))
	{
		return false;
	}
	if(port > UINT16_MAX)
	{
		return false;
	}
	IP = vecAddress[0];
	Port = port;
	return true;
}

bool CInternetAddress::FromSockAddr(const void *pSockAddr)
{
	if(pSockAddr == NULL)
	{
		return false;
	}
	const sockaddr_in *pSockAddrIn = (sockaddr_in *)pSockAddr;
	Port = ntohs(pSockAddrIn->sin_port);
	char acIp[40];
	if(inet_ntop(pSockAddrIn->sin_family, &pSockAddrIn->sin_addr, acIp, sizeof(acIp)) == NULL)
	{
		return false;
	}
	acIp[sizeof(acIp) - 1] = '\0';
	IP = acIp;
	return true;
}

string CInternetAddress::ToString() const
{
	string s;
	String::Format(s, "%s:%hu", IP.c_str(), Port);
	return s;
}

#define ASSERT_CALL_SOCKET(call)	\
	if(!m_socket.call)	\
	{	\
		m_socket.Close();	\
		return false;	\
	}	\

CTcpServer* CTcpChannel::m_pServer=NULL;
	
CTcpChannel::CTcpChannel() :
	m_channelId(-1),
	m_flag(0),
	m_lastActiveTime(0)
{
}

CTcpChannel::~CTcpChannel()
{
}

int CTcpChannel::GetChannelId()
{
	return m_channelId;
}

void CTcpChannel::SetServer(CTcpServer * pServer){
	m_pServer = pServer;
}

bool CTcpChannel::CreateServer(int channelId, const CInternetAddress &listenAddress)
{
	Close();
	ASSERT_CALL_SOCKET(Socket(SOCK_STREAM))
	ASSERT_CALL_SOCKET(SetReuseAddr(true))
//	ASSERT_CALL_SOCKET(SetBufferSize(TCP_CHANNEL_BUFFER_SIZE))
	ASSERT_CALL_SOCKET(Bind(listenAddress.Port, listenAddress.IP.c_str()))
	ASSERT_CALL_SOCKET(SetBlock(false))
	ASSERT_CALL_SOCKET(Listen(TCP_SERVER_LISTEN_CONNCETIONS))
	m_channelId = channelId;
	m_localAddress = listenAddress;
	m_lastActiveTime = Time::GetGlobalTime();
	return true;
}

bool CTcpChannel::CreateClient(int channelId, const CInternetAddress &serverAddress, const CInternetAddress *bindAddress)
{
	Close();
	ASSERT_CALL_SOCKET(Socket(SOCK_STREAM))
	ASSERT_CALL_SOCKET(SetReuseAddr(true))
	if(bindAddress != NULL)
	{
		ASSERT_CALL_SOCKET(Bind(bindAddress->Port, bindAddress->IP.c_str()))
		m_localAddress = *bindAddress;
	}
	ASSERT_CALL_SOCKET(SetBufferSize(TCP_CHANNEL_BUFFER_SIZE))
	ASSERT_CALL_SOCKET(Connect(serverAddress.Port, serverAddress.IP.c_str()))
	ASSERT_CALL_SOCKET(SetBlock(false))
	m_channelId = channelId;
	m_remoteAddress = serverAddress;
	m_lastActiveTime = Time::GetGlobalTime();
	return true;
}

bool CTcpChannel::Accept(int channelId, CTcpChannel &listenChannel)
{
	Close();
	sockaddr_in sockAddr;
	int sockAddrLength = sizeof(sockaddr_in);
	if(!listenChannel.m_socket.Accept(&m_socket, (sockaddr *)&sockAddr, &sockAddrLength))
	{
		m_socket.Close();
		return false;
	}
	ASSERT_CALL_SOCKET(SetBlock(false))
	int iNoDelay = 1;
	ASSERT_CALL_SOCKET(SetSockOpt(IPPROTO_TCP, TCP_NODELAY, &iNoDelay, sizeof(iNoDelay)))
	ASSERT_CALL_SOCKET(SetBufferSize(TCP_CHANNEL_BUFFER_SIZE))
	m_channelId = channelId;
	m_localAddress = listenChannel.m_localAddress;
	m_remoteAddress.FromSockAddr(&sockAddr);
	m_lastActiveTime = Time::GetGlobalTime();
	return true;
}

bool CTcpChannel::RealSend(){
	if(m_socket.IsClosed())
	{
		return false;
	}

	m_lastActiveTime = Time::GetGlobalTime();
	int byteLeft = m_sendBuffer.GetSize();
	if(byteLeft == 0){
		m_pServer->EnableOutput(this,false);
		return true;
	}

	int byteSent = m_socket.Send(m_sendBuffer.GetConstBuffer(), byteLeft);

	if(byteSent > 0){
		//debug_log("[%s:%d,%d]",m_remoteAddress.ToString().c_str(),byteLeft,byteSent);
		m_sendBuffer.Remove(0, byteSent);
	}

	if(byteSent < byteLeft)
	{
		error_log("send data size = %d, left size = %u, time = %u",byteSent,byteLeft,m_lastActiveTime);
	}
//		byteLeft -= byteSent;


	return true;
}

bool CTcpChannel::SendData(const IBuffer *pBuffer)
{
	if(m_socket.IsClosed())
	{
		return false;
	}

	if(pBuffer == NULL){
		return true;
	}
	
//	debug_log("send data\n%u",pBuffer->GetSize());
	bool ret = m_sendBuffer.AppendBuffer(pBuffer);
	if(ret){
		ret = m_pServer->EnableOutput(this);
	}
	else{
		error_log("send buffer append failed");
	}
	return ret;
	
	//发送滞留缓冲区数据
	int byteLeft = m_sendBuffer.GetSize();
	int sendPos = 0;
	while(byteLeft > 0)
	{
		int byteSent = m_socket.Send(m_sendBuffer.GetConstBuffer() + sendPos, byteLeft);
		if(byteSent > 0)
		{
			byteLeft -= byteSent;
			sendPos += byteSent;
			m_lastActiveTime = Time::GetGlobalTime();
		}
		else if(byteSent < 0)
		{
			int lastError = m_socket.GetLastError();
			if(lastError == EINTR)
			{
				error_log("[Send error][error=EINTR,channel=%d,fd=%d]", m_channelId, m_socket.GetSocket());
			}
			else if(lastError == EAGAIN)
			{
				if(sendPos > 0)
				{
					m_sendBuffer.Remove(0, sendPos);
				}
				if(pBuffer == NULL)
				{
					return true;
				}
				return m_sendBuffer.AppendBuffer(pBuffer);
			}
			else
			{
				error_log("[Send error][error=%d,channel=%d,fd=%d]", lastError, m_channelId, m_socket.GetSocket());
				return false;
			}
		}
		else
		{
			error_log("[Send error][socket=%d,bytesent=0]",m_socket.GetSocket());
		}
	}
	if(sendPos > 0)
	{
		m_sendBuffer.Clear();
	}

	if(pBuffer == NULL)
	{
		return true;
	}
	byteLeft = pBuffer->GetSize();
	sendPos = 0;
	while(byteLeft > 0)
	{
		int byteSent = m_socket.Send(pBuffer->GetConstBuffer() + sendPos, byteLeft);
		if(byteSent > 0)
		{
			byteLeft -= byteSent;
			sendPos += byteSent;
			m_lastActiveTime = Time::GetGlobalTime();
		}
		else if(byteSent < 0)
		{
			int lastError = m_socket.GetLastError();
			if(lastError == EINTR)
			{
				error_log("[Send error][error=EINTR,channel=%d,fd=%d]", m_channelId, m_socket.GetSocket());
			}
			else if(lastError == EAGAIN)
			{
				//系统发送缓冲区已满，放进滞留缓冲区
				error_log("send buff full length= %d",pBuffer->GetSize() - sendPos);
				return m_sendBuffer.Append(pBuffer->GetConstBuffer() + sendPos, pBuffer->GetSize() - sendPos);
			}
			else
			{
				if(lastError != ECONNRESET)
				{
					error_log("[Send error][error=%d,channel=%d,fd=%d]", lastError, m_channelId, m_socket.GetSocket());
				}
				return false;
			}
		}
	}
	return true;
}

bool CTcpChannel::ReceiveData()
{
	if(m_socket.IsClosed())
	{
		return false;
	}

	int byteRecv = 0;
	do
	{
		byteRecv = m_recvBuffer.GetFreeCapacity();
		if(byteRecv <= 0)
		{
			return true;
		}
		byteRecv = m_socket.Receive(m_recvBuffer.GetNativeBuffer() + m_recvBuffer.GetSize(), byteRecv);
		if(byteRecv > 0)
		{
			m_recvBuffer.SetSize(m_recvBuffer.GetSize() + byteRecv);
			m_lastActiveTime = Time::GetGlobalTime();
		}
		else if(byteRecv == 0)
		{
			error_log("[Receive error] 0");
			return false;
		}
		else
		{
			int lastError = m_socket.GetLastError();
			if(lastError == EINTR)
			{
				error_log("[Receive error][error=EINTR,channel=%d,fd=%d]", m_channelId, m_socket.GetSocket());
			}
			else if(lastError == EAGAIN)
			{
				return true;
			}
			else
			{
				if(lastError != ECONNRESET)
				{
					error_log("[Receive error][error=%d,channel=%d,fd=%d]", lastError, m_channelId, m_socket.GetSocket());
				}
				error_log("[Receive error][error=%d,channel=%d,fd=%d]", lastError, m_channelId, m_socket.GetSocket());

				return false;
			}
		}
	}while(false);
	return true;
}

IBuffer *CTcpChannel::GetReceiveData()
{
	return &m_recvBuffer;
}

bool CTcpChannel::SetDataRead(uint32_t byteRead)
{
	return m_recvBuffer.Remove(0, byteRead);
}

bool CTcpChannel::Close()
{
	error_log("channel %d closed",m_channelId);

	m_socket.Close();
	m_channelId = -1;
	m_flag = 0;
	m_lastActiveTime = 0;
	m_localAddress.Clear();
	m_remoteAddress.Clear();
	m_sendBuffer.Clear();
	m_recvBuffer.Clear();
	m_sendBuffer.Reset();
	m_recvBuffer.Reset();
	return true;
}

bool CTcpChannel::IsClosed() const
{
	return m_socket.IsClosed();
}

bool CTcpChannel::IsActive() const
{
	return true;
	//return m_lastActiveTime + TCP_CHANNEL_ACTIVE_TIMEOUT > Time::GetGlobalTime();
}

int CTcpChannel::GetSocket() const
{
	return m_socket.GetSocket();
}

int CTcpChannel::GetLastError() const
{
	return m_socket.GetLastError();
}

string CTcpChannel::ToString() const
{
	string s;
	String::Format(s, "ChannelId=%d&Local=%s&Remote=%s", m_channelId,
			m_localAddress.ToString().c_str(), m_remoteAddress.ToString().c_str());
	return s;
}

const CInternetAddress *CTcpChannel::GetRemoteAddress() const
{
	return &m_remoteAddress;
}

