/*
 * presentSendMail.cpp
 *
 *  Created on: 2012-12-6
 *      Author: colin
 */
#include "LogicInc.h"
#include <iostream>
#include <string>
#include <fstream>
#include <map>

#define UID_MAX_PRESENT 512
#define MAIL_TEXT_MAX_PRESENT 1000



void usage()
{
	cout << "usage: PresentSendMail file platformID" << endl;
}

int main(int argc, char *argv[])
{
	if (!Kernel::Init())
	{
		cout << "Knit kernel fail" << endl;
		return 1;
	}

	if (argc != 3)
	{
		usage();
		return 0;
	}

	char* pString = argv[1];
	int levelStart = 0;
	int levelEnd = 0;
	int levelType;
	int uidList[UID_MAX_PRESENT];
	int uidList_size = 0;
	string mailTitle;
	string mailContent;
	int r1 = 0;
	int r2 = 0;
	int r3 = 0;
	int r4 = 0;
	int r5 = 0;
	int cash = 0;
	map<int,int> equipmentMap;
	map<int,int> herosMap;
	int equList_size;

	int ret;
	uint64_t u64Id;
	DataUser user;
	CLogicUser logicUser;
	CLogicIdCtrl logicIdCtrl;
	vector<uint64_t> vecUsersUid;
	CLogicEmail presentSendEmail;
	DataEmail data;
	string operatorName;
	string operatorIP;
	int tpt = atoi(argv[2]); //获取用户昵称 需要平台信息

	//Json读取日志文件
	Json::Reader reader;
	Json::FastWriter writer;
	Json::Value root;
	Json::Value attachments;

	std::ifstream is;
	is.open(pString, std::ios::binary);
	if (reader.parse(is, root))
	{
		operatorName = root["operatorInfo"]["operatorName"].asString();
		operatorIP = root["operatorInfo"]["operatorIp"].asString();
		levelStart = root["userCondition"]["level"]["levelStart"].asInt();
		levelEnd =root["userCondition"]["level"]["levelEnd"].asInt();
		levelType = root["userCondition"]["levelType"].asInt();
		Json::Value val_uidList = root["userCondition"]["idList"];
		uidList_size = val_uidList.size();
		for (int i=0; i<uidList_size; ++i)
		{
			uidList[i] = val_uidList[i]["uid"].asInt();
		}
		mailTitle = root["mail"]["title"].asString();
		mailContent = root["mail"]["content"].asString();
		attachments["resource"] = root["resource"];
		attachments["equipment"] = root["equipment"];
		attachments["heros"]=root["heros"];
		r1 = root["resource"]["r1"].asInt();
		r2 = root["resource"]["r2"].asInt();
		r3 = root["resource"]["r3"].asInt();
		r4 = root["resource"]["r4"].asInt();
		r5 = root["resource"]["r5"].asInt();
		cash = root["resource"]["cash"].asInt();
		Json::Value val_equList = root["equipment"];
		equList_size = val_equList.size();
		for (int i=0; i<equList_size; i++)
		{
			int tempId = val_equList[i]["id"].asInt();
			int tempC = val_equList[i]["c"].asInt();
			equipmentMap.insert(pair<int,int>(tempId,tempC));
		}
		Json::Value heros = root["heros"];
		for (int i=0; i< heros.size(); i++)
		{
			int tempId = val_equList[i]["id"].asInt();
			int tempC = val_equList[i]["c"].asInt();
			herosMap.insert(pair<int,int>(tempId,tempC));
		}
	}
	else
	{
		error_log("present JSon Data parse fail! from presentSendMail");
		return -1;
	}


	is.close();

	//u64Id = 10003762;
	//获取符合的用户uid
	ret = logicIdCtrl.GetCurrentId(KEY_UID_CTRL, u64Id);
	if(ret != 0)
	{
		error_log("get user id fail! from presentSendMail ---start!");
		return ret;
	}
	unsigned userid = UID_MIN + 1;

	// cout << "input userid start!" << endl;
	if (levelType != 2)
	{
		for( userid = Config::GetIntValue(CONFIG_UID_MIN); userid <= (unsigned)u64Id; ++userid)
		{
			ret = logicUser.GetUser(userid, user);
			if(ret == 0)
			{
				if (levelType == 0 || levelType == 1)
				{
					if(user.level >= levelStart && user.level <= levelEnd )
					{
						vecUsersUid.push_back(user.uid);
						cout << "from ordinarylevel input uid:" << user.uid << endl;
					}
				}
			}
			else if(ret != R_ERR_NO_DATA)
			{
				error_log("get user fail uid =%d, ret= %d from presentSendMail--first", userid, ret);
			}
		}
	}
	// 添加指定用户
	if(levelType == 2)
	{
		for (int i=0; i<uidList_size; ++i)
		{
			vecUsersUid.push_back(uidList[i]);
			cout << "from specified input uid:" << uidList[i] << endl;
		}
	}

	//data 赋值
	data.title = mailTitle;
	data.text = mailContent;
	data.post_ts = Time::GetGlobalTime();
	//data.attachments = writer.write(attachments);
	data.read_ts = 0;
	data.from_name = "系统管理员";
	data.uid = ADMIN_UID;
	data.attach_flag = 1;

	PlatformType pt = static_cast<PlatformType>(tpt);
	ret = presentSendEmail.AddEmail(data,vecUsersUid,attachments,pt);
	cout << "ret :" << ret << endl;

	if(0 != ret)
	{
		return ret;
	}
	cout << "发送成功" << ret << endl;
	return 0;
}
