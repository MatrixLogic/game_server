/*
 * TcpServer.cpp
 *
 *  Created on: 2012-2-9
 *      Author: dada
 */

#include "TcpServer.h"
#include <sys/epoll.h>

#define EPOLL_WAIT_TIMEOUT 10
#define TCP_SERVER_STAT_INTERVAL 60

IServer::~IServer(){

}

CTcpServer::CTcpServer() :
	m_bRunning(false),
	m_listenCount(0),
	m_maxConn(0),
	m_pChannels(NULL),
	m_lastFreeChannelId(0),
	m_fdEpoll(-1)
{
}

CTcpServer::~CTcpServer()
{
	Close();
}

bool CTcpServer::Initialize(const vector<CInternetAddress> &listenList, int maxConn)
{
	m_listenList = listenList;
	m_listenCount = m_listenList.size();
	m_maxConn = maxConn;

	if(m_listenCount <= 0)
	{
		return false;
	}

	//init epoll
	int maxChannel = m_listenCount + m_maxConn;
	m_fdEpoll = epoll_create(maxChannel);
	if(m_fdEpoll == -1)
	{
		fatal_log("[epoll_create fail][error=%d]", errno);
		return false;
	}

	//创建监听Socket
	m_pChannels = new CTcpChannel[maxChannel];
	CTcpChannel::SetServer(this);
	m_lastFreeChannelId = m_listenCount - 1;
	for(int i = 0; i < m_listenCount; i++)
	{
		if(!m_pChannels[i].CreateServer(i, m_listenList[i]))
		{
			fatal_log("[CreateServer fail][error=%d,ip=%s,port=%d]",
					m_pChannels[i].GetLastError(), m_listenList[i].IP.c_str(), (int)m_listenList[i].Port);
			close(m_fdEpoll);
			return false;
		}
		epoll_event evListen;
		evListen.data.ptr = &m_pChannels[i];
		evListen.events = EPOLLIN | EPOLLERR | EPOLLHUP;
		if(epoll_ctl(m_fdEpoll, EPOLL_CTL_ADD, m_pChannels[i].GetSocket(), &evListen) == -1)
		{
			fatal_log("[epoll_ctl add fail][type=listen,error=%d]", errno);
			close(m_fdEpoll);
			return false;
		}
	}

	return true;
}

bool CTcpServer::EnableOutput(CTcpChannel * pChannel, bool flag ){
	epoll_event ev;
	ev.data.ptr = pChannel;
	if(flag){
		ev.events = EPOLLIN | EPOLLERR | EPOLLHUP |  EPOLLOUT;
	}
	else{
		ev.events = EPOLLIN | EPOLLERR | EPOLLHUP;
	}

	if(epoll_ctl(m_fdEpoll, EPOLL_CTL_MOD, pChannel->GetSocket(), &ev) == -1)
	{
		fatal_log("EPOLL_CTL_MOD failed:[%d,%d] %m",flag,pChannel->GetSocket());
		return false;
	}

	return true;
}

