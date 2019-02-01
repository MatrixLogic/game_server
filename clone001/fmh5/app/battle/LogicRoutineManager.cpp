#include "LogicRoutineManager.h"

long LogicRoutineManager::lastTimestamp = -1L;

LogicRoutineManager::LogicRoutineManager():
			sequence(0),
		    workerId(0),
		    regionId(0)
{

}

long LogicRoutineManager::GetNextRid(bool isPadding, long busId)
{
	//生成唯一rid
    long timestamp = timeGen();
    long paddingnum = regionId;

    //41 bits: Timestamp (毫秒) | 3 bits: 区域（机房） | 10 bits: 机器编号 | 10 bits: 序列号 |
    if (isPadding)
    {
         paddingnum = busId;
    }

    if (timestamp < lastTimestamp)
    {
        try
        {
            throw runtime_error("Clock moved backwards.  Refusing to generate id for milliseconds");
        }catch (exception & e)
        {
            e.what();
        }
    }

    //如果上次生成时间和当前时间相同,在同一毫秒内
    if (lastTimestamp == timestamp)
    {
        //sequence自增，因为sequence只有10bit，所以和sequenceMask相与一下，去掉高位
        sequence = (sequence + 1) & sequenceMask;
        //判断是否溢出,也就是每毫秒内超过1024，当为1024时，与sequenceMask相与，sequence就等于0
        if (sequence == 0)
        {
            //自旋等待到下一毫秒
            timestamp = tailNextMillis(lastTimestamp);
        }
    } else
    {
        // 如果和上次生成时间不同,重置sequence，就是下一毫秒开始，sequence计数重新从0开始累加,
        // 为了保证尾数随机性更大一些,最后一位设置一个随机数
        sequence = rand()%10;
    }

    lastTimestamp = timestamp;

    cout<<timestamp<<","<<twepoch<<",seq="<<sequence<<endl;
    return ((timestamp - twepoch) << timestampLeftShift) | (paddingnum << regionIdShift) | (workerId << workerIdShift) | sequence;
}

long LogicRoutineManager::tailNextMillis(const long lastTimestamp)
{
    long timestamp = timeGen();
    while (timestamp <= lastTimestamp) {
        timestamp = timeGen();
    }
    return timestamp;
}

long LogicRoutineManager::timeGen()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);

    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void LogicRoutineManager::OnTimer1()
{
	unsigned nowts = Time::GetGlobalTime();

	for(map<unsigned, vector<DataRoutineBase *> >::iterator vit = tasks_.begin(); vit != tasks_.end();)
	{
		if (vit->first > nowts)
		{
			break;
		}

		//时间已到
		for(size_t i = 0; i < vit->second.size(); ++i)
		{
			//执行到时的方法
			vit->second[i]->DoEnd();

			//释放指针空间
			delete vit->second[i];
		}

		tasks_.erase(vit++);
	}
}

long LogicRoutineManager::AddTask(DataRoutineBase * task)
{
	long rid = GetNextRid(false, 0);

	task->rid = rid;
	unsigned endts = task->endts_;
	tasks_[endts].push_back(task);
	rid_ts[rid] = endts;

	user_task[task->uid_].push_back(rid);

	return rid;
}

int LogicRoutineManager::SpeedUpTask(long rid, int sec)
{
	//判断是否存在任务
	if(!rid_ts.count(rid))
	{
		throw runtime_error("wrong queue");
	}

	unsigned int endts = rid_ts[rid];
	unsigned newts = 0;
	DataRoutineBase * task = NULL;
	unsigned currts = Time::GetGlobalTime();

	//遍历找出正确的任务
	for(size_t i = 0; i < tasks_[endts].size(); ++i)
	{
		if (tasks_[endts][i]->rid == rid)
		{
			tasks_[endts][i]->endts_ -= sec;  //修改，然后从集合中删除
			newts = tasks_[endts][i]->endts_;

			if (currts >= newts)
			{
				//当前时间大于结束时间，表示任务完成，直接在这里处理完成的动作
				TaskDone(tasks_[endts][i]);

				return 0;
			}

			//任务未完成的，将任务从旧的时间映射中删除，然后加入到新的时间映射关系中
			task = tasks_[endts][i];
			tasks_[endts].erase(tasks_[endts].begin() + i);

			break;
		}
	}

	if (0 != newts)
	{
		//修改新任务的时间
		tasks_[newts].push_back(task);
	}

	return 0;
}

void LogicRoutineManager::TaskDone(DataRoutineBase * task)
{
	//调用任务的结束动作
	task->DoEnd();

	//删除任务的相关信息
	DelTask(task->rid);
}

void LogicRoutineManager::Offline(unsigned uid)
{
	//用户离线，删除已建立的定时任务
	if (!user_task.count(uid))
	{
		return ;
	}


	for(size_t i = 0; i < user_task[uid].size(); ++i)
	{
		long rid =  user_task[uid][i];

		//这里不进行单个用户内的任务的删除
		DelTask(rid, false);
	}

	//直接清理更快
	user_task[uid].clear();
}

void LogicRoutineManager::DelTask(long rid, bool isdeluser)
{
	//从tasks_中删除
	unsigned endts = rid_ts[rid];
	unsigned uid = 0;

	//从时间相关的定时任务中删除当前任务
	map<unsigned, vector<DataRoutineBase *> >::iterator viter = tasks_.find(endts);

	if (viter != tasks_.end())
	{
		//从vector中找到该任务，然后删除
		for(size_t i = 0; i < viter->second.size(); ++i)
		{
			DataRoutineBase * proutine = viter->second[i];

			if (proutine->rid == rid)
			{
				uid = proutine->uid_;

				//释放空间
				delete proutine;
				//删除
				tasks_[endts].erase(viter->second.begin() + i);

				break;
			}
		}
	}

	rid_ts.erase(rid);

	//从用户关联的定时任务中删除当前任务
	if (isdeluser && user_task.count(uid) )
	{
		vector<long>::iterator liter = find(user_task[uid].begin(), user_task[uid].end(), rid);

		if (liter != user_task[uid].end())
		{
			user_task[uid].erase(liter);
		}
	}
}
