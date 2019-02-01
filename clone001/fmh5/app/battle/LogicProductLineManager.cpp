#include "LogicProductLineManager.h"
#include "ServerInc.h"

void DataCropProduceRoutine::CheckUd(unsigned buildud)
{
	//调用基类
	DataQueueRoutine::CheckUd(buildud);

	if (DataCroplandManager::Instance()->IsExistItem(uid_, buildud))
	{
		error_log("invalid data, cropland is not exist. uid=%u,buildud=%u", uid_, buildud);
		throw runtime_error("cropland_not_exist");
	}

	//获取农地数据
	DataCropland & cropland = DataCroplandManager::Instance()->GetData(uid_, buildud);

	if(status_growup != cropland.status)
	{
		error_log("cropland status error. uid=%u,buildud=%u", uid_, buildud);
		throw runtime_error("cropland_not_growing");
	}
}

void DataCropProduceRoutine::GetPriceAndATime(unsigned buildud, int & cash, int & diffts)
{
	//扣钻石，立即完成
	unsigned nowts = Time::GetGlobalTime();
	diffts = endts_ > nowts ? endts_ - nowts : 0;

	//获取农地数据
	DataCropland & cropland = DataCroplandManager::Instance()->GetData(uid_, buildud);

	//根据作物配置，获取加速价格
	const ConfigItem::PropItem & propscfg = ItemCfgWrap().GetPropsItem(cropland.plant);

	if (0 == propscfg.speed_price())
	{
		error_log("speed price config error. uid=%u,propsid=%u", uid_, cropland.plant);
		throw runtime_error("config_error");
	}

	cash = ceil(static_cast<double>(diffts)/propscfg.speed_price());
}

void DataCropProduceRoutine::SingleRoutineEnd(unsigned buildud, ProtoPush::PushBuildingsCPP * msg)
{
	//获取农地数据
	DataCropland & cropland = DataCroplandManager::Instance()->GetData(uid_, buildud);

	//成熟
	cropland.Mature();
	cropland.SetMessage(msg->add_croplands());

	//结束队列
	LogicQueueManager::Instance()->FinishQueue(uid_, buildud);
}

void DataProduceEquipRoutine::CheckUd(unsigned buildud)
{
	//调用基类
	DataQueueRoutine::CheckUd(buildud);

	if (DataProduceequipManager::Instance()->IsExistItem(uid_, buildud))
	{
		error_log("invalid data, build is not exist. uid=%u,buildud=%u", uid_, buildud);
		throw runtime_error("build_not_exist");
	}

	//获取设备数据
	DataProduceequip & equipment = DataProduceequipManager::Instance()->GetData(uid_, buildud);

	if(status_procing != equipment.status)
	{
		error_log("equip not producing. uid=%u,buildud=%u", uid_, buildud);
		throw runtime_error("equip_not_producing");
	}
}

void DataProduceEquipRoutine::GetPriceAndATime(unsigned buildud, int & cash, int & diffts)
{
	//扣钻石，立即完成
	unsigned nowts = Time::GetGlobalTime();
	diffts = endts_ > nowts ? endts_ - nowts : 0;

	//获取设备数据
	DataProduceequip & equipment = DataProduceequipManager::Instance()->GetData(uid_, buildud);

	//从生产队列中取队头元素，就是正在生产的产品
	unsigned product = equipment.GetQueueFront();

	//根据作物配置，获取加速价格
	const ConfigItem::PropItem & propscfg = ItemCfgWrap().GetPropsItem(product);

	if (0 == propscfg.speed_price())
	{
		error_log("speed price config error. uid=%u,propsid=%u", uid_, product);
		throw runtime_error("config_error");
	}

	cash = ceil(static_cast<double>(diffts)/propscfg.speed_price());
}

void DataProduceEquipRoutine::SingleRoutineEnd(unsigned buildud, ProtoPush::PushBuildingsCPP * msg)
{
	//获取设备数据
	DataProduceequip & equipment = DataProduceequipManager::Instance()->GetData(uid_, buildud);

	//设备升星功能
	EquipmentStarUpgrade(equipment, msg);

	//生产完成.此时设备可能状态有两种 1 -> 2或 1->0
	equipment.FinishCurrentJob();

	//结束队列.
	LogicQueueManager::Instance()->FinishQueue(uid_, buildud);

	//进行下一步动作，取决于设备状态以及生产队列
	LogicProductLineManager::Instance()->ProduceEquipNextMove(equipment);

	DataProduceequipManager::Instance()->UpdateItem(equipment);

	//设备的最终状态
	equipment.SetMessage(msg->add_equipments());
}

