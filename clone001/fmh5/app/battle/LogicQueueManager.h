#ifndef LOGIC_QUEUE_MANAGER_H_
#define LOGIC_QUEUE_MANAGER_H_

#include "Common.h"
#include "Kernel.h"
#include <bitset>
#include "DataInc.h"
#include "ConfigInc.h"

//队列基类
class DataRoutineBase
{
public:
	DataRoutineBase(unsigned int uid, unsigned endts) :
		uid_(uid),
		endts_(endts),
		rid(0)
	{

	}

	virtual int DoEnd() = 0;

	virtual ~DataRoutineBase() {}

	virtual int SpeedUp(unsigned buildud, User::SpeedUpResp * resp);

	virtual void CheckUd(unsigned buildud){};

	virtual void GetPriceAndATime(unsigned buildud, int & cash, int & diffts)
	{
		cash = 0;
		diffts = 0;
	}

	virtual vector<unsigned> & GetLands() = 0;
	virtual void SingleRoutineEnd(unsigned buildud, ProtoPush::PushBuildingsCPP * msg) = 0;

	unsigned int uid_;
	unsigned int endts_;  //操作的结束时间
	long rid;
};

//定时任务队列
class DataQueueRoutine: public DataRoutineBase
{
public:
	DataQueueRoutine(unsigned uid, unsigned endts, vector<unsigned> & uds):
		DataRoutineBase(uid, endts),
		uds_(uds)
	{
		JoinRoutine();
	}

	int JoinRoutine();

	virtual int DoEnd();

	//加速。立即完成系列
	virtual void CheckUd(unsigned buildud)
	{
		//建筑加速
		vector<unsigned>::iterator it = find(uds_.begin(), uds_.end(), buildud);

		if (it == uds_.end())
		{
			error_log("build not exist. uid=%u,ud=%u", uid_, buildud);
			throw runtime_error("build_not_exist");
		}
	}

	virtual vector<unsigned> & GetLands()
	{
		return uds_;
	}

	virtual void SingleRoutineEnd(unsigned buildud, ProtoPush::PushBuildingsCPP * msg){}

protected:
	vector<unsigned> uds_;  //同一批次的地块
};

class LogicQueueManager :public BattleSingleton, public CSingleton<LogicQueueManager>
{
private:
	friend class CSingleton<LogicQueueManager>;
	LogicQueueManager(){}

public:
	virtual void CallDestroy() { Destroy();}

	template<class T>
	int JoinRoutine(unsigned uid, unsigned endts, vector<unsigned> & lands);

	//用户离线处理
	void Offline(unsigned uid);

	//判断指定ud是否存在定时任务队列
	bool IsExistBuildRoutine(unsigned uid, unsigned ud) const;

	//队列结束
	int FinishQueue(unsigned uid, unsigned ud);

	//加速
	int Process(unsigned uid, User::SpeedUpReq* req, User::SpeedUpResp* resp);
private:
	//加速队列
	int SpeedUp(unsigned uid, unsigned ud, User::SpeedUpResp * resp);

private:
	map<unsigned, map<long, DataRoutineBase *> > routine_;  //建筑有关的定时任务 uid->(rid->p*)
	map<unsigned, map<unsigned, long> > buildRid_;  //uid->(ud->rid)
};

template<class T>
int LogicQueueManager::JoinRoutine(unsigned uid, unsigned endts, vector<unsigned> & uds)
{
	//加入到队列
	DataRoutineBase * proutine = new T(uid, endts, uds);

	//设置映射
	routine_[uid][proutine->rid] = proutine;

	for(size_t i = 0; i < uds.size(); ++i)
	{
		unsigned ud = uds[i];
		buildRid_[uid][ud] = proutine->rid;
	}

	return 0;
}

#endif //LOGIC_QUEUE_MANAGER_H_
