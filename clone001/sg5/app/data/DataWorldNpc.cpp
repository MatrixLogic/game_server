/*
 * DataWorldNpc.cpp
 *
 *  Created on: 2013-1-21
 *      Author: Administrator
 */

#include "DataWorldNpc.h"

CDataWorldNpc::CDataWorldNpc() {
	// TODO Auto-generated constructor stub
	m_init = false;
}

CDataWorldNpc::~CDataWorldNpc() {
	// TODO Auto-generated destructor stub
}

int CDataWorldNpc::Init(const std::string &worldData, semdat sem)
{
	if(m_init)
	{
		return 0;
	}

	int semgroup = 0;
	int semserver = 0;
	Config::GetDomain(semserver);
	if(!semserver)
		Config::GetDB(semserver);
	if(!m_sh.CreateOrOpen(worldData.c_str(), sizeof(NpcList), SEM_ID(sem,semgroup,semserver)))
	{
		error_log("[init_world_fail][path=%s]", worldData.c_str());
		return R_ERR_DATA;
	}
	CAutoLock lock(&m_sh, true);
	if(!m_sh.HasInit())
	{

		NpcList *pdata = (NpcList *)m_sh.GetAddress();
		//	初始化坐标系统
		memset(pdata, 0, sizeof(NpcList));
		m_sh.SetInitDone();
	}
	m_init = true;
	return 0;
}

int CDataWorldNpc::ReplaceNpc(const NpcItem &npc)
{
	NpcList *pdata = (NpcList *)m_sh.GetAddress();
	if(NULL == pdata)
	{
		error_log("get_world_npc failed");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	int i = 0;
	for (; i <pdata->count;i++)
	{
		if(pdata->npclist[i].worldpos == npc.worldpos)
		{
			pdata->npclist[i].Occupy(npc);
			return 0;
		}
	}
	if(i >= pdata->count && IsWorldNPCUid(npc.uid))
	{
		pdata->npclist[pdata->count++] = npc;
		//return R_ERR_DB;
	}else{
		error_log("no npc match worldpos = %u,uid=%d",npc.worldpos,npc.uid);
		return R_ERR_DB;
	}

	return 0;
}

int CDataWorldNpc::GetNpc(NpcItem &npc)
{
	NpcList *pdata = (NpcList *)m_sh.GetAddress();
	if(NULL == pdata)
	{
		error_log("get_world_npc failed");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	int i = 0;
	for (; i <pdata->count;i++)
	{
		if(pdata->npclist[i].worldpos == npc.worldpos)
		{
			npc = pdata->npclist[i];
			return 0;
		}
	}
	return R_ERR_DB;
}

int CDataWorldNpc::GetAllNpc(NpcItem npc[MAX_WORLD_NPC_COUNT],int &iCount)
{
	NpcList *pdata = (NpcList *)m_sh.GetAddress();
	if(NULL == pdata)
	{
		error_log("get_world_npc failed");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	iCount = pdata->count;
	memcpy(npc,pdata->npclist,sizeof(NpcItem) * iCount);
	return 0;
}
int CDataWorldNpc::RemoveNpc(const NpcItem &npc)
{
	NpcList *pdata = (NpcList *)m_sh.GetAddress();
	if(NULL == pdata)
	{
		error_log("get_world_npc failed");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	int i = 0;
	for (; i <pdata->count;i++)
	{
		if(pdata->npclist[i].worldpos == npc.worldpos)
		{
			memset(&pdata->npclist[i],0x00,sizeof(NpcItem));
			pdata->count--;
			//return 0;
			break;
		}
	}


	if(i >= pdata->count)
	{
		error_log("no npc match worldpos = %u,uid=%d",npc.worldpos,npc.uid);
		return 6;
	}
	int index = i;
	for(i = index + 1; i <= pdata->count;i++)
	{
		pdata->npclist[index++] = pdata->npclist[i];
		memset(&pdata->npclist[i],0x00,sizeof(NpcItem));
	}
	return 0;
}

int CDataWorldNpc::RemoveNpc(const int type)
{
	NpcList *pdata = (NpcList *)m_sh.GetAddress();
	if(NULL == pdata)
	{
		error_log("get_world_npc failed");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	int i = 0;
	int iCount = 0;
	for (; i <pdata->count;i++)
	{
		if(pdata->npclist[i].GetType() == type)
		{
			memset(&pdata->npclist[i],0x00,sizeof(NpcItem));
			iCount++;   //被删除的npc个数
		}
	}
	int index = -1;
	for(i = 0; i < pdata->count;i++)
	{
		if(pdata->npclist[i].worldpos != 0 && index != -1)
		{
			pdata->npclist[index++] = pdata->npclist[i];
			memset(&pdata->npclist[i],0x00,sizeof(NpcItem));
		}
		if(pdata->npclist[i].worldpos == 0 && index == -1)
		{
			index = i;
		}
	}
	pdata->count = pdata->count - iCount;
	return 0;
}
int CDataWorldNpc::ViewNpc()
{
	NpcList *pdata = (NpcList *)m_sh.GetAddress();
	if(NULL == pdata)
	{
		error_log("get_world_npc failed");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	int i;
	for(i = 0; i < pdata->count;i++)
	{
		cout<<"worldpos = "<<pdata->npclist[i].worldpos<<"type = "<<pdata->npclist[i].GetType()<<endl;
	}
	return 0;
}

int CDataWorldNpc::MakeReward()
{
	NpcList *pdata = (NpcList *)m_sh.GetAddress();
	if(NULL == pdata)
	{
		error_log("get_world_npc failed");
		return R_ERR_DB;
	}
	srand(time(NULL));
	CAutoLock lock(&m_sh, true);
	int i;
	for(i = 0; i < pdata->count;i++)
		pdata->npclist[i].MakeReward();

	return 0;
}
