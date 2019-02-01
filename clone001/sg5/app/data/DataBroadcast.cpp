#include "DataBroadcast.h"

CDataBroadcast::CDataBroadcast()
{
	m_init = false;
}

int CDataBroadcast::Init(const string &path, semdat sem)
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
	if(!m_sh.CreateOrOpen(path.c_str(), sizeof(BroadcastDATA), SEM_ID(sem,semgroup,semserver)))
	{
		error_log("[init_broadcast_data_fail][path=%s]", path.c_str());
		return R_ERROR;
	}
	CAutoLock lock(&m_sh, true);
	if(!m_sh.HasInit())
	{
		BroadcastDATA *pdata = (BroadcastDATA *)m_sh.GetAddress();
		memset(pdata, 0, sizeof(*pdata));
		m_sh.SetInitDone();
	}
	m_init = true;
	return 0;
}

int CDataBroadcast::AddBdMessage(const string &msg)
{
	BroadcastDATA *pdata = (BroadcastDATA *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	unsigned id = ++pdata->cur_id;
	int index = id % BROADCAST_AMOUNT_MAX;
	pdata->items[index].id = id;
	pdata->items[index].status = BS_IDLE;
	snprintf(pdata->items[index].content, BROADCAST_CONTENT_SIZE_MAX, "%s", msg.c_str());
	return 0;
}

int CDataBroadcast::GetBdMessage(vector<BroadcastMessage> &msgs)
{
	BroadcastDATA *pdata = (BroadcastDATA *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	for (int i = 0; i < BROADCAST_AMOUNT_MAX; i++)
	{
		if (!pdata->items[i].id)
			continue;
		BroadcastMessage temp;
		temp.id = pdata->items[i].id;
		temp.content = pdata->items[i].content;
		msgs.push_back(temp);
	}
	return 0;
}

int CDataBroadcast::GetBroadcastData(BroadcastDATA &data)
{
	BroadcastDATA *pdata = (BroadcastDATA *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	memcpy(&data, pdata, sizeof(BroadcastDATA));
	return 0;
}
