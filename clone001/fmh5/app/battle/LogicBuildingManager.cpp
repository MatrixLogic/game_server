#include "LogicBuildingManager.h"
#include "ServerInc.h"

void DataBuildRoutine::CheckUd(unsigned buildud)
{
	//先调用基类的方法
	DataQueueRoutine::CheckUd(buildud);

	if (DataBuildingMgr::Instance()->IsExistItem(uid_, buildud))
	{
		error_log("build is not exist. uid=%u,buildud=%u", uid_, buildud);
		throw runtime_error("build_not_exist");
	}

	//获取建筑数据
	DataBuildings & build = DataBuildingMgr::Instance()->GetData(uid_, buildud);

	if(0 == build.done_time)
	{
		error_log("build not upgrading. uid=%u,buildud=%u", uid_, buildud);
		throw runtime_error("build_not_upgrading");
	}
}

void DataBuildRoutine::GetPriceAndATime(unsigned buildud, int & cash, int & diffts)
{
	unsigned nowts = Time::GetGlobalTime();
	diffts = endts_ > nowts ? endts_ - nowts : 0;
	//获取建筑数据
	DataBuildings & build = DataBuildingMgr::Instance()->GetData(uid_, buildud);
	unsigned build_id = build.build_id;

	int per = 1;  //1钻减去的秒数
	BuildCfgWrap buildcfgwrap;
	int type = buildcfgwrap.GetBuildType(build_id);

	if (build_type_animal_residence == type)
	{
		//动物住所
		const ConfigBuilding::AnimalResidence & residencecfg = buildcfgwrap.GetAnimalResidenceCfgById(build_id);
		per = residencecfg.speed_price();
	}
	else if (build_type_produce_equipment == type)
	{
		//生产设备
		const ConfigBuilding::ProduceEquipment & producecfg = buildcfgwrap.GetProduceCfgById(build_id);
		per = producecfg.speed_price();
	}
	else
	{
		error_log("wrong type. uid=%u,buildud=%u", uid_, buildud);
		throw runtime_error("wrong_build_type");
	}

	if (0 == per)
	{
		error_log("speed price config error. uid=%u,buildid=%u", uid_, buildud);
		throw runtime_error("config_error");
	}

	cash = ceil(static_cast<double>(diffts)/per);
}

void DataBuildRoutine::SingleRoutineEnd(unsigned buildud, ProtoPush::PushBuildingsCPP * msg)
{
	//获取建筑数据
	DataBuildings & build = DataBuildingMgr::Instance()->GetData(uid_, buildud);

	//升级
	build.Upgrade();
	DataBuildingMgr::Instance()->UpdateItem(build);

	build.SetMessage(msg->add_buildings());

	if (1 == build.level)
	{
		//处理建筑解锁后的相关生产线
		unsigned type = BuildCfgWrap().GetBuildType(build.build_id);

		LogicProductLineManager::Instance()->ProduceAfterBuild(build.uid, build.id, type);
	}

	//结束队列
	LogicQueueManager::Instance()->FinishQueue(uid_, buildud);
}

LogicBuildManager::LogicBuildManager()
{

}

int LogicBuildManager::OnInit()
{
	bool issuccess = ParseManager::getInstance()->Init();

	if (!issuccess)
	{
		return R_ERROR;
	}

	return 0;
}

int LogicBuildManager::NewUser(unsigned uid)
{
	DataBuildingMgr::Instance()->Init(uid);

	//读取新手文档中的配置，进行新建筑的建造
	const UserCfg::User& userCfg = UserCfgWrap().User();
	unsigned build_id = 0;
	unsigned xpos = 0, ypos = 0;
	ProtoBuilding::BuildResp resp;

	for(int i = 0; i < userCfg.builds_size(); ++i)
	{
		build_id = userCfg.builds(i).buildid();
		xpos = userCfg.builds(i).pos(0u);
		ypos = userCfg.builds(i).pos(1u);

		try
		{
			Build(uid, build_id, xpos, ypos, &resp);
		}
		catch(runtime_error &e)
		{
			;
		}
	}

	return 0;
}