int DataProduceEquipRoutine::EquipmentStarUpgrade(DataProduceequip & equipment,	ProtoPush::PushBuildingsCPP * msg)
{
	//获取升星的解锁条件
	BuildCfgWrap buildcfgwrap;
	int needlevel = buildcfgwrap.GetUnlockUpgradeStarLevel();
	unsigned uid = equipment.uid;

	//判断用户等级是否满足升星的条件
	if (BaseManager::Instance()->Get(uid).level < needlevel)
	{
		return 0;
	}

	//获取该设备的建筑id，根据id获取建筑的星级属性
	DataBuildings & databuild = DataBuildingMgr::Instance()->GetData(uid, equipment.id);
	DataEquipmentStar & datastar = DataEquipmentStarManager::Instance()->GetData(uid, databuild.build_id);

	//获取生产设备的配置
	const ConfigBuilding::ProduceEquipment & equipcfg = buildcfgwrap.GetProduceCfgById(databuild.build_id);

	//判断星级属性是否达到最大
	if (datastar.star >= equipcfg.upgrade_star_size())
	{
		//星级已达到最大，不再增加使用时间
		return 0;
	}

	//获取当前商品的id，以此获得设备生产该商品所需的时间
	int productid = equipment.GetQueueFront();
	const ConfigItem::PropItem & propscfg = ItemCfgWrap().GetPropsItem(productid);

	int percent = 0;

	//计算星级属性中，生产时间缩短的百分比
	for(int i = 0; i < datastar.star; ++i)
	{
		if (equipcfg.upgrade_star(i).ptype() == property_type_time)
		{
			percent = equipcfg.upgrade_star(i).value();  //扣除时间的百分比
		}
	}

	datastar.usedtime += (100 - percent)/static_cast<double>(100) * propscfg.time_gain();

	//根据设备的使用时间，更新当前的星级属性
	int hour = datastar.usedtime/3600;  //小时

	for(int i = equipcfg.upgrade_star_size() - 1; i >= 0 ; --i)
	{
		if (hour >= equipcfg.upgrade_star(i).need_time())
		{
			datastar.star = i+1;
		}
	}

	DataEquipmentStarManager::Instance()->UpdateItem(datastar);
	datastar.SetMessage(msg->add_equipmentstar());

	return 0;
}

void DataAnimalRoutine::CheckUd(unsigned animalud)
{
	vector<unsigned>::iterator it = find(uds_.begin(), uds_.end(), animalud);

	if (it == uds_.end())
	{
		error_log("animal not exist. uid=%u,ud=%u", uid_, animalud);
		throw runtime_error("animal_not_exist");
	}

	if (DataAnimalManager::Instance()->IsExistItem(uid_, animalud))
	{
		error_log("invalid data, animal is not exist. uid=%u,animalud=%u", uid_, animalud);
		throw runtime_error("animal_not_exist");
	}

	//获取动物数据
	DataAnimal & animal = DataAnimalManager::Instance()->GetData(uid_, animalud);

	if(status_growup != animal.status)
	{
		error_log("animal not in growing. uid=%u, animalud=%u", uid_, animalud);
		throw runtime_error("animal_not_growing");
	}
}

void DataAnimalRoutine::GetPriceAndATime(unsigned animalud, int & cash, int & diffts)
{
	//扣钻石，立即完成
	unsigned nowts = Time::GetGlobalTime();
	diffts = endts_ > nowts ? endts_ - nowts : 0;

	//获取动物数据
	DataAnimal & animal = DataAnimalManager::Instance()->GetData(uid_, animalud);

	//获取动物生产线的配置，判断传递的饲料是否是该动物需要的饲料
	const ConfigProductLine::AnimalLine & animalcfg = ProductlineCfgWrap().GetAnimallineCfg(animal.animal_id);
	unsigned productid = animalcfg.product().props(0u).id();

	//根据作物配置，获取加速价格
	const ConfigItem::PropItem & propscfg = ItemCfgWrap().GetPropsItem(productid);

	//根据作物配置，获取加速价格
	if (0 == propscfg.speed_price())
	{
		error_log("speed price config error. uid=%u,propsid=%u", uid_, productid);
		throw runtime_error("config_error");
	}

	cash = ceil(static_cast<double>(diffts)/propscfg.speed_price());
}

void DataAnimalRoutine::SingleRoutineEnd(unsigned animalud, ProtoPush::PushBuildingsCPP * msg)
{
	//获取动物数据
	DataAnimal & animal = DataAnimalManager::Instance()->GetData(uid_, animalud);

	//动物饱腹
	animal.Full();
	DataAnimalManager::Instance()->UpdateItem(animal);

	//结束队列.
	LogicQueueManager::Instance()->FinishQueue(uid_, animalud);

	animal.SetMessage(msg->add_animals());
}

LogicProductLineManager::LogicProductLineManager()
{

}

int LogicProductLineManager::NewUser(unsigned uid)
{
	//动物的ud初始化
	DataAnimalManager::Instance()->Init(uid);

	return 0;
}

int LogicProductLineManager::CheckLogin(unsigned uid)
{
	//动物的ud初始化
	DataAnimalManager::Instance()->Init(uid);

	//地块生产
	OnlineCropland(uid);

	//设备生产
	OnlineEquipProduce(uid);

	//动物生产
	OnlineAnimal(uid);

	return 0;
}

