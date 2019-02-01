#ifndef LOGIC_BUILDING_MANAGER_H_
#define LOGIC_BUILDING_MANAGER_H_

#include "Common.h"
#include "Kernel.h"
#include <bitset>
#include "DataInc.h"
#include "ConfigInc.h"
#include "LogicQueueManager.h"

//建筑建造
class DataBuildRoutine : public DataQueueRoutine
{
public:
	DataBuildRoutine(unsigned uid, unsigned endts, vector<unsigned> & uds):
		DataQueueRoutine(uid, endts, uds)
	{

	}

	virtual void CheckUd(unsigned buildud);

	//获取需要扣的钻石数以及剩余时间
	virtual void GetPriceAndATime(unsigned buildud, int & cash, int & diffts);

	virtual void SingleRoutineEnd(unsigned buildud, ProtoPush::PushBuildingsCPP * msg);
};

class LogicBuildManager : public BattleSingleton, public CSingleton<LogicBuildManager>
{
private:
	friend class CSingleton<LogicBuildManager>;
	LogicBuildManager();

public:
	enum
	{
		grid_ypos_len = 1000,  //y坐标表示的范围
	};

	virtual void CallDestroy() { Destroy();}

	virtual int OnInit();

	int NewUser(unsigned uid);

	int CheckLogin(unsigned uid);

	//建造
	int Process(unsigned uid, ProtoBuilding::BuildReq* req, ProtoBuilding::BuildResp* resp);
	//移动
	int Process(unsigned uid, ProtoBuilding::MoveReq* req, ProtoBuilding::MoveResp* resp);
	//翻转
	int Process(unsigned uid, ProtoBuilding::FlipReq* req, ProtoBuilding::FlipResp* resp);
	//升级(粮仓、货仓)
	int Process(unsigned uid, ProtoBuilding::BuildingUpReq *req, ProtoBuilding::BuildingUpResp* resp);

	//获取仓库的剩余可用空间.type: 1-粮仓 2-货仓
	int GetStorageRestSpace(unsigned uid, unsigned type);

private:
	//建造
	int Build(unsigned uid, unsigned build_id, unsigned xpos, unsigned ypos, ProtoBuilding::BuildResp *resp);

	//加入到定时任务
	int JoinRoutine(unsigned uid, unsigned endts, vector<unsigned> & lands);

	//建造前的处理
	int PreBuild(unsigned uid, unsigned build_id, unsigned & wait_time, ProtoBuilding::BuildResp *resp);

	//移动
	int Move(unsigned uid, unsigned build_ud, unsigned xpos, unsigned ypos, ProtoBuilding::MoveResp *resp);
	//翻转
	int Flip(unsigned uid, unsigned build_ud, ProtoBuilding::FlipResp *resp);

	//根据建筑id,原点id，计算所占的所有格子
	int SeteBuildGrid(unsigned uid, unsigned xpos, unsigned ypos, uint8_t direct, unsigned build_id, int * status, bool isbarrier = false);

	bool IsIntersect(unsigned uid, unsigned ud, int * build_status);

	template<class P, class T>
	int CostBeforeBuild(unsigned uid, T& cfg, string reason,int num, int level, unsigned & wait_time, ProtoBuilding::BuildResp *resp);

	template<class T>
	bool IsInteractBetween(unsigned uid, int * build_status, T& buildmap);
private:
	//map<unsigned, >  ;  //障碍物格子数
};

template<class P, class T>
int LogicBuildManager::CostBeforeBuild(unsigned uid, T& cfg, string reason, int num, int level, unsigned & wait_time,
		ProtoBuilding::BuildResp *resp)
{
	//获取等级索引只用于判断当前的用户等级能够建造的最大数目。
	BuildCfgWrap buildcfgwrap;
	int levelindex = buildcfgwrap.GetLevelIndex(cfg.need_level(), level);

	//判断当前数目是否超过要求
	if (num >= cfg.gain_num(levelindex))
	{
		error_log("build num already max. uid=%u,build_id=%u", uid, cfg.id());
		throw runtime_error("build_num_max");
	}

	//建造相关的索引，而当前这个索引，是用于判断当前建造的建筑该用哪个索引的配置，比如消耗和等待时间
	int build_index = buildcfgwrap.GetGainNumIndex(cfg.gain_num(), num + 1);

	//处理消耗，获取当前建造的数目的下一个，对应的正确消耗
	P::Instance()->CommonProcess(uid, cfg.need_cost(build_index), reason, resp->mutable_commons());

	if (cfg.wait_time_size() > 0)
	{
		wait_time = cfg.wait_time(build_index);
	}

	return 0;
}

template<class T>
bool LogicBuildManager::IsInteractBetween(unsigned uid, int * build_status, T& buildmap)
{
	char conflict[10] = {0};
	string errmsg;

	for(unsigned i = 0; i < GRID_LENGTH; ++i)
	{
		if ((buildmap[i] & build_status[i]) > 0)
		{
			//记录哪些格相交
#ifdef _DEBUG_BUILD
			int result = buildmap[i] & build_status[i];
			bitset<32> bitres(result);

			for(size_t j = 0; j < bitres.size(); ++j)
			{
				if (bitres[j])
				{
					//记录哪些格子冲突
					int tempgrid = i*INT_BITS + j;

					sprintf(conflict, "%d|%d,", tempgrid/MAP_LENGTH, tempgrid%MAP_LENGTH);

					errmsg += conflict;
				}
			}

			debug_log("grid conflict. uid=%u,grid=%s.", uid, errmsg.c_str());
#endif
			return true;
		}
	}

	return false;
}

#endif //LOGIC_BUILDING_MANAGER_H_
