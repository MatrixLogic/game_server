#include "LogicInc.h"
#include <iostream>

typedef pair<unsigned, int> U_L_MAP;
typedef pair<unsigned, unsigned> U_U_MAP;

void EmailPlaye(int rank,unsigned uid, PlatformType pt)
{
	if (1 < rank || rank > 4)
	{
		return;
	}
	const string strRank[4] =
	{
		"因为您的杰出表现，您的联盟在跨服争霸的联盟主城赛中获得冠军，",
		"因为您的杰出表现，您的联盟在跨服争霸的联盟主城赛中获得亚军，",
		"因为您的杰出表现，您的联盟在跨服争霸的联盟主城赛中进入四强，",
		"因为您的杰出表现，您的联盟在跨服争霸的联盟主城赛中进入八强，"
	};
	const string strReward[4] =
	{
		"特奖励20000联盟贡献。",
		"特奖励12000联盟贡献。",
		"特奖励8000联盟贡献。",
		"特奖励5000联盟贡献。"
	};
	DataEmail data;
	CLogicEmail matchSendEmail;
	data.text = strRank[rank - 1] + strReward[rank - 1];
	data.title = "跨服争霸";
	data.post_ts = Time::GetGlobalTime();
	data.read_ts = 0;
	data.from_name = "系统管理员";
	data.attach_flag = 0;
	data.uid = ADMIN_UID;
	vector<uint64_t> vecUsersUid;
	vecUsersUid.push_back(uid);
	int ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
	if(0 != ret)
	{
		cout << "AddEmail faile." << ret << endl;
	}
	return;
}

