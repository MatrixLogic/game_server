/*
 * JiangReward.cpp
 *
 *  Created on: 2013-2-4
 *      Author: Administrator
 */

#include "LogicInc.h"
#include <iostream>
#include <fstream>
int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		cout<<"usage:"<<argv[0]<<" pathalliance"<<endl;
		return 0;
	}
	if (!Kernel::Init(APP_CONFIG_PATH))
	{
		cout << "init fail" << endl;
		return 0;
	}
	Config::SetDomain(0);
	string sdata;
	string pathalliance = string(argv[1]);

	Json::Reader reader;
	Json::Value allianceRank;
	int result = File::Read(pathalliance, sdata);
	if(result != 0)
	{
		cout<<"[init instance fail][ret="<< result<<" , path= "<< pathalliance<<"]"<<endl;
		cout<<"read file failed"<<endl;
		return -1;
	}
	if (!reader.parse(sdata, allianceRank))
	{
		cout<<"parse failed"<<endl;
		return -1;
	}
	int iCount = allianceRank.size();
	CLogicAlliance logicAlliance;
	CLogicUser logicUser;
	DataUser user;
	CLogicEmail logicEmail;
	DataEmail email;
	CLogicUserBasic logicUserBasic;
	DataUserBasic userBasic;
	vector<uint64_t> uidList;
	vector<DataAllianceMember> vVembers;
	DataAlliance alliance;
	CLogicPay logicPay;
	unsigned aid = 0;
	int level = 0;
	int RecReward = 0;
	int memCoinReward = 0;
	int leaderCoinReward = 0;
	for(int i = 0; i < iCount; i++)
	{
		 aid = allianceRank[i]["aid"].asUInt();
		 result = logicAlliance.GetAlliance(aid,alliance);
		 if(result != 0)
		 {
			 cout<<"[get aillance failed][aid=" << aid <<",ret="<< result << "]" << endl;
			 continue;
		 }
		 result = logicAlliance.GetMembers(aid,vVembers);
		 if(result != 0)
		 {
			 cout<<"[get aillance member failed][aid=" << aid << ",ret=" << result << "]" <<endl;
			 continue;
		 }
		 level = allianceRank[i]["level"].asInt();
		 switch(level)
		 {
		 case 3:
		 case 4:
			 RecReward = 100000;
			 memCoinReward = 100;
			 leaderCoinReward = 200;
			 break;
		 case 5:
		 case 6:
			 RecReward = 300000;
			 memCoinReward = 200;
			 leaderCoinReward = 500;
			 break;
		 case 7:
		 case 8:
		 case 9:
		 case 10:
			 RecReward = 500000;
			 memCoinReward = 500;
			 leaderCoinReward = 1000;
			 break;
		 default:
			 RecReward = 0;
			 memCoinReward = 0;
			 leaderCoinReward = 0;
			 break;
		 }
		 if(RecReward == 0)
		 {
			 continue;
		 }
		 for(int index = 0; index < vVembers.size();index++)
		 {
			 result = logicUser.GetUserLimit(vVembers[index].uid,user);
			 {
				 if(result != 0)
				 {
					 cout<<"get user failed"<<endl;
					 continue;
				 }
				 if(user.uid != alliance.leader_uid)
				 {
					 result = logicPay.ChangePay(user.uid,0,memCoinReward,"ALLIANCE_RUSH_REWARD",1);
					 if(result != 0)
					 {
						 cout<<"change cash failed,uid="<<user.uid<<"cash = "<<memCoinReward<<endl;
					 }
				 }else{
					 result = logicPay.ChangePay(user.uid,0,leaderCoinReward,"ALLIANCE_RUSH_REWARD",1);
					 if(result != 0)
					 {
						 cout<<"change cash failed,uid="<<user.uid<<"cash = "<<leaderCoinReward<<endl;
					 }
				 }
				 user.r1 += RecReward;
				 user.r2 += RecReward;
				 user.r3 += RecReward;
				 user.r4 += RecReward;
				 user.r5 += RecReward;
				 uidList.push_back(user.uid);
			 }
			 result = logicUser.SetUserLimit(vVembers[index].uid,user);
			 if(result != 0)
			 {
				 cout<<"Set user failed"<<endl;
				 continue;
			 }

			 cout<<"memcash="<<memCoinReward<<",leadercoins="<<leaderCoinReward<<",recreward="<<RecReward<<",uid="<<
					 vVembers[index].uid<<endl;
		 }
		 vVembers.clear();
	}
	email.attach_flag = 0;
	email.post_ts = Time::GetGlobalTime();
	email.title = "联盟冲级奖励";
	email.text = "联盟冲级奖励已发放完毕（无需提取附件，系统已自动发放）,如有疑问，请联系GM";
	email.uid = ADMIN_UID;
	result = logicEmail.AddEmail(email,uidList,(PlatformType)2);
	if(result != 0)
	{
		cout<<"send email failed"<<endl;
	}
	return 0;
}



