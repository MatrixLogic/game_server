/*
 * LogicParams.cpp
 *
 *  Created on: 2017-8-17
 *      Author: dawx62fac
 */


#include "LogicAdminBase.h"


AdminBaseUnit::AdminBaseUnit(const std::string& name
		, const std::string& skey
		, int level)
{
	//校验
	CLogicAdmin logicAdmin;
	int ret = logicAdmin.CheckSession(name, skey);
	if (ret != 0)
	{
		throw std::runtime_error("Session non-valid!");
	}

	ret = logicAdmin.CheckLevel(name, (ADMIN_LEVEL)level);
	if (ret != 0)
	{
		throw std::runtime_error("Admin privilege not enough!");
	}
}

void AdminBaseUnit::_Send(unsigned uid, Message* msg)
{
	if (NULL == msg) return;

	if (uid == 0) return ;

	//发送协议
	CSG17Packet packet;
	packet.cmd = PROTOCOL_ADMIN;
	packet.m_msg = msg;

	int ret = BattleSocket::Send(Config::GetZoneByUID(uid), &packet);
	if(ret)
	{
		throw std::runtime_error("send_data_to_battle_faild!");
	}


	ret = BattleSocket::Receive(Config::GetZoneByUID(uid), &replay_packet_);
	if(ret)
	{
		throw std::runtime_error("redv_data_from_battle_faild!");
	}
}

const Message* AdminBaseUnit::SendPacket(unsigned uid, Message* msg)
{
	this->_Send(uid, msg);
	if (replay_packet_.m_msg == NULL)
	{
		throw std::runtime_error("recv message is null!");
	}
	return replay_packet_.m_msg;
}



