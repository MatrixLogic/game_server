#include "LogicDragonball.h"

static int CalcStartTime()
{
	time_t now;
	time(&now);
	int tempNow = now;
	struct tm *pTm = localtime(&now);
	int tempSec = pTm->tm_sec;
	int tempMin = pTm->tm_min;
	int tempHour = pTm->tm_hour;
	int startTime = tempNow - tempSec - tempMin * 60 - tempHour *3600;
	return startTime;
}

CDataDragonball* CLogicDragonball::GetCDataDragonball()
{
	GET_MEM_DATA_SEM(CDataDragonball, CONFIG_DRAGONBALL_DATA ,sem_dragonball)
	/*static CDataDragonball* pdragonball = NULL;
	if (!pdragonball)
	{
		pdragonball = new CDataDragonball();
		if (pdragonball->Init(Config::GetValue(CONFIG_DRAGONBALL_DATA)) != 0)
		{
			delete pdragonball;
			pdragonball = NULL;
		}
	}
	return pdragonball;*/
}

int CLogicDragonball::WhichDragonballHold(unsigned uid, unsigned &ballId, unsigned &ts)
{
	CDataDragonball* pball = GetCDataDragonball();
	if (!pball)
	{
		DB_ERROR_RETURN_MSG("get_ball_instance_fail");
	}
	int ret = pball->WhichDragonballHold(uid, ballId, ts);
	if (ret != 0)
	{
		DB_ERROR_RETURN_MSG("get_dragonball_fail");
	}
	return 0;
}

int CLogicDragonball::GetDragonballs(Json::Value &data)
{
	CDataDragonball* pball = GetCDataDragonball();
	if (!pball)
	{
		DB_ERROR_RETURN_MSG("get_ball_instance_fail");
	}
	DragonballData balls;
	int ret = pball->ViewAllDragonball(balls);
	if (ret != 0)
	{
		DB_ERROR_RETURN_MSG("get_dragonball_fail");
	}
	DataUser user;
	CLogicUser logicUser;
	data.resize(DRAGONBALL_ID_MAX - DRAGONBALL_ID_MIN + 1);
	for (int i = DRAGONBALL_ID_MIN; i <= DRAGONBALL_ID_MAX; i++)
	{
		unsigned index = DragonballIdToIndex(i);
		if (!IsDragonballActivityTs() || !IsValidUid(balls.ball[index].holderUid))
		{
			data[index]["ballid"] = INVALID_BALL_ID;
		}
		else
		{
			data[index]["ballid"] = i;
		}
		data[index]["uid"] = balls.ball[index].holderUid;
		data[index]["name"] = balls.ball[index].name;
		data[index]["union_id"] = balls.ball[index].allianceId;
		data[index]["union_name"] = balls.ball[index].aname;
		data[index]["ts"] = balls.ball[index].ts;
		if (logicUser.GetUserLimit(balls.ball[index].holderUid, user) == 0)
			data[index]["level"] = user.level;
	}
	return 0;
}

int CLogicDragonball::ViewAllDragonball(DragonballData &balls)
{
	CDataDragonball* pball = GetCDataDragonball();
	if (!pball)
	{
		DB_ERROR_RETURN_MSG("get_ball_instance_fail");
	}
	int ret = pball->ViewAllDragonball(balls);
	if (ret != 0)
	{
		DB_ERROR_RETURN_MSG("get_dragonball_fail");
	}
	return 0;
}

bool CLogicDragonball::EnableAttackForDragonball(unsigned ballid, unsigned level)
{
	switch(ballid)
	{
	case 1: return (level >= 30 && level <= 44);
	case 2: return (level >= 45 && level <= 54);
	case 3: return (level >= 55 && level <= 64);
	case 4: return (level >= 65 && level <= 70);
	case 5: return (level >= 71 && level <= 75);
	case 6: return (level >= 76 && level <= 80);
	case 7: return (level > 80);
	default: break;
	}
	return false;
}

