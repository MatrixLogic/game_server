#include "LogicInc.h"
#include <iostream>
#include <fstream>

typedef pair<unsigned, int> U_L_MAP;
typedef pair<unsigned, unsigned> U_U_MAP;

void usage()
{
	cout << "usage: AllServerPersonMatchTool regular PlatformType" << endl
		 << "                                guess PlatformType" << endl
		 << "                                top8 PlatformType" << endl
		 << "                                top4 PlatformType" << endl
		 << "                                top2 PlatformType" << endl
		 << "                                finish PlatformType" << endl
		 << "                                next PlatformType" << endl;
}

int AddUpdate(unsigned uid, int rank)
{
	int ret = 0;
	Json::Value UpdateData = Json::Value(Json::arrayValue);
	UpdateData[0u]["ts"] = Time::GetGlobalTime();
	UpdateData[0u]["s"] = "allserverpersonmatch";
	UpdateData[0u]["rank"] = rank;
	CLogicUpdates logicUpdates;
	ret = logicUpdates.AddUpdates(uid,UpdateData);
	if (0 != ret)
	{
		cout << "AddUpdates fail,uid=" << uid << ",ret=" << ret << endl;
		return ret;
	}
	return 0;
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

	CLogicEmail matchSendEmail;
	DataEmail data;
	vector <U_L_MAP> vecTeams;

	string action = argv[1];
	int ret = 0;
	CLogicAllServerPersonMatch match;
	if (ret != 0)
	{
		cout << "init match fail,ret:" << ret << endl;
		return 0;
	}

	if (action == "regular")
	{
		ret = match.StartRegular(1);
		if (ret != 0)
		{
			cout << "start regular_1 fail,ret:" << ret << endl;
		}
		ret = match.StartRegular(2);
		if (ret != 0)
		{
			cout << "start regular_2 fail,ret:" << ret << endl;
		}
		ret = match.StartRegular(3);
		if (ret != 0)
		{
			cout << "start regular_3 fail,ret:" << ret << endl;
		}
		cout << "start regular ok" << endl;
	}
	else if(action == "guess")
	{
		ret = match.StartGuess(1);
		if (ret != 0)
		{
			cout << "start guess_1 fail,ret:" << ret << endl;
		}
		ret = match.StartGuess(2);
		if (ret != 0)
		{
			cout << "start guess_2 fail,ret:" << ret << endl;
		}
		ret = match.StartGuess(3);
		if (ret != 0)
		{
			cout << "start guess_3 fail,ret:" << ret << endl;
		}
		cout << "start guess ok" << endl;
	}
	else if (action == "top8")
	{
		ret = match.StartPlayoff8(1);
		if (ret != 0)
		{
			cout << "start top8_1 fail,ret:" << ret << endl;
		}
		ret = match.StartPlayoff8(2);
		if (ret != 0)
		{
			cout << "start top8_2 fail,ret:" << ret << endl;
		}
		ret = match.StartPlayoff8(3);
		if (ret != 0)
		{
			cout << "start top8_3 fail,ret:" << ret << endl;
		}
		cout << "start top8 ok" << endl;
	}
	else if (action == "top4")
	{
		ret = match.StartPlayoff4(1);
		if (ret != 0)
		{
			cout << "start top4_1 fail,ret:" << ret << endl;
		}
		ret = match.StartPlayoff4(2);
		if (ret != 0)
		{
			cout << "start top4_2 fail,ret:" << ret << endl;
		}
		ret = match.StartPlayoff4(3);
		if (ret != 0)
		{
			cout << "start top4_3 fail,ret:" << ret << endl;
		}
		cout << "start top4 ok" << endl;
	}
	else if (action == "top2")
	{
		ret = match.StartPlayoff2(1);
		if (ret != 0)
		{
			cout << "start top2_1 fail,ret:" << ret << endl;
		}
		ret = match.StartPlayoff2(2);
		if (ret != 0)
		{
			cout << "start top2_2 fail,ret:" << ret << endl;
		}
		ret = match.StartPlayoff2(3);
		if (ret != 0)
		{
			cout << "start top2_3 fail,ret:" << ret << endl;
		}
		cout << "start top2 ok" << endl;
	}
	else if (action == "finish")
	{
		ret = match.FinishMatch(1);
		if (ret != 0)
		{
			cout << "start finish_1 fail,ret:" << ret << endl;
		}
		ret = match.FinishMatch(2);
		if (ret != 0)
		{
			cout << "start finish_2 fail,ret:" << ret << endl;
		}
		ret = match.FinishMatch(3);
		if (ret != 0)
		{
			cout << "start finish_3 fail,ret:" << ret << endl;
		}
		cout << "start finish ok" << endl;
	}
	else if (action == "next")
	{
		ret = match.NextTurn(1);
		if (ret != 0)
		{
			cout << "next turn fail_1,ret:" << ret << endl;
		}
		ret = match.NextTurn(2);
		if (ret != 0)
		{
			cout << "next turn fail_2,ret:" << ret << endl;
		}
		ret = match.NextTurn(3);
		if (ret != 0)
		{
			cout << "next turn fail_3,ret:" << ret << endl;
		}
		cout << "start next ok" << endl;
	}
	else if(action == "watch")
	{
		cout << "--------------------------level_1-----------------------" << endl;
		ret = match.Watch(1);
		cout << "--------------------------level_2-----------------------" << endl;
		ret = match.Watch(2);
		cout << "--------------------------level_3-----------------------" << endl;
		ret = match.Watch(3);
	}
	else
	{
		usage();
	}

	return 0;
}


