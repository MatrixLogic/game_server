#include "kfc_client_socket.h"
#include <sys/time.h>
#include "Convert.h"

using namespace std;
using namespace kfc;

kfc::CClientSocketTcp::CClientSocketTcp(const string & strIp, int iPort, unsigned uTimeOut, int iRetryTimes)
{
	m_strIp = strIp;
	m_iPort = iPort;
	m_uTimeOut = uTimeOut;
	m_iRetryTimes = iRetryTimes;
	//checkAndConnect();
}

void kfc::CClientSocketTcp::setConfig(const string & strIp, int iPort, unsigned uTimeOut, int iRetryTimes)
{
	m_strIp = strIp;
	m_iPort = iPort;
	m_uTimeOut = uTimeOut;
	m_iRetryTimes = iRetryTimes;
}

void kfc::CClientSocketTcp::checkAndConnect()
{
	if(!CSocket::socket_is_ok())
	{
		create();
		int iFlags = fcntl(CSocket::fd(), F_GETFL, 0);			//保存flag
		try
		{
			CSocket::set_nonblock();
			CSimpleSocketTcp::connect(m_strIp, (port_t)m_iPort);
			fcntl(CSocket::fd(), F_SETFL, iFlags);			//恢复flag
			//return;
		}
		catch(kfc::socket_error& e)
		{
			if(errno == EINPROGRESS)		//连接还未完成
			{
				fd_set  fdsw;
				FD_ZERO(&fdsw);
				FD_SET(CSocket::fd(), &fdsw);
				struct timeval tv;
				tv.tv_sec = m_uTimeOut;
				tv.tv_usec = 0;
				int iRet;
				if((iRet = select(CSocket::fd()+1,NULL,&fdsw,NULL,&tv)) > 0)			//套接字可写
				{
					int error = 0;
					int len = sizeof(CSocket::fd());
					if(getsockopt(CSocket::fd(), SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len) < 0)
					{
						m_strError = string("connect error:") + strerror(errno);
						iRet = -1;
					}
					else
					{
						if(error != 0)
						{
							m_strError = "connect error";
							iRet = -1;
						}
						else
						{
							setsockopt(CSocket::fd(), SOL_SOCKET, SO_SNDTIMEO, (char*)&tv, sizeof(tv));
							setsockopt(CSocket::fd(), SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));
							fcntl(CSocket::fd(), F_SETFL, iFlags);			//恢复flag
						}
					}
				}
				else if(iRet == 0)
				{
					m_strError = "connect timeout";
				}
				else
				{
					m_strError = string("connect error:") + strerror(errno);
				}

				if (iRet <= 0)
				{
					fcntl(CSocket::fd(), F_SETFL, iFlags);			//恢复flag
					CSocket::close();
					throw kfc::socket_error(m_strError);
				}
			}
			else
			{
				fcntl(CSocket::fd(), F_SETFL, iFlags);			//恢复flag
				CSocket::close();
				throw e;
			}
		}
	}
}

int kfc::CClientSocketTcp::sendBuff(const char * sBuff, const unsigned uLen)
{
	unsigned  uByteLeft = uLen;
	int  iByteSent = 0;

	if(sBuff == NULL)
	{
		m_strError = "sendBuff: sBuff is null";
		return -1;
	}
	char *pBuff = (char *) sBuff;

	try
	{
		while(uByteLeft > 0)
		{
			iByteSent = CSocket::send(pBuff, uByteLeft);
			if(iByteSent <= 0)		//error
			{
				m_strError = string("sendBuff: send error:") + strerror(errno);
				CSocket::close();
				return -2;
			}
			uByteLeft -= iByteSent;
			pBuff += iByteSent;
		}
		return 0;
	}
	catch(kfc::socket_again& e)
	{
		m_strError = string("sendBuff socket_again Excep:") + e.what();
		return -2;
	}
	catch(kfc::socket_intr& e)
	{
		m_strError = string("sendBuff socket_intr Excep:") + e.what();
		return -2;
	}
	catch(kfc::socket_error& e)
	{
		m_strError = string("sendBuff Excep:") + e.what();
		CSocket::close();
		return -2;
	}

	m_strError = "sendBuff: Unknown error";
	CSocket::close();
	return -2;
}