void EmailLeader(int rank,unsigned uid, AllianceMemberType type, PlatformType pt)
{
	if (1 < rank || rank > 4)
	{
		return;
	}
	const string LeaderContent[4] =
	{
		"在您的带领下，您的联盟在跨服争霸的联盟主城赛中获得冠军，",
		"在您的带领下，您的联盟在跨服争霸的联盟主城赛中获得亚军，",
		"在您的带领下，您的联盟在跨服争霸的联盟主城赛中进入四强，",
		"在您的带领下，您的联盟在跨服争霸的联盟主城赛中进入八强，"
	};
	const string strLeaderReward[4] =
	{
		"特奖励500金币。",
		"特奖励300进步。",
		"特奖励200金币。",
		"特奖励150金币。"
	};
	const string strManagerReward[4] =
	{
		"特奖励300金币。",
		"特奖励200进步。",
		"特奖励100金币。",
		"特奖励50金币。"
	};

	DataEmail data;
	CLogicEmail matchSendEmail;
	if (type == AMT_LEADER)
	{
		data.text = LeaderContent[rank - 1] + strLeaderReward[rank - 1];
	}
	else
	{
		data.text = LeaderContent[rank - 1] + strManagerReward[rank - 1];
	}
	data.title = "跨服争霸";
	data.post_ts = Time::GetGlobalTime();
	data.read_ts = 0;
	data.from_name = "系统管理员";
	data.attach_flag = 0;
	data.uid = ADMIN_UID;
	vector<uint64_t> vecUsersUid;
	vecUsersUid.push_back(uid);
	int ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
	if(0 != ret)
	{
		cout << "AddEmail faile." << ret << endl;
	}
	return;
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
	ret = match.Init(Config::GetValue(CONFIG_ALL_SERVER_BASE_MATCH_PATH),sem_basematchallserver);
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
	else if(action == "guess")
	{
		ret = match.StartGuess(true);
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
		ret = match.StartPlayoff4(true);
		if (ret != 0)
		{
			cout << "start top4 fail,ret:" << ret << endl;
			return 0;
		}
		cout << "start top4 ok" << endl;
	}
	else if (action == "top2")
	{
		ret = match.StartPlayoff2(true);
		if (ret != 0)
		{
			cout << "start top2 fail,ret:" << ret << endl;
			return 0;
		}
		cout << "start top2 ok" << endl;
	}
	else if (action == "finish")
	{
		const BaseMatchData *pdata = NULL;
		ret = match.FinishMatch(&pdata,true);
		if (ret != 0)
		{
			cout << "start finish fail,ret:" << ret << endl;
			return 0;
		}
		cout << "start finish ok" << endl;
		CLogicPay logicPay;
		CLogicAlliance logicAlliance;
		for (unsigned i = 0; i < pdata->numOfReg; i++)
		{
			for (unsigned j = 0; j < 5; j++)
			{
				logicPay.ChangePay((pdata->rteams[i]).rivals[j].player.uid, 0, 10, "ALL_SERVER_BASEMATCH_REGULAR", 1);
			}
		}
		CDataAllianceMember dbMember;

		for (unsigned i = 0; i < 8; i++)
		{
			if ((pdata->top8[i]).aid == 0)
				continue;
			vecAlliances.push_back(make_pair((pdata->top8[i]).aid,8));
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
		}

		CLogicAllServerGuess logicGuess;
		vector<U_U_MAP> vecGuessors;
		logicGuess.GuessorsPay(pdata->champion.aid,1);
		// 获取投中竞猜者的uid和获奖金币
		logicGuess.GuessorsUidCoins(pdata->champion.aid, vecGuessors, 1,0);
		for(vector<U_U_MAP>::iterator myItr = vecGuessors.begin(); myItr != vecGuessors.end(); myItr++)
		{
			string tempString("您在跨服联盟主城赛中，竞猜投注获得");
			string strCoins;
			String::Format(strCoins, "%u", myItr->second);
			tempString.append(strCoins).append("金币");
			data.text = tempString;
			data.title = "武林大会竞猜";

			data.post_ts = Time::GetGlobalTime();
			data.read_ts = 0;
			data.from_name = "系统管理员";
			data.attach_flag = 0;
			data.uid = ADMIN_UID;
			vector<uint64_t> vecUsersUid;
			vecUsersUid.push_back(myItr->first);
			ret = matchSendEmail.AddEmail(data, vecUsersUid, pt);
			if(0 != ret)
			{
				cout << "champion leader prize send mail fail!ret=" << ret << ",uid=" << myItr->first << endl;
			}
		}

		for(vector<U_L_MAP>::iterator myItr=vecAlliances.begin(); myItr != vecAlliances.end(); myItr++)
		{
			if (myItr->second == 1)
			{
				for (unsigned j = 0; j < 5; j++)
				{
					ret = logicAlliance.AddPoint((pdata->champion).aid,(pdata->champion).players[j].uid,20000);
					if (0 != ret)
					{
						cout << "AddPoint fail,ret=" << ret
							<< ",aid=" << (pdata->champion).aid
							<< ",uid=" << (pdata->champion).players[j].uid << endl;
					}
					else
					{
						EmailPlaye(1,(pdata->champion).players[j].uid,pt);
					}
				}
				vector<DataAllianceMember> members;
				dbMember.GetMembers(myItr->first, members);
				for(vector<DataAllianceMember>::iterator itr=members.begin(); itr != members.end(); itr++)
				{
					if ((*itr).type == AMT_LEADER)
					{
						ret = logicPay.ChangePay((*itr).uid, 0, 500, "ALL_BASEMATCH_CHAMPION_LEADER", 1);
						if (0 == ret)
						{
							EmailLeader(1,itr->uid,AMT_LEADER,pt);
						}
					}
					else if ((*itr).type == AMT_MANAGER)
					{
						ret = logicPay.ChangePay((*itr).uid, 0, 300, "ALL_BASEMATCH_CHAMPION_MANAGER", 1);
						if (0 == ret)
						{
							EmailLeader(1,itr->uid,AMT_MANAGER,pt);
						}
					}
				}
				ret = logicAlliance.ChangeResource(myItr->first, 1000,1000,1000,1000,0,1000,"ALL_BASEMATCH_CHAMPION");
				if (0 != ret)
				{
					cout << "ChangeResource fail ret=" << ret << ",aid=" << myItr->first << endl;
				}
			}
			else if (myItr->second == 2)
			{
				for (unsigned i=0;i<2;i++)
				{
					if (pdata->top2[i].aid == myItr->first)
					{
						for (unsigned j = 0; j < 5; j++)
						{
							ret = logicAlliance.AddPoint((pdata->top2[i]).aid,(pdata->top2[i]).players[j].uid,12000);
							if (0 != ret)
							{
								cout << "AddPoint fail,ret=" << ret << ",aid=" << (pdata->top2[i]).aid
										<< ",uid=" << (pdata->top2[i]).players[j].uid << endl;
							}
							else
							{
								EmailPlaye(2,(pdata->top2[i]).players[j].uid,pt);
							}
						}
					}
				}
				vector<DataAllianceMember> members;
				dbMember.GetMembers(myItr->first, members);
				for(vector<DataAllianceMember>::iterator itr=members.begin(); itr != members.end(); itr++)
				{
					if ((*itr).type == AMT_LEADER)
					{
						ret = logicPay.ChangePay((*itr).uid, 0, 300, "ALL_BASEMATCH_TOP2_LEADER", 1);
						if (0 == ret)
						{
							EmailLeader(2,itr->uid,AMT_LEADER,pt);
						}
					}
					else if ((*itr).type == AMT_MANAGER)
					{
						ret = logicPay.ChangePay((*itr).uid, 0, 200, "ALL_BASEMATCH_TOP2_MANAGER", 1);
						if (0 == ret)
						{
							EmailLeader(2,itr->uid,AMT_MANAGER,pt);
						}
					}
				}
				ret = logicAlliance.ChangeResource(myItr->first, 800,800,800,800,0,800,"ALL_BASEMATCH_TOP2");
				if (0 != ret)
				{
					cout << "ChangeResource fail ret=" << ret << ",aid=" << myItr->first << endl;
				}
			}
			else if (myItr->second == 4)
			{
				for (unsigned i=0; i<4; i++)
				{
					if (myItr->first == pdata->top4[i].aid)
					{
						for (unsigned j = 0; j < 5; j++)
						{
							ret = logicAlliance.AddPoint((pdata->top4[i]).aid,(pdata->top4[i]).players[j].uid,8000);
							if (0 != ret)
							{
								cout << "AddPoint fail,ret=" << ret
									<< ",aid=" << (pdata->top4[i]).aid
									<< ",uid=" << (pdata->top4[i]).players[j].uid << endl;
							}
							else
							{
								EmailPlaye(3,(pdata->top4[i]).players[j].uid,pt);
							}
						}
					}
				}

				vector<DataAllianceMember> members;
				dbMember.GetMembers(myItr->first, members);
				for(vector<DataAllianceMember>::iterator itr=members.begin(); itr != members.end(); itr++)
				{
					if ((*itr).type == AMT_LEADER)
					{
						ret = logicPay.ChangePay((*itr).uid, 0, 200, "ALL_BASEMATCH_TOP4_LEADER", 1);
						if (0 == ret)
						{
							EmailLeader(3,itr->uid,AMT_LEADER,pt);
						}
					}
					else if ((*itr).type == AMT_MANAGER)
					{
						ret = logicPay.ChangePay((*itr).uid, 0, 100, "ALL_BASEMATCH_TOP4_MANAGER", 1);
						if (0 == ret)
						{
							EmailLeader(3,itr->uid,AMT_MANAGER,pt);
						}
					}
				}
				ret = logicAlliance.ChangeResource(myItr->first, 500,500,500,500,0,500,"ALL_BASEMATCH_TOP4");
				if (0 != ret)
				{
					cout << "ChangeResource fail ret=" << ret << ",aid=" << myItr->first << endl;
				}
			}
			else
			{
				for (unsigned i=0; i<8; i++)
				{
					if (myItr->first == pdata->top8[i].aid)
					{
						for (unsigned j = 0; j < 5; j++)
						{
							ret = logicAlliance.AddPoint((pdata->top8[i]).aid,(pdata->top8[i]).players[j].uid,5000);
							if (0 != ret)
							{
								cout << "AddPoint fail,ret=" << ret
									<< ",aid=" << (pdata->top8[i]).aid
									<< ",uid=" << (pdata->top8[i]).players[j].uid << endl;
							}
							else
							{
								EmailPlaye(4,(pdata->top8[i]).players[j].uid,pt);
							}
						}
					}
				}

				vector<DataAllianceMember> members;
				dbMember.GetMembers(myItr->first, members);
				for(vector<DataAllianceMember>::iterator itr=members.begin(); itr != members.end(); itr++)
				{
					if ((*itr).type == AMT_LEADER)
					{
						ret = logicPay.ChangePay((*itr).uid, 0, 150, "ALL_BASEMATCH_TOP8_LEADER", 1);
						if (0 == ret)
						{
							EmailLeader(4,itr->uid,AMT_LEADER,pt);
						}
					}
					else if ((*itr).type == AMT_MANAGER)
					{
						ret = logicPay.ChangePay((*itr).uid, 0, 50, "ALL_BASEMATCH_TOP8_MANAGER", 1);
						if (0 == ret)
						{
							EmailLeader(4,itr->uid,AMT_MANAGER,pt);
						}
					}
				}
				ret = logicAlliance.ChangeResource(myItr->first, 300,300,300,300,0,300,"ALL_BASEMATCH_TOP8");
				if (0 != ret)
				{
					cout << "ChangeResource fail ret=" << ret << ",aid=" << myItr->first << endl;
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
			CLogicMatchInstance logicMatchInstance;
			ret = logicMatchInstance.SetAllServerMatchInstance(instId,json);
			if (0 != ret)
			{
				cout << "SetAllServerMatchInstance fail,ret=" << ret <<  endl;
			}
		}
	}

	return 0;
}


