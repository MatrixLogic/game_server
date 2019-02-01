#include "DataAllManager.h"

int DataBuildingMgr::Init(unsigned uid)
{
	Online(uid);

	//初始化建筑ud
	unsigned maxud = 0;  //初始化ud = 1
	map<uint32_t, uint32_t>::reverse_iterator reviter = m_map[uid].rbegin();

	if (reviter != m_map[uid].rend() && maxud < reviter->first)
	{
		maxud = reviter->first;
	}

	m_userMaxUd[uid] = maxud;

	if (m_BuildUd.count(uid))
	{
		m_BuildUd[uid].clear();
	}

	//初始化建筑ud与build_id之间的映射关系
	for(; reviter != m_map[uid].rend(); ++reviter)
	{
		unsigned index = reviter->second;
		m_BuildUd[uid][m_data->data[index].build_id].push_back(reviter->first);
	}

	return 0;
}

unsigned DataBuildingMgr::GetUserNextUd(unsigned uid)
{
	m_userMaxUd[uid] = m_userMaxUd[uid] + 1;

	return m_userMaxUd[uid];
}

DataBuildings & DataBuildingMgr::AddNewBuilding(unsigned uid, unsigned build_id)
{
	unsigned build_ud = GetUserNextUd(uid);

	DataBuildings & databuild = GetData(uid, build_ud);
	databuild.build_id = build_id;

	m_BuildUd[uid][build_id].push_back(build_ud);

	return databuild;
}

unsigned DataBuildingMgr::GetBuildUd(unsigned uid, uint16_t build_id)
{
	if (m_BuildUd.count(uid)
			&& m_BuildUd[uid].count(build_id)
			&& m_BuildUd[uid][build_id].size() > 0)
	{
		return m_BuildUd[uid][build_id][0u];
	}

	return -1;
}


int DataBuildingMgr::GetBuildNum(unsigned uid, uint16_t build_id)
{
	if (m_BuildUd.count(uid) && m_BuildUd[uid].count(build_id))
	{
		return m_BuildUd[uid][build_id].size();
	}

	return 0;
}

int DataBuildingMgr::GetAllBuildNum(unsigned uid)
{
	if (!m_BuildUd.count(uid))
	{
		return 0;
	}

	map<uint16_t, vector<uint16_t> >::iterator viter = m_BuildUd[uid].begin();

	int nums = 0;

	for (; viter != m_BuildUd[uid].end(); ++viter)
	{
		nums += viter->second.size();
	}

	return nums;
}

int DataItemManager::Init(unsigned uid)
{
	Online(uid);

	//初始化道具ud
	unsigned maxud = 0;  //初始化ud = 1
	map<uint32_t, uint32_t>::reverse_iterator reviter = m_map[uid].rbegin();

	if (reviter != m_map[uid].rend() && maxud < reviter->first)
	{
		maxud = reviter->first;
	}

	m_userMaxUd[uid] = maxud;

	if (m_PropsUd.count(uid))
	{
		m_PropsUd[uid].clear();
	}

	//初始化道具ud与props_id之间的映射关系
	for(; reviter != m_map[uid].rend(); ++reviter)
	{
		unsigned index = reviter->second;
		m_PropsUd[uid][m_data->data[index].props_id].push_back(reviter->first);
	}

	return 0;
}

unsigned DataItemManager::GetUserNextUd(unsigned uid)
{
	m_userMaxUd[uid] = m_userMaxUd[uid] + 1;

	return m_userMaxUd[uid];
}

bool DataItemManager::IsPropsExist(unsigned uid, unsigned propsid)
{
	if (m_PropsUd.count(uid)
			&& m_PropsUd[uid].count(propsid)
			&& m_PropsUd[uid][propsid].size() > 0)
	{
		return true;
	}

	return false;
}

unsigned DataItemManager::GetPropsUd(unsigned uid, unsigned propsid)
{
	if (m_PropsUd.count(uid)
			&& m_PropsUd[uid].count(propsid)
			&& m_PropsUd[uid][propsid].size() > 0)
	{
		return m_PropsUd[uid][propsid][0u];
	}

	return -1;
}

int DataItemManager::AddNewProps(DataItem & propsitem)
{
	unsigned index = NewItem(propsitem.uid, propsitem.id);

	DataItem & item = GetDataByIndex(index);
	item = propsitem;

	m_PropsUd[propsitem.uid][propsitem.props_id].push_back(propsitem.id);

	return 0;
}

