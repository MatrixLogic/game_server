#include "LogicInc.h"

#define CheckLevel(level) if (type == 2 && (level < 1 || level > 3)) return R_ERR_PARAM;

CGuessAllServerData* CLogicAllServerGuess::GetDataAllServerGuess(int type,int lev)
{
	int ret = 0;
	static map<int, CGuessAllServerData*> dataMap;
	string dataPath = Config::GetValue(ALL_SERVER_GUESS_DATA_PATH);
	if (dataPath.empty())
	{
		return NULL;
	}
	char buf[512] = {0};
	if (type == 2)
	{
		if (lev < 1 || lev > 3)
			return NULL;
		map<int, CGuessAllServerData*>::iterator itr = dataMap.find(lev);
		if (itr != dataMap.end() && NULL != itr->second)
		{
			return itr->second;
		}

		CGuessAllServerData *pdata = new CGuessAllServerData();
		snprintf(buf,sizeof(buf),"%s.%d.%d",dataPath.c_str(),type,lev);
		dataPath = buf;
		const int sem[3] = {sem_guessallserver_1,sem_guessallserver_2,sem_guessallserver_3};
		ret = pdata->Init(dataPath,type,(semdat)sem[lev-1]);
		if (0 != ret)
		{
			delete pdata;
			return NULL;
		}
		dataMap[lev] = pdata;
		return pdata;
	}

	static CGuessAllServerData* guessData = NULL;
	if(NULL == guessData)
	{
		guessData = new CGuessAllServerData();
		snprintf(buf,sizeof(buf),"%s.%d",dataPath.c_str(),type);
		dataPath = buf;
		ret = guessData->Init(dataPath,type,sem_guessallserver);
		if (0 != ret)
		{
			delete guessData;
			guessData = NULL;
			return NULL;
		}
	}
	return guessData;
}

int CLogicAllServerGuess::GuessorsPay(unsigned gid,int type,int lev)
{
	CheckLevel(lev);
	CGuessAllServerData* pGuessData = GetDataAllServerGuess(type,lev);
	GuessData* pData = pGuessData->GetData();
	if(pData == NULL){
		error_log("GuessorsPay get shm data failed");
		return R_ERR_LOGIC;
	}
	unsigned winCoins = 0;
	unsigned failCoins = 0;

	for(int i = 0; i < GUESS_GROUP_COUNT; ++i){
		if(pData->guessor[i].gid == gid){
			winCoins = pData->guessor[i].coins;
		}
		else{
			failCoins += pData->guessor[i].coins;
		}
	}

	CLogicPay logicPay;
	CLogicUpdates logicUpdates;
	for(int i = 0; i < GUESS_GROUP_COUNT; ++i){
		if(pData->guessor[i].gid == gid){
			GuessItem* pSupports = pData->guessor[i].supports;
			for(unsigned j = 0; j < pData->guessor[i].numOfApply;++j){
				int reward = int((pSupports[j].coins*1.0/winCoins)*failCoins*0.9 * PER_BET_COINS);
				if(reward < 1) reward = 1;
				logicPay.ChangePay(pSupports[j].uid, 0, pSupports[j].coins * PER_BET_COINS + reward, "MATCHGUESS",1);

				Json::Value updates;
				updates.resize(1);
				updates[(unsigned)0]["s"] = "MATCHGUESS";
				updates[(unsigned)0]["coins"] = reward;
				updates[(unsigned)0]["ts"] = Time::GetGlobalTime();
				logicUpdates.AddUpdates(pSupports[j].uid, updates);
			}
			break;
		}
	}

	return 0;
}

