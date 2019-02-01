#include "LogicPersonMatch.h"


CPersonBaseMatch* CLogicPersonMatch::GetPsersonBaseMatch()
{
	GET_MEM_DATA_SEM(CPersonBaseMatch, CONFIG_PERSON_MATCH_PATH, sem_personmatch)
	/*static CPersonBaseMatch* pMatch = NULL;

	if(!pMatch){
		pMatch = new CPersonBaseMatch();
		if(pMatch->Init(Config::GetValue(CONFIG_PERSON_MATCH_PATH)) != 0){
			error_log("CPersonBaseMatch init failed");
			delete pMatch;
			pMatch = NULL;
		}
	}

	return pMatch;*/
}

int CLogicPersonMatch::GetBaseMatchInfo(unsigned uid, Json::Value &data){
	CPersonBaseMatch *pmatch = GetPsersonBaseMatch();
	if (!pmatch)
	{
		DB_ERROR_RETURN_MSG("init_personmatch_fail");
	}
	
	PersonMatchPlayer person;
	vector<PersonMatchPlayer> playoff;
	int stage;
	int ret;
	
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

int CLogicPersonMatch::Apply(unsigned uid){
	CPersonBaseMatch *pmatch = GetPsersonBaseMatch();
	if (!pmatch)
	{
		DB_ERROR_RETURN_MSG("init_personmatch_fail");
	}

	DataUserBasic userBasic;
	CLogicUserBasic logicUserBasic;
	//rein[取消新人王比赛]
	// add by colin
	/*time_t nt = time(NULL) + (8 * 3600);
	struct tm* pTm = gmtime(&nt);
	if (pTm->tm_wday == 1)
	{
		DataUser user;
		CLogicUser logicUser;
		int ret = logicUser.GetUser(uid, user);
		if (ret == 0)
		{
			if (user.level > CONFIG_LEVEL_RESTRICT)
			{
				error_log("person match apply fail, level exceed! uid = %d", uid);
				return -1;
			}
		}
		else
		{
			return ret;
		}

	}*/
	int ret = logicUserBasic.GetUserBasicLimit(uid, OpenPlatform::GetType(), userBasic);
	if (ret != 0)
		return ret;

	unsigned instanceid;
	ret = pmatch->Apply(uid, userBasic.name, instanceid);
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
	ret = logicMatchInstance.SetLocalMatchInstance(instanceid, json);
	if (0 != ret)
	{
		return ret;
	}

	/*Json::FastWriter writer;
	DataInstance instance;
	instance.uid = PERSONMATCH_INST_UID;
	instance.id = instanceid;
	instance.count = 0;
	instance.damage = 0;
	instance.lastts = 0;
	instance.win = 0;
	instance.data = writer.write(json);
	logicInst.ReplaceInstance(instance);*/

	return 0;
}

int CLogicPersonMatch::ReportResult(unsigned uid, int order, int damage){
	CPersonBaseMatch *pmatch = GetPsersonBaseMatch();
	if (!pmatch)
	{
		DB_ERROR_RETURN_MSG("init_personmatch_fail");
	}

	return pmatch->ReportResult(uid,order,damage);
}

int CLogicPersonMatch::Load(unsigned instid, unsigned uidBy, Json::Value &result){
	CLogicMatchInstance logicMatchInstance;
	int ret = logicMatchInstance.GetLocalMatchInstance(instid,result);
	if (ret != 0)
	{
		error_log("[load match instance fail][ret=%d,instid=%u]",ret,instid);
		return ret;
	}
	result["userid"] = instid;
	return 0;
	/*DataInstance data;
	CDataInstance instanceDb;
	int ret = instanceDb.GetInstance(PERSONMATCH_INST_UID, instid, data);
	if (ret != 0)
	{
		error_log("[GetInstance fail][uid=%u,instid=%u]", uidBy,instid);
		DB_ERROR_RETURN_MSG("get_personmatchinst_fail");
	}
	Json::Reader reader;
	if (!reader.parse(data.data, result))
	{
		error_log("[parse instance fail][uid=%u,instid=%u]", uidBy,instid);
		DB_ERROR_RETURN_MSG("parse_instance_fail");
	}
	result["userid"] = instid;
	return 0;*/
}

int CLogicPersonMatch::GetStage(int& stage){
	CPersonBaseMatch *pmatch = GetPsersonBaseMatch();
	if (!pmatch)
	{
		DB_ERROR_RETURN_MSG("init_personmatch_fail");
	}

	return pmatch->GetStage(stage);
}
	
