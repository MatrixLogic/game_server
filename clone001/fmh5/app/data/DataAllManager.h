#ifndef DATA_ALL_MANAGER_H_
#define DATA_ALL_MANAGER_H_

#include "Kernel.h"
#include "DBCMultipleTemplate.h"
#include "DataBuildings.h"
#include "DataCropLand.h"
#include "DataItem.h"
#include "DataProduceEquip.h"
#include "DataAnimal.h"
#include "DataEquipmentStar.h"

class DataBuildingMgr: public DBCMultipleTemplate<DataBuildings, DB_BUILD,
		DB_BUILD_FULL, CDataBuildings>, public CSingleton<DataBuildingMgr> {
public:
	virtual void CallDestroy() {
		Destroy();
	}

	const char* name() const {
		return "DataBuildings";
	}

	int Init(unsigned uid);

	DataBuildings & AddNewBuilding(unsigned uid, unsigned build_id);

	//获取指定建筑id的数目
	int GetBuildNum(unsigned uid, uint16_t build_id);

	//获取所有已建筑数目
	int GetAllBuildNum(unsigned uid);

	unsigned GetUserNextUd(unsigned uid);

	//适合获取唯一存在的建筑，如果不是唯一的，那么ud值是第一个
	unsigned GetBuildUd(unsigned uid, uint16_t build_id);

private:
	map<unsigned, uint16_t> m_userMaxUd;
	map<unsigned, map<uint16_t, vector<uint16_t> > > m_BuildUd; //uid =》( build->ud)
};

class DataItemManager : public DBCMultipleTemplate<DataItem, DB_ITEM, DB_ITEM_FULL, CDataItem>
						, public CSingleton<DataItemManager>
{
public:
	virtual void CallDestroy() { Destroy();}
	const char* name() const  { return "DataItemManager"; }

	int Init(unsigned uid);

	//获取下一个ud
	unsigned GetUserNextUd(unsigned uid);

	//判断道具id用户是否存在
	bool IsPropsExist(unsigned uid, unsigned propsid);

	//适合获取唯一存在的建筑，如果不是唯一的，那么ud值是第一个
	unsigned GetPropsUd(unsigned uid, unsigned propsid);

	//新增道具
	int AddNewProps(DataItem & propsitem);

	//删除装备
	int DelProps(DataItem & propsitem);

private:
	map<unsigned, uint32_t> m_userMaxUd;
	map<unsigned, map<uint32_t, vector<uint32_t> > > m_PropsUd; //uid =》(propsid->ud)
};

class DataCroplandManager: public DBCMultipleTemplate<DataCropland, DB_CROPLAND, DB_CROPLAND_FULL, CDataCropland>,
	public CSingleton<DataCroplandManager>
{
public:
	virtual void CallDestroy()
	{
		Destroy();
	}

	const char* name() const
	{
		return "DataCroplandManager";
	}

	unsigned int AddNewCropLand(unsigned uid, unsigned cropud);
};

class DataProduceequipManager : public DBCMultipleTemplate<DataProduceequip, DB_PRODUCEEQUIP, DB_PRODUCEEQUIP_FULL, CDataProduceequip> ,
	public CSingleton<DataProduceequipManager>
{
public:
	virtual void CallDestroy()
	{
		Destroy();
	}

	const char* name() const
	{
		return "DataProduceequipManager";
	}

	unsigned int AddNewEquip(unsigned uid, unsigned equipud);
};

class DataAnimalManager : public DBCMultipleTemplate<DataAnimal, DB_ANIMAL, DB_ANIMAL_FULL, CDataAnimal> ,
	public CSingleton<DataAnimalManager>
{
public:
	enum
	{
		animal_ud_begin = 10000000,  //ud起始值
	};

	virtual void CallDestroy()
	{
		Destroy();
	}

	const char* name() const
	{
		return "DataAnimalManager";
	}

	int Init(unsigned uid);

	//获取下一个ud
	unsigned GetUserNextUd(unsigned uid);

	//获取指定动物住所已容纳的动物数目
	unsigned GetBuildAdoptedNum(unsigned uid, unsigned buildud) const;

	//获取指定动物的领养数目
	unsigned GetAdoptedNum(unsigned uid, unsigned animalid) const;

	//动物
	DataAnimal & AddNewAnimal(unsigned uid, unsigned animalid, unsigned buildud);

private:
	map<unsigned, uint32_t> m_userMaxUd;
	map<unsigned, map<uint32_t, vector<uint32_t> > > m_AnimalIdUd; //uid =》(animal->ud)  动物id相关的映射
	map<unsigned, map<uint32_t, vector<uint32_t> > > m_BuildUd; //uid =》(buildud->ud)  动物住所相关的映射
};

class DataEquipmentStarManager : public DBCMultipleTemplate<DataEquipmentStar, DB_EQUIPMENT_STAR, DB_EQUIPMENT_STAR_FULL, CDataEquipmentStar> ,
	public CSingleton<DataEquipmentStarManager>
{
public:
	virtual void CallDestroy() 	{ Destroy();}

	const char* name() const
	{
		return "DataEquipment_starManager";
	}
};

#endif //DATA_ALL_MANAGER_H_
