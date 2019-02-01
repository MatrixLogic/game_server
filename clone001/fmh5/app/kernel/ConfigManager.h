/*
 * ConfigManager.h
 *
 *  Created on: 2016-8-22
 *      Author: Ralf
 */

#ifndef CONFIGMANAGER_H_
#define CONFIGMANAGER_H_

#include "Common.h"
#include "ConfigPB.h"
#include "ConfigInc.h"

/*
 *  配置文件管理器，增加新配置时，把新的pb文件加入ConfigInc.h，定义新的const成员，构造函数中写入配置文件名
 */
class ConfigManager : public CSingleton<ConfigManager>
{
private:
	friend class CSingleton<ConfigManager>;
	virtual ~ConfigManager(){}
	ConfigManager()
		:user("UserConfig.json")
		,activity("ActivityTime.json", false)
		,builds("building.json")
		,barriers("barrier.json")
		,propsitem("item.json")
		,productline("productline.json")
	{
		Init();
	}

	void Init();
	static bool m_init;

public:
	static bool Reload();
	static void Fail(){m_init = false;}
	bool Inited(){return m_init;}

	const ConfigPB<UserCfg::User> user;
	const ConfigPB<User::Activity> activity;
	const ConfigPB<ConfigBuilding::Buildings> builds;
	const ConfigPB<ConfigBuilding::Barriers> barriers;
	const ConfigPB<ConfigItem::Items> 	propsitem;
	const ConfigPB<ConfigProductLine::ProductLine> 	productline;

	map<unsigned, unsigned> m_actmap;
	map<unsigned, unsigned > buildsFootIndex;  //id->index
	map<unsigned, pair<unsigned, unsigned> > buildsIndex;  //id->type->index  建筑
	map<unsigned, unsigned > BarrierIndex;  //id->index 障碍物
	map<unsigned, unsigned > ItemIndex;  //id->index，道具
	map<unsigned, pair<unsigned, unsigned> > productlineIndex;  //id->type->index  生产线

	bool GetActivity(unsigned id, User::ActivityItem& act);
};

#endif /* CONFIGMANAGER_H_ */
