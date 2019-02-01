/*
 * ConfigWrap.h
 *
 *  Created on: 2016-9-12
 *      Author: dawx62fac
 */

#ifndef USERCFGWRAP_H_
#define USERCFGWRAP_H_

#include "DataInc.h"

class UserCfgWrap
{
public:
	UserCfgWrap();

	const UserCfg::User& User() const;
	const UserCfg::UserBase& UserBase() const;
private:
	const UserCfg::User& cfg_;
};

class ActivityCfgWrap
{
public:
	ActivityCfgWrap();

	const User::ActivityItem& GetItem(unsigned id) const;
private:
	const User::Activity& cfg_;
};

class BuildCfgWrap
{
public:
	enum
	{
		build_type_len = 10000, //建筑id类型映射关系
	};

	BuildCfgWrap();

	const ConfigBuilding::Buildings & GetBuildingCfg() const {return cfg_;}

	//获取设备升星的解锁等级
	int GetUnlockUpgradeStarLevel() const;

	int GetBuildType(unsigned build_id) const;
	//根据用户等级，获取对应的下标
	int GetLevelIndex(const google::protobuf::RepeatedField< ::google::protobuf::uint32 >& needlevel, unsigned lv);

	//获取数目对应的消耗
	int GetGainNumIndex(const google::protobuf::RepeatedField< ::google::protobuf::uint32 >& gain_num, unsigned num);

	//获取房子的配置
	const ConfigBuilding::House & GetHouseCfg() const;

	//判断是否是仓库
	bool IsStorage(unsigned build_id);

	//获取仓库id
	const ConfigBuilding::StorageHouse & GetStorageCfg(unsigned index) const;

	//获取建筑占据的格子数
	int GetFootPrint(unsigned build_id, vector<unsigned> & foots);

	//根据建筑id，获取仓库配置
	const ConfigBuilding::StorageHouse & GetStorageCfgById(unsigned id) const;

	//根据建筑id，获取农地配置
	const ConfigBuilding::CropLand & GetCropLandCfg() const;

	//获取动物住所配置
	const ConfigBuilding::AnimalResidence & GetAnimalResidenceCfgById(unsigned id) const;

	//获取动物配置
	const ConfigBuilding::Animal & GetAnimalCfgById(unsigned id) const;

	//根据建筑id，获取生产设备配置
	const ConfigBuilding::ProduceEquipment & GetProduceCfgById(unsigned id) const;

	//根据建筑id,获取果树配置
	const ConfigBuilding::FruitTree & GetFruitTreeCfgById(unsigned id) const;

	//根据建筑id,获取装饰配置
	const ConfigBuilding::Decoration & GetDecorationCfgById(unsigned id) const;

private:
	const ConfigBuilding::Buildings & cfg_;
};

class BarrierCfgWrap
{
public:
	BarrierCfgWrap();

	const ConfigBuilding::Barriers & GetBarrierCfg() const {return cfg_;}

	//获取建筑占据的格子数
	int GetFootPrint(unsigned barrier_id, vector<unsigned> & foots);

private:
	const ConfigBuilding::Barriers & cfg_;
};

class ItemCfgWrap
{
public:
	ItemCfgWrap();

	//获取道具配置
	const ConfigItem::PropItem & GetPropsItem(unsigned propsid) const;
private:
	const ConfigItem::Items & cfg_;
};

//------------------生产线配置-------------
class ProductlineCfgWrap
{
public:
	ProductlineCfgWrap();

	//获取设备的生产线配置
	const ConfigProductLine::EquipLine & GetEquiplineCfg(unsigned equipid) const;

	int GetEquipProductIndex(unsigned equipid, unsigned productid) const;

	//获取动物的生产线配置
	const ConfigProductLine::AnimalLine & GetAnimallineCfg(unsigned animalid) const;

	const ConfigProductLine::MaterailReward & GetMaterialCfg() const;

private:
	const ConfigProductLine::ProductLine & cfg_;
};

#endif /* USERCFGWRAP_H_ */
