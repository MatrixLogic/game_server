/*
 * UserCfgWrap.cpp
 *
 *  Created on: 2016-9-12
 *      Author: dawx62fac
 */

#include "ConfigWrap.h"

UserCfgWrap::UserCfgWrap()
	: cfg_(ConfigManager::Instance()->user.m_config)
{
}

const UserCfg::User& UserCfgWrap::User() const
{
	return cfg_;
}

const UserCfg::UserBase& UserCfgWrap::UserBase() const
{
	return cfg_.user_init();
}

///////////////////////////////////////////////////////////////////////////
ActivityCfgWrap::ActivityCfgWrap()
	: cfg_(ConfigManager::Instance()->activity.m_config)
{
}

const User::ActivityItem& ActivityCfgWrap::GetItem(unsigned id) const
{
	for(int i = 0; i < cfg_.act_size(); ++i)
	{
		if (cfg_.act(i).id() == id)
		{
			return cfg_.act(i);
		}
	}

	error_log("unknown_activity_cfg. id: %u", id);
	throw std::runtime_error("unknown_activity_cfg");
}
//////////////////////////////////////////////////////////////////////////

BuildCfgWrap::BuildCfgWrap()
	: cfg_(ConfigManager::Instance()->builds.m_config)
{

}

int BuildCfgWrap::GetUnlockUpgradeStarLevel() const
{
	return cfg_.unlock_upgardestar_level();
}

int BuildCfgWrap::GetBuildType(unsigned build_id) const
{
	int type = build_id/build_type_len;

	if (build_type_corpland == type)
	{
		//可能是仓库或者房子
		//获取房子配置
		const ConfigBuilding::House & housecfg = cfg_.house();
		const ConfigBuilding::CropLand & corpcfg = cfg_.cropland(0u);

		if (housecfg.id() == build_id)
		{
			//房子
			return build_type_house;
		}
		else if (corpcfg.id() == build_id)
		{
			//农地
			return build_type_corpland;
		}
		else
		{
			//仓库，暂时没有消耗.不允许多个
			return build_type_storage;
		}
	}

	//其他类型，值就是该类型
	return type;
}

int BuildCfgWrap::GetLevelIndex(const google::protobuf::RepeatedField< ::google::protobuf::uint32 >& needlevel, unsigned lv)
{
	int max = needlevel.size();

	if (0 == max)
	{
		return 0;
	}

	//找出lv第一个大于等于的下标
	for(int i = max - 1; i >= 0; --i)
	{
		if (lv >= needlevel.Get(i))
		{
			return i;
		}
	}

	error_log("level not enough. level=%u,min=%u", lv, needlevel.Get(0u));

	throw runtime_error("level_not_enough");
}

int BuildCfgWrap::GetGainNumIndex(const google::protobuf::RepeatedField< ::google::protobuf::uint32 >& gain_num, unsigned num)
{
	int max = gain_num.size();

	if (0 == max)
	{
		return 0;
	}

	//找出lv第一个大于等于的下标
	for(int i = max - 1; i >= 0; --i)
	{
		if (num >= gain_num.Get(i))
		{
			return i;
		}
	}

	return 0;
}

const ConfigBuilding::House & BuildCfgWrap::GetHouseCfg() const
{
	return cfg_.house();
}

bool BuildCfgWrap::IsStorage(unsigned build_id)
{
	for(int i = 0; i < cfg_.storagehouse_size(); ++i)
	{
		if (build_id == cfg_.storagehouse(i).id())
		{
			return true;
		}
	}

	return false;
}

const ConfigBuilding::StorageHouse & BuildCfgWrap::GetStorageCfg(unsigned index) const
{
	if (index > cfg_.storagehouse_size())
	{
		error_log("param error. index=%u", index);
		throw runtime_error("param_error");
	}

	return cfg_.storagehouse(index - 1);
}

int BuildCfgWrap::GetFootPrint(unsigned build_id, vector<unsigned> & foots)
{
	if (!ConfigManager::Instance()->buildsFootIndex.count(build_id))
	{
		error_log("param error. build_id=%u", build_id);
		throw runtime_error("param_error");
	}

	foots.resize(2);
	unsigned index_ = ConfigManager::Instance()->buildsFootIndex[build_id];

	foots[0u] = cfg_.build_print(index_).footprint(0u);
	foots[1u] = cfg_.build_print(index_).footprint(1u);

	return 0;
}

const ConfigBuilding::StorageHouse & BuildCfgWrap::GetStorageCfgById(unsigned id) const
{
	if (!ConfigManager::Instance()->buildsIndex.count(id))
	{
		error_log("param error. id=%u", id);
		throw runtime_error("param_error");
	}

	if (build_type_storage != ConfigManager::Instance()->buildsIndex[id].first)
	{
		error_log("type error. id=%u", id);
		throw runtime_error("type_error");
	}

	unsigned index_ = ConfigManager::Instance()->buildsIndex[id].second;

	return cfg_.storagehouse(index_);
}

const ConfigBuilding::CropLand & BuildCfgWrap::GetCropLandCfg() const
{
	if (cfg_.cropland_size() == 0)
	{
		error_log("corpland config error.");
		throw runtime_error("config_error");
	}

	return cfg_.cropland(0u);
}

const ConfigBuilding::AnimalResidence & BuildCfgWrap::GetAnimalResidenceCfgById(unsigned id) const
{
	if (!ConfigManager::Instance()->buildsIndex.count(id))
	{
		error_log("param error. id=%u", id);
		throw runtime_error("param_error");
	}

	if (build_type_animal_residence != ConfigManager::Instance()->buildsIndex[id].first)
	{
		error_log("type error. id=%u", id);
		throw runtime_error("type_error");
	}

	unsigned index_ = ConfigManager::Instance()->buildsIndex[id].second;

	return cfg_.animal_residence(index_);
}