int CLogicDragonball::SnatchDragonball(unsigned ballid, unsigned snatchUid, const string &snatchName)
{
	CDataDragonball* pball = GetCDataDragonball();
	if (!pball)
	{
		DB_ERROR_RETURN_MSG("get_ball_instance_fail");
	}
	unsigned preUid = 0;
	unsigned preTs = 0;
	string preName;
	int ret = pball->SetDragonball(ballid, snatchUid, snatchName, Time::GetGlobalTime(), preUid, preTs, preName);
	if (ret != 0)
	{
		DB_ERROR_RETURN_MSG("set_dragonball_fail");
	}
	unsigned detlatime = Time::GetGlobalTime() - preTs;
	if(detlatime > 86400)
		return -1;

	AllianceBall aBall;
	SetAlliance(aBall);
	PayAlliance(aBall);

	Json::Value updates;
	updates.resize(1);
	updates[(unsigned)0]["s"] = "DRAGONBALLLOSS";
	updates[(unsigned)0]["name"] = snatchName;
	updates[(unsigned)0]["ballid"] = ballid;
	updates[(unsigned)0]["tm"] = detlatime;
	updates[(unsigned)0]["ts"] = Time::GetGlobalTime();
	CLogicUpdates logicUpdates;
	logicUpdates.AddUpdates(preUid, updates, true);

	Json::Value updates2;
	updates2.resize(1);
	updates2[(unsigned)0]["s"] = "DRAGONBALLHOLD";
	updates2[(unsigned)0]["name"] = preName;
	updates2[(unsigned)0]["ballid"] = ballid;
	updates2[(unsigned)0]["ballts"] = Time::GetGlobalTime();
	updates2[(unsigned)0]["ts"] = Time::GetGlobalTime();
	logicUpdates.AddUpdates(snatchUid, updates2);
	return 0;
}

int CLogicDragonball::GetAllianceDragons(map<unsigned,AllianceBall>& allianceBalls){

	CDataDragonball* pball = GetCDataDragonball();
	if (!pball)
	{
		DB_ERROR_RETURN_MSG("get_ball_instance_fail");
	}

	return pball->GetAllianceDragons(allianceBalls);
}

int CLogicDragonball::PayAlliance(AllianceBall& aBall){
	int gap = (Time::GetGlobalTime() - aBall.ts) / 600;
	int reward = gap * 5;
	if (reward < 0) reward = 0;
	return 0;
	if(aBall.allianceId > 0 && reward > 0){
		CLogicAlliance logicAlliance;
		logicAlliance.ChangeResource(aBall.allianceId,0ll,0ll,0ll,0ll,0ll,reward,"DRAGONBALL");

		DataAlliance alliance;
		int ret = logicAlliance.GetAllianceLimit(aBall.allianceId, alliance);
		if(ret != 0)
		{
			return ret;
		}

		CLogicUpdates logicUpdates;
		Json::Value updates;
		updates.resize(1);
		updates[(unsigned)0]["s"] = "DBALLREWARD_ALLIANCE";
		updates[(unsigned)0]["tm"] = Time::GetGlobalTime() - aBall.ts;
		updates[(unsigned)0]["ts"] = Time::GetGlobalTime();
		logicUpdates.AddUpdates(alliance.leader_uid, updates);
	}

	return 0;
}

int CLogicDragonball::SetAlliance(AllianceBall& aBall){
	CDataDragonball* pball = GetCDataDragonball();
	if (!pball)
	{
		DB_ERROR_RETURN_MSG("get_ball_instance_fail");
	}

	return pball->SetAlliance(aBall);
}

int CLogicDragonball::ResetAlliance(){
	CDataDragonball* pball = GetCDataDragonball();
	if (!pball)
	{
		DB_ERROR_RETURN_MSG("get_ball_instance_fail");
	}

	return pball->ResetAlliance();
}

bool CLogicDragonball::IsDragonballActivityTs(void)
{
	string rangeTime = Config::GetValue(CONFIG_DRAGONBALL_ACTIVITY_TS);
	Json::Value value;
	Json::Reader().parse(rangeTime, value);

	unsigned tempNow = time(NULL);
	unsigned startTime = CalcStartTime();
	tempNow -= startTime;

	for(unsigned int i=0;i<value.size();++i)
	{
		if(tempNow >= value[i][0u].asUInt() && tempNow <= value[i][1u].asUInt())
		{
			return true;
		}
	}
	return false;
}


