/*
 * ConfigManager.cpp
 *
 *  Created on: 2016-8-22
 *      Author: Ralf
 */


#include "ConfigManager.h"

bool ConfigManager::m_init = true;

bool ConfigManager::Reload()
{
	ConfigManager *temp = new ConfigManager;
	if(!m_init)
	{
		m_init = true;
		delete temp;
		return false;
	}
	delete temp;
	Destroy();
	temp = Instance();
	return true;
}

void ConfigManager::Init()
{
	if(!m_init)
		return;

	for(int i=0;i<activity.m_config.act_size();++i)
		m_actmap[activity.m_config.act(i).id()] = i;

	buildsFootIndex.clear();

	for(int i = 0; i < builds.m_config.build_print_size(); ++i)
	{
		unsigned id = builds.m_config.build_print(i).id();

		buildsFootIndex[id] = i;
	}

	buildsIndex.clear();
	//仓库
	for(int i = 0; i < builds.m_config.storagehouse_size(); ++i)
	{
		unsigned id = builds.m_config.storagehouse(i).id();

		buildsIndex[id].first = build_type_storage;
		buildsIndex[id].second = i;
	}

	//农地
	for(int i = 0; i < builds.m_config.cropland_size(); ++i)
	{
		unsigned id = builds.m_config.cropland(i).id();

		buildsIndex[id].first = build_type_corpland;
		buildsIndex[id].second = i;
	}

	//动物住所
	for(int i = 0; i < builds.m_config.animal_residence_size(); ++i)
	{
		unsigned id = builds.m_config.animal_residence(i).id();

		buildsIndex[id].first = build_type_animal_residence;
		buildsIndex[id].second = i;
	}

	//动物
	for(int i = 0; i < builds.m_config.animal_size(); ++i)
	{
		unsigned id = builds.m_config.animal(i).id();

		buildsIndex[id].first = build_type_animal;
		buildsIndex[id].second = i;
	}

	//生产设备
	for(int i = 0; i < builds.m_config.produce_equipment_size(); ++i)
	{
		unsigned id = builds.m_config.produce_equipment(i).id();

		buildsIndex[id].first = build_type_produce_equipment;
		buildsIndex[id].second = i;
	}

	//果树
	for(int i = 0; i < builds.m_config.fruit_tree_size(); ++i)
	{
		unsigned id = builds.m_config.fruit_tree(i).id();

		buildsIndex[id].first = build_type_fruit_tree;
		buildsIndex[id].second = i;
	}

	//装饰
	for(int i = 0; i < builds.m_config.decorate_size(); ++i)
	{
		unsigned id = builds.m_config.decorate(i).id();

		buildsIndex[id].first = build_type_decorate;
		buildsIndex[id].second = i;
	}

	//障碍物
	BarrierIndex.clear();

	for(int i = 0; i < barriers.m_config.barriers_size(); ++i)
	{
		unsigned id = barriers.m_config.barriers(i).id();
		BarrierIndex[id] = i;
	}

	//道具
	ItemIndex.clear();

	for(int i = 0; i < propsitem.m_config.itemes_size(); ++i)
	{
		unsigned id = propsitem.m_config.itemes(i).id();
		ItemIndex[id] = i;
	}

	//生产线
	productlineIndex.clear();

	//动物
	for(int i = 0; i < productline.m_config.animal_line_size(); ++i)
	{
		unsigned id = productline.m_config.animal_line(i).id();

		productlineIndex[id].first = build_type_animal;
		productlineIndex[id].second = i;
	}

	//生产设备
	for(int i = 0; i < productline.m_config.equipline_size(); ++i)
	{
		unsigned id = productline.m_config.equipline(i).id();

		productlineIndex[id].first = build_type_produce_equipment;
		productlineIndex[id].second = i;
	}
}


bool ConfigManager::GetActivity(unsigned id, User::ActivityItem& act)
{
	if(m_actmap.count(id))
	{
		act = activity.m_config.act(m_actmap[id]);
		return true;
	}
	return false;
}