int DataItemManager::DelProps(DataItem & propsitem)
{
	unsigned uid = propsitem.uid;
	unsigned ud = propsitem.id;

	if (!m_map.count(uid) || !m_map[uid].count(ud))
	{
		error_log("get props error. uid=%u, id=%u", uid, ud);
		throw std::runtime_error("get_props_error");
	}

	unsigned index = m_map[uid][ud];

	//将删除操作立即加入到dbc的更新队列中
	//mark删除状态
	m_data->MarkDel(index);
	//添加至操作队列
	AddSave(index);

	//删除映射
	unsigned propsid = propsitem.props_id;

	m_map[uid].erase(ud);  //清除映射

	vector<unsigned>::iterator piter = find(m_PropsUd[uid][propsid].begin(), m_PropsUd[uid][propsid].end(), ud);

	if (piter != m_PropsUd[uid][propsid].end())
	{
		//删除
		m_PropsUd[uid][propsid].erase(piter);
	}

	if (m_PropsUd[uid][propsid].empty())
	{
		m_PropsUd[uid].erase(propsid);
	}

	return 0;
}

unsigned int DataCroplandManager::AddNewCropLand(unsigned uid, unsigned cropud)
{
	//判断ud是否存在，如果存在，则不重复添加
	if (IsExistItem(uid, cropud))
	{
		return GetIndex(uid, cropud);
	}

	unsigned index = NewItem(uid, cropud);

	return index;
}

unsigned int DataProduceequipManager::AddNewEquip(unsigned uid, unsigned equipud)
{
	//判断ud是否存在，如果存在，则不重复添加
	if (IsExistItem(uid, equipud))
	{
		return GetIndex(uid, equipud);
	}

	unsigned index = NewItem(uid, equipud);

	return index;
}

int DataAnimalManager::Init(unsigned uid)
{
	Online(uid);

	//初始化建筑ud
	unsigned maxud = animal_ud_begin;  //初始化ud
	map<uint32_t, uint32_t>::reverse_iterator reviter = m_map[uid].rbegin();

	if (reviter != m_map[uid].rend() && maxud < reviter->first)
	{
		maxud = reviter->first;
	}

	m_userMaxUd[uid] = maxud;

	if (m_AnimalIdUd.count(uid))
	{
		m_AnimalIdUd[uid].clear();
	}

	if (m_BuildUd.count(uid))
	{
		m_BuildUd[uid].clear();
	}

	//初始化建筑ud与build_id之间的映射关系
	for(; reviter != m_map[uid].rend(); ++reviter)
	{
		unsigned mindex = reviter->second;
		m_AnimalIdUd[uid][m_data->data[mindex].animal_id].push_back(reviter->first);
		m_BuildUd[uid][m_data->data[mindex].residence_ud].push_back(reviter->first);
	}

	return 0;
}

unsigned DataAnimalManager::GetUserNextUd(unsigned uid)
{
	m_userMaxUd[uid] = m_userMaxUd[uid] + 1;

	return m_userMaxUd[uid];
}

unsigned DataAnimalManager::GetBuildAdoptedNum(unsigned uid, unsigned buildud) const
{
	if (m_BuildUd.count(uid) && m_BuildUd.at(uid).count(buildud))
	{
		return m_BuildUd.at(uid).at(buildud).size();
	}

	return 0;
}

unsigned DataAnimalManager::GetAdoptedNum(unsigned uid, unsigned animalid) const
{
	if (m_AnimalIdUd.count(uid) && m_AnimalIdUd.at(uid).count(animalid))
	{
		return m_AnimalIdUd.at(uid).at(animalid).size();
	}

	return 0;
}

DataAnimal & DataAnimalManager::AddNewAnimal(unsigned uid, unsigned animalid, unsigned buildud)
{
	unsigned animal_ud = GetUserNextUd(uid);

	DataAnimal & dataanimal = GetData(uid, animal_ud);

	dataanimal.animal_id = animalid;
	dataanimal.residence_ud = buildud;

	m_BuildUd[uid][buildud].push_back(animal_ud);
	m_AnimalIdUd[uid][animalid].push_back(animal_ud);

	return dataanimal;
}
