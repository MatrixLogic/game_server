#include "LogicPersonMatch.h"
#include <fstream>

#define CheckLevel(level) if (level < 1 || level > 3) return R_ERR_PARAM;

typedef pair<unsigned, int> U_L_MAP;

static int AddUpdate(unsigned uid, int rank, int level)
{
	int ret = 0;
	if (level < 1 || level > 3)
	{
		cout << "level is over!" << endl;
		return 1;
	}
	Json::Value UpdateData = Json::Value(Json::arrayValue);
	UpdateData[0u]["ts"] = Time::GetGlobalTime();
	UpdateData[0u]["s"] = "allserverpersonmatch";
	UpdateData[0u]["rank"] = rank;
	UpdateData[0u]["level"] = level;
	CLogicUpdates logicUpdates;
	ret = logicUpdates.AddUpdates(uid,UpdateData);
	if (0 != ret)
	{
		cout << "AddUpdates fail,uid=" << uid << ",ret=" << ret << endl;
		return ret;
	}
	return 0;
}

CAllServerPersonBaseMatch* CLogicAllServerPersonMatch::GetAllServerPsersonBaseMatch(int level)
{
	if (level < 1 || level > 3)
	{
		return NULL;
	}
	static map<int,CAllServerPersonBaseMatch *> dataMap;
	map<int,CAllServerPersonBaseMatch *>::iterator itr = dataMap.find(level);
	if (itr != dataMap.end())
	{
		return itr->second;
	}

	CAllServerPersonBaseMatch *pdata = new CAllServerPersonBaseMatch;
	string dataPath = Config::GetValue(CONFIG_ALL_SERVER_PERSON_MATCH_PATH);
	if (dataPath.empty())
	{
		return NULL;
	}
	string dataPathlevel;
	String::Format(dataPathlevel,"%s.%d",dataPath.c_str(),level);

	int sem[3] = {sem_personmatchallserver_1,sem_personmatchallserver_2,sem_personmatchallserver_3};

	int ret = pdata->Init(dataPathlevel,(semdat)sem[level-1]);
	if (0 != ret)
	{
		error_log("Init data fail!");
		delete pdata;
		return NULL;
	}
	dataMap[level] = pdata;
	return pdata;
}