int LogicBuildManager::CheckLogin(unsigned uid)
{
	DataBuildingMgr::Instance()->Init(uid);

	vector<unsigned> builds;
	//将正在建筑的建筑加入定时任务队列
	DataBuildingMgr::Instance()->GetIndexs(uid, builds);
	map<unsigned, vector<unsigned> > tobuild;  //endts -> uds

	for(int i = 0; i < builds.size(); ++i)
	{
		DataBuildings & build = DataBuildingMgr::Instance()->GetDataByIndex(builds[i]);

		if (build.done_time > 0)
		{
			tobuild[build.done_time].push_back(build.id);
		}
	}

	//遍历map
	for(map<unsigned, vector<unsigned> >::iterator viter = tobuild.begin(); viter != tobuild.end(); ++viter)
	{
		LogicQueueManager::Instance()->JoinRoutine<DataBuildRoutine>(uid, viter->first, viter->second);
	}

	return 0;
}

int LogicBuildManager::Process(unsigned uid, ProtoBuilding::BuildReq* req, ProtoBuilding::BuildResp* resp)
{
	unsigned build_id = req->buildid();
	unsigned xpos = req->pos(0u);
	unsigned ypos = req->pos(1u);

	Build(uid, build_id, xpos, ypos, resp);

	return 0;
}

int LogicBuildManager::Build(unsigned uid, unsigned build_id, unsigned xpos, unsigned ypos, ProtoBuilding::BuildResp *resp)
{
	//判断总的建筑数目是否超出上限
	int allnum = DataBuildingMgr::Instance()->GetAllBuildNum(uid);

	if (allnum >= DB_BUILD_FULL)
	{
		error_log("build num is max. uid=%u", uid);
		throw runtime_error("build_num_max");
	}

	//判断位置是否被占用
	int buildstatus[GRID_LENGTH] = {0};  //全地图各格子的位置
	SeteBuildGrid(uid, xpos, ypos, direct_right, build_id, buildstatus);

	//是否相交
	bool isintersect = IsIntersect(uid, 0, buildstatus);

	if (isintersect)
	{
		error_log("wrong position. uid=%u,build_id=%u,xpos=%u,ypos=%u", uid, build_id, xpos, ypos);
		throw runtime_error("wrong_position");
	}

	//不相交，则继续处理建造的相关事项
	unsigned wait_time = 0;

	//建筑前的检查以及扣除资源的消耗
	PreBuild(uid, build_id, wait_time, resp);

	DataBuildings & databuild = DataBuildingMgr::Instance()->AddNewBuilding(uid, build_id);

	databuild.position = xpos*grid_ypos_len + ypos;
	databuild.direct = direct_right;

	int type = BuildCfgWrap().GetBuildType(build_id);

	//判断是否是生产设备
	if (build_type_produce_equipment == type)
	{
		//生产设备，判断是否存在该设备的星级，如果不存在，则新增
		if (!DataEquipmentStarManager::Instance()->IsExistItem(uid, build_id))
		{
			//不存在，新增
			DataEquipmentStar & datastar = DataEquipmentStarManager::Instance()->GetData(uid, build_id);

			datastar.SetMessage(resp->mutable_equipmentstar());
		}
	}

	if (0 == wait_time)
	{
		//立即完成类型
		databuild.level = 1;

		DataBuildingMgr::Instance()->UpdateItem(databuild);
		databuild.SetMessage(resp->mutable_building());

		//处理建筑升级后的相关生产线
		unsigned type = BuildCfgWrap().GetBuildType(databuild.build_id);

		LogicProductLineManager::Instance()->ProduceAfterBuild(uid, databuild.id, type, false, resp);

		return 0;
	}

	unsigned endts = Time::GetGlobalTime() + wait_time;
	databuild.done_time = endts;

	vector<unsigned> builds;
	builds.push_back(databuild.id);

	//非立即完成系列，则加入到定时任务列表
	LogicQueueManager::Instance()->JoinRoutine<DataBuildRoutine>(uid, endts, builds);

	DataBuildingMgr::Instance()->UpdateItem(databuild);
	databuild.SetMessage(resp->mutable_building());

	return 0;
}