// iLenPos <0	无长度字段, iLenPos >=0	有长度字段
// iLenLen: 只能为1,2,4
int kfc::CClientSocketTcp::recvBuff(char * sBuff, unsigned uLen, int iLenPos, int iLenLen, bool bLongRecv)
{
	if(sBuff == NULL)
	{
		m_strError = "recvBuff: sBuff is null";
		return -1;
	}

	if (iLenPos >= 0 && (iLenLen != 1 && iLenLen != 2 && iLenLen != 4))
	{
		m_strError = "recvBuff: invalid length field";
		return -1;
	}

	size_t uByteRecved = 0;
	int iRet = 0;
	char * pBuff = sBuff;

	bool bRecvLenFlag = false;
	unsigned iMaxRecvLen = uLen;			//最大应该接收长度

	try {
		while(uByteRecved<iMaxRecvLen) {
			if(iLenPos < 0)
			{
				iRet = CSocket::receive(pBuff, uLen-uByteRecved);
				/*
				if(iRet < 0)		//error
				{
					m_strError = string("recvBuff receive error:") + strerror(errno);
					CSocket::close();
					return -2;
				}
				else */if(iRet == 0)		//消息收完或者对方关闭连接
				{
					uLen = uByteRecved;
					return uByteRecved;
				}
				else
				{
					uByteRecved+=iRet;
					pBuff += iRet;
					if(bLongRecv)		//收到超时、对方关闭连接、buff满了为止
					{
						continue;
					}
					else					//收一次就走人
					{
						uLen = uByteRecved;
						return uByteRecved;
					}
				}
			}
			else		//有偏移长度字段地址,收到固定长度字节、对方关闭连接为止
			{
				if(bRecvLenFlag == false)
				{
					iRet = CSocket::receive(pBuff, iLenPos+iLenLen);			//先取到长度位,只支持1,2,4个字节
					/*
					if(iRet < 0)		//error
					{
						m_strError = string("recvBuff receive error:") + strerror(errno);
						CSocket::close();
						return -2;
					}
					else */if(iRet == 0)		//消息收完或者对方关闭连接
					{
						m_strError = "recvBuff: peer close socket";
						CSocket::close();
						return -2;
					}
					else
					{
						if(iRet != iLenPos+iLenLen)
						{
							m_strError = "recvBuff: can not recv len field";
							return -2;
						}
						uByteRecved+=iRet;
						pBuff += iLenPos;
						switch(iLenLen)
						{
							case 1:
								iMaxRecvLen = *pBuff;
								break;
							case 2:
								iMaxRecvLen = ntohs(*(short *)pBuff);
								break;
							case 4:
								iMaxRecvLen = ntohl(*(int *)pBuff);
								break;
							default:
								iMaxRecvLen = 0;
								break;
						}
						if(iMaxRecvLen < (unsigned)iLenPos+iLenLen)		//长度字段值太小
						{
							m_strError = "recvBuff: len value is too short";
							return -2;
						}
						if(iMaxRecvLen > uLen)
						{
							m_strError = "recvBuff: buff is too short, buf len:" + Convert::IntToString(uLen)
											+ ", need:"+ Convert::IntToString(iMaxRecvLen);
							return -2;
						}
						pBuff += iLenLen;
					}
					bRecvLenFlag = true;
				}


				//iRet = CSocket::receive(pBuff, iMaxRecvLen-(iLenPos+iLenLen));
				iRet = CSocket::receive(pBuff, iMaxRecvLen-uByteRecved);
				/*
				if(iRet < 0)		//error
				{
					m_strError = string("recvBuff receive error:") + strerror(errno);
					CSocket::close();
					return iRet;
				}
				else */if(iRet == 0)		//消息收完或者对方关闭连接
				{
					m_strError = "recvBuff: peer close socket";
					CSocket::close();
					return -2;
				}
				else
				{
					uByteRecved += iRet;
					pBuff += iRet;
				}
			}

		}
		uLen = uByteRecved;
		return uByteRecved;
	}
	catch(kfc::socket_again& e)
	{
		m_strError = string("recvBuff socket_again Excep:") + e.what();
		return -2;
	}
	catch(kfc::socket_intr& e)
	{
		m_strError = string("recvBuff socket_intr Excep:") + e.what();
		return -2;
	}
	catch(kfc::socket_error& e)
	{
		m_strError = string("recvBuff Excep:") + e.what();
		CSocket::close();
		return -2;
	}

	m_strError = "recvBuff: Unknown error";
	CSocket::close();
	return -2;
}

// Return:
// >0 success 表示读到的字节数
// =0 对方关闭了socket
// <0 失败, 有网络错误发生
int kfc::CClientSocketTcp::sendAndRecv(const char * sSendBuff, const unsigned uSendLen, char * sRecvBuff, unsigned uRecvLen, int iLenPos, int iLenLen, bool bLongRecv, bool bLongConn)
{
	if(sSendBuff==NULL || sRecvBuff==NULL)
	{
		m_strError = "sendAndRecv: sBuff is null";
		return -1;
	}

	if (iLenPos >= 0 && (iLenLen != 1 && iLenLen != 2 && iLenLen != 4))
	{
		m_strError = "sendAndRecv: invalid length field";
		return -1;
	}

	int iRet;
	int iRetry = 0;
	while(iRetry < m_iRetryTimes)
	{
		try
		{
			iRetry++;
			checkAndConnect();

			//发送消息
			iRet = sendBuff(sSendBuff, uSendLen);
			if(iRet < -1)
			{
				//CSocket::close();
				continue;
			}

			iRet = recvBuff(sRecvBuff, uRecvLen, iLenPos, iLenLen, bLongRecv);
			if(iRet > 0)		//收到数据
			{
				if(!bLongConn)
				{
					CSocket::close();
				}
				return iRet;
			}
			else if(iRet == 0)	//对方关闭连接
			{
				CSocket::close();
				return iRet;
			}
			else			//出错
			{
				//CSocket::close();
				//m_strError = "recv fail";
				//return -1;
				continue;
			}
		}
		catch(kfc::socket_error& e)
		{
			//CSocket::close();
			m_strError = string("sendAndRecv Excep:") + e.what();
			//return -1;
			continue;
		}
	}

	m_strError = "sendAndRecv: retry times over";
	if(!bLongConn)
	{
		CSocket::close();
	}

	return -1;
}