int CLogicAllServerPersonMatch::GetBaseMatchInfo(unsigned uid, Json::Value &data, int level)
{
	CheckLevel(level);
	int ret = 0;
	CAllServerPersonBaseMatch *pmatch = GetAllServerPsersonBaseMatch(level);
	if (!pmatch)
	{
		DB_ERROR_RETURN_MSG("init_personmatch_fail");
	}
	
	PersonMatchPlayer person;
	vector<PersonMatchPlayer> playoff;
	int stage = 0;
	ret = pmatch->View(uid,stage,person,playoff);
	if(ret != 0){
		DB_ERROR_RETURN_MSG("view_personmatch_fail");
	}

	data["stage"] = stage;	//0:报名阶段，1：常规赛阶段，2：8强赛，3：4强赛，4：决赛 5：比赛结束，尚未开始报名
	data["apply"] = (person.uid == 0? 0 : 1);	//0:你未报名，1：你已报名 2:报名后未抽到 3报名后抽到 4报名未成功
	data["status"] = 0; //0：报名阶段 1：报名失败 2:进预赛 3；进8强 4：进4强 5：进决赛 6：亚军 7：冠军
	if(stage >= PMS_REGULAR){
		data["status"] = 2;
		data["apply"] = (person.uid == 0? 0 : 3);
	}
	
	data["teams"].resize(15);
	for(unsigned i = 0; i < 15; ++i){
		data["teams"][i]["uid"] = playoff[i].uid;
		data["teams"][i]["name"] = playoff[i].name;
		data["teams"][i]["flag"] = playoff[i].flag[0];
		if(playoff[i].uid != 0){
			DataUserBasic userBasic;
			CLogicUserBasic logicUserBasic;
			ret = logicUserBasic.GetUserBasicLimit(playoff[i].uid, OpenPlatform::GetType(), userBasic);
			if (ret != 0)
				continue;
			data["teams"][i]["fingure"] = userBasic.figure_url;
		}
	}
	

	data["fights"].resize(7);
	for (unsigned i = 0; i < 7; i++){
		data["fights"][i]["teams1"]["uid"] = playoff[2*i+1].uid;
		data["fights"][i]["teams1"]["name"] = playoff[2*i+1].name; 
		data["fights"][i]["teams1"]["score"] = playoff[2*i+1].damage[0]; 

		data["fights"][i]["teams2"]["uid"] = playoff[2*i+2].uid;
		data["fights"][i]["teams2"]["name"] = playoff[2*i+2].name; 
		data["fights"][i]["teams2"]["score"] = playoff[2*i+2].damage[0]; 
	}
	
	data["rivals"].resize(6);
	for (unsigned i = 0; i < 6; i++)
	{
		data["rivals"][i]["order"] = (i+1);
		data["rivals"][i]["ruid"] = 0;
	}

	if(!data["apply"]){
		return 0;
	}

	int index = 0;
	for(; index < 3 ; ++index){
		PersonMatchPlayer rival;
		int idx = person.rivals[index];
		int ret = pmatch->GetRival(idx,rival);
		if(ret != 0){
			return ret;
		}

		data["rivals"][index]["ruid"] = rival.instanceid;
		data["rivals"][index]["rname"] = rival.name;
		data["rivals"][index]["result"] = person.damage[index];
	}

	for(unsigned i = 3; i < 7; ++i){
		if(playoff[2*i+1].uid == uid){
			data["status"] = 3;
			data["rivals"][index]["ruid"] = playoff[2*i+2].instanceid;
			data["rivals"][index]["rname"] = playoff[2*i+2].name;
			data["rivals"][index]["result"] = playoff[2*i+1].damage[0];
			break;
		}
		if(playoff[2*i+2].uid == uid){
			data["status"] = 3;
			data["rivals"][index]["ruid"] = playoff[2*i+1].instanceid;
			data["rivals"][index]["rname"] = playoff[2*i+1].name;
			data["rivals"][index]["result"] = playoff[2*i+2].damage[0];
			break;
		}
	}
	++index;

	for (unsigned i = 1; i < 3; i++){
		if(playoff[2*i+1].uid == uid){
			data["status"] = 4;
			data["rivals"][index]["ruid"] = playoff[2*i+2].instanceid;
			data["rivals"][index]["rname"] = playoff[2*i+2].name;
			data["rivals"][index]["result"] = playoff[2*i+1].damage[0];
			break;
		}
		if(playoff[2*i+2].uid == uid){
			data["status"] = 4;
			data["rivals"][index]["ruid"] = playoff[2*i+1].instanceid;
			data["rivals"][index]["rname"] = playoff[2*i+1].name;
			data["rivals"][index]["result"] = playoff[2*i+2].damage[0];
			break;
		}
	}
	++index;

	if(playoff[1].uid == uid){
		data["status"] = 5;
		data["rivals"][index]["ruid"] = playoff[2].instanceid;
		data["rivals"][index]["rname"] = playoff[2].name;
		data["rivals"][index]["result"] = playoff[1].damage[0];
	}

	if(playoff[2].uid == uid){
		data["status"] = 5;
		data["rivals"][index]["ruid"] = playoff[1].instanceid;
		data["rivals"][index]["rname"] = playoff[1].name;
		data["rivals"][index]["result"] = playoff[2].damage[0];
	}
	
	if(playoff[0].uid == uid){
		data["status"] = 7;
	}

	if(playoff[0].uid != 0 && data["status"] == 5){
		data["status"] = 6;
	}
	
	return 0;
}