int LogicBuildManager::PreBuild(unsigned uid, unsigned build_id, unsigned & wait_time, ProtoBuilding::BuildResp *resp)
{
	//先判断是否能被建造
	//包括数量，消耗的限制
	BuildCfgWrap buildcfgwrap;

	int num = DataBuildingMgr::Instance()->GetBuildNum(uid, build_id);
	int type = buildcfgwrap.GetBuildType(build_id);

	char build[20] = {0};
	sprintf(build, "%u", build_id);
	string reason("Build_");
	reason += string(build);
	DBCUserBaseWrap userwrap(uid);
	wait_time = 0;

	switch(type)
	{
		case build_type_corpland:
			//消耗
			{
				const ConfigBuilding::CropLand & cropcfg = buildcfgwrap.GetCropLandCfg();
				CostBeforeBuild<LogicUserManager>(uid, cropcfg, reason, num, userwrap.Obj().level, wait_time, resp);
			}
			break;
		case build_type_animal_residence :
		{
			//动物住所
			const ConfigBuilding::AnimalResidence & residencecfg = buildcfgwrap.GetAnimalResidenceCfgById(build_id);
			//消耗
			CostBeforeBuild<LogicUserManager>(uid, residencecfg, reason, num, userwrap.Obj().level, wait_time, resp);
		}
			break;
		case build_type_produce_equipment :
		{
			//生产设备
			const ConfigBuilding::ProduceEquipment & producecfg = buildcfgwrap.GetProduceCfgById(build_id);
			//消耗
			CostBeforeBuild<LogicUserManager>(uid, producecfg, reason, num, userwrap.Obj().level, wait_time, resp);
		}
			break;
		case build_type_fruit_tree :
		{
			//果树
			const ConfigBuilding::FruitTree & fruitcfg = buildcfgwrap.GetFruitTreeCfgById(build_id);
			//消耗
			CostBeforeBuild<LogicUserManager>(uid, fruitcfg, reason, num, userwrap.Obj().level, wait_time, resp);
		}
			break;
		case build_type_decorate:
		{
			//装饰物消耗
			const ConfigBuilding::Decoration & decoratecfg = buildcfgwrap.GetDecorationCfgById(build_id);

			//判断是否解锁
			if (userwrap.Obj().level < decoratecfg.need_level(0u))
			{
				error_log("level not enough. level=%u,need=%u", userwrap.Obj().level, decoratecfg.need_level(0u));
				throw runtime_error("level_not_enough");
			}

			//不检查数量，直接进行消耗
			LogicUserManager::Instance()->CommonProcess(uid, decoratecfg.need_cost(0u), reason, resp->mutable_commons());
		}
			break;
		case build_type_house:
		{
			if (num > 0)
			{
				error_log("build already exist. uid=%u,build_id=%u", uid, build_id);
				throw runtime_error("build_already_exist");
			}

			resp->mutable_commons();
		}
			break;
		case build_type_storage:
			//仓库，暂时没有消耗.不允许多个
			if (num > 0)
			{
				error_log("build already exist. uid=%u,build_id=%u", uid, build_id);
				throw runtime_error("build_already_exist");
			}

			resp->mutable_commons();

			break;
		default:
			error_log("wrong build type. uid=%u,build_id=%u", uid, build_id);
			throw runtime_error("wrong_build_type");
	}

	return 0;
}

int LogicBuildManager::Process(unsigned uid, ProtoBuilding::MoveReq* req, ProtoBuilding::MoveResp* resp)
{
	unsigned build_ud = req->ud();
	unsigned xpos = req->pos(0u);
	unsigned ypos = req->pos(1u);

	Move(uid, build_ud, xpos, ypos, resp);

	return 0;
}

int LogicBuildManager::Move(unsigned uid, unsigned build_ud, unsigned xpos, unsigned ypos, ProtoBuilding::MoveResp *resp)
{
	//根据ud，判断是否在建筑表中
	bool isexist = DataBuildingMgr::Instance()->IsExistItem(uid, build_ud);

	if (!isexist)
	{
		error_log("build not exist. uid=%u, ud=%u", uid, build_ud);
		throw runtime_error("build_not_exist");
	}

	//获取建筑信息
	DataBuildings & databuild = DataBuildingMgr::Instance()->GetData(uid, build_ud);

	int buildstatus[GRID_LENGTH] = {0};  //当前建筑的地图状态
	SeteBuildGrid(uid, xpos, ypos, databuild.direct, databuild.build_id, buildstatus);

	//是否相交
	bool isintersect = IsIntersect(uid, build_ud, buildstatus);

	if (isintersect)
	{
		error_log("build will have intersection. uid=%u,build_ud=%u", uid, build_ud);
		throw runtime_error("build_intersect");
	}

	//不冲突，则修改坐标位置
	databuild.position = xpos*grid_ypos_len + ypos;

	DataBuildingMgr::Instance()->UpdateItem(databuild);

	resp->set_result(true);

	return 0;
}

int LogicBuildManager::Process(unsigned uid, ProtoBuilding::FlipReq* req, ProtoBuilding::FlipResp* resp)
{
	unsigned build_ud = req->ud();

	Flip(uid, build_ud, resp);

	return 0;
}

