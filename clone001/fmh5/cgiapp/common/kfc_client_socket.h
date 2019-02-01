#ifndef _KFC_CLIENT_SOCKET_H_
#define _KFC_CLIENT_SOCKET_H_

#include "kfc_socket.h"

namespace kfc
{
	class CClientSocketTcp: public kfc::CSimpleSocketTcp
	{
	public:
		CClientSocketTcp(const std::string& strIp, int iPort, unsigned uTimeOut = 5,
				int iRetryTimes = 3);
		~CClientSocketTcp()
		{
		}

		/*
		 sSendBuff:发送缓冲区
		 uSendLen:发送缓冲区长度
		 sRecvBuff:接收缓冲区
		 uRecvLen:接收缓冲区的长度,回传接收到的字节数
		 iLenPos:长度字段的位置偏移,负值表示协议中无长度字段
		 iLenLen:长度字段自身的长度
		 bLongRecv:是否recv到uRecvLen字节为止
		 bLongConn:发送接收结束是否关闭连接
		 return:接收到的字节数
		 支持三种方式:
		 1.协议中有长度字段的:iLenPos表示长度字段的位置偏移,bLongRecv无意义
		 2.只recv一次的:iLenPos要置为负值,bLongRecv置为false
		 3.recv到uRecvLen个字节为止:iLenPos要置为负值,bLongRecv置为true
		 */
		int sendAndRecv(const char* sSendBuff, const unsigned uSendLen,
				char* sRecvBuff, unsigned uRecvLen, int iLenPos = -1, int iLenLen =
						4, bool bLongRecv = false, bool bLongConn = true);

		std::string getError()
		{
			return m_strError;
		}

		void setConfig(const std::string& strIp, int iPort, unsigned uTimeOut = 5,
				int iRetryTimes = 3);

	protected:
		void checkAndConnect();
		int sendBuff(const char* sBuff, const unsigned uLen);
		//iLenPos表示长度INT的偏移位,<0表示无长度字段
		int recvBuff(char* sBuff, unsigned uLen, int iLenPos = -1, int iLenLen = 4,
				bool bLongRecv = false);

	private:
		std::string m_strIp;
		int m_iPort;
		unsigned m_uTimeOut;
		int m_iRetryTimes;

		std::string m_strError;
	};
}

#endif