int LogicProductLineManager::OnlineCropland(unsigned uid)
{
	vector<unsigned> croplands;
	//将正在建筑的建筑加入定时任务队列
	DataCroplandManager::Instance()->GetIndexs(uid, croplands);
	map<unsigned, vector<unsigned> > tobuild;  //endts -> uds

	for(int i = 0; i < croplands.size(); ++i)
	{
		DataCropland & crop = DataCroplandManager::Instance()->GetDataByIndex(croplands[i]);

		if (crop.harvest_time > 0)
		{
			tobuild[crop.harvest_time].push_back(crop.id);
		}
	}

	//遍历map
	for(map<unsigned, vector<unsigned> >::iterator viter = tobuild.begin(); viter != tobuild.end(); ++viter)
	{
		LogicQueueManager::Instance()->JoinRoutine<DataCropProduceRoutine>(uid, viter->first, viter->second);
	}

	return 0;
}

int LogicProductLineManager::OnlineEquipProduce(unsigned uid)
{
	vector<unsigned> equips;
	//将正在建筑的建筑加入定时任务队列
	DataProduceequipManager::Instance()->GetIndexs(uid, equips);
	map<unsigned, vector<unsigned> > tobuild;  //endts -> uds

	for(int i = 0; i < equips.size(); ++i)
	{
		DataProduceequip & equip = DataProduceequipManager::Instance()->GetDataByIndex(equips[i]);

		if (equip.finish_time > 0)
		{
			tobuild[equip.finish_time].push_back(equip.id);
		}
	}

	//遍历map
	for(map<unsigned, vector<unsigned> >::iterator viter = tobuild.begin(); viter != tobuild.end(); ++viter)
	{
		LogicQueueManager::Instance()->JoinRoutine<DataProduceEquipRoutine>(uid, viter->first, viter->second);
	}

	return 0;
}

int LogicProductLineManager::OnlineAnimal(unsigned uid)
{
	vector<unsigned> animals;

	//将正在生长的动物加入定时任务队列
	DataAnimalManager::Instance()->GetIndexs(uid, animals);
	map<unsigned, vector<unsigned> > tobuild;  //endts -> uds

	for(int i = 0; i < animals.size(); ++i)
	{
		DataAnimal & animal = DataAnimalManager::Instance()->GetDataByIndex(animals[i]);

		if (animal.full_time > 0)
		{
			tobuild[animal.full_time].push_back(animal.id);
		}
	}

	//遍历map
	for(map<unsigned, vector<unsigned> >::iterator viter = tobuild.begin(); viter != tobuild.end(); ++viter)
	{
		LogicQueueManager::Instance()->JoinRoutine<DataAnimalRoutine>(uid, viter->first, viter->second);
	}

	return 0;
}

int LogicProductLineManager::ProduceAfterBuild(unsigned uid, unsigned ud, unsigned type, bool ispush, ProtoBuilding::BuildResp * resp)
{
	ProtoPush::PushBuildingsCPP *msg = NULL;
	unsigned index = 0;

	switch(type)
	{
		case build_type_corpland:
			{
				//农地
				//新增地块生产需要的农地信息
				index = DataCroplandManager::Instance()->AddNewCropLand(uid, ud);
				DataCropland & crop = DataCroplandManager::Instance()->GetDataByIndex(index);

				if (!ispush)
				{
					//不推送
					crop.SetMessage(resp->mutable_lands());
				}
				else
				{
					msg = new ProtoPush::PushBuildingsCPP;
					crop.SetMessage(msg->add_croplands());
				}
			}
			break;
		case build_type_animal_residence :
			//动物住所
			break;
		case build_type_produce_equipment :
			{
				//生产设备
				index = DataProduceequipManager::Instance()->AddNewEquip(uid, ud);

				DataProduceequip & equip = DataProduceequipManager::Instance()->GetDataByIndex(index);
				DataBuildings & build = DataBuildingMgr::Instance()->GetData(uid, ud);

				//队列长度初始值读配置
				//生产设备
				const ConfigBuilding::ProduceEquipment & producecfg = BuildCfgWrap().GetProduceCfgById(build.build_id);
				equip.queuenum = producecfg.init_queue();  //初始化队列长度

				DataProduceequipManager::Instance()->UpdateItem(equip);

				if (ispush)
				{
					msg = new ProtoPush::PushBuildingsCPP;
					equip.SetMessage(msg->add_equipments());
				}
			}

			break;
		case build_type_fruit_tree :
			break;
		case build_type_decorate:
			break;
		case build_type_house:
			break;
		case build_type_storage:
			break;
		default:
			break;
	}

	if (NULL != msg)
	{
		LogicManager::Instance()->sendMsg(uid, msg);
	}

	return 0;
}

int LogicProductLineManager::Process(unsigned uid, ProtoProduce::PlantCropReq* req, ProtoProduce::PlantCropResp* resp)
{
	unsigned cropud = req->plant();

	vector<unsigned> lands;

	for(int i = 0; i < req->uds_size(); ++i)
	{
		lands.push_back(req->uds(i));
	}

	PlantCrop(uid, cropud, lands, resp);

	return 0;
}