int LogicBuildManager::Flip(unsigned uid, unsigned build_ud, ProtoBuilding::FlipResp *resp)
{
	//根据ud，判断是否在建筑表中
	bool isexist = DataBuildingMgr::Instance()->IsExistItem(uid, build_ud);

	if (!isexist)
	{
		error_log("build not exist. uid=%u, ud=%u", uid, build_ud);
		throw runtime_error("build_not_exist");
	}

	//获取建筑信息
	DataBuildings & databuild = DataBuildingMgr::Instance()->GetData(uid, build_ud);
	vector<unsigned> grids;

	uint8_t changed_direct = databuild.direct ^ 0x03;

	int buildstatus[GRID_LENGTH] = {0};  //全地图各格子的位置
	unsigned xpos = databuild.position/grid_ypos_len;
	unsigned ypos = databuild.position - xpos*grid_ypos_len;

	SeteBuildGrid(uid, xpos, ypos, changed_direct, databuild.build_id, buildstatus);

	//是否相交
	bool isintersect = IsIntersect(uid, build_ud, buildstatus);

	if (isintersect)
	{
		error_log("build will have intersection. uid=%u,build_ud=%u", uid, build_ud);
		throw runtime_error("build_intersect");
	}

	//不冲突，则修改方向即可
	databuild.direct = changed_direct;

	DataBuildingMgr::Instance()->UpdateItem(databuild);
	resp->set_result(true);

	return 0;
}

int LogicBuildManager::Process(unsigned uid, ProtoBuilding::BuildingUpReq *req, ProtoBuilding::BuildingUpResp* resp)
{
	unsigned build_ud = req->ud();
	//根据ud，判断是否在建筑表中
	bool isexist = DataBuildingMgr::Instance()->IsExistItem(uid, build_ud);
	if (!isexist)
	{
		error_log("build not exist. uid=%u, ud=%u", uid, build_ud);
		throw runtime_error("build_not_exist");
	}

	//获取建筑信息
	DataBuildings & databuild = DataBuildingMgr::Instance()->GetData(uid, build_ud);
	unsigned build_id = databuild.build_id;

	//判定其是否为仓库
	bool isstorage = BuildCfgWrap().IsStorage(build_id);
	if (!isstorage)
	{
		error_log("build is not  storage. uid=%u, build_id=%u", uid, build_id);
		throw runtime_error("build is not  storage");
	}

	//根据建筑id获取相应的配置信息
	unsigned cur_level = databuild.level;
	const ConfigBuilding::StorageHouse &storage_house_cfg = BuildCfgWrap().GetStorageCfgById(build_id);
	unsigned max_level = storage_house_cfg.level_storage_size();
	if(cur_level >= max_level)
	{
		error_log("build level is biggest. cur_level=%u", cur_level);
		throw runtime_error("build level is biggest");
	}
	const CommonGiftConfig::CommonModifyItem &item_cfg = storage_house_cfg.need_cost(cur_level);

	//处理升级消耗
	string reason = "storage_house_up";
	LogicUserManager::Instance()->CommonProcess(uid, item_cfg, reason, resp->mutable_commons());

	//增加建筑等级
	databuild.level = cur_level + 1;
	DataBuildingMgr::Instance()->UpdateItem(databuild);
	return 0;
}