int CLogicAllServerPersonMatch::Apply(unsigned uid,int level)
{
	CheckLevel(level);
	CAllServerPersonBaseMatch *pmatch = GetAllServerPsersonBaseMatch(level);
	if (NULL == pmatch)
	{
		DB_ERROR_RETURN_MSG("init_personmatch_fail");
	}

	DataUserBasic userBasic;
	CLogicUserBasic logicUserBasic;
	int ret = logicUserBasic.GetUserBasicLimit(uid, OpenPlatform::GetType(), userBasic);
	if (ret != 0)
		return ret;

	/*DataUser user;
	CLogicUser logicUser;
	ret = logicUser.GetUser(uid, user);
	if (ret == 0)
	{
		if (user.level < CONFIG_LEVEL_RESTRICT)
		{
			error_log("person match apply fail, level exceed! uid = %d", uid);
			return R_ERR_REFUSE;
		}
	}*/

	int serverid = (int)(uid - UID_MIN) / 500000 + 1;
	string userName;
	String::Format(userName, "%d区_%s", serverid, userBasic.name.c_str());
	//debug_log("servreid=%d,username=%s",serverid,userName.c_str());
	unsigned instanceid;
	ret = pmatch->Apply(uid, userName, instanceid);
	if (ret != 0)
		return ret;

	Json::Value json;
	CLogicInstance logicInst;
	ret = logicInst.GetMinInstance(uid, json);
	if (ret != 0)
		return ret;
	json["name"] = userBasic.name;
	json["pic"] = userBasic.figure_url;

	json["userid"] = instanceid;
	CLogicMatchInstance logicMatchInstance;
	ret = logicMatchInstance.SetAllServerMatchInstance(instanceid, json);
	if (0 != ret)
	{
		return ret;
	}
	return 0;
}

int CLogicAllServerPersonMatch::ReportResult(unsigned uid, int order, int damage, int level)
{
	CheckLevel(level);
	CAllServerPersonBaseMatch *pmatch = GetAllServerPsersonBaseMatch(level);
	if (!pmatch)
	{
		DB_ERROR_RETURN_MSG("init_personmatch_fail");
	}

	return pmatch->ReportResult(uid,order,damage);
}

int CLogicAllServerPersonMatch::Load(unsigned instid, unsigned uidBy, Json::Value &result)
{
	int ret = 0;
	CLogicMatchInstance logicMatchInstance;
	ret = logicMatchInstance.GetAllServerMatchInstance(instid,result);
	if (ret != 0)
	{
		error_log("[load match instance fail][ret=%d,instid=%u]",ret,instid);
		return ret;
	}
	result["userid"] = instid;
	return 0;
}

int CLogicAllServerPersonMatch::GetStage(int& stage,int level)
{
	CheckLevel(level);
	CAllServerPersonBaseMatch *pmatch = GetAllServerPsersonBaseMatch(level);
	if (!pmatch)
	{
		DB_ERROR_RETURN_MSG("init_personmatch_fail");
	}

	return pmatch->GetStage(stage);
}