int LogicProductLineManager::PlantCrop(unsigned uid, unsigned propud, vector<unsigned> &lands, ProtoProduce::PlantCropResp *resp)
{
	//验证参数
	if(!DataItemManager::Instance()->IsExistItem(uid, propud))
	{
		error_log("crop ud is not exist. uid=%u,ud=%u", uid, propud);
		throw runtime_error("crop_not_exist");
	}

	//获取农作物
	DataItem & crops = DataItemManager::Instance()->GetData(uid, propud);

	//可以种植的地块
	vector<unsigned> validlands;

	//批量种植，批量消耗
	for(size_t i = 0; i < lands.size(); ++i)
	{
		//判断地块是否存在，如果不存在，则记录错误日志，不做功能处理
		unsigned ud = lands[i];

		if (!DataCroplandManager::Instance()->IsExistItem(uid, ud))
		{
			error_log("cropud is not exist. uid=%u,ud=%u", uid, ud);
			continue;
		}

		//判断地块是否已有作物
		DataCropland & cropland = DataCroplandManager::Instance()->GetData(uid, ud);

		if (status_free != cropland.status)
		{
			error_log("cropland is unavailable. uid=%u,ud=%u", uid, ud);
			continue;
		}

		validlands.push_back(lands[i]);
	}

	//判断作物是否为空
	if (0 == validlands.size())
	{
		error_log("no lands can plant. uid=%u,cropud=%u", uid, propud);
		throw runtime_error("none_land_plant");
	}

	//判断道具是否满足条件
	if (0 == crops.item_cnt)
	{
		error_log("no crop can plant. uid=%u,cropud=%u", uid, propud);
		throw runtime_error("none_crop_plant");
	}

	int realnum = crops.item_cnt;
	realnum = min((size_t)realnum, validlands.size());

	//获取农作物的配置
	ItemCfgWrap propscfgwrap;
	const ConfigItem::PropItem & propscfg = propscfgwrap.GetPropsItem(crops.props_id);

	unsigned need_time = propscfg.time_gain();
	unsigned endts = Time::GetGlobalTime() + need_time;

	//作物消耗
	CommonGiftConfig::CommonModifyItem cfg;
	CommonGiftConfig::PropsItem* cropscfg = cfg.add_props();
	cropscfg->set_id(crops.props_id);
	cropscfg->set_count(-realnum);

	LogicUserManager::Instance()->CommonProcess(uid, cfg, "CropPlant", resp->mutable_commons());

	//种植
	for(int i = 0; i < realnum; ++i)
	{
		unsigned ud = validlands[i];
		DataCropland & cropland = DataCroplandManager::Instance()->GetData(uid, ud);

		cropland.harvest_time = endts;
		cropland.plant = crops.props_id;
		cropland.status = status_growup;

		DataCroplandManager::Instance()->UpdateItem(cropland);

		cropland.SetMessage(resp->add_cropland());
	}

	//将批量的种植任务加入到常规的任务中
	LogicQueueManager::Instance()->JoinRoutine<DataCropProduceRoutine>(uid, endts, validlands);

	return 0;
}

int LogicProductLineManager::Process(unsigned uid, ProtoProduce::ReapCropReq* req, ProtoProduce::ReapCropResp* resp)
{
	vector<unsigned> lands;

	for(int i = 0; i < req->uds_size(); ++i)
	{
		lands.push_back(req->uds(i));
	}

	ReapCrop(uid, lands, resp);

	return 0;
}

int LogicProductLineManager::ReapCrop(unsigned uid, vector<unsigned> & lands, ProtoProduce::ReapCropResp * resp)
{
	//可以收获的地块
	vector<unsigned> gainlands;

	for(size_t i = 0; i < lands.size(); ++i)
	{
		//判断地块是否存在，如果不存在，则记录错误日志，不做功能处理
		unsigned ud = lands[i];

		if (!DataCroplandManager::Instance()->IsExistItem(uid, ud))
		{
			error_log("cropud is not exist. uid=%u,ud=%u", uid, ud);
			continue;
		}

		//判断地块是否已有作物
		DataCropland & cropland = DataCroplandManager::Instance()->GetData(uid, ud);

		if (status_harvest != cropland.status)
		{
			error_log("cropland isn't harvest. uid=%u,ud=%u", uid, ud);
			continue;
		}

		gainlands.push_back(ud);
	}

	unsigned restspace = LogicBuildManager::Instance()->GetStorageRestSpace(uid, type_granary);

	//一块地，默认产出2份作物
	int gainum = min(restspace, (uint32_t)gainlands.size() * 2)/2;  //之所以除以2，是为了得到可以收获的地块数目
	map<unsigned, unsigned> crops;  //id->num

	CommonGiftConfig::CommonModifyItem reward;

	for(int i = 0; i < gainum; ++i)
	{
		unsigned ud = gainlands[i];
		DataCropland & cropland = DataCroplandManager::Instance()->GetData(uid, ud);

		//获取收割作物的经验奖励
		GetExpReward(cropland.plant, 2, reward);

		crops[cropland.plant] += 2;  //产出2份
		cropland.Harvest();
		DataCroplandManager::Instance()->UpdateItem(cropland);
		cropland.SetMessage(resp->add_cropland());
	}

	//收获作物
	for(map<unsigned, unsigned>::iterator iter = crops.begin(); iter != crops.end(); ++iter)
	{
		LogicPropsManager::Instance()->AddProps(uid, iter->first, iter->second, "CropHarvest", resp->mutable_commons()->mutable_props());
	}

	//额外奖励，暂时只处理了经验
	LogicUserManager::Instance()->CommonProcess(uid, reward, "CropHarvest", resp->mutable_commons());

	//判断是否满仓
	if (restspace < gainlands.size() * 2)
	{
		//满仓
		resp->set_isfull(true);
	}
	else
	{
		resp->set_isfull(false);
	}

	//更新收获次数
	ActAfterHarvest(uid, gainum * 2, resp->mutable_commons());

	return 0;
}