int CLogicAllServerGuess::GuessorsUid(unsigned gid, int type,int lev)
{
	CheckLevel(lev);
	CGuessAllServerData* pGuessData = GetDataAllServerGuess(type,lev);
	GuessData* pData = pGuessData->GetData();
	if(pData == NULL)
	{
		error_log("GuessorsPay get shm data failed");
		return R_ERR_LOGIC;
	}

	for(int i = 0; i < GUESS_GROUP_COUNT; ++i)
	{
		if(pData->guessor[i].gid == gid)
		{
			GuessItem* pSupports = pData->guessor[i].supports;
			for(unsigned j = 0; j < pData->guessor[i].numOfApply;++j)
			{
				cout << "个人赛uid:" << gid <<"的支持者" << j << " id为:" << pSupports[j].uid << " 竞猜花费金币为：" << pSupports[j].coins * PER_BET_COINS <<endl;
			}
		}
	}
	return 0;
}

int CLogicAllServerGuess::GuessorsUidCoins(unsigned gid, vector<U_U_MAP> &vecGuessors, int type,int lev)
{
	CheckLevel(lev);
	CGuessAllServerData* pGuessData = GetDataAllServerGuess(type,lev);
	GuessData* pData = pGuessData->GetData();
	if(pData == NULL){
		error_log("GuessorsPay get shm data failed");
		return R_ERR_LOGIC;
	}
	unsigned winCoins = 0;
	unsigned failCoins = 0;

	for(int i = 0; i < GUESS_GROUP_COUNT; ++i){
		if(pData->guessor[i].gid == gid){
			winCoins = pData->guessor[i].coins;
		}
		else{
			failCoins += pData->guessor[i].coins;
		}
	}
	for(int i = 0; i < GUESS_GROUP_COUNT; ++i)
	{
		if(pData->guessor[i].gid == gid)
		{
			GuessItem* pSupports = pData->guessor[i].supports;
			for(unsigned j = 0; j < pData->guessor[i].numOfApply;++j)
			{
				int reward = int((pSupports[j].coins*1.0/winCoins)*failCoins*0.9 * PER_BET_COINS);
				if(reward < 1) reward = 1;
				unsigned rewardAll = pSupports[j].coins * PER_BET_COINS + reward;
				vecGuessors.push_back(make_pair(pSupports[j].uid, rewardAll));
			}
		}
	}
	return 0;
}

int CLogicAllServerGuess::SetInitValues(int type,int lev,const vector<GuessViewData> &dataVec)
{
	CheckLevel(lev);
	CGuessAllServerData* pGuessData = GetDataAllServerGuess(type,lev);
	if (NULL == pGuessData)
	{
		return R_ERR_DATA;
	}
	int ret = pGuessData->SetInitValues(dataVec);
	if (0 != ret)
	{
		return ret;
	}
	return 0;
}

int CLogicAllServerGuess::GetSupportCount(unsigned gid,int type,int lev,unsigned &coins)
{
	CheckLevel(lev);
	CGuessAllServerData* pGuessData = GetDataAllServerGuess(type,lev);
	if (NULL == pGuessData)
	{
		return R_ERR_DATA;
	}
	return pGuessData->GetSupportCount(gid,coins);
}

int CLogicAllServerGuess::ViewGuess(unsigned uid, Json::Value &data, int type, int lev)
{
	CheckLevel(lev);
	CGuessAllServerData* pGuessData = GetDataAllServerGuess(type,lev);
	vector<GuessViewData> dataVec;
	if(pGuessData->View(uid,dataVec) != 0)
	{
		error_log("GuessView view failed");
		return R_ERR_LOGIC;
	}

	data["teams"].resize(dataVec.size());
	for(unsigned i = 0; i < dataVec.size();++i)
	{
		data["teams"][i]["gid"] = dataVec[i].gid;
		data["teams"][i]["gname"] = dataVec[i].gname;
		data["teams"][i]["coins"] = dataVec[i].coins;
		data["teams"][i]["uc"] = dataVec[i].numOfApply;
		data["teams"][i]["ucoins"] = dataVec[i].ucoins;
	}
	return 0;
}

int CLogicAllServerGuess::ApplyGuess(unsigned uid,unsigned gid, unsigned coins, int type, int lev)
{
	CheckLevel(lev);
	CGuessAllServerData* pGuessData = GetDataAllServerGuess(type,lev);
	if (NULL == pGuessData)
	{
		return R_ERR_DATA;
	}
	return pGuessData->Apply(uid,gid,coins);
}
