/*
 * DataWorldNpc.h
 *
 *  Created on: 2013-1-21
 *      Author: Administrator
 */

#ifndef DATAWORLDNPC_H_
#define DATAWORLDNPC_H_
#include "Kernel.h"
struct NpcItem{
	unsigned uid;
	unsigned worldpos;
	unsigned type;
	char cityName[32];
	char name[32];
	NpcItem &operator =(const NpcItem &other)
	{
		if(this == &other)
		{
			return *this;
		}
		uid = other.uid;
		worldpos = other.worldpos;
		type = other.type;
		memcpy(name,other.name,sizeof(name) - 1);
		memcpy(cityName,other.cityName,sizeof(cityName) - 1);
		return *this;
	}
	unsigned GetType()
	{
		return (type & 0xff);
	}
	unsigned GetReward()
	{
		return ((type >> 8) &0xff);
	}
	unsigned GetNextReward()
	{
		return ((type>> 16) &0xff);
	}
	void SetType(unsigned t)
	{
		type = (type & 0xffffff00) | (t & 0xff);
	}
	void MakeReward()
	{
		unsigned n = (rand()%5) << 16;
		unsigned r = (type & 0x00ff0000) >> 8;
		type = (type & 0xff) | n | r;
	}
	void Occupy(const NpcItem &other)
	{
		uid = other.uid;
		memcpy(name,other.name,sizeof(name) - 1);
		memcpy(cityName,other.cityName,sizeof(cityName) - 1);
	}
};
struct NpcList{
	int count;
	NpcItem npclist[MAX_WORLD_NPC_COUNT];

};
class CDataWorldNpc {
public:
	CDataWorldNpc();
	virtual ~CDataWorldNpc();
	int Init(const std::string &path, semdat sem=sem_worldnpc);
	int ReplaceNpc(const NpcItem &npc);
	int GetNpc(NpcItem &npc);
	int GetAllNpc(NpcItem npc[MAX_WORLD_NPC_COUNT],int &iCount);
	int RemoveNpc(const NpcItem &npc);
	int RemoveNpc(const int type);
	int ViewNpc();
	int MakeReward();
private:
	bool m_init;
	CShareMemory m_sh;
};
#endif /* DATAWORLDNPC_H_ */
