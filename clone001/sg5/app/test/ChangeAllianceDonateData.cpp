/*
 * ChangeAllianceDonateData.cpp
 *
 *  Created on: 2013-11-14
 *      Author: Administrator
 */

#include "LogicInc.h"
#include <iostream>

#define CHANGE_AMOUNT 10000

int main(int argc, char *argv[]) {
	if (!Kernel::Init(APP_CONFIG_PATH))
	{
		cout << "Init kernel fail!" << endl;
		return -1;
	}
	Config::SetDomain(0);
	if (argc != 2)
	{
		cout << "usage: " << argv[0] << "[serverid]" << endl;
		return 1;
	}
	int ret;
	string strserver(argv[1]);
	vector<string> rlt;
	String::Split(strserver, '-', rlt);
	if (rlt.size() >= 2)
	{
		unsigned begin = Convert::StringToUInt(rlt[0]);
		unsigned end = Convert::StringToUInt(rlt[1]);
		if (end < begin || end <= 0 || begin <= 0)
		{
			cout << "usage: " << argv[0] << "[serverid]" << endl;
			return 1;
		}
		for (unsigned i = begin; i <= end; ++i)
		{
			uint64_t uidEnd;
			uint64_t Alliance_idEnd;
			unsigned userid = UID_MIN + 500000 * (i - 1) + 1;
			unsigned alliance_id = ALLIANCE_ID_START + 500000 * (i - 1) + 1;

			CLogicIdCtrl logicIdCtrl;
			ret = logicIdCtrl.GetCurrentId(KEY_UID_CTRL, uidEnd, i);
			if (ret != 0)
			{
				cout<<"get current user id fail! from ChangeAllianceDonateData ---start!"<<endl;
				return ret;
			}
			ret = logicIdCtrl.GetCurrentId(KEY_ALLIANCE_ID_CTRL, Alliance_idEnd, i);
			if (ret != 0)
			{
				cout<<"get current alliance id fail! from ChangeAllianceDonateData ---start!"<<endl;
				return ret;
			}
			int i = time(NULL);
			cout << "i=" << i << endl;

			cout << "uid_min=" << userid << "uid_max=" << uidEnd << endl;
			cout << "allamce_id_min=" << alliance_id << "alliance_id_max=" << Alliance_idEnd << endl;

			CLogicAlliance LogicAlliance;
			for(;alliance_id <=(unsigned) Alliance_idEnd; ++alliance_id)
			{
				DataAlliance DataAlliance;
				ret = LogicAlliance.GetAlliance(alliance_id, DataAlliance);
				if(ret != 0 )
				{
					cout<<"getAlliance fail"<<alliance_id<<"ret:"<<ret<<endl;
//					return ret;
				}
				Json::Value data;
				Json::FromString(data, DataAlliance.extra_data);
				cout<<"allianceId:"<<alliance_id<<"oldData:"<<DataAlliance.extra_data<<endl;
				Json::Value &resource = data["resource"];
				uint64_t oldres1 = 0;
				uint64_t oldres2 = 0;
				uint64_t oldres3 = 0;
				uint64_t oldres4 = 0;
				Json::GetUInt64(resource,"r1",oldres1);
				Json::GetUInt64(resource,"r2",oldres2);
				Json::GetUInt64(resource,"r3",oldres3);
				Json::GetUInt64(resource,"r4",oldres4);
				oldres1 = oldres1 / CHANGE_AMOUNT;
				oldres2 = oldres2 / CHANGE_AMOUNT;
				oldres3 = oldres3 / CHANGE_AMOUNT;
				oldres4 = oldres4 / CHANGE_AMOUNT;
				resource["r1"] = Convert::UInt64ToString(oldres1);
				resource["r2"] = Convert::UInt64ToString(oldres2);
				resource["r3"] = Convert::UInt64ToString(oldres3);
				resource["r4"] = Convert::UInt64ToString(oldres4);
				DataAlliance.extra_data = Json::ToString(data);
				cout<<"allianceId:"<<alliance_id<<"newData:"<<DataAlliance.extra_data<<endl;
				ret = LogicAlliance.SetAlliance(DataAlliance);
				if(ret != 0)
				{
					cout<<"setAlliance fail"<<alliance_id<<"ret:"<<ret<<endl;
					return 1;
				}
			}

			CLogicUser Logicuser;
			for (; userid <= (unsigned) uidEnd; ++userid)
			{
				DataAllianceMember member;
				DataUser userdata;
				ret = Logicuser.GetUserLimit(userid,userdata);
				if(ret != 0)
				{
					cout<< "GetUserLimit fail!"<<" uid ="<<userid<<endl;
					return ret;
				}
				unsigned allianceId = userdata.alliance_id;
				if(allianceId != 0)
				{
					ret = LogicAlliance.GetMember(allianceId, userid, member);
					if(ret != 0)
					{
						cout<<"GetMember fail"<<"  ret="<<ret<<"userid:"<<userid<<"alliacneid:"<<allianceId<<endl;
//						return ret;
					}

					cout<<"uid:"<<userid<<"oldData:"<<member.donate_data<<endl;
					Json::Value donate_data;
					if (member.donate_data.empty())
					{
						continue;
					}
					if (!Json::FromString(donate_data, member.donate_data))
					{
						cout<<"Parse donate data faile!"<<endl;
						return R_ERR_DATA;
					}
					unsigned temp_resource = 0;
					Json::GetUInt(donate_data, "dr1", temp_resource);
					donate_data["dr1"] = temp_resource / CHANGE_AMOUNT;

					temp_resource = 0;
					Json::GetUInt(donate_data, "dr2", temp_resource);
					donate_data["dr2"] = temp_resource / CHANGE_AMOUNT;

					temp_resource = 0;
					Json::GetUInt(donate_data, "dr3", temp_resource);
					donate_data["dr3"] = temp_resource / CHANGE_AMOUNT;

					temp_resource = 0;
					Json::GetUInt(donate_data, "dr4", temp_resource);
					donate_data["dr4"] = temp_resource / CHANGE_AMOUNT;

					Json::GetUInt(donate_data, "r1", temp_resource);
					donate_data["r1"] = temp_resource / CHANGE_AMOUNT;

					temp_resource = 0;
					Json::GetUInt(donate_data, "r2", temp_resource);
					donate_data["r2"] = temp_resource / CHANGE_AMOUNT;

					temp_resource = 0;
					Json::GetUInt(donate_data, "r3", temp_resource);
					donate_data["r3"] = temp_resource / CHANGE_AMOUNT;

					temp_resource = 0;
					Json::GetUInt(donate_data, "r4", temp_resource);
					donate_data["r4"] = temp_resource / CHANGE_AMOUNT;

					member.donate_data = Json::ToString(donate_data);

					cout<<"uid:"<<userid<<"newData:"<<member.donate_data<<endl;

					ret = LogicAlliance.SetMember(member);
					if(ret != 0)
					{
						cout<<"SetMember fail"<<"  ret="<<ret<<"userid:"<<userid<<"alliacneid:"<<allianceId<<endl;
						return ret;
					}
				}
			}
			int j = time(NULL);
			cout << "j=" << j << endl;
			cout << "take out:" << j - i << "s" << endl;
		}
	}
	else
	{
		int serverid = CTrans::STOI(argv[1]);
		if (serverid <= 0)
		{
			cout << "usage: " << argv[0] << " [serverid]" << endl;
			return 1;
		}
		uint64_t uidEnd;
		uint64_t Alliance_idEnd;
		unsigned userid = UID_MIN + 500000 * (serverid - 1) + 1;
		unsigned alliance_id = ALLIANCE_ID_START + 500000 * (serverid - 1) + 1;

		CLogicIdCtrl logicIdCtrl;
		ret = logicIdCtrl.GetCurrentId(KEY_UID_CTRL, uidEnd, serverid);
		if (ret != 0)
		{
			cout<<"get current user id fail! from ChangeAllianceDonateData ---start!"<<endl;
			return ret;
		}
		ret = logicIdCtrl.GetCurrentId(KEY_ALLIANCE_ID_CTRL, Alliance_idEnd, serverid);
		if (ret != 0)
		{
			  cout<<"get current alliance id fail! from ChangeAllianceDonateData ---start!"<<endl;
			return ret;
		}
		int i = time(NULL);
		cout << "i=" << i << endl;

		cout << "uid_min=" << userid << "uid_max=" << uidEnd << endl;
		cout << "allamce_id_min=" << alliance_id << "alliance_id_max=" << Alliance_idEnd << endl;

		CLogicAlliance LogicAlliance;
		for(;alliance_id <=(unsigned) Alliance_idEnd;alliance_id++)
		{
			DataAlliance DataAlliance;
			ret = LogicAlliance.GetAlliance(alliance_id, DataAlliance);
			if(ret != 0 )
			{
				cout<<"getAlliance fail"<<alliance_id<<"ret:"<<ret<<endl;
//				return ret;
			}
			cout<<"allianceId:"<<alliance_id<<"oldData:"<<DataAlliance.extra_data<<endl;
			Json::Value data;
			Json::FromString(data, DataAlliance.extra_data);
			Json::Value &resource = data["resource"];
			uint64_t oldres1 = 0;
			uint64_t oldres2 = 0;
			uint64_t oldres3 = 0;
			uint64_t oldres4 = 0;
			Json::GetUInt64(resource,"r1",oldres1);
			Json::GetUInt64(resource,"r2",oldres2);
			Json::GetUInt64(resource,"r3",oldres3);
			Json::GetUInt64(resource,"r4",oldres4);
			oldres1 = oldres1 / CHANGE_AMOUNT;
			oldres2 = oldres2 / CHANGE_AMOUNT;
			oldres3 = oldres3 / CHANGE_AMOUNT;
			oldres4 = oldres4 / CHANGE_AMOUNT;
			resource["r1"] = Convert::UInt64ToString(oldres1);
			resource["r2"] = Convert::UInt64ToString(oldres2);
			resource["r3"] = Convert::UInt64ToString(oldres3);
			resource["r4"] = Convert::UInt64ToString(oldres4);

			DataAlliance.extra_data = Json::ToString(data);

			cout<<"allianceId:"<<alliance_id<<"newData:"<<DataAlliance.extra_data<<endl;
			ret = LogicAlliance.SetAlliance(DataAlliance);
			if(ret != 0)
			{
				cout<<"setalliance fail"<<"  ret="<<ret<<" alliance_id:"<<alliance_id<<endl;
				return 1;
			}
		}

		CLogicUser Logicuser;
		for (; userid <= (unsigned) uidEnd; userid++)
		{
			DataAllianceMember member;
			DataUser userdata;
			ret = Logicuser.GetUserLimit(userid,userdata);
			if(ret != 0)
			{
				cout<<"GetUserLimit fail!"<<"uid ="<<userid<<endl;
				return ret;
			}
			unsigned allianceId = userdata.alliance_id;
			if(allianceId != 0)
			{
				ret = LogicAlliance.GetMember(allianceId, userid, member);
				if(ret != 0)
				{
					cout<<"GetMember fail"<<"  ret="<<ret<<"userid:"<<userid<<"alliacneid:"<<allianceId<<endl;
//					return ret;
				}

				cout<<"uid:"<<userid<<"oldData:"<<member.donate_data<<endl;
				Json::Value donate_data;
				if (member.donate_data.empty())
				{
					continue;
				}
				if (!Json::FromString(donate_data, member.donate_data))
				{
					cout<<"Parse donate data faile!"<<endl;
					return R_ERR_DATA;
				}
				unsigned temp_resource = 0;
				Json::GetUInt(donate_data, "dr1", temp_resource);
				donate_data["dr1"] = temp_resource / CHANGE_AMOUNT;

				temp_resource = 0;
				Json::GetUInt(donate_data, "dr2", temp_resource);
				donate_data["dr2"] = temp_resource / CHANGE_AMOUNT;

				temp_resource = 0;
				Json::GetUInt(donate_data, "dr3", temp_resource);
				donate_data["dr3"] = temp_resource / CHANGE_AMOUNT;

				temp_resource = 0;
				Json::GetUInt(donate_data, "dr4", temp_resource);
				donate_data["dr4"] = temp_resource / CHANGE_AMOUNT;

				Json::GetUInt(donate_data, "r1", temp_resource);
				donate_data["r1"] = temp_resource / CHANGE_AMOUNT;

				temp_resource = 0;
				Json::GetUInt(donate_data, "r2", temp_resource);
				donate_data["r2"] = temp_resource / CHANGE_AMOUNT;

				temp_resource = 0;
				Json::GetUInt(donate_data, "r3", temp_resource);
				donate_data["r3"] = temp_resource / CHANGE_AMOUNT;

				temp_resource = 0;
				Json::GetUInt(donate_data, "r4", temp_resource);
				donate_data["r4"] = temp_resource / CHANGE_AMOUNT;

				member.donate_data = Json::ToString(donate_data);

				cout<<"uid:"<<userid<<"newData:"<<member.donate_data<<endl;
				ret = LogicAlliance.SetMember(member);
				if(ret != 0)
				{
					cout<<"SetMember fail"<<"  ret="<<ret<<"userid:"<<userid<<"alliacneid:"<<allianceId<<endl;
					return ret;
				}
			}
		}

		int j = time(NULL);
		cout << "j=" << j << endl;
		cout << "take out:" << j - i << "s" << endl;
	}
	return 0;
}

