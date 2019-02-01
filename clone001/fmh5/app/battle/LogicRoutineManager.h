#ifndef LOGIC_ROUTINE_MANGAGER_H_
#define LOGIC_ROUTINE_MANGAGER_H_

#include "ServerInc.h"

class DataRoutineBase;

//这个应该与用户分开
class LogicRoutineManager : public BattleSingleton, public CSingleton<LogicRoutineManager>
{
private:
	friend class CSingleton<LogicRoutineManager>;
	LogicRoutineManager();

	long tailNextMillis(const long lastTimestamp) ;

    long timeGen();

	long GetNextRid(bool isPadding, long busId);

public:
	virtual void CallDestroy() { Destroy();}

	void OnTimer1();

	//添加常规任务，返回任务id
	long AddTask(DataRoutineBase * task);

	//用户离线处理
	void Offline(unsigned uid);

	void DelTask(long rid, bool isdeluser = true);

	//对某个任务进行加速
	int SpeedUpTask(long rid, int sec);

	//任务完成
	void TaskDone(DataRoutineBase * task);

private:
	map<unsigned, vector<DataRoutineBase *> > tasks_;  //ts -> task
	map<long, unsigned> rid_ts;  //rid-> task
	map<unsigned, vector<long> > user_task;  //用户的任务

	long sequence;
    long workerId;
    long regionId;

	static long lastTimestamp;
    // 基准时间
     const static long twepoch = 1288834974657L; //Thu, 04 Nov 2010 01:42:54 GMT
    // 区域标志位数
     const static long regionIdBits = 3L;
    // 机器标识位数
     const static long workerIdBits = 10L;
    // 序列号识位数
    const static long sequenceBits = 10L;

    // 区域标志ID最大值
     const static long maxRegionId = -1L ^ (-1L << regionIdBits);
    // 机器ID最大值
     const static long maxWorkerId = -1L ^ (-1L << workerIdBits);
    // 序列号ID最大值
     const static long sequenceMask = -1L ^ (-1L << sequenceBits);

     // 机器ID偏左移10位
     const static long workerIdShift = sequenceBits;
     // 业务ID偏左移20位
     const static long regionIdShift = sequenceBits + workerIdBits;
     // 时间毫秒左移23位
     const static long timestampLeftShift = sequenceBits + workerIdBits + regionIdBits;
};

#endif  //LOGIC_ROUTINE_MANGAGER_H_
