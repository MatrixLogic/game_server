#include "LogicInc.h"
#include <iostream>
#include <fstream>

typedef pair<unsigned, int> U_L_MAP;
typedef pair<unsigned, unsigned> U_U_MAP;

static int extractItem(const string &configPath, map<string, string>& config)
{
	//load config
	CMarkupSTL xmlConf;
	if(!xmlConf.Load(configPath.c_str()))
	{
		error_log("[load language config fail][path=%s]", configPath.c_str());
		return -1;
	}
	if(!xmlConf.FindElem("app"))
	{
		error_log("[parse language config fail][path=%s,error=parse_config_fail]", configPath.c_str());
		return -1;
	}
	xmlConf.IntoElem();
	while(xmlConf.FindElem("param"))
	{
		string name = xmlConf.GetAttrib("name");
		string value = xmlConf.GetAttrib("value");
		config[name] = value;
	}
	return 0;
}

void usage()
{
	cout << "usage: MatchTool regular PlatformType" << endl
		 << "                 top8 PlatformType" << endl
		 << "                 top4 PlatformType" << endl
		 << "                 top2 PlatformType" << endl
		 << "                 finish PlatformType" << endl
		 << "                 next PlatformType" << endl;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		usage();
		return 0;
	}
	if (!Kernel::Init(APP_CONFIG_PATH))
	{
		cout << "init fail" << endl;
		return 0;
	}
	Config::SetDomain(0);

	int tpt = atoi(argv[2]); //获取用户昵称 需要平台信息
	PlatformType pt = static_cast<PlatformType>(tpt);
	CLogicEmail matchSendEmail;
	DataEmail data;
	vector <U_L_MAP> vecTeams;

	string action = argv[1];
	int ret = 0;
	CPersonBaseMatch match;
	ret = match.Init(Config::GetValue(CONFIG_PERSON_MATCH_PATH));
	if (ret != 0)
	{
		cout << "init match fail,ret:" << ret << endl;
		return 0;
	}

	if (action == "regular")
	{
		ret = match.StartRegular();
		if (ret != 0)
		{
			cout << "start regular fail,ret:" << ret << endl;
			return 0;
		}
		cout << "start regular ok" << endl;
	}
	else if(action == "guess"){
		ret = match.StartGuess();
		if (ret != 0)
		{
			cout << "start guess fail,ret:" << ret << endl;
			return 0;
		}
		cout << "start guess ok" << endl;
	}
	else if (action == "top8")
	{
		ret = match.StartPlayoff8();
		if (ret != 0)
		{
			cout << "start top8 fail,ret:" << ret << endl;
			return 0;
		}
		cout << "start top8 ok" << endl;
	}
	else if (action == "top4")
	{
		ret = match.StartPlayoff4();
		if (ret != 0)
		{
			cout << "start top4 fail,ret:" << ret << endl;
			return 0;
		}
		cout << "start top4 ok" << endl;
	}
	else if (action == "top2")
	{
		ret = match.StartPlayoff2();
		if (ret != 0)
		{
			cout << "start top2 fail,ret:" << ret << endl;
			return 0;
		}
		cout << "start top2 ok" << endl;
	}
	else if (action == "finish")
	{

		// 获取邮件文字信息
		map<string, string> config;
		map<string, string> mapLanguageConfig;
		string mappingPath;
		if(!Config::GetValue(mappingPath, CONFIG_PRIZEMAILTEXT_PATH))
		{
			error_log("[get config fail][config_name=%s]", CONFIG_PRIZEMAILTEXT_PATH);
		}
		CMarkupSTL xmlConf;
		if(!xmlConf.Load(mappingPath.c_str()))
		{
			error_log("[load prize_mail_text mapping fail][path=%s]", mappingPath.c_str());
		}
		if(!xmlConf.FindElem("mapping"))
		{
			error_log("[parse platform mapping fail][path=%s,error=parse_xml_fail]", mappingPath.c_str());
		}
		xmlConf.IntoElem();
		while(xmlConf.FindElem("languageItem"))
		{
			string name = xmlConf.GetAttrib("language");
			string value = xmlConf.GetAttrib("config");
			if(name.empty() || value.empty())
			{
				error_log("[invalid platform mapping][path=%s,host=%s,config=%s]",
						mappingPath.c_str(), name.c_str(), value.c_str());
			}
			else
			{
				String::ToLower(name);
				mapLanguageConfig[name] = value;
			}
		}
		string languageType = "CN";
		String::ToLower(languageType);
		map<string, string>::iterator itrMapConfig;
		itrMapConfig = mapLanguageConfig.find(languageType);
		if(itrMapConfig == mapLanguageConfig.end())
		{
			error_log("[unknow_Language_Type][languageType=%s]", languageType.c_str());
		}
		ret = extractItem(itrMapConfig->second, config);
		if (ret !=0)
		{
			error_log("load mail info text fail!");
		}

		const PersonMatchData *pdata = NULL;
		ret = match.FinishMatch(&pdata);
		if (ret != 0)
		{
			cout << "start finish fail,ret:" << ret << endl;
			return 0;
		}
		cout << "start finish ok" << endl;

		CLogicPay logicPay;
		vector<uint64_t> vecRMailUid;	// 存储参赛队未进八强的uid用于发送预赛奖励邮件

		for(unsigned i = 0; i < pdata->numOfApply;++i){
			if(pdata->mems[i].flag[0] == 1 && pdata->mems[i].flag[1] == 1 && pdata->mems[i].flag[2] == 1){
				logicPay.ChangePay(pdata->mems[i].uid,0,10,"PERSONMATCH_REGULAR",1);
				vecRMailUid.push_back(pdata->mems[i].uid);
			}
		}

		for(unsigned i = 0; i < 8; ++i){
			logicPay.ChangePay(pdata->top8[i].uid,0,90,"PERSONMATCH_REGULAR",1);
			vecTeams.push_back(make_pair(pdata->top8[i].uid, 8));
		}

		for(unsigned i = 0; i < 4; ++i){
			logicPay.ChangePay(pdata->top4[i].uid,0,100,"PERSONMATCH_REGULAR",1);
			for(vector<U_L_MAP>::iterator itr=vecTeams.begin(); itr != vecTeams.end(); itr++)
			{
				if (itr->first == pdata->top4[i].uid)
				{
					itr->second = 4;
				}
			}
		}

		for(unsigned i = 0; i < 2; ++i){
			logicPay.ChangePay(pdata->top2[i].uid,0,100,"PERSONMATCH_REGULAR",1);
			for(vector<U_L_MAP>::iterator itr=vecTeams.begin(); itr != vecTeams.end(); itr++)
			{
				if (itr->first == pdata->top2[i].uid)
				{
					itr->second = 2;
				}
			}
		}

		logicPay.ChangePay(pdata->champion.uid,0,200,"PERSONMATCH_REGULAR",1);
		for(vector<U_L_MAP>::iterator itr=vecTeams.begin(); itr != vecTeams.end(); itr++)
		{
			if (itr->first == pdata->champion.uid)
			{
				itr->second = 1;
			}
		}

		CLogicGuess logicGuess;
		vector<U_U_MAP> vecGuessors;
		logicGuess.GuessorsPay(pdata->champion.uid,2);

		// 获取投中竞猜者的uid和获奖金币
		logicGuess.GuessorsUidCoins(pdata->champion.uid, vecGuessors, 0);

		for(vector<U_U_MAP>::iterator myItr = vecGuessors.begin(); myItr != vecGuessors.end(); myItr++)
		{
			//int tmpUid = myItr->first;
			string tmpCoinsStr = "";
			stringstream ss;
			ss << myItr->second;
			ss >> tmpCoinsStr;
			data.title = config["mailGuessTitle"];
			char str[200];
			snprintf(str,sizeof(str),config["templetGuessContent2"].c_str(), tmpCoinsStr.c_str());
			string tempString(str);
			data.text = tempString;
			data.post_ts = Time::GetGlobalTime();
			data.read_ts = 0;
			data.from_name = "系统管理员";
			data.attach_flag = 0;
			data.uid = ADMIN_UID;
			vector<uint64_t> vecUsersUid;
			vecUsersUid.push_back(myItr->first);
			ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
			if(0 == ret)
			{
				cout << "竞猜金币发送成功" << ret << endl;
			}
			else
			{
				error_log("champion leader prize send mail fail!");
			}
		}

		for(vector<U_L_MAP>::iterator myItr=vecTeams.begin(); myItr != vecTeams.end(); myItr++)
		{
			if (myItr->second == 1)
			{
				data.title = config["mailTitle"];
				char str[200];
				snprintf(str,sizeof(str),config["templetNormalContent2"].c_str(), "冠军", "500");
				string tempString(str);
				data.text = tempString;
				data.post_ts = Time::GetGlobalTime();
				data.read_ts = 0;
				data.from_name = "系统管理员";
				data.attach_flag = 0;
				data.uid = ADMIN_UID;
				vector<uint64_t> vecUsersUid;
				vecUsersUid.push_back(myItr->first);
				ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
				if(0 == ret)
				{
					cout << "冠军邮件发送成功" << endl;
				}
				else
				{
					error_log("champion team prize send mail fail!");
				}
			}
			else if (myItr->second == 2)
			{
				data.title = config["mailTitle"];
				char str[200];
				snprintf(str,sizeof(str),config["templetNormalContent2"].c_str(), "亚军", "300");
				string tempString(str);
				data.text = tempString;
				data.post_ts = Time::GetGlobalTime();
				data.read_ts = 0;
				data.from_name = "系统管理员";
				data.attach_flag = 0;
				data.uid = ADMIN_UID;
				vector<uint64_t> vecUsersUid;
				vecUsersUid.push_back(myItr->first);
				ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
				if(0 == ret)
				{
					cout << "亚军邮件发送成功" << endl;
				}
				else
				{
					error_log("top2 team prize send mail fail!");
				}
			}
			else if (myItr->second == 4)
			{
				data.title = config["mailTitle"];
				char str[200];
				snprintf(str,sizeof(str),config["templetNormalContent2"].c_str(), "四强", "200");
				string tempString(str);
				data.text = tempString;
				data.post_ts = Time::GetGlobalTime();
				data.read_ts = 0;
				data.from_name = "系统管理员";
				data.attach_flag = 0;
				data.uid = ADMIN_UID;
				vector<uint64_t> vecUsersUid;
				vecUsersUid.push_back(myItr->first);
				ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
				if(0 == ret)
				{
					cout << "四强邮件发送成功" << endl;
				}
				else
				{
					error_log("top4 team prize send mail fail!");
				}
			}
			else // if (myItr->second == 8)
			{
				data.title = config["mailTitle"];
				char str[200];
				snprintf(str,sizeof(str),config["templetNormalContent2"].c_str(), "八强", "100");
				string tempString(str);
				data.text = tempString;
				data.post_ts = Time::GetGlobalTime();
				data.read_ts = 0;
				data.from_name = "系统管理员";
				data.attach_flag = 0;
				data.uid = ADMIN_UID;
				vector<uint64_t> vecUsersUid;
				vecUsersUid.push_back(myItr->first);
				ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
				if(0 == ret)
				{
					cout << "八强邮件发送成功" << endl;
				}
				else
				{
					error_log("top8 team prize send mail fail!");
				}
			}
		}

		//预赛邮件通知
		for (unsigned i = 0; i < 8; i++)
		{
			if (pdata->top8[i].uid != 0)
			{
				vector<uint64_t>::iterator itr = find(vecRMailUid.begin(), vecRMailUid.end(), pdata->top8[i].uid);
				if (itr != vecRMailUid.end())
				{
					vecRMailUid.erase(itr);
				}
			}
		}

		for(vector<uint64_t>::iterator itr=vecRMailUid.begin(); itr != vecRMailUid.end(); itr++)
		{

			data.title = config["mailTitle"];
			char str[200];
			snprintf(str,sizeof(str),config["templetRegularContent2"].c_str(), "10");
			string tempString(str);
			data.text = tempString;
			data.post_ts = Time::GetGlobalTime();
			data.read_ts = 0;
			data.from_name = "系统管理员";
			data.attach_flag = 0;
			data.uid = ADMIN_UID;
			vector<uint64_t> vecUsersUid;
			vecUsersUid.push_back(*itr);
			ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
			if(0 == ret)
			{
				cout << "常规赛金币邮件发送成功" << endl;
			}
			else
			{
				error_log("regular prize mail send fail!");
			}
		}
	}
	else if (action == "next")
	{
		ret = match.NextTurn();
		if (ret != 0)
		{
			cout << "next turn fail,ret:" << ret << endl;
			return 0;
		}
		cout << "start next ok" << endl;
	}
	else if(action == "watch"){
		ret = match.Watch();
	}
	else if(action == "setrival"){
		ret = match.SetRivals();
		if(ret == 0){
			cout << "setrival ok" << endl;
		}
		else{
			cout << "failed:" << ret << endl;
		}
	}
	else if(action == "ri"){
		ret = match.ResetInsts();
		if(ret == 0){
			cout << "reset inst ok" << endl;
		}
		else{
			cout << "failed:" << ret << endl;
		}
	}
	else{
		usage();
	}

	return 0;
}

