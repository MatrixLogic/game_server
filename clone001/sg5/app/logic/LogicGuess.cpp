#include "LogicInc.h"

CGuessData* CLogicGuess::GetDataGuess(int type){

	static map<int, CGuessData*> dataMap;
	int serverId = 0;
	Config::GetDomain(serverId);
	map<int, CGuessData*>::iterator itr = dataMap.find(serverId);
	if (itr != dataMap.end() && NULL != itr->second)
	{
		return itr->second;
	}
	CGuessData *pdata = new CGuessData();
	string dataPath = Config::GetValue(GUESS_DATA_PATH);
	if (dataPath.empty())
	{
		return NULL;
	}
	int ret = pdata->Init(dataPath,type);
	if (0 != ret)
	{
		delete pdata;
		return NULL;
	}
	dataMap[serverId] = pdata;
	return pdata;
}

CLogicGuess::CLogicGuess(){

}

int CLogicGuess::GuessorsPay(unsigned gid,int type){
	CGuessData* pGuessData = GetDataGuess(type);
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

int CLogicGuess::GuessorsUid(unsigned gid, int type)
{
	CGuessData* pGuessData = GetDataGuess(type);
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
}

int CLogicGuess::GuessorsUidCoins(unsigned gid, vector<U_U_MAP> &vecGuessors, int type)
{
	CGuessData* pGuessData = GetDataGuess(type);
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
				//logicPay.ChangeCash(pSupports[j].uid,"MATCHGUESS",pSupports[j].coins * PER_BET_COINS + reward,1);
				vecGuessors.push_back(make_pair(pSupports[j].uid, rewardAll));
			}
		}
	}
	return 0;
}