bool CTcpServer::Run()
{
	int maxChannel = m_listenCount + m_maxConn;
	//Run
	epoll_event *events = new epoll_event[maxChannel];
	if(events == NULL)
	{
		fatal_log("[new epoll_event fail][error=%d]", errno);
		close(m_fdEpoll);
		return false;
	}
	//unsigned s_lastStatTime = 0;
	m_bRunning = true;
	while(m_bRunning)
	{
		int nReady = epoll_wait(m_fdEpoll, events, maxChannel, EPOLL_WAIT_TIMEOUT);
		Time::UpdateGlobalTime();
		if(nReady != -1)
		{
			if(nReady == 0){
				OnIdle();
				continue;
			}

			for(int i = 0; i < nReady; i++)
			{
				CTcpChannel *pChannel = (CTcpChannel *)events[i].data.ptr;
				if(pChannel == NULL)
				{
					error_log("[epoll data null][events=0x%08X]", events[i].events);
				}
				else if(pChannel->IsClosed())
				{
					error_log("[epoll socket closed][id=%d,events=0x%08X]", pChannel->GetSocket(), events[i].events);
				}
				else if(pChannel->GetChannelId() < m_listenCount)
				{
					//accept
					if((events[i].events & (EPOLLERR | EPOLLHUP)) != 0)
					{
						error_log("[listen socket error][id=%d,events=%d]", pChannel->GetChannelId(), events[i].events);
					}
					else
					{
						int acceptChannelId = GetFreeChannelId();
						if(acceptChannelId < 0)
						{
							fatal_log("[tcp connection overrun][maxconn=%d]", m_maxConn);
						}
						else
						{
							if(m_pChannels[acceptChannelId].Accept(acceptChannelId, *pChannel))
							{
								epoll_event evConnect;
								evConnect.data.ptr = &m_pChannels[acceptChannelId];
								evConnect.events = EPOLLIN | EPOLLERR | EPOLLHUP;	// | EPOLLET;
								if(epoll_ctl(m_fdEpoll, EPOLL_CTL_ADD, m_pChannels[acceptChannelId].GetSocket(), &evConnect) == -1)
								{
									error_log("[epoll_ctl add fail][type=accept,error=%d,fd=%d]", errno, m_pChannels[acceptChannelId].GetSocket());
									CloseChannel(acceptChannelId);
								}
								OnConnect(&m_pChannels[acceptChannelId]);
								info_log("ip=%s,accept,ChannelId=%d", m_pChannels[acceptChannelId].GetRemoteAddress()->ToString().c_str(),acceptChannelId);
							}
							else
							{
								error_log("[socket accept error][error=%d,id=%d,fd=%d]",
										m_pChannels[acceptChannelId].GetLastError(), acceptChannelId, m_pChannels[acceptChannelId].GetSocket());
							}
						}
					}
				}
				else
				{
					if(events[i].events & EPOLLIN){
						//recv
						if(pChannel->ReceiveData())
						{
							OnChannelReceive(pChannel);
						}
						else
						{
							error_log("ReceiveData failed");
							CloseChannel(pChannel->GetChannelId());
						}
					}

					if(events[i].events & EPOLLOUT){
						if(!pChannel->RealSend()){
							CloseChannel(pChannel->GetChannelId());
						}
					}
				}
			}
		}
		else if(errno == EINTR)
		{
			error_log("[epoll_wait fail][error=EINTR]");
		}
		else
		{
			error_log("[epoll_wait fail][error=%d,fd=%d]", errno, m_fdEpoll);
			m_bRunning = false;
		}
//		int connectCount = 0;
//		for(int i = m_listenCount; i < maxChannel; i++)
//		{
//			if(!m_pChannels[i].IsClosed())
//			{
//				if(m_pChannels[i].SendData(NULL))
//				{
//					if(m_pChannels[i].IsActive())
//					{
//						connectCount++;
//					}
//					else
//					{
//						CloseChannel(i);
//					}
//				}
//				else
//				{
//					CloseChannel(i);
//				}
//			}
//		}
//		if(s_lastStatTime + TCP_SERVER_STAT_INTERVAL <= Time::GetGlobalTime())
//		{
//			s_lastStatTime = Time::GetGlobalTime();
//			//OLUSER_LOG("%u|%d", s_lastStatTime, connectCount);
//		}
	}

	info_log("[TcpServer Run End]");

	for(int i = 0; i < maxChannel; i++)
	{
		CloseChannel(i);
	}
	close(m_fdEpoll);
	m_fdEpoll = -1;

	delete [] events;
	delete [] m_pChannels;

	return true;
}

bool CTcpServer::Close()
{
	m_bRunning = false;
	return true;
}

bool CTcpServer::CloseChannel(int channelId)
{
	if(channelId < 0 || channelId >= m_listenCount + m_maxConn)
	{
		return false;
	}
	CTcpChannel *pChannel = &m_pChannels[channelId];
	if(!pChannel->IsClosed())
	{
		if(!pChannel->GetReceiveData()->IsEmpty())
		{
			error_log("[channel_close_unexpect][id=%d,data=%s]", channelId, pChannel->GetReceiveData()->ToString().c_str());
		}
		epoll_event evDel;
		if(epoll_ctl(m_fdEpoll, EPOLL_CTL_DEL, pChannel->GetSocket(), &evDel) == -1)
		{
			error_log("[epoll_ctl del fail][error=%d,id=%d,fd=%d]", errno, channelId, pChannel->GetSocket());
		}
		pChannel->Close();
		OnChannelClose(channelId);
	}
	return true;
}

bool CTcpServer::SendData(CFirePacket *pHead, IPacketSend *pBody)
{
	if(pHead == NULL)
	{
		return false;
	}
	if(pHead->ChannelId < m_listenCount || pHead->ChannelId >= m_listenCount + m_maxConn)
	{
		return false;
	}
	if(m_pChannels[pHead->ChannelId].IsClosed())
	{
		error_log("[channel_closed][channelid=%d,fd=%u,time=%u,cmd=%u,body=%u]",
				pHead->ChannelId, pHead->fd, (int)pHead->time, pHead->cmd,pHead->body.GetSize());
		return false;
	}
	if(pBody != NULL)
	{
		if(!pBody->Encode(&pHead->body))
		{
			error_log("[channel_closed][channelid=%d,fd=%u,time=%u,cmd=%u,body=%u]",
							pHead->ChannelId, pHead->fd, (int)pHead->time, pHead->cmd,pHead->body.GetSize());
			return false;
		}
	}
	else
	{
		//DATA_LOG("tcp", m_pChannels[pHead->ChannelId].GetRemoteAddress()->ToString().c_str(), "send",
		//		"%s&Protocol&", pHead->ToString().c_str());
	}
	CBuffer sendBuffer(MAX_PACKET_SIZE);
	if(!pHead->Encode(&sendBuffer))
	{
		error_log("[channel_closed][channelid=%d,fd=%u,time=%u,cmd=%u,body=%u]",
						pHead->ChannelId, pHead->fd, (int)pHead->time, pHead->cmd,pHead->body.GetSize());
		return false;
	}
	if(!m_pChannels[pHead->ChannelId].SendData(&sendBuffer))
	{
		CloseChannel(pHead->ChannelId);
		return false;
	}
	return true;
}