int CLogicAllServerPersonMatch::NextTurn(int lev)
{
	CheckLevel(lev);
	CAllServerPersonBaseMatch *pmatch = GetAllServerPsersonBaseMatch(lev);
	if (NULL == pmatch)
	{
		DB_ERROR_RETURN_MSG("init_personmatch_fail");
	}
	int ret = pmatch->NextTurn();
	if (0 != ret)
	{
		error_log("[data person match next turn fail][ret=%d]",ret);
		return ret;
	}

	const int personMatchLevel[3] = {1,5000,10000};
	pmatch->SetCurrInstId(ALL_SERVER_PERSONMATCH_INST_ID_MIN + personMatchLevel[lev-1]);

	string instPath;
	getInstPath(instPath,lev);
	ifstream ifile(instPath.c_str(),ios::in);
	if(!ifile)
	{
		error_log("[open inst file fail][path=%s]", instPath.c_str());
		return R_ERR_LOGIC;
	}

	char szTmp[100];
	int i = 0;
	PersonMatchPlayer rival[8];
	memset(rival,0,sizeof(PersonMatchPlayer)*8);
	while(ifile.getline(szTmp,sizeof(szTmp)))
	{
		vector<string> rlt;
		String::Split(szTmp,' ',rlt);
		int uid = atoi(rlt[0].c_str());
		CLogicUserBasic logicUserBasic;
		DataUserBasic userBasic;

		rival[i].uid = uid;
		if(!IsNPCUid(uid))
		{
			ret = logicUserBasic.GetUserBasicLimitSmart(uid, OpenPlatform::GetType(), userBasic);
			if (ret != 0)
			{
				break;
			}
			snprintf(rival[i].name,PERSONMATCH_NAME_LEN_MAX,"%s",userBasic.name.c_str());
		}
		else
		{
			string npcName = "先锋";
			string npcName2 = "号";
			snprintf(rival[i].name,PERSONMATCH_NAME_LEN_MAX,"%s%d%s",npcName.c_str(), i, npcName2.c_str());
		}

		if(rlt.size() >= 2)
		{
			rival[i].instanceid = atoi(rlt[1].c_str());
		}
		else
		{
			Json::Value json;
			if(IsNPCUid(uid))
			{
				CLogicNpc logicNpc;
				ret = logicNpc.Load(uid, uid, json);
				if (ret != 0)
					return ret;
				json["name"] = "npc";
				string npcName = "先锋";
				string npcName2 = "号";
				snprintf(rival[i].name,PERSONMATCH_NAME_LEN_MAX,"%s%d%s",npcName.c_str(), i, npcName2.c_str());
			}
			else
			{
				CLogicInstance logicInst;
				ret = logicInst.GetMinInstance(uid, json);
				if (ret != 0)
					break;
				json["name"] = userBasic.name;
				json["pic"] = userBasic.figure_url;
			}

			unsigned instanceId = ALL_SERVER_PERSONMATCH_INST_ID_MIN;
			ret = pmatch->GetNextInstId(instanceId);
			if (0 != ret)
			{
				break;
			}
			json["userid"] = instanceId;
			CLogicMatchInstance logicMatchInstance;
			ret = logicMatchInstance.SetAllServerMatchInstance(instanceId, json);
			if (0 != ret)
			{
				error_log("[SetAllServerMatchInstance fail][ret=%d,instid=%u]",ret,instanceId);
			}
			rival[i].instanceid = instanceId;
		}

		++i;
		if(i >= 8){
			break;
		}
	}
	ret = pmatch->SetRivals(rival);
	if (0 != ret)
	{
		error_log("[SetRivals fail][ret=%d]",ret);
		return ret;
	}
	return 0;
}

int CLogicAllServerPersonMatch::StartRegular(int lev)
{
	CheckLevel(lev);
	CAllServerPersonBaseMatch *pmatch = GetAllServerPsersonBaseMatch(lev);
	if (NULL == pmatch)
	{
		DB_ERROR_RETURN_MSG("init_personmatch_fail");
	}

	int ret = pmatch->StartRegular();
	if (0 != ret)
	{
		error_log("[StartRegular fail][ret=%d]",ret);
		return ret;
	}
	return 0;
}


int CLogicAllServerPersonMatch::StartGuess(int lev)
{
	CheckLevel(lev);
	CAllServerPersonBaseMatch *pmatch = GetAllServerPsersonBaseMatch(lev);
	if (NULL == pmatch)
	{
		DB_ERROR_RETURN_MSG("init_personmatch_fail");
	}
	int ret = pmatch->StartGuess();
	if (0 != ret)
	{
		error_log("[StartGuess fail][ret=%d]",ret);
		return ret;
	}
	CShareMemory* sh = NULL;
	PersonMatchData *pMatchData = pmatch->GetPersonMatchData(sh);
	if (NULL == pMatchData)
	{
		return R_ERR_DATA;
	}
	CAutoLock(sh,true);

	vector<GuessViewData> dataVec;
	for (int i=0; i<8; i++)
	{
		GuessViewData tmpData;
		memset(&tmpData, 0 ,sizeof(GuessViewData));
		tmpData.gid = pMatchData->top8[i].uid;
		snprintf(tmpData.gname,sizeof(tmpData.gname),"%s",pMatchData->top8[i].name);
		dataVec.push_back(tmpData);
	}

	string instPath;
	getInstPath(instPath, lev);
	ofstream ofile(instPath.c_str(),ios::trunc);
	if (ofile.is_open())
	{
		for(int i = 0; i<8;++i)
		{
			if (pMatchData->top8[i].uid)
				ofile << pMatchData->top8[i].uid << " "
						<< pMatchData->top8[i].instanceid << endl;
			else
				ofile << 61 + lev * 10 + i << endl;
		}
	}
	ofile.close();

	CLogicAllServerGuess logicAllServerGuess;
	ret = logicAllServerGuess.SetInitValues(2,lev,dataVec);
	if (0 != ret)
	{
		error_log("[SetInitValues fail][ret=%d]",ret);
		return ret;
	}

	return 0;
}