const ConfigBuilding::Animal & BuildCfgWrap::GetAnimalCfgById(unsigned id) const
{
	if (!ConfigManager::Instance()->buildsIndex.count(id))
	{
		error_log("param error. id=%u", id);
		throw runtime_error("param_error");
	}

	if (build_type_animal != ConfigManager::Instance()->buildsIndex[id].first)
	{
		error_log("type error. id=%u", id);
		throw runtime_error("type_error");
	}

	unsigned index_ = ConfigManager::Instance()->buildsIndex[id].second;

	return cfg_.animal(index_);
}

const ConfigBuilding::ProduceEquipment & BuildCfgWrap::GetProduceCfgById(unsigned id) const
{
	if (!ConfigManager::Instance()->buildsIndex.count(id))
	{
		error_log("param error. id=%u", id);
		throw runtime_error("param_error");
	}

	if (build_type_produce_equipment != ConfigManager::Instance()->buildsIndex[id].first)
	{
		error_log("type error. id=%u", id);
		throw runtime_error("type_error");
	}

	unsigned index_ = ConfigManager::Instance()->buildsIndex[id].second;

	return cfg_.produce_equipment(index_);
}

const ConfigBuilding::FruitTree & BuildCfgWrap::GetFruitTreeCfgById(unsigned id) const
{
	if (!ConfigManager::Instance()->buildsIndex.count(id))
	{
		error_log("param error. id=%u", id);
		throw runtime_error("param_error");
	}

	if (build_type_fruit_tree != ConfigManager::Instance()->buildsIndex[id].first)
	{
		error_log("type error. id=%u", id);
		throw runtime_error("type_error");
	}

	unsigned index_ = ConfigManager::Instance()->buildsIndex[id].second;

	return cfg_.fruit_tree(index_);
}

const ConfigBuilding::Decoration & BuildCfgWrap::GetDecorationCfgById(unsigned id) const
{
	if (!ConfigManager::Instance()->buildsIndex.count(id))
	{
		error_log("param error. id=%u", id);
		throw runtime_error("param_error");
	}

	if (build_type_decorate != ConfigManager::Instance()->buildsIndex[id].first)
	{
		error_log("type error. id=%u", id);
		throw runtime_error("type_error");
	}

	unsigned index_ = ConfigManager::Instance()->buildsIndex[id].second;

	return cfg_.decorate(index_);
}

BarrierCfgWrap::BarrierCfgWrap()
	: cfg_(ConfigManager::Instance()->barriers.m_config)
{

}

int BarrierCfgWrap::GetFootPrint(unsigned barrier_id, vector<unsigned> & foots)
{
	if (!ConfigManager::Instance()->BarrierIndex.count(barrier_id))
	{
		error_log("param error. barrier_id=%u", barrier_id);
		throw runtime_error("param_error");
	}

	foots.resize(2);
	unsigned index_ = ConfigManager::Instance()->buildsFootIndex[barrier_id];

	foots[0u] = cfg_.barriers(index_).footprint(0u);
	foots[1u] = cfg_.barriers(index_).footprint(1u);

	return 0;
}

ItemCfgWrap::ItemCfgWrap()
	: cfg_(ConfigManager::Instance()->propsitem.m_config)
{

}

const ConfigItem::PropItem & ItemCfgWrap::GetPropsItem(unsigned propsid) const
{
	if (!ConfigManager::Instance()->ItemIndex.count(propsid))
	{
		error_log("param error. propsid=%u", propsid);
		throw runtime_error("param_error");
	}

	unsigned index = ConfigManager::Instance()->ItemIndex[propsid];

	return cfg_.itemes(index);
}

ProductlineCfgWrap::ProductlineCfgWrap()
	: cfg_(ConfigManager::Instance()->productline.m_config)
{

}

const ConfigProductLine::EquipLine & ProductlineCfgWrap::GetEquiplineCfg(unsigned equipid) const
{
	if (!ConfigManager::Instance()->productlineIndex.count(equipid))
	{
		error_log("param error. equipid=%u", equipid);
		throw runtime_error("param_error");
	}

	unsigned index = ConfigManager::Instance()->productlineIndex[equipid].second;

	return cfg_.equipline(index);
}

int ProductlineCfgWrap::GetEquipProductIndex(unsigned equipid, unsigned productid) const
{
	const ConfigProductLine::EquipLine & equipline = GetEquiplineCfg(equipid);

	//遍历产品列表
	for(int i = 0; i < equipline.product_list_size(); ++i)
	{
		for(int j = 0; j < equipline.product_list(i).props_size(); ++j)
		{
			if (equipline.product_list(i).props(j).id() == productid)
			{
				return i;
			}
		}
	}

	error_log("param error. equipid=%u,productid=%u", equipid, productid);
	throw runtime_error("param_error");
}

const ConfigProductLine::AnimalLine & ProductlineCfgWrap::GetAnimallineCfg(unsigned animalid) const
{
	if (!ConfigManager::Instance()->productlineIndex.count(animalid))
	{
		error_log("param error. animalid=%u", animalid);
		throw runtime_error("param_error");
	}

	unsigned index = ConfigManager::Instance()->productlineIndex[animalid].second;

	return cfg_.animal_line(index);
}

const ConfigProductLine::MaterailReward & ProductlineCfgWrap::GetMaterialCfg() const
{
	return cfg_.get_storage_material();
}
