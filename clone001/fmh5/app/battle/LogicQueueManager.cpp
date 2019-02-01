#include "LogicQueueManager.h"
#include "ServerInc.h"

int DataRoutineBase::SpeedUp(unsigned buildud, User::SpeedUpResp * resp)
{
	//检查队列是否存在
	CheckUd(buildud);

	int cash = 0;
	int diffts = 0;

	//获取消耗的钻石以及钻石减去的时间
	GetPriceAndATime(buildud, cash, diffts);

	//钻石消耗
	CommonGiftConfig::CommonModifyItem cfg;
	cfg.mutable_based()->set_cash(-cash);

	LogicUserManager::Instance()->CommonProcess(uid_, cfg, "SpeedUp", resp->mutable_commons());

	vector<unsigned> & uds = GetLands();

	//立即完成
	if (1 == uds.size())
	{
		//最后一个加速，则调用routine，用于结束routine
		LogicRoutineManager::Instance()->SpeedUpTask(rid, diffts);
	}
	else
	{
		vector<unsigned>::iterator it = find(uds.begin(), uds.end(), buildud);

		//直接中当前任务中删除该ud
		ProtoPush::PushBuildingsCPP * msg = new ProtoPush::PushBuildingsCPP;
		SingleRoutineEnd(buildud, msg);

		//推送
		LogicManager::Instance()->sendMsg(uid_, msg);

		uds.erase(it);
	}

	return 0;
}

int DataQueueRoutine::JoinRoutine()
{
	LogicRoutineManager::Instance()->AddTask(this);

	return 0;
}

int DataQueueRoutine::DoEnd()
{
	//判断是否在线，如果离线，则不做处理
	if (!UserManager::Instance()->IsOnline(uid_))
	{
		return R_ERROR;
	}

	ProtoPush::PushBuildingsCPP * msg = new ProtoPush::PushBuildingsCPP;

	for(size_t i = 0; i < uds_.size(); ++i)
	{
		unsigned build_ud = uds_[i];
		SingleRoutineEnd(build_ud, msg);
	}

	//推送
	LogicManager::Instance()->sendMsg(uid_, msg);

	return 0;
}

void LogicQueueManager::Offline(unsigned uid)
{
	if (buildRid_.count(uid))
	{
		buildRid_.erase(uid);
	}

	if (routine_.count(uid))
	{
		routine_.erase(uid);
	}
}

int LogicQueueManager::Process(unsigned uid, User::SpeedUpReq* req, User::SpeedUpResp* resp)
{
	unsigned ud = req->ud();

	SpeedUp(uid, ud, resp);

	return 0;
}

int LogicQueueManager::SpeedUp(unsigned uid, unsigned ud, User::SpeedUpResp * resp)
{
	//加速
	//根据ud，获取队列信息
	if (!buildRid_.count(uid) || !buildRid_[uid].count(ud))
	{
		error_log("build ud can't find routine. uid=%u,ud=%u", uid, ud);
		throw runtime_error("build_no_routine");
	}

	long rid = buildRid_[uid][ud];

	DataRoutineBase * proutine = routine_[uid][rid];

	proutine->SpeedUp(ud, resp);

	return 0;
}

bool LogicQueueManager::IsExistBuildRoutine(unsigned uid, unsigned ud) const
{
	if (buildRid_.count(uid) && buildRid_.at(uid).count(ud))
	{
		return true;
	}

	return false;
}

int LogicQueueManager::FinishQueue(unsigned uid, unsigned ud)
{
	if (!buildRid_.count(uid) || !buildRid_[uid].count(ud))
	{
		return 0;
	}

	//判断是否是最后一个
	if (buildRid_[uid].size() == 1)
	{
		long rid = buildRid_[uid][ud];

		//清除映射
		routine_[uid].erase(rid);
		buildRid_[uid].clear();
	}
	else
	{
		//只删除uid与ud的映射
		buildRid_[uid].erase(ud);
	}

	return 0;
}
