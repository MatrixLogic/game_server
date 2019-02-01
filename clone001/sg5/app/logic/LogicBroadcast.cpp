#include "LogicBroadcast.h"

CDataBroadcast* CLogicBroadcast::GetCDataBroadcast()
{
	GET_MEM_DATA_SEM(CDataBroadcast, CONFIG_BROADCAST_DATA, sem_broadcast)
	/*static CDataBroadcast* pBroadcast = NULL;
	if (!pBroadcast)
	{
		pBroadcast = new CDataBroadcast();
		if (pBroadcast->Init(Config::GetValue(CONFIG_BROADCAST_DATA)) != 0)
		{
			delete pBroadcast;
			pBroadcast = NULL;
		}
	}
	return pBroadcast;*/
}

int CLogicBroadcast::AddBdMessage(const string &msg)
{
	CDataBroadcast* pBroadcast = GetCDataBroadcast();
	if (!pBroadcast)
	{
		DB_ERROR_RETURN_MSG("get_bd_instance_fail");
	}
	return pBroadcast->AddBdMessage(msg);
}

int CLogicBroadcast::GetBdMessage(vector<BroadcastMessage> &msgs)
{
	CDataBroadcast* pBroadcast = GetCDataBroadcast();
	if (!pBroadcast)
	{
		DB_ERROR_RETURN_MSG("get_bd_instance_fail");
	}
	return pBroadcast->GetBdMessage(msgs);
}
