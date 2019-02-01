/*
 * TcpChannel.h
 *
 *  Created on: 2012-2-10
 *      Author: dada
 */

#ifndef TCPCHANNEL_H_
#define TCPCHANNEL_H_

#include "Common.h"

class CTcpServer;
class CInternetAddress
{
public:
	string IP;
	uint16_t Port;
	CInternetAddress();
	CInternetAddress(const char *ip, uint16_t port);
	CInternetAddress &operator =(const CInternetAddress &address);
	void Clear();
	bool FromString(const string &sAddress);
	bool FromSockAddr(const void *pSockAddr);
	string ToString() const;
};

//#define TCP_CHANNEL_SEND_BUFFER_SIZE 0x800000
#define TCP_CHANNEL_BUFFER_SIZE      0x800000	//8M
#define TCP_CHANNEL_DEFAULT_SIZE	 0x20000   //128K

class CTcpChannel
{
public:
	CTcpChannel();
	~CTcpChannel();
	int GetChannelId();
	bool CreateServer(int channelId, const CInternetAddress &listenAddress);
	bool CreateClient(int channelId, const CInternetAddress &serverAddress, const CInternetAddress *bindAddress = NULL);
	bool Accept(int channelId, CTcpChannel &listenChannel);
	bool SendData(const IBuffer *pBuffer = NULL);
	bool RealSend();
	bool ReceiveData();
	IBuffer *GetReceiveData();
	bool SetDataRead(uint32_t byteRead);	//璁剧疆鎺ユ敹鏁版嵁宸茶瀛楄妭鏁�
	bool Close();
	bool IsClosed() const;
	bool IsActive() const;
	int GetSocket() const;
	int GetLastError() const;
	string ToString() const;
	const CInternetAddress *GetRemoteAddress() const;
	static void SetServer(CTcpServer* pServer);
	void setFlag(int flag){m_flag = flag;}
	int getFlag(){return m_flag;}
private:
	int m_channelId;
	int m_flag;
	CSocket m_socket;
	unsigned m_lastActiveTime;
	CInternetAddress m_localAddress;
	CInternetAddress m_remoteAddress;
	CDynamicBuffer<TCP_CHANNEL_DEFAULT_SIZE>m_sendBuffer;
	CDynamicBuffer<TCP_CHANNEL_DEFAULT_SIZE>m_recvBuffer;
	//CStaticBuffer<TCP_CHANNEL_BUFFER_SIZE> m_sendBuffer;
	//CStaticBuffer<TCP_CHANNEL_BUFFER_SIZE> m_recvBuffer;
	static CTcpServer* m_pServer;
};

#endif /* TCPCHANNEL_H_ */