int LogicProductLineManager::Process(unsigned uid, ProtoProduce::ExpandQueueReq* req, ProtoProduce::ExpandQueueResp* resp)
{
	unsigned equipud = req->equipud();

	ExpandQueue(uid, equipud, resp);

	return 0;
}

int LogicProductLineManager::ExpandQueue(unsigned uid, unsigned equipud, ProtoProduce::ExpandQueueResp * resp)
{
	unsigned build_id = GetBuildId(uid, equipud);

	//根据建筑id，获取设备生产线的配置
	const ConfigProductLine::EquipLine & equipcfg = ProductlineCfgWrap().GetEquiplineCfg(build_id);
	//获取生产设备建筑的配置
	const ConfigBuilding::ProduceEquipment & producecfg = BuildCfgWrap().GetProduceCfgById(build_id);

	DataProduceequip & equipment = DataProduceequipManager::Instance()->GetData(uid, equipud);

	if (equipcfg.maxqueue() < producecfg.init_queue())
	{
		error_log("config error. uid=%u,equipud=%u", uid, equipud);
		throw runtime_error("config_error");
	}

	//判断队列是否最大
	if (equipment.queuenum >= equipcfg.maxqueue())
	{
		error_log("queue already max. uid=%u,equipud=%u", uid, equipud);
		throw runtime_error("queue_already_max");
	}

	//下一个队列的解锁消耗索引
	int nextindex = equipment.queuenum - producecfg.init_queue();

	if (nextindex < 0)
	{
		nextindex = 0;
	}

	//消耗
	LogicUserManager::Instance()->CommonProcess(uid, equipcfg.queue_price(nextindex), "QueueExpand", resp->mutable_commons());

	//增加设备的队列数
	equipment.queuenum += 1;
	DataProduceequipManager::Instance()->UpdateItem(equipment);

	equipment.SetMessage(resp->mutable_equipment());

	return 0;
}

int LogicProductLineManager::Process(unsigned uid, ProtoProduce::JoinQueueReq* req, ProtoProduce::JoinQueueResp* resp)
{
	unsigned equipud = req->equipud();
	unsigned product = req->propsid();

	JoinEquipQueue(uid, equipud, product, resp);

	return 0;
}

int LogicProductLineManager::JoinEquipQueue(unsigned uid, unsigned equipud, unsigned productid, ProtoProduce::JoinQueueResp * resp)
{
	//获取设备id
	unsigned build_id = GetBuildId(uid, equipud);

	DataProduceequip & equipment = DataProduceequipManager::Instance()->GetData(uid, equipud);
	int workqueue = equipment.GetWorkQueueNum(equipment.queuedata);

	//判断是否有空余队列
	if (workqueue >= equipment.queuenum)
	{
		error_log("queue is full. uid=%u,equipud=%u", uid, equipud);
		throw runtime_error("queue_is_full");
	}

	//检查该设备是否产出该产品
	ProductlineCfgWrap productlinecfg;
	productlinecfg.GetEquipProductIndex(build_id, productid);

	//判断该产品是否解锁
	DBCUserBaseWrap userwrap(uid);
	ItemCfgWrap itemcfgwrap;
	const ConfigItem::PropItem & propscfg = itemcfgwrap.GetPropsItem(productid);

	if (propscfg.unlock_level() > userwrap.Obj().level)
	{
		error_log("product locked. uid=%u,equipud=%u,productid=%u", uid, equipud, productid);
		throw runtime_error("product_locked");
	}

	//根据产品id，进行原料的扣除
	//消耗
	LogicUserManager::Instance()->CommonProcess(uid, propscfg.material(), "EquipProduce", resp->mutable_commons());

	//向设备的生产队列插入数据
	equipment.InsertQueue(equipment.queuedata, productid);

	//判断定时任务队列中是否有该设备的任务，如果有，则不做处理，否则，就加入队列
	bool hasroutine = LogicQueueManager::Instance()->IsExistBuildRoutine(uid, equipud);

	if (!hasroutine)
	{
		//不存在正在生产的任务，则开始生产
		ProduceEquipNextMove(equipment);
	}

	DataProduceequipManager::Instance()->UpdateItem(equipment);

	equipment.SetMessage(resp->mutable_equipment());

	return 0;
}

int LogicProductLineManager::Process(unsigned uid, ProtoProduce::FetchProductReq* req, ProtoProduce::FetchProductResp* resp)
{
	unsigned equipud = req->equipud();
	unsigned pos = req->pos();

	FetchBackStorage(uid, equipud, pos, resp);

	return 0;
}

