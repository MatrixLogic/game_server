#include "LogicInc.h"
#include <iostream>

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
	vector <U_L_MAP> vecAlliances;

	string action = argv[1];
	int ret = 0;
	CDataBaseMatch match;
	ret = match.Init(Config::GetValue(CONFIG_BASE_MATCH_PATH),sem_basematch);
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
		if(ret != 0){
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
		const BaseMatchData *pdata = NULL;
		ret = match.FinishMatch(&pdata);
		if (ret != 0)
		{
			cout << "start finish fail,ret:" << ret << endl;
			return 0;
		}
		cout << "start finish ok" << endl;
		CLogicPay logicPay;
		vector<uint64_t> vecRMailAid;	// 存储预赛联盟aid用于发送预赛奖励邮件
		for (unsigned i = 0; i < pdata->numOfReg; i++)
		{
			vecRMailAid.push_back((pdata->rteams[i]).aid);
			for (unsigned j = 0; j < 5; j++)
			{
				logicPay.ChangePay((pdata->rteams[i]).rivals[j].player.uid, 0, 10, "BASEMATCH_REGULAR", 1);
			}
		}
		CDataAllianceMember dbMember;

		for (unsigned i = 0; i < 8; i++)
		{
			if ((pdata->top8[i]).aid == 0)
				continue;
			vecAlliances.push_back(make_pair((pdata->top8[i]).aid,8));
			for (unsigned j = 0; j < 5; j++)
			{
				logicPay.ChangePay((pdata->top8[i]).players[j].uid, 0, 40, "BASEMATCH_TOP8", 1);
			}
		}
		for (unsigned i = 0; i < 4; i++)
		{
			if ((pdata->top4[i]).aid == 0)
				continue;
			for(vector<U_L_MAP>::iterator itr=vecAlliances.begin(); itr != vecAlliances.end(); itr++)
			{
				if (itr->first == (pdata->top4[i]).aid)
				{
					itr->second = 4;
				}
			}
			for (unsigned j = 0; j < 5; j++)
			{
				logicPay.ChangePay((pdata->top4[i]).players[j].uid, 0, 50, "BASEMATCH_TOP4", 1);
			}
		}
		for (unsigned i = 0; i < 2; i++)
		{
			if ((pdata->top2[i]).aid == 0)
				continue;
			for(vector<U_L_MAP>::iterator itr=vecAlliances.begin(); itr != vecAlliances.end(); itr++)
			{
				if (itr->first == (pdata->top2[i]).aid)
				{
					itr->second = 2;
				}
			}
			for (unsigned j = 0; j < 5; j++)
			{
				logicPay.ChangePay((pdata->top2[i]).players[j].uid, 0, 50, "BASEMATCH_TOP2", 1);
			}
		}
		if ((pdata->champion).aid != 0)
		{
			for(vector<U_L_MAP>::iterator itr=vecAlliances.begin(); itr != vecAlliances.end(); itr++)
			{
				if (itr->first == (pdata->champion).aid)
				{
					itr->second = 1;
				}
			}
			for (unsigned j = 0; j < 5; j++)
			{
				logicPay.ChangePay((pdata->champion).players[j].uid, 0, 50, "BASEMATCH_CHAMPION", 1);
			}
		}

		CLogicGuess logicGuess;
		vector<U_U_MAP> vecGuessors;
		logicGuess.GuessorsPay(pdata->champion.aid,1);
		// 获取投中竞猜者的uid和获奖金币
		logicGuess.GuessorsUidCoins(pdata->champion.aid, vecGuessors, 1);

		for(vector<U_U_MAP>::iterator myItr = vecGuessors.begin(); myItr != vecGuessors.end(); myItr++)
		{

			//int tmpUid = myItr->first;
			string tmpCoinsStr = "";
			stringstream ss;
			ss << myItr->second;
			ss >> tmpCoinsStr;
			char str[200];
			snprintf(str,sizeof(str),config["templetGuessContent1"].c_str(), tmpCoinsStr.c_str());
			string tempString(str);
			data.text = tempString;
			data.title = config["mailGuessTitle"];

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

		// 盟主、副盟主额外奖励与参赛队员邮件通知
		for(vector<U_L_MAP>::iterator myItr=vecAlliances.begin(); myItr != vecAlliances.end(); myItr++)
		{
			if (myItr->second == 1)
			{
				vector<DataAllianceMember> members;
				dbMember.GetMembers(myItr->first, members);
				for(vector<DataAllianceMember>::iterator itr=members.begin(); itr != members.end(); itr++)
				{
					//DataAllianceMember myMember;
					//myMember = *itr;
					if ((*itr).type == AMT_LEADER)
					{
						logicPay.ChangePay((*itr).uid, 0, 200, "BASEMATCH_CHAMPION_LEADER", 1);
						// 发送邮件
						data.title = config["mailTitle"];
						char str[200];
						snprintf(str,sizeof(str),config["templetLeaderStrContent"].c_str(), "冠军", "200");
						string tempString(str);
						data.text = tempString;
						data.post_ts = Time::GetGlobalTime();
						data.read_ts = 0;
						data.from_name = "系统管理员";
						data.attach_flag = 0;
						data.uid = ADMIN_UID;
						vector<uint64_t> vecUsersUid;
						vecUsersUid.push_back((*itr).uid);
						ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
						if(0 == ret)
						{
							cout << "冠军盟主发送成功" << ret << endl;
						}
						else
						{
							error_log("champion leader prize send mail fail!");
						}
					}
					else if ((*itr).type == AMT_MANAGER)
					{
						logicPay.ChangePay((*itr).uid, 0, 100, "BASEMATCH_CHAMPION_MANAGER", 1);
						// 增加发送邮件
						data.title = config["mailTitle"];
						char str[200];
						snprintf(str,sizeof(str),config["templetLeaderStrContent"].c_str(), "冠军", "100");
						string tempString(str);
						data.text = tempString;
						data.post_ts = Time::GetGlobalTime();
						data.read_ts = 0;
						data.from_name = "系统管理员";
						data.attach_flag = 0;
						data.uid = ADMIN_UID;
						vector<uint64_t> vecUsersUid;
						vecUsersUid.push_back((*itr).uid);
						ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
						if(0 == ret)
						{
							cout << "冠军副盟主发送成功" << ret << endl;
						}
						else
						{
							error_log("champion manager prize send mail fail!");
						}
					}
					else // 非比赛成员邮件奖励通知
					{
						bool flag = true;
						for (unsigned j = 0; j < 5; j++)
						{
							if ((pdata->champion).players[j].uid == itr->uid)
							{
								flag = false;
								break;
							}
						}
						if (flag)
						{
							logicPay.ChangePay(itr->uid, 0, 20, "BASEMATCH_CHAMPION_MEMBER", 1);
							data.title = config["mailTitle"];
							char str[200];
							snprintf(str,sizeof(str),config["templetNormalContent0"].c_str(), "冠军", "20");
							string tempString(str);
							data.text = tempString;
							data.post_ts = Time::GetGlobalTime();
							data.read_ts = 0;
							data.from_name = "系统管理员";
							data.attach_flag = 0;
							data.uid = ADMIN_UID;
							vector<uint64_t> vecUsersUid;
							vecUsersUid.push_back(itr->uid);
							ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
							if(0 == ret)
							{
								cout << "冠军联盟成员奖励发送成功" << ret << endl;
							}
							else
							{
								error_log("champion member prize send mail fail!");
							}
						}
					}
				}
				//比赛队伍邮件通知
				for (unsigned j = 0; j < 5; j++)
				{
					data.title = config["mailTitle"];
					char str[200];
					snprintf(str,sizeof(str),config["templetNormalContent1"].c_str(), "冠军", "200");
					string tempString(str);
					data.text = tempString;
					data.post_ts = Time::GetGlobalTime();
					data.read_ts = 0;
					data.from_name = "系统管理员";
					data.attach_flag = 0;
					data.uid = ADMIN_UID;
					vector<uint64_t> vecUsersUid;
					vecUsersUid.push_back((pdata->champion).players[j].uid);
					ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
					if(0 == ret)
					{
						cout << "冠军邮件发送成功" << j << endl;
					}
					else
					{
						error_log("champion team prize send mail fail!");
					}
				}
				CLogicAlliance logicAlliance;
				ret = logicAlliance.ChangeResource(myItr->first, 150,150,150,150,0,150,"BASEMATCH_CHAMPION");
				if (0 != ret)
				{
					cout << "ChangeResource fail ret=" << ret << ",aid=" << myItr->first << endl;
				}
			}
			else if (myItr->second == 2)
			{
				vector<DataAllianceMember> members;
				dbMember.GetMembers(myItr->first, members);
				for(vector<DataAllianceMember>::iterator itr=members.begin(); itr != members.end(); itr++)
				{
					if ((*itr).type == AMT_LEADER)
					{
						logicPay.ChangePay((*itr).uid, 0, 150, "BASEMATCH_TOP2_LEADER", 1);
						// 增加发送邮件
						data.title = config["mailTitle"];
						char str[200];
						snprintf(str,sizeof(str),config["templetLeaderStrContent"].c_str(), "亚军", "150");
						string tempString(str);
						data.text = tempString;
						data.post_ts = Time::GetGlobalTime();
						data.read_ts = 0;
						data.from_name = "系统管理员";
						data.attach_flag = 0;
						data.uid = ADMIN_UID;
						vector<uint64_t> vecUsersUid;
						vecUsersUid.push_back((*itr).uid);
						ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
						if(0 == ret)
						{
							cout << "二强盟主邮件发送成功" << ret << endl;
						}
						else
						{
							error_log("top2 leader prize send mail fail!");
						}

					}
					else if ((*itr).type == AMT_MANAGER)
					{
						logicPay.ChangePay((*itr).uid, 0, 75, "BASEMATCH_TOP2_MANAGER", 1);
						// 增加发送邮件
						data.title = config["mailTitle"];
						char str[200];
						snprintf(str,sizeof(str),config["templetLeaderStrContent"].c_str(), "亚军", "75");
						string tempString(str);
						data.text = tempString;
						data.post_ts = Time::GetGlobalTime();
						data.read_ts = 0;
						data.from_name = "系统管理员";
						data.attach_flag = 0;
						data.uid = ADMIN_UID;
						vector<uint64_t> vecUsersUid;
						vecUsersUid.push_back((*itr).uid);
						ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
						if(0 == ret)
						{
							cout << "二强副盟主邮件发送成功" << ret << endl;
						}
						else
						{
							error_log("top2 manager prize send mail fail!");
						}
					}
					else // 非比赛成员邮件奖励通知
					{
						int tempI = 0;
						for (unsigned i = 0; i < 2; i++)
						{
							if((pdata->top2[i]).aid == myItr->first)
							{
								tempI = i;
								break;
							}
						}
						bool flag = true;
						for (unsigned j = 0; j < 5; j++)
						{
							if ((pdata->top2[tempI]).players[j].uid == itr->uid)
							{
								flag = false;
								break;
							}
						}
						if (flag)
						{
							logicPay.ChangePay(itr->uid, 0, 15, "BASEMATCH_TOP2_MEMBER", 1);
							data.title = config["mailTitle"];
							char str[200];
							snprintf(str,sizeof(str),config["templetNormalContent0"].c_str(), "亚军", "15");
							string tempString(str);
							data.text = tempString;
							data.post_ts = Time::GetGlobalTime();
							data.read_ts = 0;
							data.from_name = "系统管理员";
							data.attach_flag = 0;
							data.uid = ADMIN_UID;
							vector<uint64_t> vecUsersUid;
							vecUsersUid.push_back(itr->uid);
							ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
							if(0 == ret)
							{
								cout << "亚军联盟成员奖励发送成功" << ret << endl;
							}
							else
							{
								error_log("top2 member prize send mail fail!");
							}
						}
					}
				}
				// 发送邮件通知
				int tempI = 0;
				for (unsigned i = 0; i < 2; i++)
				{
					if((pdata->top2[i]).aid == myItr->first)
					{
						tempI = i;
						break;
					}
				}
				for (unsigned j = 0; j < 5; j++)
				{
					data.title = config["mailTitle"];
					char str[200];
					snprintf(str,sizeof(str),config["templetNormalContent1"].c_str(), "亚军", "150");
					string tempString(str);
					data.text = tempString;
					data.post_ts = Time::GetGlobalTime();
					data.read_ts = 0;
					data.from_name = "系统管理员";
					data.attach_flag = 0;
					data.uid = ADMIN_UID;
					vector<uint64_t> vecUsersUid;
					vecUsersUid.push_back((pdata->top2[tempI]).players[j].uid);
					ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
					if(0 == ret)
					{
						cout << "亚军邮件发送成功" << j << endl;
					}
					else
					{
						error_log("top2 team prize send mail fail!");
					}
				}
				CLogicAlliance logicAlliance;
				ret = logicAlliance.ChangeResource(myItr->first, 130,130,130,130,0,130,"BASEMATCH_TOP2");
				if (0 != ret)
				{
					cout << "ChangeResource fail ret=" << ret << ",aid=" << myItr->first << endl;
				}
			}
			else if (myItr->second == 4)
			{
				vector<DataAllianceMember> members;
				dbMember.GetMembers(myItr->first, members);
				for(vector<DataAllianceMember>::iterator itr=members.begin(); itr != members.end(); itr++)
				{
					if ((*itr).type == AMT_LEADER)
					{
						logicPay.ChangePay((*itr).uid, 0, 100, "BASEMATCH_TOP4_LEADER", 1);
						// 增加发送邮件
						data.title = config["mailTitle"];
						char str[200];
						snprintf(str,sizeof(str),config["templetLeaderStrContent"].c_str(), "四强", "100");
						string tempString(str);
						data.text = tempString;
						data.post_ts = Time::GetGlobalTime();
						data.read_ts = 0;
						data.from_name = "系统管理员";
						data.attach_flag = 0;
						data.uid = ADMIN_UID;
						vector<uint64_t> vecUsersUid;
						vecUsersUid.push_back((*itr).uid);
						ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
						if(0 == ret)
						{
							cout << "四强盟主邮件发送成功" << ret << endl;
						}
						else
						{
							error_log("top4 leader prize send mail fail!");
						}
					}
					else if ((*itr).type == AMT_MANAGER)
					{
						logicPay.ChangePay((*itr).uid, 0, 50, "BASEMATCH_TOP4_MANAGER", 1);
						// 增加发送邮件
						data.title = config["mailTitle"];
						char str[200];
						snprintf(str,sizeof(str),config["templetLeaderStrContent"].c_str(), "四强", "50");
						string tempString(str);
						data.text = tempString;
						data.post_ts = Time::GetGlobalTime();
						data.read_ts = 0;
						data.from_name = "系统管理员";
						data.attach_flag = 0;
						data.uid = ADMIN_UID;
						vector<uint64_t> vecUsersUid;
						vecUsersUid.push_back((*itr).uid);
						ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
						if(0 == ret)
						{
							cout << "四强副盟主邮件发送成功" << ret << endl;
						}
						else
						{
							error_log("top4 manager prize send mail fail!");
						}
					}
					else
					{
						for (unsigned i = 0; i < 4; i++)
						{
							if ((pdata->top4[i]).aid == 0)
								continue;
							if ((pdata->top4[i]).aid == myItr->first)
							{
								bool flag = true;
								for (unsigned j = 0; j < 5; j++)
								{
									if ((pdata->top4[i]).players[j].uid == itr->uid)
									{
										flag = false;
										break;
									}
								}
								if (flag)
								{
									logicPay.ChangePay(itr->uid, 0, 10, "BASEMATCH_TOP4_MEMBER", 1);
									data.title = config["mailTitle"];
									char str[200];
									snprintf(str,sizeof(str),config["templetNormalContent0"].c_str(), "四强", "10");
									string tempString(str);
									data.text = tempString;
									data.post_ts = Time::GetGlobalTime();
									data.read_ts = 0;
									data.from_name = "系统管理员";
									data.attach_flag = 0;
									data.uid = ADMIN_UID;
									vector<uint64_t> vecUsersUid;
									vecUsersUid.push_back(itr->uid);
									ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
									if(0 == ret)
									{
										cout << "四强联盟成员奖励发送成功" << ret << endl;
									}
									else
									{
										error_log("top4 member prize send mail fail!");
									}
								}
							}
						}
					}
				}
				// 发送邮件通知
				for (unsigned i = 0; i < 4; i++)
				{
					if ((pdata->top4[i]).aid == 0)
						continue;
					if ((pdata->top4[i]).aid == myItr->first)
					{
						for (unsigned j = 0; j < 5; j++)
						{
							data.title = config["mailTitle"];
							char str[200];
							snprintf(str,sizeof(str),config["templetNormalContent1"].c_str(), "四强", "100");
							string tempString(str);
							data.text = tempString;
							data.post_ts = Time::GetGlobalTime();
							data.read_ts = 0;
							data.from_name = "系统管理员";
							data.attach_flag = 0;
							data.uid = ADMIN_UID;
							vector<uint64_t> vecUsersUid;
							vecUsersUid.push_back((pdata->top4[i]).players[j].uid);
							ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
							if(0 == ret)
							{
								cout << "四强赛邮件发送成功" << j << endl;
							}
							else
							{
								error_log("top4 team prize send mail fail!");
							}
						}
					}
				}
				CLogicAlliance logicAlliance;
				ret = logicAlliance.ChangeResource(myItr->first, 100,100,100,100,0,100,"BASEMATCH_TOP4");
				if (0 != ret)
				{
					cout << "ChangeResource fail ret=" << ret << ",aid=" << myItr->first << endl;
				}
			}
			else //if (myItr->second == 8)
			{
				vector<DataAllianceMember> members;
				dbMember.GetMembers(myItr->first, members);
				for(vector<DataAllianceMember>::iterator itr=members.begin(); itr != members.end(); itr++)
				{
					if ((*itr).type == AMT_LEADER)
					{
						logicPay.ChangePay((*itr).uid, 0, 50, "BASEMATCH_TOP8_LEADER", 1);
						// 增加发送邮件
						data.title = config["mailTitle"];
						char str[200];
						snprintf(str,sizeof(str),config["templetLeaderStrContent"].c_str(), "八强", "50");
						string tempString(str);
						data.text = tempString;
						data.post_ts = Time::GetGlobalTime();
						data.read_ts = 0;
						data.from_name = "系统管理员";
						data.attach_flag = 0;
						data.uid = ADMIN_UID;
						vector<uint64_t> vecUsersUid;
						vecUsersUid.push_back((*itr).uid);
						ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
						if(0 == ret)
						{
							cout << "八强盟主发送成功" << ret << endl;
						}
						else
						{
							error_log("top8 leader prize send mail fail!");
						}

					}
					else if ((*itr).type == AMT_MANAGER)
					{
						logicPay.ChangePay((*itr).uid, 0, 25, "BASEMATCH_TOP8_MANAGER", 1);
						// 增加发送邮件
						data.title = config["mailTitle"];
						char str[200];
						snprintf(str,sizeof(str),config["templetLeaderStrContent"].c_str(), "八强", "25");
						string tempString(str);
						data.text = tempString;
						data.post_ts = Time::GetGlobalTime();
						data.read_ts = 0;
						data.from_name = "系统管理员";
						data.attach_flag = 0;
						data.uid = ADMIN_UID;
						vector<uint64_t> vecUsersUid;
						vecUsersUid.push_back((*itr).uid);
						ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
						if(0 == ret)
						{
							cout << "八强副盟主邮件发送成功" << ret << endl;
						}
						else
						{
							error_log("top8 manager prize send mail fail!");
						}
					}
					else
					{
						for (unsigned i = 0; i < 8; i++)
						{
							if ((pdata->top8[i]).aid == 0)
								continue;
							if ((pdata->top8[i]).aid == myItr->first)
							{
								bool flag = true;
								for (unsigned j = 0; j < 5; j++)
								{
									if ((pdata->top8[i]).players[j].uid == itr->uid)
									{
										flag = false;
										break;
									}
								}
								if (flag)
								{
									logicPay.ChangePay(itr->uid, 0, 5, "BASEMATCH_TOP8_MEMBER", 1);
									data.title = config["mailTitle"];
									char str[200];
									snprintf(str,sizeof(str),config["templetNormalContent0"].c_str(), "八强", "5");
									string tempString(str);
									data.text = tempString;
									data.post_ts = Time::GetGlobalTime();
									data.read_ts = 0;
									data.from_name = "系统管理员";
									data.attach_flag = 0;
									data.uid = ADMIN_UID;
									vector<uint64_t> vecUsersUid;
									vecUsersUid.push_back(itr->uid);
									ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
									if(0 == ret)
									{
										cout << "八强联盟成员奖励发送成功" << ret << endl;
									}
									else
									{
										error_log("top8 member prize send mail fail!");
									}
								}
							}
						}
					}
				}

				for (unsigned i = 0; i < 8; i++)
				{
					if ((pdata->top8[i]).aid == 0)
						continue;
					if ((pdata->top8[i]).aid == myItr->first)
					{
						for (unsigned j = 0; j < 5; j++)
						{
							data.title = config["mailTitle"];
							char str[200];
							snprintf(str,sizeof(str),config["templetNormalContent1"].c_str(), "八强", "50");
							string tempString(str);
							data.text = tempString;
							data.post_ts = Time::GetGlobalTime();
							data.read_ts = 0;
							data.from_name = "系统管理员";
							data.attach_flag = 0;
							data.uid = ADMIN_UID;
							vector<uint64_t> vecUsersUid;
							vecUsersUid.push_back((pdata->top8[i]).players[j].uid);
							ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
							if(0 == ret)
							{
								cout << "八强赛金币邮件发送成功" << j << endl;
							}
							else
							{
								error_log("top8 team prize send mail fail!");
							}
						}
					}
				}
				CLogicAlliance logicAlliance;
				ret = logicAlliance.ChangeResource(myItr->first, 80,80,80,80,0,80,"BASEMATCH_TOP8");
				if (0 != ret)
				{
					cout << "ChangeResource fail ret=" << ret << ",aid=" << myItr->first << endl;
				}
			}
		}
		//预赛邮件通知
		for (unsigned i = 0; i < 8; i++)
		{
			if ((pdata->top8[i]).aid == 0)
				continue;
			for(vector<uint64_t>::iterator itr=vecRMailAid.begin(); itr != vecRMailAid.end(); itr++)
			{
				if (*itr == (pdata->top8[i]).aid)
				{
					vecRMailAid.erase(itr);
					break;
				}
			}
		}

		for (unsigned i = 0; i < pdata->numOfReg; i++)
		{
			for(vector<uint64_t>::iterator itr=vecRMailAid.begin(); itr != vecRMailAid.end(); itr++)
			{
				//cout << "*itr11" << *itr << endl;
				if (*itr == (pdata->rteams[i]).aid)
				{
					//cout << "*itr12" << *itr << endl;
					for (unsigned j = 0; j < 5; j++)
					{
						// 发送邮件通知
						data.title = config["mailTitle"];
						char str[200];
						snprintf(str,sizeof(str),config["templetRegularContent1"].c_str(), "10");
						string tempString(str);
						data.text = tempString;
						data.post_ts = Time::GetGlobalTime();
						data.read_ts = 0;
						data.from_name = "系统管理员";
						data.attach_flag = 0;
						data.uid = ADMIN_UID;
						vector<uint64_t> vecUsersUid;
						vecUsersUid.push_back((pdata->rteams[i]).rivals[j].player.uid);
						ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
						if(0 == ret)
						{
							cout << "常规赛金币邮件发送成功" << j << endl;
						}
						else
						{
							error_log("regular prize mail send fail!");
						}
					}
					vecRMailAid.erase(itr);
					break;
				}
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
		CLogicInstance instanceDb;
		instanceDb.RemoveInstance(BASEMATCH_INST_UID);
		cout << "next turn ok" << endl;
	}
	else if(action == "ri"){
		ret = match.ResetInsts();
		if(ret == 0){
			cout << "reset inst ok" << endl;
		}
	}
	else
	{
		usage();
	}
	return 0;
}

int CDataBaseMatch::ResetInsts(){

	BaseMatchData *pdata = (BaseMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);

	cout << "team num:" << pdata->numOfApply << endl;

	for(unsigned i=0; i<pdata->numOfApply;++i){
		BaseMatchTeam* pTeam = &(pdata->teams[i]);

		for(unsigned j=0; j < pTeam->number;++j){
			unsigned uid = pTeam->players[j].uid;
			unsigned instId = pTeam->players[j].instanceid;

			Json::Value json;
			CLogicInstance logicInst;
			int ret = logicInst.GetMinInstance(uid, json);
			if(ret != 0){
				cout << "import inst:" << uid << " failed" << endl;
				continue;
			}
			json["userid"] = instId;
			Json::FastWriter writer;
			DataInstance instance;
			instance.uid = BASEMATCH_INST_UID;
			instance.id = instId;
			instance.count = 0;
			instance.damage = 0;
			instance.lastts = 0;
			instance.win = 0;
			instance.data = writer.write(json);
			logicInst.ReplaceInstance(instance);
		}
	}

	return 0;
}