int CPersonBaseMatch::NextTurn(){
	PersonMatchData *pdata = (PersonMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	//if (pdata->stage != PMS_MATCH_FINISH)
	//	return R_ERR_LOGIC;
	CAutoLock lock(&m_sh, true);
	memset(pdata,0,sizeof(*pdata));
	pdata->stage = PMS_APPLY;
	time_t nt = time(NULL) + (8 * 3600);
	struct tm* pTm = gmtime(&nt);
	unsigned offset = pTm->tm_wday == 1 ? 0 : 5000;
	pdata->instance_ndx = PERSONMATCH_INST_ID_MIN + offset;

	string instPath;
	getInstPath(instPath);
	ifstream ifile(instPath.c_str(),ios::in);
	if(!ifile){
		cout << "open inst file failed:" << instPath << endl;
		return R_ERR_LOGIC;
	}
	
	char szTmp[100];
	int i = 0;
	int ret = 0;
	while(ifile.getline(szTmp,sizeof(szTmp))){

		vector<string> rlt;
		String::Split(szTmp,' ',rlt);
		//CLogicUser logicUser;
		//DataUser user;
		//ret = logicUser.GetUserLimit(atoi(rlt[0]), user);
		int uid = atoi(rlt[0].c_str());
		CLogicUserBasic logicUserBasic;
		DataUserBasic userBasic;

		pdata->rival[i].uid = uid;
		if(!IsNPCUid(uid))
		{
			ret = logicUserBasic.GetUserBasicLimitSmart(uid, OpenPlatform::GetType(), userBasic);

			if (ret != 0){
				return ret;
			}
			snprintf(pdata->rival[i].name,PERSONMATCH_NAME_LEN_MAX,"%s",userBasic.name.c_str());
		}

		//snprintf(pdata->rival[i].name,PERSONMATCH_NAME_LEN_MAX,"随机%d",i);

		if(rlt.size() >= 2){
			pdata->rival[i].instanceid = atoi(rlt[1].c_str());
		}
		else{
			Json::Value json;
			CLogicInstance logicInst;
			if(IsNPCUid(uid))
			{
				CLogicNpc logicNpc;
				ret = logicNpc.Load(uid, uid, json);
				if (ret != 0)
					return ret;
				json["name"] = "npc";
				string npcName = "先锋";
				string npcName2 = "号";
				snprintf(pdata->rival[i].name,PERSONMATCH_NAME_LEN_MAX,"%s%d%s",npcName.c_str(), i, npcName2.c_str());
			//	json["pic"] = "npc";
			}
			else
			{
				ret = logicInst.GetMinInstance(uid, json);
				if (ret != 0)
					return ret;
				json["name"] = userBasic.name;
				json["pic"] = userBasic.figure_url;
			}

			int instanceId = pdata->instance_ndx++;
			json["userid"] = instanceId;
			CLogicMatchInstance logicMatchInstance;
			ret = logicMatchInstance.SetLocalMatchInstance(instanceId,json);
			if (0 != ret)
			{
				cout << "SetLocalMatchInstance fail,ret=" << ret << ",instanceId=" << instanceId << endl;
			}
			/*Json::FastWriter writer;
			DataInstance instance;
			instance.uid = PERSONMATCH_INST_UID;
			instance.id = instanceId;
			instance.count = 0;
			instance.damage = 0;
			instance.lastts = 0;
			instance.win = 0;
			instance.data = writer.write(json);
			logicInst.ReplaceInstance(instance);*/

			pdata->rival[i].instanceid = instanceId;
		}
		
		++i;
		if(i >= 8){
			break;
		}
	}
	
	return 0;
}

int CPersonBaseMatch::StartRegular(){

	PersonMatchData *pdata = (PersonMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	if (pdata->stage != PMS_APPLY)
		return R_ERR_LOGIC;

	memset(pdata->top8,0,sizeof(pdata->top8));
	memset(pdata->top4,0,sizeof(pdata->top4));
	memset(pdata->top2,0,sizeof(pdata->top2));
	memset(&(pdata->champion),0,sizeof(pdata->champion));

	pdata->stage = PMS_REGULAR;

	static int arr[21][3] = {{0,1,2},{0,1,3},{0,1,4},{0,1,5},{0,1,6},{0,1,7},
		{1,2,3},{1,2,4},{1,2,5},{1,2,6},{1,2,7},
		{2,3,4},{2,3,5},{2,3,6},{2,3,7},
		{3,4,5},{3,4,6},{3,4,7},
		{4,5,6},{4,5,7},
		{5,6,7}};
	
	srandom(time(NULL));

	for(unsigned i=0;i<pdata->numOfApply;++i){
		int idx = random() % 21;
		pdata->mems[i].rivals[0] = arr[idx][0];
		pdata->mems[i].rivals[1] = arr[idx][1];
		pdata->mems[i].rivals[2] = arr[idx][2];
	}

	return 0;
}

int CPersonBaseMatch::SetRivals(){
		PersonMatchData *pdata = (PersonMatchData *)m_sh.GetAddress();
		if(pdata == NULL)
		{
			error_log("[GetAddress fail][]");
			return R_ERR_DB;
		}

		string instPath;
		getInstPath(instPath);
		ifstream ifile(instPath.c_str(),ios::in);
		if(!ifile){
			cout << "open inst file failed:" << instPath << endl;
			return R_ERR_LOGIC;
		}

		char szTmp[100];
		int i = 0;
		int ret = 0;
		while(ifile.getline(szTmp,sizeof(szTmp))){
			vector<string> rlt;
			String::Split(szTmp,' ',rlt);
			//CLogicUser logicUser;
			//DataUser user;
			//ret = logicUser.GetUserLimit(atoi(rlt[0]), user);
			int uid = atoi(rlt[0].c_str());
			CLogicUserBasic logicUserBasic;
			DataUserBasic userBasic;

			pdata->rival[i].uid = uid;
			if(!IsNPCUid(uid))
			{
				ret = logicUserBasic.GetUserBasicLimitSmart(uid, OpenPlatform::GetType(), userBasic);

				if (ret != 0){
					return ret;
				}
				snprintf(pdata->rival[i].name,PERSONMATCH_NAME_LEN_MAX,"%s",userBasic.name.c_str());
			}
			if(rlt.size() >= 2){
				pdata->rival[i].instanceid = atoi(rlt[1].c_str());
			}
			else{
				Json::Value json;
				CLogicInstance logicInst;

				if(IsNPCUid(uid))
				{
					CLogicNpc logicNpc;
					ret = logicNpc.Load(uid, uid, json);
					if (ret != 0)
						return ret;
					json["name"] = "npc";
					string npcName = "先锋";
					string npcName2 = "号";
					snprintf(pdata->rival[i].name,PERSONMATCH_NAME_LEN_MAX,"%s%d%s",npcName.c_str(),i,npcName2.c_str());
				}
				else
				{
					ret = logicInst.GetMinInstance(uid, json);
					if (ret != 0)
						return ret;
					json["name"] = userBasic.name;
					json["pic"] = userBasic.figure_url;
				}

				int instanceId = pdata->instance_ndx++;
				json["userid"] = instanceId;
				CLogicMatchInstance logicMatchInstance;
				ret = logicMatchInstance.SetLocalMatchInstance(instanceId,json);
				if (0 != ret)
				{
					cout << "SetLocalMatchInstance fail,ret=" << ret << ",instanceId=" << instanceId << endl;
				}
				/*Json::FastWriter writer;
				DataInstance instance;
				instance.uid = PERSONMATCH_INST_UID;
				instance.id = instanceId;
				instance.count = 0;
				instance.damage = 0;
				instance.lastts = 0;
				instance.win = 0;
				instance.data = writer.write(json);
				logicInst.ReplaceInstance(instance);*/

				pdata->rival[i].instanceid = instanceId;
			}

			++i;
			if(i >= 8){
				break;
			}
		}

		cout << "set rivals success" << endl;

		return 0;
}

static bool PersonCompare(const PersonMatchPlayer &left, const PersonMatchPlayer &right)
{
	return (left.damage[0] + left.damage[1] + left.damage[2]) > 
		   (right.damage[0] + right.damage[1] + right.damage[2] );
}

int CPersonBaseMatch::setPersonInst(int uid,int instId){
	int ret;
	CLogicUserBasic logicUserBasic;
	DataUserBasic userBasic;
	ret = logicUserBasic.GetUserBasicLimitSmart(uid, OpenPlatform::GetType(), userBasic);
	Json::Value json;
	CLogicInstance logicInst;
	ret = logicInst.GetMinInstance(uid, json);
	if (ret != 0)
		return ret;
	json["name"] = userBasic.name;
	json["pic"] = userBasic.figure_url;

	int instanceId = instId;
	json["userid"] = instanceId;
	Json::FastWriter writer;
	DataInstance instance;
	instance.uid = PERSONMATCH_INST_UID;
	instance.id = instanceId;
	instance.count = 0;
	instance.damage = 0;
	instance.lastts = 0;
	instance.win = 0;
	instance.data = writer.write(json);
	logicInst.ReplaceInstance(instance);

	return 0;
}

#define SET_TOP8_POS(idx,ddx) \
	pdata->top8[idx].uid = personVector[ddx].uid;\
	snprintf(pdata->top8[idx].name,PERSONMATCH_NAME_LEN_MAX,"%s",personVector[ddx].name);\
	pdata->top8[idx].damage[1] = personVector[ddx].damage[0] + personVector[ddx].damage[1] + personVector[ddx].damage[2];\
	pdata->top8[idx].instanceid = personVector[ddx].instanceid;

//setPersonInst(personVector[ddx].uid,pdata->top8[idx].instanceid);
	

int CPersonBaseMatch::StartGuess(){
	PersonMatchData *pdata = (PersonMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}

	if (pdata->stage != PMS_REGULAR)
		return R_ERR_LOGIC;

	CAutoLock lock(&m_sh, true);
	pdata->stage = PMS_GUESS;

	vector<PersonMatchPlayer> personVector;

	for(unsigned i = 0; i < (pdata->numOfApply < 8 ? 8:pdata->numOfApply); ++i){
		personVector.push_back(pdata->mems[i]);
	}

	sort(personVector.begin(),personVector.end(),PersonCompare);

	string instPath;
	getInstPath(instPath);
	ofstream ofile(instPath.c_str(),ios::trunc);
	for(int i = 0; i<8;++i){
		if (personVector[i].uid)
			ofile << personVector[i].uid << " " << personVector[i].instanceid
					<< endl;
		else
			ofile << 91 + i << endl;
		cout << personVector[i].uid << "," << personVector[i].name << "," << personVector[i].damage[0] + personVector[i].damage[1] + personVector[i].damage[2] << endl;
	}
	ofile.close();

	memset(pdata->top8,0,sizeof(pdata->top8));
	memset(pdata->top4,0,sizeof(pdata->top4));
	memset(pdata->top2,0,sizeof(pdata->top2));
	memset(&(pdata->champion),0,sizeof(pdata->champion));

	SET_TOP8_POS(0,0)
	SET_TOP8_POS(1,7)

	SET_TOP8_POS(2,3)
	SET_TOP8_POS(3,4)

	SET_TOP8_POS(4,1)
	SET_TOP8_POS(5,6)

	SET_TOP8_POS(6,2)
	SET_TOP8_POS(7,5)

	CGuessData guessData;
	if(guessData.Init(Config::GetValue(GUESS_DATA_PATH),2) != 0){
		cerr << "guess data init fail" << endl;
		return R_ERR_LOGIC;
	}
	GuessViewData tmpData;
	vector<GuessViewData> dataVec;
	for(int i = 0; i < 8; ++i){
		tmpData.gid = personVector[i].uid;
		snprintf(tmpData.gname,sizeof(tmpData.gname),"%s",personVector[i].name);
		dataVec.push_back(tmpData);
	}
	guessData.SetInitValues(dataVec);

	return 0;
}

int CPersonBaseMatch::StartPlayoff8()
{
	PersonMatchData *pdata = (PersonMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}

	if (pdata->stage != PMS_GUESS)
		return R_ERR_LOGIC;

	CAutoLock lock(&m_sh, true);
	pdata->stage = PMS_PLAYOFF8;

	return 0;

	vector<PersonMatchPlayer> personVector;
	
	for(unsigned i = 0; i < (pdata->numOfApply < 8 ? 8:pdata->numOfApply); ++i){
		personVector.push_back(pdata->mems[i]);
	}

	sort(personVector.begin(),personVector.end(),PersonCompare);

	string instPath;
	getInstPath(instPath);
	ofstream ofile(instPath.c_str(),ios::trunc);
	for(int i = 0; i<8;++i){
		if (personVector[i].uid)
			ofile << personVector[i].uid << " " << personVector[i].instanceid
					<< endl;
		else
			ofile << 91 + i << endl;
		cout << personVector[i].uid << "," << personVector[i].name << "," << personVector[i].damage[0] + personVector[i].damage[1] + personVector[i].damage[2] << endl;
	}
	ofile.close();
	
	memset(pdata->top8,0,sizeof(pdata->top8));
	memset(pdata->top4,0,sizeof(pdata->top4));
	memset(pdata->top2,0,sizeof(pdata->top2));
	memset(&(pdata->champion),0,sizeof(pdata->champion));

	SET_TOP8_POS(0,0)
	SET_TOP8_POS(1,7)

	SET_TOP8_POS(2,3)
	SET_TOP8_POS(3,4)

	SET_TOP8_POS(4,1)
	SET_TOP8_POS(5,6)

	SET_TOP8_POS(6,2)
	SET_TOP8_POS(7,5)

	CGuessData guessData;
	if(guessData.Init(Config::GetValue(GUESS_DATA_PATH),2) != 0){
		cerr << "guess data init fail" << endl;
		return R_ERR_LOGIC;
	}
	GuessViewData tmpData;
	vector<GuessViewData> dataVec;
	for(int i = 0; i < 8; ++i){
		tmpData.gid = personVector[i].uid;
		snprintf(tmpData.gname,sizeof(tmpData.gname),"%s",personVector[i].name);
		dataVec.push_back(tmpData);
	}
	guessData.SetInitValues(dataVec);

	return 0;
}

int CPersonBaseMatch::ResetInsts()
{
	PersonMatchData *pdata = (PersonMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}

	CAutoLock lock(&m_sh, true);

	for(unsigned i=0;i<8;++i){
		unsigned uid = pdata->rival[i].uid;
		unsigned instId = pdata->rival[i].instanceid;

		cout << "instid=" << instId << endl;

		Json::Value json;
		CLogicInstance logicInst;
		int ret = logicInst.GetMinInstance(uid, json);
		if(ret != 0){
			cout << "import inst:" << uid << " failed" << endl;
			continue;
		}
		json["userid"] = instId;
		CLogicMatchInstance logicMatchInstance;
		logicMatchInstance.SetLocalMatchInstance(instId,json);

		/*Json::FastWriter writer;
		DataInstance instance;
		instance.uid = PERSONMATCH_INST_UID;
		instance.id = instId;
		instance.count = 0;
		instance.damage = 0;
		instance.lastts = 0;
		instance.win = 0;
		instance.data = writer.write(json);
		logicInst.ReplaceInstance(instance);*/
	}

	return 0;
}

