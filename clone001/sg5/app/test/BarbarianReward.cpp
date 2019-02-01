/*
 * BarbarianReward.cpp
 *
 *  Created on: 2012-12-18
 *      Author: Administrator
 */
#include "LogicInc.h"
#include <iostream>
#include <stdlib.h>
#include<stdlib.h>
#define TEAM_REWARD_PERSENT_1   1
#define TEAM_REWARD_PERSENT_2   0.9
#define TEAM_REWARD_PERSENT_3   0.8
#define TEAM_REWARD_PERSENT_4   0

#define TEAM_REWARD_LEVEL_1   1
#define TEAM_REWARD_LEVEL_2   2
#define TEAM_REWARD_LEVEL_3   3
#define TEAM_REWARD_LEVEL_4   0


void GetAllianceReward(int rank, unsigned &coins,unsigned &resource)
{
	rank++;
	if (rank == 1)
	{
		coins = 100;
		resource = 100;
	}
	else if (rank >=2 && rank <=3)
	{
		coins = 90;
		resource = 90;
	}
	else if (rank >=4 && rank <=10 )
	{
		coins = 70;
		resource = 70;
	}
	else
	{
		coins = 0;
		resource = 0;
	}

}

int main(int argc, char *argv[])
{
	int i = 0,j  = 0;
	if (!Kernel::Init(APP_CONFIG_PATH))
	{
		cout << "init kernel fail" << endl;
		return 1;
	}
	Config::SetDomain(0);
	CLogicBarbarianInvasion logicBarbarianInvasion;
	DataPlayersPoints playersRank;
	memset(&playersRank,0x00,sizeof(DataPlayersPoints));
	DataTeamItem teamRank[512];
	CLogicPay logicPay;
	CLogicAlliance logicAlliance;
	int teamNum = 0;
	int ret = logicBarbarianInvasion.GetTeamRank(teamRank,teamNum);
	if(ret != 0)
	{
		cout<<" GetTeamRank failed"<<endl;
		return 0;
	}
	double teamRewardPersent = 0;
	double rewardCash = 0;
	unsigned personPoint = 0;
	for(i = 0; i < teamNum; i++)
	{
		personPoint = 0;
		if(!IsAllianceId(teamRank[i].aid))
		{
			break;
		}
		memset(&playersRank,0x00,sizeof(playersRank));
		if(i == 0)
		{
			teamRewardPersent = TEAM_REWARD_PERSENT_1;
		}else if(i == 1 || i == 2)
		{
			teamRewardPersent = TEAM_REWARD_PERSENT_2;
		}else if(i > 2 && i <= 10)
		{
			teamRewardPersent = TEAM_REWARD_PERSENT_3;
		}else{
			teamRewardPersent = TEAM_REWARD_PERSENT_4;
		}
		ret = logicBarbarianInvasion.GetTeamPlayersRank(playersRank,teamRank[i].aid);
		if(ret != 0)
		{
			cout<<" GetTeamPlayersRank failed"<<endl;
			return 0;
		}
		for(j = 0; j < playersRank.playerNum; j++)
		{
			if (playersRank.players[j].points >= 4800)
			{
				rewardCash = 60;
				personPoint = 4000;
			}else if(playersRank.players[j].points >= 3200 && playersRank.players[j].points < 4800)
			{
				rewardCash = 40;
				personPoint = 2000;
			}else if(playersRank.players[j].points >= 2000 && playersRank.players[j].points < 3200)
			{
				rewardCash = 30;
				personPoint = 1000;
			}else if(playersRank.players[j].points >= 1000 && playersRank.players[j].points < 2000)
			{
				rewardCash = 20;
				personPoint = 500;
			}else if(playersRank.players[j].points >= 300 && playersRank.players[j].points < 1000)
			{
				rewardCash = 10;
				personPoint = 200;
			}else{
				rewardCash = 0;
				personPoint = 0;
			}

			int addCash = teamRewardPersent * rewardCash;

			unsigned TwiceTimeB = 0;
			Config::GetUIntValue(TwiceTimeB, CONFIG_DOUBLE_BARBARIAN_BEGIN);
			unsigned TwiceTimeE = 0;
			Config::GetUIntValue(TwiceTimeE, CONFIG_DOUBLE_BARBARIAN_END);
			cout << "TwiceTimeB=" << TwiceTimeB << ",TwiceTimeE=" << TwiceTimeE << endl;

			int twiceFlag = 1;
			if (Time::GetGlobalTime() >= TwiceTimeB && Time::GetGlobalTime() <= TwiceTimeE)
			{
				twiceFlag = 2;
			}

			addCash *= twiceFlag;
			unsigned uTotalPoints = (personPoint+playersRank.players[j].points) * twiceFlag;
			if(addCash < 0 )
			{
				continue;
			}
			if(addCash > 60)
			{
				cout<<"error add cash exception uid= %"<<playersRank.players[j].uid<<"addcash = "<<addCash<<endl;
			}
			if(addCash > 0)
			{
				playersRank.players[j].rewardCash = addCash;
				cout<<"aid="<<playersRank.aid<<",uid="<<playersRank.players[j].uid<<",add_cash="
				    <<addCash<<",persent="<<teamRewardPersent<<",rewardCash="<<rewardCash<<",points="
				    <<playersRank.players[j].points << ",addPoints="<<personPoint<<endl;

				ret = logicPay.ChangePay(playersRank.players[j].uid,0, addCash, "barbarianreward",1);
				if(ret != 0)
				{
					cout<<"error  ChangeCash failed uid = "<<playersRank.players[j].uid<<endl;
					return 0;
				}
			}
			ret = logicAlliance.AddPoint(playersRank.aid, playersRank.players[j].uid, uTotalPoints);
			if(ret != 0)
			{
				cout<<"error add point failed uid="<<playersRank.players[j].uid << " aid="<<playersRank.aid<<endl;
			}
			Json::Value updates;
			updates.resize(1);
			updates[(unsigned)0]["s"] = "BARBARIANREWARD";
			updates[(unsigned)0]["name"] = string(playersRank.players[j].name);
			updates[(unsigned)0]["coins"] = addCash;
			updates[(unsigned)0]["points"] = playersRank.players[j].points;
			updates[(unsigned)0]["ts"] = Time::GetGlobalTime();
			CLogicUpdates logicUpdates;
			ret = logicUpdates.AddUpdates(playersRank.players[j].uid, updates);
			if(ret != 0)
			{
				cout<<"add updates failed uid="<<playersRank.players[j].uid<<endl;
			}
		}

		unsigned allianceCoins = 0;
		unsigned ar = 0;
		GetAllianceReward(i,allianceCoins,ar);
		cout << "allianceCoins=" << allianceCoins << "ar=" << ar << endl;
		if (0 != ar || 0 != allianceCoins)
		{
			ret = logicAlliance.ChangeResource(teamRank[i].aid,ar,ar,ar,ar,0,allianceCoins,"barbarianreward");
			if (0 != ret)
			{
				cout << "ChangeResource fail,ret=" << ret << ",aid=" << teamRank[i].aid << endl;
			}
		}
	}

	return 0;
}