int CLogicAllServerPersonMatch::StartPlayoff8(int lev)
{
	CheckLevel(lev);
	CAllServerPersonBaseMatch *pmatch = GetAllServerPsersonBaseMatch(lev);
	if (NULL == pmatch)
	{
		DB_ERROR_RETURN_MSG("init_personmatch_fail");
	}
	int ret = pmatch->StartPlayoff8();
	if (0 != ret)
	{
		error_log("[StartPlayoff8 fail][ret=%d]",ret);
		return ret;
	}
	return 0;
}

int CLogicAllServerPersonMatch::StartPlayoff4(int lev)
{
	CheckLevel(lev);
	CAllServerPersonBaseMatch *pmatch = GetAllServerPsersonBaseMatch(lev);
	if (NULL == pmatch)
	{
		DB_ERROR_RETURN_MSG("init_personmatch_fail");
	}

	CShareMemory* sh = NULL;
	PersonMatchData *pdata = pmatch->GetPersonMatchData(sh);
	if (NULL == pdata)
	{
		return R_ERR_DATA;
	}
	CAutoLock(sh,true);

	PersonMatchPlayer top4[4];
	memset(top4,0,sizeof(top4));

	PromotionRule(pdata->top8[0],pdata->top8[1],lev,top4[0]);
	PromotionRule(pdata->top8[2],pdata->top8[3],lev,top4[1]);
	PromotionRule(pdata->top8[4],pdata->top8[5],lev,top4[2]);
	PromotionRule(pdata->top8[6],pdata->top8[7],lev,top4[3]);

	int ret = pmatch->StartPlayoff4(top4);
	if (0 != ret)
	{
		error_log("[StartPlayoff4 fail][ret=%d]",ret);
		return ret;
	}
	return 0;
}

int CLogicAllServerPersonMatch::StartPlayoff2(int lev)
{
	CheckLevel(lev);
	CAllServerPersonBaseMatch *pmatch = GetAllServerPsersonBaseMatch(lev);
	if (NULL == pmatch)
	{
		DB_ERROR_RETURN_MSG("init_personmatch_fail");
	}

	CShareMemory* sh = NULL;
	PersonMatchData *pdata = pmatch->GetPersonMatchData(sh);
	if (NULL == pdata)
	{
		return R_ERR_DATA;
	}
	CAutoLock(sh,true);

	PersonMatchPlayer top2[2];
	memset(top2,0,sizeof(top2));
	PromotionRule(pdata->top4[0],pdata->top4[1],lev,top2[0]);
	PromotionRule(pdata->top4[2],pdata->top4[3],lev,top2[1]);

	int ret = pmatch->StartPlayoff2(top2);
	if (0 != ret)
	{
		error_log("[StartPlayoff2 fail][ret=%d]",ret);
		return ret;
	}
	return 0;
}