int LogicProductLineManager::FetchBackStorage(unsigned uid, unsigned equipud, unsigned pos, ProtoProduce::FetchProductResp * resp)
{
	//获取设备id
	unsigned equip_id = GetBuildId(uid, equipud);

	DataProduceequip & equipment = DataProduceequipManager::Instance()->GetData(uid, equipud);

	//参数判断
	if (pos > equipment.queuenum)
	{
		error_log("param error. uid=%u,pos=%u", uid, pos);
		throw runtime_error("param_error");
	}

	//获取收获的产品个数
	int productid = equipment.GetPosValue(equipment.shelfdata, pos);

	if (0 == productid)
	{
		error_log("position has no product. uid=%u,equipud=%u,pos=%u", uid, equipud, pos);
		throw runtime_error("position_no_product");
	}

	ProductlineCfgWrap productlinecfg;

	//产品在产品列表中的位置
	unsigned index = productlinecfg.GetEquipProductIndex(equip_id, productid);
	const ConfigProductLine::EquipLine & equipcfg = productlinecfg.GetEquiplineCfg(equip_id);

	int count = 0;
	unsigned id = 0, num = 0;
	CommonGiftConfig::CommonModifyItem reward;

	for(int i = 0; i < equipcfg.product_list(index).props_size(); ++i)
	{
		id = equipcfg.product_list(index).props(i).id();
		num = equipcfg.product_list(index).props(i).count();

		//获取经验奖励
		GetExpReward(id, num, reward);
		count += num;
	}

	//获取货仓的已用空间
	int restspace = LogicBuildManager::Instance()->GetStorageRestSpace(uid, type_warehouse);

	if (restspace < count)
	{
		resp->set_isfull(true);
		equipment.SetMessage(resp->mutable_equipment());

		return 0;
	}

	//货仓有足够的空间
	LogicUserManager::Instance()->CommonProcess(uid, equipcfg.product_list(index), "EquipFetch", resp->mutable_commons());
	//额外奖励，暂时只处理了经验
	LogicUserManager::Instance()->CommonProcess(uid, reward, "EquipFetch", resp->mutable_commons());

	//处理设备.首先，弹出展示架上取出的物品
	equipment.PopArray(equipment.shelfdata, pos);

	//处理设备的暂停生产状态
	if (status_suspend == equipment.status)
	{
		//重新处理一遍设备的生产结束方法
		equipment.FinishCurrentJob();

		//考虑设备的下一步操作
		ProduceEquipNextMove(equipment);
	}

	DataProduceequipManager::Instance()->UpdateItem(equipment);
	equipment.SetMessage(resp->mutable_equipment());

	resp->set_isfull(false);

	return 0;
}

void LogicProductLineManager::ProduceEquipNextMove(DataProduceequip & equipment)
{
	if (status_suspend == equipment.status)
	{
		//暂停生产状态，不做任何处理
		return ;
	}
	else if (status_free == equipment.status)
	{
		//空闲状态，则判断生产队列是否有数据，如果有，则开启常规队列
		int productid = equipment.GetQueueFront();

		if (0 == productid)
		{
			//生产队列空闲
			return ;
		}

		//生产队列不空闲
		//根据产品id，获取生产时间
		ItemCfgWrap itemcfgwrap;
		const ConfigItem::PropItem & propscfg = itemcfgwrap.GetPropsItem(productid);

		//获取设备的星级属性，判断是否有减速的星级
		unsigned build_id = GetBuildId(equipment.uid, equipment.id);
		const ConfigBuilding::ProduceEquipment & equipcfg = BuildCfgWrap().GetProduceCfgById(build_id);

		DataEquipmentStar & datastar = DataEquipmentStarManager::Instance()->GetData(equipment.uid, build_id);
		int percent = 0;

		//计算星级属性中，生产时间缩短的百分比
		for(int i = 0; i < datastar.star; ++i)
		{
			if (equipcfg.upgrade_star(i).ptype() == property_type_time)
			{
				percent = equipcfg.upgrade_star(i).value();  //扣除时间的百分比
			}
		}

		unsigned endts = Time::GetGlobalTime() + (100 - percent)/static_cast<double>(100) * propscfg.time_gain();

		vector<unsigned> equips;
		equips.push_back(equipment.id);

		//将设备生产任务加入到常规的任务中
		LogicQueueManager::Instance()->JoinRoutine<DataProduceEquipRoutine>(equipment.uid, endts, equips);

		//设置设备状态
		equipment.status = status_procing;
		equipment.finish_time = endts;
	}
}

unsigned LogicProductLineManager::GetBuildId(unsigned uid, unsigned equipud)
{
	if (!DataBuildingMgr::Instance()->IsExistItem(uid, equipud) || !DataProduceequipManager::Instance()->IsExistItem(uid, equipud))
	{
		error_log("equip not exist. uid=%u,buildud=%u", uid, equipud);
		throw runtime_error("build_not_exist");
	}

	//获取建筑消息
	DataBuildings & databuild = DataBuildingMgr::Instance()->GetData(uid, equipud);

	//判断建筑是否可用
	if (databuild.level == 0)
	{
		error_log("build is unavailable. uid=%u,equipud=%u", uid, equipud);
		throw runtime_error("build_unavailable");
	}

	return databuild.build_id;
}

