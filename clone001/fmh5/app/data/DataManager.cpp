#include "DataManager.h"

int DataGameActivityManager::OnInit()
{
	for(unsigned i = 0; i < MAX_BUFF; ++i)
	{
		if(!m_data->Empty(i))
		{
			uint32_t uid = m_data->data[i].uid;
			uint32_t id = m_data->data[i].id;

			m_map[uid][id] = i;
		}
	}

	return 0;
}

DataGameActivity & DataGameActivityManager::GetUserActivity(unsigned uid, unsigned id)
{
	//load内已判断是否加载到内存
	int ret = LoadBuff(uid);

	if (ret > 0 && ret != R_ERR_NO_DATA)
	{
		//报错
		throw runtime_error("load activity data error");
	}

	if (!m_map.count(uid) || !m_map[uid].count(id))
	{
		//不包含活动的数据，则新增
		DataGameActivity activity;
		activity.uid = uid;
		activity.id = id;

		ret = AddBuff(activity);

		if (ret)
		{
			throw runtime_error("add activity data error");
		}
	}

	unsigned index = m_map[uid][id];

	return m_data->data[index];
}

bool DataGameActivityManager::UpdateActivity(DataGameActivity & activity)
{
	unsigned uid = activity.uid;
	unsigned id = activity.id;

	if (!m_map.count(uid) || !m_map[uid].count(id))
	{
		throw runtime_error("activity_data_error");
	}

	unsigned index = m_map[uid][id];

	return  m_data->MarkChange(index);
}

int DataGameActivityManager::CheckBuff(unsigned uid)
{
	if(!m_map.count(uid))
		return R_ERR_NO_DATA;

	return 0;
}

int DataGameActivityManager::AddBuff(DataGameActivity & activity)
{
	unsigned index = GetFreeIndex();

	uint32_t uid = activity.uid;
	uint32_t id = activity.id;

	if(index == -1)
	{
		error_log("get free index error. uid=%u", uid);
		return R_ERR_DATA;
	}

	if(Add(index, activity))
	{
		m_map[uid][id] = index;
	}
	else
	{
		error_log("Add to dbc failed. uid=%u", uid);
		return R_ERR_DATA;
	}

	return 0;
}

int DataGameActivityManager::LoadBuff(unsigned uid)
{
	//为防止重复加载
	if (m_map.count(uid) > 0)
	{
		return 0;
	}

	vector<DataGameActivity> activities(1);
	activities[0].uid = uid;

	int ret = Load(activities);

	if (ret)
	{
		return ret;
	}

	if (0 == activities.size())
	{
		return R_ERR_NO_DATA;
	}

	//加载用户的所有装备数据
	//单个逐个加载
	for(int i = 0; i < activities.size(); ++i)
	{
		unsigned index = GetFreeIndex();

		if(index == -1)
		{
			error_log("get free index error. uid=%u", uid);
			return R_ERR_DATA;
		}

		m_data->data[index] = activities[i];  //给m_data内的数据赋值
		unsigned id = activities[i].id;

		if(m_data->MardLoad(index))
		{
			m_freeIndex.erase(index);
		}
		else
		{
			error_log("mark load failed. uid=%u,id=%d.", uid, id);
			return R_ERROR;
		}

		m_map[uid][id] = index;
	}

	return 0;
}

void DataGameActivityManager::DoClear(unsigned uid)
{
	if(m_map.count(uid))
	{
		 map<uint32_t, uint32_t>::iterator miter = m_map[uid].begin();

		 for(; miter != m_map[uid].end(); )
		 {
			 Clear(miter->second);
			 m_map[uid].erase(miter++);
		 }

		 m_map.erase(uid);
	}
}

void DataGameActivityManager::DoSave(unsigned uid)
{
	if(m_map.count(uid))
	{
		 map<uint32_t, uint32_t>::iterator miter = m_map[uid].begin();

		 for(; miter != m_map[uid].end(); ++miter)
		 {
				AddSave(miter->second);
		 }
	}
}
