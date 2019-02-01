/*
 * LogicWorldNpc.h
 *
 *  Created on: 2013-1-23
 *      Author: Administrator
 */

#ifndef LOGICWORLDNPC_H_
#define LOGICWORLDNPC_H_
#include "LogicInc.h"
class CLogicWorldNpc {
public:
	CLogicWorldNpc();
	virtual ~CLogicWorldNpc();
	CDataWorldNpc *GetDataWorldNpc();
	int ReplaceNpc(const NpcItem &npc );
	int GetNpc(NpcItem &npc );
	int GetAllNpc( Json::Value &npcValue);
	int RemoveNpc(const NpcItem &npc );
	//int UpdateNpc(const NpcItem)
};

#endif /* LOGICWORLDNPC_H_ */
