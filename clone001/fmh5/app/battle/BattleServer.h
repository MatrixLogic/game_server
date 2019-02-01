#ifndef _BATTLE_SERVER_H_
#define _BATTLE_SERVER_H_

#include "ServerInc.h"


class BattleServer : public CTcpServer, public CSingleton<BattleServer>
{
private:
	friend class CSingleton<BattleServer>;
	BattleServer():m_pReceivePacket(NULL),m_pReceiveProtocol(NULL),m_pReplyProtocol(NULL){};
	virtual ~BattleServer(){}
public:
	bool Initialize();
	virtual void OnReceive(CFirePacket *pPacket);
	virtual void OnChannelClose(int channelId);
	virtual void OnIdle(){}
	virtual void onExit(){}
private:
	CFirePacket* m_pReceivePacket;
	IPacketReceive* m_pReceiveProtocol;
	IPacketSend* m_pReplyProtocol;

};


#endif