int LogicProductLineManager::Process(unsigned uid, ProtoProduce::AdoptAnimalReq* req, ProtoProduce::AdoptAnimalResp* resp)
{
	unsigned buildud = req->buildud();

	Adopt(uid, buildud, resp);

	return 0;
}

int LogicProductLineManager::Adopt(unsigned uid, unsigned buildud, ProtoProduce::AdoptAnimalResp * resp)
{
	//判断该动物住所是否存在
	if (!DataBuildingMgr::Instance()->IsExistItem(uid, buildud))
	{
		error_log("build not exist. uid=%u,buildud=%u", uid, buildud);
		throw runtime_error("build_not_exist");
	}

	DataBuildings & build = DataBuildingMgr::Instance()->GetData(uid, buildud);

	//读取该动物住所的配置
	BuildCfgWrap buildcfgwrap;
	const ConfigBuilding::AnimalResidence & residencecfg = buildcfgwrap.GetAnimalResidenceCfgById(build.build_id);

	unsigned animalid = residencecfg.animal_id();  //住所豢养的动物id
	unsigned singlemax = residencecfg.capacity();  //动物住所容纳的动物数量

	//首先，判断该建筑能够容纳的动物是否到上限
	unsigned build_adopted = DataAnimalManager::Instance()->GetBuildAdoptedNum(uid, buildud);

	if (build_adopted >= singlemax)
	{
		error_log("build is full. uid=%u,buildud=%u", uid, buildud);
		throw runtime_error("build_full");
	}

	//获取动物配置
	const ConfigBuilding::Animal &  animalcfg = buildcfgwrap.GetAnimalCfgById(animalid);

	//获取当前动物的领养数目

	//获取本次领养动物需要的价格
	DBCUserBaseWrap userwrap(uid);
	int levelindex = buildcfgwrap.GetLevelIndex(animalcfg.need_level(), userwrap.Obj().level);

	unsigned num = DataAnimalManager::Instance()->GetAdoptedNum(uid, animalid);

	//判断当前数目是否超过要求
	if (num >= animalcfg.gain_num(levelindex))
	{
		error_log("animal num already max. uid=%u,animalid=%u", uid, animalid);
		throw runtime_error("animal_num_max");
	}

	//获取消耗
	//动物相关的索引，而当前这个索引，是用于判断当前领养的动物该用哪个索引的配置，比如消耗
	int num_index = buildcfgwrap.GetGainNumIndex(animalcfg.gain_num(), num + 1);

	//处理消耗，获取当前建造的数目的下一个，对应的正确消耗
	LogicUserManager::Instance()->CommonProcess(uid, animalcfg.need_cost(num_index), "AnimalAdopt", resp->mutable_commons());

	//添加动物
	DataAnimal & dataanimal = DataAnimalManager::Instance()->AddNewAnimal(uid, animalid, buildud);
	DataAnimalManager::Instance()->UpdateItem(dataanimal);

	dataanimal.SetMessage(resp->mutable_animal());

	return 0;
}

int LogicProductLineManager::Process(unsigned uid, ProtoProduce::FeedAnimalReq* req, ProtoProduce::FeedAnimalResp* resp)
{
	unsigned animalud = req->animalud();
	unsigned fodderud = req->fodderud();

	Feed(uid, animalud, fodderud, resp);

	return 0;
}

int LogicProductLineManager::Feed(unsigned uid, unsigned animalud, unsigned fodderud, ProtoProduce::FeedAnimalResp * resp)
{
	//判断该动物是否存在
	if (!DataAnimalManager::Instance()->IsExistItem(uid, animalud))
	{
		error_log("animal not exist. uid=%u,animalud=%u", uid, animalud);
		throw runtime_error("animal_not_exist");
	}

	DataAnimal & animal = DataAnimalManager::Instance()->GetData(uid, animalud);

	//判断动物是否处于饥饿状态
	if (animal.status != status_hungry)
	{
		error_log("animal not hungry. uid=%u,animalud=%u", uid, animalud);
		throw runtime_error("animal_not_hungry");
	}

	//获取饲料数据
	if (!DataItemManager::Instance()->IsExistItem(uid, fodderud))
	{
		error_log("fodder not exist. uid=%u,fodderud=%u", uid, fodderud);
		throw runtime_error("fodder_not_exist");
	}

	DataItem & props = DataItemManager::Instance()->GetData(uid, fodderud);

	//获取动物生产线的配置，判断传递的饲料是否是该动物需要的饲料
	const ConfigProductLine::AnimalLine & animalcfg = ProductlineCfgWrap().GetAnimallineCfg(animal.animal_id);

	if (props.props_id != animalcfg.fodder())
	{
		error_log("fodder not match. uid=%u,animalud=%u,fodderud=%u", uid, animalud, fodderud);
		throw runtime_error("fodder_not_match");
	}

	//扣除饲料消耗
	CommonGiftConfig::CommonModifyItem costcfg;
	CommonGiftConfig::PropsItem* propscfgmsg = costcfg.add_props();
	propscfgmsg->set_id(props.props_id);
	propscfgmsg->set_count(-1);

	//处理消耗
	LogicUserManager::Instance()->CommonProcess(uid, costcfg, "AnimalFeed", resp->mutable_commons());

	//获取动物产品的等待时间
	unsigned productid = animalcfg.product().props(0u).id();
	//根据作物配置，获取等待时长
	const ConfigItem::PropItem & propscfg = ItemCfgWrap().GetPropsItem(productid);

	//修改动物状态
	unsigned endts = Time::GetGlobalTime() + propscfg.time_gain();
	animal.status = status_growup;
	animal.full_time = endts;

	DataAnimalManager::Instance()->UpdateItem(animal);

	vector<unsigned> animals;
	animals.push_back(animalud);

	//加入到动物饱腹的队列
	LogicQueueManager::Instance()->JoinRoutine<DataAnimalRoutine>(uid, endts, animals);

	animal.SetMessage(resp->mutable_animal());

	return 0;
}