bool LogicBuildManager::IsIntersect(unsigned uid, unsigned ud, int * build_status)
{
	//统计耗时
	timeval tv;
	gettimeofday(&tv,NULL);
	debug_log("start:millisecond:%ld\n",tv.tv_sec*1000 + tv.tv_usec/1000);  //毫秒
	//先判断与不可移除障碍物的相交情况
	vector<int> & cells = ParseManager::getInstance()->getParser()->getCells();
	bool isintersect = IsInteractBetween(uid, build_status, cells);

	if (isintersect)
	{
		gettimeofday(&tv,NULL);
		debug_log("end:millisecond:%ld\n",tv.tv_sec*1000 + tv.tv_usec/1000);  //毫秒

		return isintersect;
	}

	//如果ud=0，说明是新的建筑，不考虑自己与自己重合的情况
	//先排除障碍物
	DBCUserBaseWrap userwrap(uid);

	int mapstatus[GRID_LENGTH] = {0};  //全地图各格子的位置

	//遍历当前拥有的所有建筑的格子数，排除自身
	vector<unsigned> builds;
	DataBuildingMgr::Instance()->GetIndexs(uid, builds);

	for(size_t i = 0; i < builds.size(); ++i)
	{
		DataBuildings & databuild = DataBuildingMgr::Instance()->GetDataByIndex(builds[i]);

		if (databuild.id == ud)
		{
			//排除自身
			continue;
		}

		unsigned xpos = databuild.position/grid_ypos_len;
		unsigned ypos = databuild.position - xpos*grid_ypos_len;

		SeteBuildGrid(uid, xpos, ypos, databuild.direct, databuild.build_id, mapstatus);
	}

	//遍历障碍物
	//获取障碍物的配置
	const ConfigBuilding::Barriers & barriercfg = BarrierCfgWrap().GetBarrierCfg();
	unsigned id = 0;
	int pos = 0, left = 0;

	for(int i = 0; i < barriercfg.barriers_size(); ++i)
	{
		id = barriercfg.barriers(i).id();

		//判断是否解锁
		pos = (id - 1)/CHAR_BITS;
		left = (id - 1) % CHAR_BITS;

		if (1 == ((userwrap.Obj().barrier[pos] >> left) & 1))
		{
			//已解锁
			continue;
		}

		//未解锁，设置占位
		SeteBuildGrid(uid, barriercfg.barriers(i).pos(0u), barriercfg.barriers(i).pos(1u), direct_right, id, mapstatus, true);
	}

	isintersect = IsInteractBetween(uid, build_status, mapstatus);

	gettimeofday(&tv,NULL);
	debug_log("end:millisecond:%ld\n",tv.tv_sec*1000 + tv.tv_usec/1000);  //毫秒

	return isintersect;
}

int LogicBuildManager::SeteBuildGrid(unsigned uid, unsigned xpos, unsigned ypos, uint8_t direct, unsigned build_id,  int * status
		, bool isbarrier)
{
	vector<unsigned> foots;

	if (!isbarrier)
	{
		BuildCfgWrap().GetFootPrint(build_id, foots);
	}
	else
	{
		BarrierCfgWrap().GetFootPrint(build_id, foots);
	}

	unsigned width = 0;
	unsigned height = 0;

	if (direct_right == direct)
	{
		//往右
		width = foots[0u];
		height = foots[1u];
	}
	else if (direct_down == direct)
	{
		//往下
		width = foots[1u];
		height = foots[0u];
	}
	else
	{
		error_log("wrong direction. uid=%u,build_id=%u,direction=%u", uid, build_id, direct);
		throw runtime_error("wrong_direction");
	}

	//判断是否越界
	if (ypos > MAP_LENGTH || ypos + width > MAP_LENGTH)
	{
		error_log("beyond edge. uid=%u,build_id=%u,ypos=%u,width=%u", uid, build_id, ypos, width);
		throw runtime_error("beyond_edge");
	}

	if (xpos > MAP_WIDTH || xpos + height > MAP_WIDTH)
	{
		error_log("beyond edge. uid=%u,build_id=%u,xpos=%u,height=%u", uid, build_id, xpos, height);
		throw runtime_error("beyond_edge");
	}

	//将经过的格子计入到数组中
	unsigned gridid = 0;
	int pos = 0, left = 0;

	for(unsigned i = 0; i < height; ++i)
	{
		for(unsigned j = 0; j < width; ++j)
		{
			gridid = (xpos + i)*MAP_LENGTH + (ypos + j);

			pos = (gridid)/INT_BITS;
			left = (gridid) % INT_BITS;

			status[pos] |= 1 <<left;
		}
	}

	return 0;
}

//获取粮仓的剩余可用空间
int LogicBuildManager::GetStorageRestSpace(unsigned uid, unsigned type)
{
	//先获取仓库配置
	const ConfigBuilding::StorageHouse & storagecfg = BuildCfgWrap().GetStorageCfg(type);  //粮仓

	unsigned build_ud = DataBuildingMgr::Instance()->GetBuildUd(uid, storagecfg.id());

	int level = 0;

	//判断仓库是否存在，如果不存在，则默认1级
	if (DataBuildingMgr::Instance()->IsExistItem(uid, build_ud))
	{
		level = DataBuildingMgr::Instance()->GetData(uid, build_ud).level;
	}

	if (level < 1)
	{
		//仓库还未解锁
		level = 1;
	}

	int used = LogicPropsManager::Instance()->GetStorageUsedSpace(uid, type);
	int capacity = storagecfg.level_storage(level - 1);

	return capacity > used ?  capacity - used : 0;
}