int CLogicAllServerPersonMatch::FinishMatch(int lev)
{
	CheckLevel(lev);
	CAllServerPersonBaseMatch *pmatch = GetAllServerPsersonBaseMatch(lev);
	if (NULL == pmatch)
	{
		DB_ERROR_RETURN_MSG("init_personmatch_fail");
	}

	CShareMemory* sh = NULL;
	PersonMatchData *matchData = pmatch->GetPersonMatchData(sh);
	if (NULL == matchData)
	{
		return R_ERR_DATA;
	}
	CAutoLock(sh,true);

	PersonMatchPlayer champion;
	memset(&champion,0,sizeof(champion));
	PromotionRule(matchData->top2[0],matchData->top2[1],lev,champion);

	int ret = pmatch->FinishMatch(&champion);
	if (0 != ret)
	{
		error_log("[FinishMatch fail][ret=%d]",ret);
		return ret;
	}

	vector<unsigned> players; //报名的成员
	for(unsigned i = 0; i < matchData->numOfApply;++i)
	{
		if(    matchData->mems[i].flag[0] == 1
			&& matchData->mems[i].flag[1] == 1
			&& matchData->mems[i].flag[2] == 1)
		{
			players.push_back(matchData->mems[i].uid);
		}
	}

	vector<U_L_MAP> vecTeams;
	for(unsigned i = 0; i < 8; ++i)
	{
		vecTeams.push_back(make_pair(matchData->top8[i].uid, 8));
		//报名成员中去掉八强
		vector<unsigned>::iterator playersItr = find(players.begin(),players.end(),matchData->top8[i].uid);
		if (playersItr != players.end())
		{
			players.erase(playersItr);
		}
	}

	for(unsigned i = 0; i < 4; ++i)
	{
		for(vector<U_L_MAP>::iterator itr=vecTeams.begin(); itr != vecTeams.end(); itr++)
		{
			if (itr->first == matchData->top4[i].uid)
			{
				itr->second = 4;
			}
		}
	}

	for(unsigned i = 0; i < 2; ++i)
	{
		for(vector<U_L_MAP>::iterator itr=vecTeams.begin(); itr != vecTeams.end(); itr++)
		{
			if (itr->first == matchData->top2[i].uid)
			{
				itr->second = 2;
			}
		}
	}

	for(vector<U_L_MAP>::iterator itr=vecTeams.begin(); itr != vecTeams.end(); itr++)
	{
		if (itr->first == matchData->champion.uid)
		{
			itr->second = 1;
		}
	}

	for(vector<U_L_MAP>::iterator itr=vecTeams.begin(); itr != vecTeams.end(); itr++)
	{
		if (IsValidUid(itr->first))
		{
			AddUpdate(itr->first, itr->second,lev);
		}
	}

	//发奖励
	CLogicAllServerGuess logicGuess;
	vector<U_U_MAP> vecGuessors;
	logicGuess.GuessorsPay(matchData->champion.uid,2,lev);
	logicGuess.GuessorsUidCoins(matchData->champion.uid, vecGuessors, 0,lev);
	vector<unsigned>::iterator pitr = players.begin();
	for (;pitr != players.end(); ++pitr)
	{
		AddUpdate(*pitr,1000,lev);
	}
	return 0;
}

int CLogicAllServerPersonMatch::getInstPath(string &path, int lev)
{
	CheckLevel(lev);
	char buf[512] = {0};
	string instPath = Config::GetValue(ALL_SERVER_PERSON_INST_PATH);
	snprintf(buf,sizeof(buf),"%s.%d",instPath.c_str(),lev);
	path = buf;
	return 0;
}

int CLogicAllServerPersonMatch::PromotionRule(
		const PersonMatchPlayer &p1,
		const PersonMatchPlayer &p2,
		int lev,
		PersonMatchPlayer &winner)
{
	CheckLevel(lev);
	int idx = (p1.damage[0] >= p2.damage[0] ? 1 : 2);
	if(p1.damage[0] == p2.damage[0])
	{
		unsigned s1=0,s2=0;
		CLogicAllServerGuess guessData;
		guessData.GetSupportCount(p1.uid,2,lev,s1);
		guessData.GetSupportCount(p2.uid,2,lev,s2);
		idx = s1 > s2 ? 1 : 2;
		if(s1 == s2)
		{
			int temp = random() % 2;
			if(temp == 0)
			{
				if(p1.uid != 0 && p2.uid == 0)
					idx = 1;
				else
					idx = 2;
			}
			else
			{
				if(p1.uid != 0 && p2.uid == 0)
					idx = 2;
				else
					idx = 1;
			}
		}
	}
	if (1 == idx)
	{
		winner.uid = p1.uid;
		snprintf(winner.name,PERSONMATCH_NAME_LEN_MAX,"%s",p1.name);
		winner.instanceid = p1.instanceid;
	}
	else
	{
		winner.uid = p2.uid;
		snprintf(winner.name,PERSONMATCH_NAME_LEN_MAX,"%s",p2.name);
		winner.instanceid = p2.instanceid;
	}
	return 0;
}

int CLogicAllServerPersonMatch::Watch(int lev)
{
	CheckLevel(lev);
	CAllServerPersonBaseMatch *pmatch = GetAllServerPsersonBaseMatch(lev);
	if (NULL == pmatch)
	{
		DB_ERROR_RETURN_MSG("init_personmatch_fail");
	}
	return pmatch->Watch();
}