int LogicProductLineManager::Process(unsigned uid, ProtoProduce::ObtainProductReq* req, ProtoProduce::ObtainProductResp* resp)
{
	unsigned animalud = req->animalud();

	Obtain(uid, animalud, resp);

	return 0;
}

int LogicProductLineManager::Obtain(unsigned uid, unsigned animalud, ProtoProduce::ObtainProductResp * resp)
{
	//判断该动物是否存在
	if (!DataAnimalManager::Instance()->IsExistItem(uid, animalud))
	{
		error_log("animal not exist. uid=%u,animalud=%u", uid, animalud);
		throw runtime_error("animal_not_exist");
	}

	DataAnimal & animal = DataAnimalManager::Instance()->GetData(uid, animalud);

	//判断动物是否处于饱腹
	if (animal.status != status_full)
	{
		error_log("animal not full. uid=%u,animalud=%u", uid, animalud);
		throw runtime_error("animal_not_full");
	}

	//获取货仓的已用空间
	int restspace = LogicBuildManager::Instance()->GetStorageRestSpace(uid, type_warehouse);

	//获取动物生产线的配置，判断传递的饲料是否是该动物需要的饲料
	const ConfigProductLine::AnimalLine & animalcfg = ProductlineCfgWrap().GetAnimallineCfg(animal.animal_id);
	int count = 0;
	unsigned id = 0, num = 0;
	CommonGiftConfig::CommonModifyItem reward;

	for(int i = 0; i < animalcfg.product().props_size(); ++i)
	{
		id = animalcfg.product().props(i).id();
		num = animalcfg.product().props(i).count();

		//获取经验奖励
		GetExpReward(id, num, reward);
		count += num;
	}

	if (restspace < count)
	{
		//满仓
		resp->set_isfull(true);
		return 0;
	}

	//收获动物的产品
	LogicUserManager::Instance()->CommonProcess(uid, animalcfg.product(), "AnimalObtain", resp->mutable_commons());

	//额外奖励，暂时只处理了经验
	LogicUserManager::Instance()->CommonProcess(uid, reward, "AnimalObtain", resp->mutable_commons());

	//产品已收获
	animal.Obtain();

	DataAnimalManager::Instance()->UpdateItem(animal);

	animal.SetMessage(resp->mutable_animal());

	//更新收获次数
	ActAfterHarvest(uid, count, resp->mutable_commons());

	return 0;
}

int LogicProductLineManager::GetExpReward(unsigned productid, unsigned count, CommonGiftConfig::CommonModifyItem &reward)
{
	ItemCfgWrap itemcfgwrap;
	//获取物品的奖励配置
	const ConfigItem::PropItem &  propscfg = itemcfgwrap.GetPropsItem(productid);

	unsigned oldexp = reward.based().exp();

	//只处理经验
	if (propscfg.extra_reward().has_based() &&
			propscfg.extra_reward().based().has_exp())
	{
		int exp = propscfg.extra_reward().based().exp();

		exp *= count;
		//经验
		reward.mutable_based()->set_exp(exp + oldexp);
	}

	return 0;
}

int LogicProductLineManager::ActAfterHarvest(unsigned uid, unsigned count, DataCommon::CommonItemsCPP * msg)
{
	//根据uid，获取当前已经收获的作物个数
	DBCUserBaseWrap userwrap(uid);
	userwrap.Obj().count += count;

	//根据用户等级，开启随机产生道具的机会需要的次数
	const ConfigProductLine::MaterailReward & materialcfg = ProductlineCfgWrap().GetMaterialCfg();

	int cond_count = materialcfg.init_count() + userwrap.Obj().level;  //条件数量

	//判断当前次数是否满足条件
	if (userwrap.Obj().count < cond_count)
	{
		//条件不满足，返回
		return 0;
	}

	//次数满足要求，随机一个数，判断是否能产生材料
	int val = Math::GetRandomInt(100); //在0与100之间随机一个值

	if (val >= materialcfg.rate())
	{
		//不满足概率的条件，次数重置
		userwrap.Obj().count = 0;
		userwrap.Save();

		return 0;
	}

	//从材料的数组中随机抽取一个产品
	int index = Math::GetRandomInt(materialcfg.random_reward_size());

	//发放奖励
	LogicUserManager::Instance()->CommonProcess(uid, materialcfg.random_reward(index), "HarvestRandom", msg);

	//收获次数重置
	userwrap.Obj().count = 0;
	userwrap.Save();

	return 0;
}