void CTcpServer::OnChannelReceive(CTcpChannel *pChannel)
{
	if(pChannel == NULL)
	{
		return;
	}
	uint32_t decodeSize = 0;
	//debug_log("received data\n%s",pChannel->GetReceiveData()->ToString().c_str());
	do
	{
		CFirePacket packet;
		bool decodeSuccess = packet.Decode(pChannel->GetReceiveData());
		///TODO: decode size error auto close
		decodeSize = packet.GetDecodeSize();
		if(decodeSize > 0)
		{
			if(!decodeSuccess)
			{
				error_log("[Decode fail][channelId=%d,size=%u,packet=%s]",
						pChannel->GetChannelId(), pChannel->GetReceiveData()->GetSize(),
						pChannel->GetReceiveData()->ToString().c_str());
				pChannel->Close();
				decodeSize = 0;
				break;
			}
			pChannel->SetDataRead(decodeSize);
		}
		if(decodeSuccess)
		{
			//debug_log("decode success,bodyLen=%u",packet.bodyLen);
			packet.ChannelId  = pChannel->GetChannelId();

			OnReceive(&packet);
		}
	}while(decodeSize > 0);
	return ;
}


const CTcpChannel *CTcpServer::GetChannel(int channelId) const
{
	if(channelId < 0 || channelId >= m_listenCount + m_maxConn)
	{
		return NULL;
	}
	return &m_pChannels[channelId];
}

int CTcpServer::GetFreeChannelId()
{
	int maxChannel = m_listenCount + m_maxConn;
	for(int i = 0; i < m_maxConn; i++)
	{
		m_lastFreeChannelId++;
		if(m_lastFreeChannelId >= maxChannel)
		{
			m_lastFreeChannelId = m_listenCount;
		}
		if(m_pChannels[m_lastFreeChannelId].IsClosed())
		{
			return m_lastFreeChannelId;
		}
	}
	return -1;
}

bool CTcpServer::SendData(CFirePacket* packet){

	if(packet == NULL)
	{
		return false;
	}
	if(packet->ChannelId < m_listenCount || packet->ChannelId >= m_listenCount + m_maxConn)
	{
		return false;
	}
	if(m_pChannels[packet->ChannelId].IsClosed())
	{
		error_log("[channel_closed][channelid=%d,fd=%u,time=%u,cmd=%u,body=%u]",
				packet->ChannelId, packet->fd, (int)packet->time, packet->cmd,packet->body.GetSize());
		return false;
	}

	static CBuffer sendBuffer(MAX_PACKET_SIZE);
	if(!packet->Encode(&sendBuffer))
	{
		error_log("[channel_closed][channelid=%d,fd=%u,time=%u,cmd=%u,body=%u]",
				packet->ChannelId, packet->fd, (int)packet->time, packet->cmd,packet->body.GetSize());
		return false;
	}
	if(!m_pChannels[packet->ChannelId].SendData(&sendBuffer))
	{
		CloseChannel(packet->ChannelId);
		return false;
	}

	return true;
}

CTcpChannel* CTcpServer::GetSelfClientChannel(){
	int id = GetFreeChannelId();
	if(-1 == id){
		return NULL;
	}

	if(!m_pChannels[id].CreateClient(id,m_listenList[0])){
		error_log("connect %s failed",m_listenList[0].ToString().c_str());
		return NULL;
	}

	epoll_event evConnect;
	evConnect.data.ptr = &m_pChannels[id];
	evConnect.events = EPOLLIN | EPOLLERR | EPOLLHUP;	// | EPOLLET;
	if(epoll_ctl(m_fdEpoll, EPOLL_CTL_ADD, m_pChannels[id].GetSocket(), &evConnect) == -1)
	{
		error_log("[epoll_ctl add fail][type=accept,error=%d,fd=%d]", errno, m_pChannels[id].GetSocket());
		CloseChannel(id);
	}

	return &m_pChannels[id];
}

CTcpChannel* CTcpServer::GetRemoteClientChannel(const CInternetAddress& remote,int flag){
	int id = GetFreeChannelId();
	if(-1 == id){
		return NULL;
	}

	if(!m_pChannels[id].CreateClient(id,remote)){
		error_log("connect %s failed",remote.ToString().c_str());
		return NULL;
	}

	epoll_event evConnect;
	evConnect.data.ptr = &m_pChannels[id];
	evConnect.events = EPOLLIN | EPOLLERR | EPOLLHUP;	// | EPOLLET;
	if(epoll_ctl(m_fdEpoll, EPOLL_CTL_ADD, m_pChannels[id].GetSocket(), &evConnect) == -1)
	{
		error_log("[epoll_ctl add fail][type=accept,error=%d,fd=%d]", errno, m_pChannels[id].GetSocket());
		CloseChannel(id);
	}

	m_pChannels[id].setFlag(flag);
	return &m_pChannels[id];
}
