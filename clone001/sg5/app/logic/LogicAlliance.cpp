/*
 * LogicAlliance.cpp
 *
 *  Created on: 2011-9-19
 *      Author: dada
 */
#include<math.h>
#include "LogicAlliance.h"

struct DataMemberDonate
{
	unsigned uid;
	int donate_total;
	int today_donate;
};

static bool CompareDonateTotal(const DataMemberDonate &left, const DataMemberDonate &right)
{
	return (left.donate_total > right.donate_total);
}

static bool CompareTodayDonate(const DataMemberDonate &left, const DataMemberDonate &right)
{
	return (left.today_donate > right.today_donate);
}

static int ParseMemberDonate(vector<DataAllianceMember> &members,
		vector<DataMemberDonate> &members_donate)
{
	uint64_t r1 = 0;
	uint64_t r2 = 0;
	uint64_t r3 = 0;
	uint64_t r4 = 0;
	uint64_t r5 = 0;
	int cash = 0;
	uint64_t dr1 = 0;
	uint64_t dr2 = 0;
	uint64_t dr3 = 0;
	uint64_t dr4 = 0;
	uint64_t dr5 = 0;
	int dcash = 0;
	Json::Value donate;
	DataMemberDonate tempData;
	vector<DataAllianceMember>::iterator itr = members.begin();
	for (; itr!=members.end(); ++itr)
	{
		if (!itr->donate_data.empty())
		{
			if (!Json::FromString(donate, itr->donate_data))
			{
				error_log("Parse donate data faile!");
				return R_ERR_DATA;
			}
		}

		r1 = 0;
		Json::GetUInt64(donate, "r1", r1);
		r2 = 0;
		Json::GetUInt64(donate, "r2", r2);
		r3 = 0;
		Json::GetUInt64(donate, "r3", r3);
		r4 = 0;
		Json::GetUInt64(donate, "r4", r4);
		r5 = 0;
		Json::GetUInt64(donate, "r5", r5);
		cash = 0;
		Json::GetInt(donate, "cash", cash);

		dr1 = 0;
		Json::GetUInt64(donate, "dr1", dr1);
		dr2 = 0;
		Json::GetUInt64(donate, "dr2", dr2);
		dr3 = 0;
		Json::GetUInt64(donate, "dr3", dr3);
		dr4 = 0;
		Json::GetUInt64(donate, "dr4", dr4);
		dr5 = 0;
		Json::GetUInt64(donate, "dr5", dr5);
		dcash = 0;
		Json::GetInt(donate, "dcash", dcash);
		tempData.uid = itr->uid;
		tempData.donate_total = (int)(r1 + r2 + r3 + r4 + r5) / 10000 + cash;
		tempData.today_donate = (int)(dr1 + dr2 + dr3 + dr4 + dr5) / 10000 + dcash;
		members_donate.push_back(tempData);
	}
	return 0;
}


int CLogicAlliance::AddAlliance(unsigned uid, string &name, int flag, const string &description,
		int cash, int coins, bool useResource, DataAlliance &alliance)
{
	int ret;

	//名字检查
	String::Trim(name);
	if(OpenPlatform::GetType() != PT_FACEBOOK)
	{
		if(!StringFilter::Check(name))
		{
			error_log("[forbidden_alliance_name][uid=%u,name=%s]", uid, name.c_str());
			ERROR_RETURN_MSG(R_ERR_DATA_LIMIT, "forbidden_alliance_name");
		}
	}
	CDataAllianceMapping dbMapping;
	unsigned allianceId;
	ret = dbMapping.GetMapping(name, allianceId);
	if(ret == 0)
	{
		error_log("[duplicate_alliance_name][uid=%u,allianceId=%u,name=%s]", uid, alliance.alliance_id, name.c_str());
		ERROR_RETURN_MSG(R_ERR_DATA_LIMIT, "duplicate_alliance_name");
	}
	else if(ret != R_ERR_NO_DATA)
	{
		error_log("[GetMapping fail][ret=%d,uid=%u,allianceId=%u,name=%s]", ret, uid, alliance.alliance_id, name.c_str());
		DB_ERROR_RETURN_MSG("get_alliance_mapping_fail");
	}

	//原联盟检查
	CDataUser dbUser;
	ret = dbUser.GetAllianceId(uid, allianceId);
	if(ret != 0)
	{
		error_log("[GetAllianceId fail][uid=%u]", uid);
		DB_ERROR_RETURN_MSG("get_user_alliance_fail");
	}
	if(IsAllianceId(allianceId))
	{
		error_log("[user_in_alliance][uid=%u,allianceId=%u]", uid, allianceId);
		ERROR_RETURN_MSG(R_ERR_LOGIC, "in_alliance");
	}

	//检查资源
	CLogicUser logicUser;
	DataUser user;
	ret = logicUser.GetUserLimit(uid, user);
	if(ret != 0)
	{
		return ret;
	}
//	if(user.r1 < 2000000 || user.r2 < 2000000 || user.r3 < 2000000)
//	{
//		error_log("[not_enough_resource][uid=%u,r1=%u,r2=%u,r3=%u]", uid, user.r1, user.r2, user.r3);
//		ERROR_RETURN_MSG(R_ERR_LOGIC, "not_enough_resource");
//	}
	DataPay pay;
	CLogicPay logicPay;
	if(cash != 0)
	{
		ret = logicPay.GetPay(uid, pay);
		if(ret != 0)
		{
			return ret;
		}
		if((unsigned)cash > pay.cash)
		{
			error_log("[not_enough_cash][uid=%u,cash=%d,oldCash=%u]",
					uid, cash, pay.cash);
			ERROR_RETURN_MSG(R_ERR_LOGIC, "not_enough_cash");
		}
	}
	if(coins != 0)
	{
		ret = logicPay.GetPay(uid, pay);
		if(ret != 0)
		{
			return ret;
		}
		if((unsigned)coins > pay.coins)
		{
			error_log("[not_enough_coins][uid=%u,coins=%d,oldCoins=%u]",
					uid, coins, pay.coins);
			ERROR_RETURN_MSG(R_ERR_LOGIC, "not_enough_coins");
		}
	}

	//添加联盟
	CLogicIdCtrl logicIdCtrl;
	uint64_t u64AllianceId;
	ret = logicIdCtrl.GetNextId(KEY_ALLIANCE_ID_CTRL, u64AllianceId);
	if(ret != 0)
	{
		return ret;
	}
	unsigned allianc_id_start;
	if(!Config::GetUIntValue(allianc_id_start,CONFIG_AID_START))
	{
		allianc_id_start = ALLIANCE_ID_START;
	}
	alliance.alliance_id = (unsigned)u64AllianceId;
	alliance.kingdom = user.kingdom;
	alliance.name = name;
	alliance.flag = flag;
	alliance.leader_uid = uid;
	alliance.create_time = Time::GetGlobalTime();
	alliance.remove_time = 0;
	alliance.status = AS_ACTIVE;
	alliance.level = 1;
	alliance.point = 0;
	alliance.curr_point = 0;
	alliance.rank = alliance.alliance_id - allianc_id_start;
	alliance.member_count = 1;
	alliance.enemy_alliance_id = 0;
	alliance.description = description;
	alliance.activity = "[]";

	Json::Value extraData;
	Json::Value &resource = extraData["resource"];
	resource["r1"] = 0;
	resource["r2"] = 0;
	resource["r3"] = 0;
	resource["r4"] = 0;
	alliance.extra_data = Json::ToString(extraData);
	CDataAlliance dbAlliance;
	ret = dbAlliance.AddAlliance(alliance);
	if(ret != 0)
	{
		fatal_log("[AddAlliance fail][ret=%d,uid=%u,allianceId=%u,name=%s]", ret, uid, alliance.alliance_id, name.c_str());
		DB_ERROR_RETURN_MSG("add_alliance_fail");
	}

	//添加成员
	DataAllianceMember member;
	member.alliance_id = alliance.alliance_id;
	member.uid = uid;
	member.type = AMT_LEADER;
	member.point = 0;
	member.curr_point = 0;
	member.join_time = Time::GetGlobalTime();
	member.extra_data = "{}";
	member.donate_data = "{}";
	member.donate_rank = 0;
	member.donate_drank = 0;
	CDataAllianceMember dbMember;
	ret = dbMember.AddMember(member);
	if(ret != 0)
	{
		fatal_log("[AddMember fail][ret=%d,uid=%u,allianceId=%u]", ret, uid, alliance.alliance_id);
		dbAlliance.RemoveAlliance(alliance.alliance_id);
		DB_ERROR_RETURN_MSG("add_alliance_member_fail");
	}

	//添加映射
	ret = dbMapping.AddMapping(alliance.name, alliance.alliance_id);
	if(ret != 0)
	{
		fatal_log("[AddMapping fail][ret=%d,uid=%u,allianceId=%u,name=%s]", ret, uid, alliance.alliance_id, name.c_str());
		dbMember.RemoveMember(alliance.alliance_id, uid);
		dbAlliance.RemoveAlliance(alliance.alliance_id);
		DB_ERROR_RETURN_MSG("add_alliance_mapping_fail");
	}

	//设置用户联盟，扣除资源
	if(useResource)
	{
#define DECREASE_RESOUCE(res, count)	\
		if(user.res < count)	\
		{	\
			user.res = 0;	\
		}	\
		else	\
		{	\
			user.res -= count;	\
		}	\

		DECREASE_RESOUCE(r1, 1000000);
		DECREASE_RESOUCE(r2, 1000000);
		DECREASE_RESOUCE(r3, 1000000);
		DECREASE_RESOUCE(r4, 1000000);
		DECREASE_RESOUCE(r5, 1000000);
	}
	user.alliance_id = alliance.alliance_id;
	ret = logicUser.SetUserLimit(user);
	if(ret != 0)
	{
		error_log("[SetUserLimit fail][ret=%d,uid=%u,allianceId=%u]", ret, uid, alliance.alliance_id);
		dbMapping.RemoveMapping(alliance.name);
		dbMember.RemoveMember(alliance.alliance_id, uid);
		dbAlliance.RemoveAlliance(alliance.alliance_id);
		DB_ERROR_RETURN_MSG("set_user_alliance_fail");
	}
	if(cash != 0)
	{
		//ret = logicPay.ChangeCash(uid, "ALCR", -cash);
		ret = logicPay.ChangePay(uid, -cash, 0, "ALCR");
		if(ret != 0)
		{
			error_log("[ChangeCash fail][ret=%d,uid=%u,allianceId=%u,cash=%d]", ret, uid, alliance.alliance_id, cash);
		}
	}
	if (coins != 0)
	{
		ret = logicPay.ChangePay(uid, 0, -coins, "ALCR");
		if(ret != 0)
		{
			error_log("[ChangeCash fail][ret=%d,uid=%u,allianceId=%u,coins=%d]", ret, uid, alliance.alliance_id, coins);
		}
	}

	CLogicNominateAlliance nAlliance;
	ret = nAlliance.SetAlliance(alliance.alliance_id, 0);
	if (0 != ret)
	{
		error_log("add alliance to nominate list fail!ret=%d", ret);
	}
	return 0;
}

int CLogicAlliance::SetAlliance(const DataAlliance &alliance)
{
	int ret;
	CDataAlliance dbAlliance;
	ret = dbAlliance.SetAlliance(alliance);
	if(ret != 0)
	{
		error_log("[SetAlliance fail][ret=%d,allianceId=%u]", ret, alliance.alliance_id);
		DB_ERROR_RETURN_MSG("set_alliance_fail");
	}
	return 0;
}

int CLogicAlliance::SetAllianceLimit(const DataAlliance &alliance)
{
	int ret;
	CDataAlliance dbAlliance;
	ret = dbAlliance.SetAllianceLimit(alliance);
	if(ret != 0)
	{
		error_log("[SetAllianceLimit fail][ret=%d,allianceId=%u]", ret, alliance.alliance_id);
		DB_ERROR_RETURN_MSG("set_alliance_fail");
	}
	return 0;
}

int CLogicAlliance::GetAlliance(unsigned allianceId, DataAlliance &alliance)
{
	int ret;
	CDataAlliance dbAlliance;
	ret = dbAlliance.GetAlliance(allianceId, alliance);
	if(ret != 0)
	{
		if(ret == R_ERR_NO_DATA)
		{
			ERROR_RETURN_MSG(R_ERR_NO_DATA, "alliance_not_exist");
		}
		error_log("[GetAlliance fail][ret=%d,allianceId=%u]", ret, allianceId);
		DB_ERROR_RETURN_MSG("get_alliance_fail");
	}
	if(alliance.status != AS_ACTIVE)
	{
		ERROR_RETURN_MSG(R_ERR_NO_DATA, "alliance_removed");
	}
	return 0;
}

int CLogicAlliance::GetAllianceLimit(unsigned allianceId, DataAlliance &alliance)
{
	int ret;
	CDataAlliance dbAlliance;
	ret = dbAlliance.GetAllianceLimit(allianceId, alliance);
	if(ret != 0)
	{
		if(ret == R_ERR_NO_DATA)
		{
			ERROR_RETURN_MSG(R_ERR_NO_DATA, "alliance_not_exist");
		}
		error_log("[GetAllianceLimit fail][ret=%d,allianceId=%u]", ret, allianceId);
		DB_ERROR_RETURN_MSG("get_alliance_fail");
	}
	if(alliance.status != AS_ACTIVE)
	{
		ERROR_RETURN_MSG(R_ERR_NO_DATA, "alliance_removed");
	}
	return 0;
}

int CLogicAlliance::RemoveAlliance(unsigned allianceId)
{
	//删除联盟
	int ret;
	DataAlliance alliance;
	ret = GetAllianceLimit(allianceId, alliance);
	if(ret != 0)
	{
		return ret;
	}
	alliance.status = AS_REMOVE;
	ret = SetAllianceLimit(alliance);
	if(ret != 0)
	{
		return ret;
	}
	CDataAllianceMapping dbMapping;
	ret = dbMapping.RemoveMapping(alliance.name);

	//删除联盟成员
	vector<DataAllianceMember> members;
	ret = GetMembers(allianceId, members);
	if(ret != 0)
	{
		return ret;
	}
	CDataUser dbUser;
	CDataAllianceMember dbMember;
	CLogicMessage logicMessage;
	Json::Value message;
	message["id"] = "remove_alliance";
	message["name"] = alliance.name;
	string sMessage = Json::ToString(message);
	for(vector<DataAllianceMember>::iterator itr = members.begin(); itr != members.end(); itr++)
	{
		ret = dbUser.SetAllianceId(itr->uid, ALLIANCE_ID_NULL);
		if(ret != 0)
		{
			error_log("[SetAllianceId fail][ret=%d,uid=%u,allianceId=%u]", ret, itr->uid, ALLIANCE_ID_NULL);
		}
		ret = dbMember.RemoveMember(allianceId, itr->uid);
		if(ret != 0)
		{
			error_log("[RemoveMember fail][ret=%d,uid=%u,allianceId=%u]", ret, itr->uid, allianceId);
		}
		ret = logicMessage.AddSysMessage(itr->uid, Json::ToString(sMessage));
	}

	//删除消息
	ret = logicMessage.CancelRequest(allianceId);
	if (0 != ret)
	{
		error_log("CancelRequest fail!,ret=%d", ret);
	}

	CLogicNominateAlliance nAlliance;
	ret = nAlliance.RemoveAlliance(allianceId);
	if (0 != ret)
	{
		error_log("remov alliance from nominate list fail,ret=%d", ret);
	}
	return 0;
}

int CLogicAlliance::RemoveAlliance(unsigned allianceId, bool isReal)
{
	int ret = 0;
	if (isReal)
	{
		return RemoveAlliance(allianceId);
	}

	DataAlliance alliance;
	ret = GetAllianceLimit(allianceId, alliance);
	if(ret != 0)
	{
		return ret;
	}
	alliance.remove_time = Time::GetGlobalTime();
	ret = SetAllianceLimit(alliance);
	if(ret != 0)
	{
		return ret;
	}

	CLogicUpdates logicUpdates;
	Json::Value updates;
	updates["ts"] = Time::GetGlobalTime();
	updates["s"] = "remove_alliance";
	updates["uid"] = alliance.leader_uid;
	vector<DataAllianceMember> members;
	ret = GetMembers(allianceId,members);
	if (0 != ret)
	{
		return ret;
	}

	for (vector<DataAllianceMember>::iterator itr=members.begin(); itr!=members.end(); ++itr)
	{
		ret = logicUpdates.AddUpdates(itr->uid, updates);
		if (0 != ret)
		{
			error_log("AddUpdates failed![uid=%u,ret=%d]",itr->uid,ret);
		}
	}

	return 0;
}

int CLogicAlliance::AddMember(unsigned allianceId, unsigned uid)
{
	int ret;

	//原联盟检查
	unsigned oldAllianceId;
	CDataUser dbUser;
	ret = dbUser.GetAllianceId(uid, oldAllianceId);
	if(ret != 0)
	{
		error_log("[GetAllianceId fail][uid=%u]", uid);
		DB_ERROR_RETURN_MSG("get_user_alliance_fail");
	}
	if(IsAllianceId(oldAllianceId))
	{
		error_log("[user_in_alliance][uid=%u,allianceId=%u,oldAllianceId=%u]", uid, allianceId, oldAllianceId);
		ERROR_RETURN_MSG(R_ERR_LOGIC, "in_alliance");
	}

	DataUser user;
	CLogicUser logicUser;
	ret = logicUser.GetUserLimit(uid, user);
	if (ret != 0)
		return ret;


	DataAlliance alliance;
	ret = GetAllianceLimit(allianceId, alliance);
	if(ret != 0)
	{
		return ret;
	}

	if(alliance.kingdom != user.kingdom)
	{
		error_log("[user_kingdom_wrong][uid=%u,allianceId=%u]", uid, allianceId);
		ERROR_RETURN_MSG(R_ERR_LOGIC, "kingdom_wrong");
	}

	//检查人数上限
	int limitCount = GetMemberMaxCount(alliance.level);
	if(alliance.member_count >= limitCount)
	{
		error_log("[alliance_member_count_limit][uid=%u,allianceId=%u,count=%d,level=%d]",
				uid, allianceId, alliance.member_count, alliance.level);
		ERROR_RETURN_MSG(R_ERR_DATA_LIMIT, "alliance_member_count_limit");
	}


	//添加成员
	DataAllianceMember member;
	member.alliance_id = allianceId;
	member.uid = uid;
	member.type = AMT_NORMAL;
	member.point = 0;
	member.curr_point = 0;
	member.join_time = Time::GetGlobalTime();
	member.extra_data = "{}";
	member.donate_data = "{}";
	member.donate_rank = 0;
	member.donate_drank = 0;
	CDataAllianceMember dbMember;
	ret = dbMember.AddMember(member);
	if(ret != 0)
	{
		error_log("[AddMember fail][ret=%d,uid=%u,allianceId=%u]", ret, uid, allianceId);
		DB_ERROR_RETURN_MSG("add_alliance_member_fail");
	}

	//设置用户联盟
	ret = dbUser.SetAllianceId(uid, allianceId);
	if(ret != 0)
	{
		error_log("[SetAllianceId fail][ret=%d,uid=%u,allianceId=%u]", ret, uid, allianceId);
		dbMember.RemoveMember(allianceId, uid);
		DB_ERROR_RETURN_MSG("set_user_alliance_fail");
	}

	//设置联盟成员数
	alliance.member_count++;
	ret = SetAllianceLimit(alliance);
	if(ret != 0)
	{
		error_log("[set_member_count_fail][ret=%d,uid=%u,allianceId=%u,count=%d]", ret, uid, allianceId, alliance.member_count);
	}

	CDataUserBasic dbUserBasic;
	string userName;
	ret = dbUserBasic.GetUserName(uid, OpenPlatform::GetType(), userName);
	if(0 == ret)
	{
		Json::Value activity;
		activity["type"] = "join";
		activity["time"] = Time::GetGlobalTime();
		activity["name"] = userName;
		AddActivity(allianceId, activity);
	}

	return 0;
}

int CLogicAlliance::SetMember(const DataAllianceMember &member)
{
	int ret;
	CDataAllianceMember dbMember;
	ret = dbMember.SetMember(member);
	if(ret != 0)
	{
		error_log("[SetMember fail][ret=%d,allianceId=%u,uid=%u]", ret, member.alliance_id, member.uid);
		DB_ERROR_RETURN_MSG("set_alliance_member_fail");
	}
	return 0;
}

int CLogicAlliance::GetMember(unsigned allianceId, unsigned uid, DataAllianceMember &member)
{
	int ret;
	CDataAllianceMember dbMember;
	ret = dbMember.GetMember(allianceId, uid, member);
	if(ret != 0)
	{
		error_log("[GetMember fail][ret=%d,allianceId=%u,uid=%u]", ret, allianceId, uid);
		DB_ERROR_RETURN_MSG("get_alliance_member_fail");
	}
	return 0;
}

int CLogicAlliance::GetMembers(unsigned allianceId, vector<DataAllianceMember> &members)
{
	int ret;
	CDataAllianceMember dbMember;
	ret = dbMember.GetMembers(allianceId, members);
	if(ret != 0)
	{
		error_log("[GetMembers fail][ret=%d,allianceId=%u]", ret, allianceId);
		DB_ERROR_RETURN_MSG("get_alliance_member_fail");
	}
	return 0;
}

int CLogicAlliance::RemoveMember(unsigned allianceId, unsigned uid)
{
	int ret;

	//检查是否盟主
	DataAlliance alliance;
	ret = GetAllianceLimit(allianceId, alliance);
	if(ret != 0)
	{
		return ret;
	}
	if(alliance.leader_uid == uid)
	{
		ERROR_RETURN_MSG(R_ERR_LOGIC, "is_leader");
	}

	//删除成员
	DataAllianceMember member;
	ret = GetMember(allianceId, uid, member);
	if(ret != 0)
	{
		return ret;
	}
	CDataAllianceMember dbMember;
	ret = dbMember.RemoveMember(allianceId, uid);
	if(ret != 0)
	{
		error_log("[RemoveMember fail][ret=%d,uid=%u,allianceId=%u]", ret, uid, allianceId);
		DB_ERROR_RETURN_MSG("remove_alliance_member_fail");
	}

	//设置用户联盟
	CDataUser dbUser;
	ret = dbUser.SetAllianceId(uid, ALLIANCE_ID_NULL);
	if(ret != 0)
	{
		error_log("[SetAllianceId fail][ret=%d,uid=%u,allianceId=%u]", ret, uid, ALLIANCE_ID_NULL);
	}

	//设置联盟成员数
	alliance.member_count--;
	ret = SetAllianceLimit(alliance);
	if(ret != 0)
	{
		error_log("[set_member_count_fail][ret=%d,uid=%u,allianceId=%u,count=%d]", ret, uid, allianceId, alliance.member_count);
	}

	return 0;
}

int CLogicAlliance::ChangeLeader(unsigned allianceId, unsigned newLeaderUid)
{
	int ret;
//	DataUser user;
//	CLogicUser logicUser;
//	ret = logicUser.GetUserLimit(newLeaderUid, user);
//	if(ret != 0)
//	{
//		return ret;
//	}
//	if(user.level < 40)
//	{
//		error_log("[leader_level_dissatisfy][allianceId=%u,newLeaderUid=%u,level=%d]", allianceId, newLeaderUid, user.level);
//		ERROR_RETURN_MSG(R_ERR_LOGIC, "leader_level_dissatisfy");
//	}
	DataAlliance alliance;
	ret = GetAllianceLimit(allianceId, alliance);
	if(ret != 0)
	{
		return ret;
	}
	if(alliance.leader_uid == newLeaderUid)
	{
		ERROR_RETURN_MSG(R_ERR_LOGIC, "is_leader");
	}
	DataAllianceMember oldLeader;
	ret = GetMember(allianceId, alliance.leader_uid, oldLeader);
	if(ret != 0)
	{
		return ret;
	}
	DataAllianceMember newLeader;
	ret = GetMember(allianceId, newLeaderUid, newLeader);
	if(ret != 0)
	{
		return ret;
	}

	alliance.leader_uid = newLeaderUid;
	oldLeader.type = AMT_NORMAL;
	newLeader.type = AMT_LEADER;

	ret = SetMember(newLeader);
	if(ret != 0)
	{
		return ret;
	}
	ret = SetMember(oldLeader);
	if(ret != 0)
	{
		return ret;
	}
	ret = SetAllianceLimit(alliance);
	if(ret != 0)
	{
		return ret;
	}

	DataUserBasic userBasicNewLeader;
	CLogicUserBasic logicUserBasic;
	ret = logicUserBasic.GetUserBasicLimitSmart(newLeaderUid, OpenPlatform::GetType(), userBasicNewLeader);
	if(ret == 0)
	{
		Json::Value activity;
		activity["type"] = "change_leader";
		activity["time"] = Time::GetGlobalTime();
		activity["name"] = userBasicNewLeader.name;
		AddActivity(allianceId, activity);
		Json::Value message;
		message["id"] = "alliance_change_leader";
		message["name"] = userBasicNewLeader.name;
		message["aname"] = alliance.name;
		CLogicMessage logicMessage;
		logicMessage.AddSysMessage(newLeaderUid, Json::ToString(message));
	}

	return 0;
}

int CLogicAlliance::CompeteLaeder(unsigned allianceId, unsigned uid)
{
	int ret = 0;
	DataAlliance alliance;
	ret = GetAllianceLimit(allianceId, alliance);
	if (0 != ret)
	{
		return ret;
	}

	if (0 == alliance.remove_time)
	{
		return R_ERR_LOGIC;
	}

	alliance.remove_time = 0;

	if (alliance.leader_uid == uid)
	{
		return SetAllianceLimit(alliance);
	}

	DataAllianceMember newLeader;
	ret = GetMember(allianceId, uid, newLeader);
	if (0 != ret)
	{
		return ret;
	}
	DataAllianceMember oldLeader;
	ret = GetMember(allianceId, alliance.leader_uid, oldLeader);
	if(ret != 0)
	{
		return ret;
	}
	alliance.leader_uid = uid;
	newLeader.type = AMT_LEADER;
	oldLeader.type = AMT_NORMAL;
	ret = SetMember(newLeader);
	if (0 != ret)
	{
		return ret;
	}
	ret = SetMember(oldLeader);
	if (0 != ret)
	{
		return ret;
	}
	ret = SetAllianceLimit(alliance);
	if (0 != ret)
	{
		return ret;
	}
	return 0;
}

int CLogicAlliance::AddPoint(unsigned allianceId, unsigned uid, int point)
{
	int ret;
	CDataAlliance dbAlliance;

	ret = dbAlliance.AddPoint(allianceId, point);
	if(ret != 0)
	{
		error_log("[AddPoint fail][ret=%d,target=alliance,allianceId=%u,uid=%u,point=%d]", ret, allianceId, uid, point);
		return ret;
	}
	ret = dbAlliance.AddCurrPoint(allianceId, point);
	if(ret != 0)
	{
		error_log("[AddCurrPoint fail][ret=%d,target=alliance,allianceId=%u,uid=%u,point=%d]", ret, allianceId, uid, point);
		return ret;
	}
	if(IsValidUid(uid))
	{
		CDataAllianceMember dbMember;
		ret = dbMember.AddPoint(allianceId, uid, point);
		if(ret != 0)
		{
			error_log("[AddPoint fail][ret=%d,target=member,allianceId=%u,uid=%u,point=%d]", ret, allianceId, uid, point);
			return ret;
		}
		ret = dbMember.AddCurrPoint(allianceId, uid, point);
		if(ret != 0)
		{
			error_log("[AddCurrPoint fail][ret=%d,target=member,allianceId=%u,uid=%u,point=%d]", ret, allianceId, uid, point);
			return ret;
		}
	}
	return 0;
}

int CLogicAlliance::AddAttackPoint(unsigned allianceId, unsigned uid, const Json::Value &data, int defenceLevel)
{
	int r1_loot = 0;
	int r2_loot = 0;
	int r3_loot = 0;
	int r4_loot = 0;
	int r5_loot = 0;
	int resource = 0;
	if (data.isMember("lootreport") && data["lootreport"].isObject())
	{
		Json::GetInt(data["lootreport"], "r1", r1_loot);
		Json::GetInt(data["lootreport"], "r2", r2_loot);
		Json::GetInt(data["lootreport"], "r3", r3_loot);
		Json::GetInt(data["lootreport"], "r4", r4_loot);
		Json::GetInt(data["lootreport"], "r5", r5_loot);
	}

	resource = r1_loot + r2_loot + r3_loot + r4_loot + r5_loot;

	if(resource <= 0)
	{
		return 0;
	}

	double point;
	double tempData = pow(defenceLevel+1, 3.767) + 5000;
	point = (double)resource / tempData;
	int nPoint = (int)(point * 10);
	if(nPoint <= 0)
	{
		nPoint = 0;
	}
	return AddPoint(allianceId, uid, nPoint);
}

int CLogicAlliance::GetRandomAlliances(int count, vector<DataAlliance> &alliances)
{
	int ret;
	if(count <= 0)
	{
		return 0;
	}
	CLogicIdCtrl logicIdCtrl;
	uint64_t u64Id;
	ret = logicIdCtrl.GetCurrentId(KEY_ALLIANCE_ID_CTRL, u64Id);
	if(ret != 0)
	{
		return ret;
	}
	unsigned maxId = (unsigned)u64Id;
	unsigned total = maxId - ALLIANCE_ID_START;
	if(total <= 0)
	{
		return 0;
	}
	int maxTime = count * 3;
	int currTime = 0;
	while(currTime < maxTime && (int)alliances.size() < count)
	{
		currTime++;
		unsigned allianceId = (unsigned)Math::GetRandomInt(total) + ALLIANCE_ID_START + 1;
		bool unique = true;
		for(vector<DataAlliance>::const_iterator itr = alliances.begin(); itr != alliances.end(); itr++)
		{
			if(allianceId == itr->alliance_id)
			{
				unique = false;
				break;
			}
		}
		if(unique)
		{
			DataAlliance alliance;
			ret = GetAlliance(allianceId, alliance);
			if(ret == 0)
			{
				alliances.push_back(alliance);
			}
			else if(ret != R_ERR_NO_DATA)
			{
				return ret;
			}
		}
	}
	return 0;
}

int CLogicAlliance::AddActivity(DataAlliance &alliance, const Json::Value &activity)
{
	Json::Value data;
	Json::FromString(data, alliance.activity);
	if(!data.isArray())
	{
		data = Json::Value(Json::arrayValue);
	}
	unsigned count = data.size();
	if(count >= ALLIANCE_ACTIVE_COUNT)
	{
		for(unsigned i = 1; i < ALLIANCE_ACTIVE_COUNT; i++)
		{
			data[i - 1] = data[i];
		}
		data[ALLIANCE_ACTIVE_COUNT - 1] = activity;
	}
	else
	{
		data[data.size()] = activity;
	}
	alliance.activity = Json::ToString(data);
	return SetAlliance(alliance);
}

int CLogicAlliance::AddActivity(unsigned &allianceId, const Json::Value &activity)
{
	int ret;
	DataAlliance alliance;
	ret = GetAlliance(allianceId, alliance);
	if(ret != 0)
	{
		return ret;
	}
	return AddActivity(alliance, activity);
}

int CLogicAlliance::AddLastAttack(unsigned allianceId, unsigned uid, const Json::Value &attackData)
{
	int ret;
	unsigned attackUid;
	if(!Json::GetUInt(attackData, "a_uid", attackUid))
	{
		return R_ERR_PARAM;
	}
	DataAllianceMember member;
	ret = GetMember(allianceId, uid, member);
	if(ret != 0)
	{
		return ret;
	}
	Json::Value data;
	Json::FromString(data, member.extra_data);
	Json::Value &attacks = data["attacks"];
	if(!attacks.isArray())
	{
		attacks = Json::Value(Json::arrayValue);
	}
	for(unsigned i = 0; i < attacks.size();)
	{
		unsigned currAttackUid;
		if(Json::GetUInt(attacks[i], "a_uid", currAttackUid))
		{
			if(currAttackUid == attackUid)
			{
				for(unsigned j = i + 1; j < attacks.size(); j++)
				{
					attacks[j - 1] = attacks[j];
				}
				attacks.resize(attacks.size() - 1);
			}
			else
			{
				i++;
			}
		}
		else
		{
			i++;
		}
	}
	if(attacks.size() >= ALLIANCE_MEMBER_ATTACK_COUNT)
	{
		for(unsigned i = 1; i < ALLIANCE_MEMBER_ATTACK_COUNT; i++)
		{
			attacks[i - 1] = attacks[i];
		}
		attacks.resize(ALLIANCE_MEMBER_ATTACK_COUNT - 1);
	}
	attacks[attacks.size()] = attackData;
	member.extra_data = Json::ToString(data);
	return SetMember(member);
}

bool CLogicAlliance::IsInSameAlliance(unsigned uid, unsigned targetUid)
{
	int ret;
	CDataUser dbUser;
	unsigned allianceId;
	ret = dbUser.GetAllianceId(uid, allianceId);
	if(ret != 0)
	{
		error_log("[GetAllianceId fail][uid=%u]", uid);
		return false;
	}
	if(!IsAllianceId(allianceId))
	{
		return false;
	}
	unsigned targetAllianceId;
	ret = dbUser.GetAllianceId(targetUid, targetAllianceId);
	if(ret != 0)
	{
		error_log("[GetAllianceId fail][uid=%u]", targetUid);
		return false;
	}
	return allianceId == targetAllianceId;
}

int CLogicAlliance::Donate(unsigned uid, unsigned allianceId, int r1, int r2, int r3, int r4, int r5, int cash, bool isCoins)
{
	if(r1 < 0 || r2 < 0 || r3 < 0 || r4 < 0 || r5 < 0 || cash < 0)
	{
		error_log("[donate_param_error][uid=%u,r1=%d,r2=%d,r3=%d,r4=%d,r5=%d,cash=%d]", uid, r1, r2, r3, r4, r5, cash);
		PARAM_ERROR_RETURN();
	}
	int ret;

	DataAllianceMember member;
	ret = GetMember(allianceId, uid, member);
	if(ret != 0)
	{
		return ret;
	}

	DataUser user;
	CLogicUser logicUser;
	ret = logicUser.GetUserLimit(uid, user);
	if(ret != 0)
	{
		return ret;
	}

	if(cash != 0 && member.type != AMT_LEADER && member.type != AMT_MANAGER && user.level < 80 && user.viplevel < 2)
	{
		error_log("[can_not_donate_cash][uid=%u,aid=%u,type=%d,level=%d,cash=%d,viplevel=%d]",
				uid, allianceId, member.type, user.level, cash,user.viplevel);
		ERROR_RETURN_MSG(R_ERR_LOGIC, "can_not_donate_cash");
	}

	if(user.alliance_id != allianceId)
	{
		error_log("[user_not_in_alliance][uid=%u,allianceId=%u,realAllianceId=%u]", uid, allianceId, user.alliance_id);
		PARAM_ERROR_RETURN_MSG("no_in_alliance");
	}

	if( (unsigned)r1 > user.r1 || (unsigned)r2 > user.r2 ||
		(unsigned)r3 > user.r3 || (unsigned)r4 > user.r4 ||
		(unsigned)r5 > user.r5)
	{
		error_log("[donate_not_enough_resource][uid=%u,r1=%d,r2=%d,r3=%d,r4=%d,r5=%d,rr1=%u,rr2=%u,rr3=%u,rr4=%u,rr5=%d]",
				uid, r1, r2, r3, r4, r5, user.r1, user.r2, user.r3, user.r4, user.r5);
		ERROR_RETURN_MSG(R_ERR_LOGIC, "not_enough_resource");
	}
	DataPay pay;
	CLogicPay logicPay;
	if(cash > 0)
	{
		ret = logicPay.GetPay(uid, pay);
		if(ret != 0)
		{
			return ret;
		}
		if (isCoins)
		{
			if((unsigned)cash > pay.coins)
			{
				error_log("[donate_not_enough_coins][uid=%u,coins=%d,userCoins=%u]", uid, cash, pay.coins);
				ERROR_RETURN_MSG(R_ERR_LOGIC, "pay_overspend");
			}
		}
		else
		{
			if((unsigned)cash > pay.cash)
			{
				error_log("[donate_not_enough_cash][uid=%u,cash=%d,userCash=%u]", uid, cash, pay.cash);
				ERROR_RETURN_MSG(R_ERR_LOGIC, "pay_overspend");
			}
		}
	}

	ret = AddMemberDonate(uid, allianceId, r1, r2, r3, r4, r5, cash);
	if (0 != ret)
	{
		return ret;
	}
	int dPoint = 0;
	ret = DonatePoint(user.level, r1, r2, r3, r4, r5, dPoint);
	int point = dPoint + cash;
	ret = AddPoint(allianceId, uid, point);
	if (0 != ret )
	{
		return ret;
	}

	ret = ChangeResource(allianceId, r1, r2, r3, r4, r5, cash, "ALRECV");
	if(ret != 0)
	{
		return ret;
	}

	if(r1 > 0 || r2 > 0 || r3 > 0 || r4 > 0 || r5 > 0)
	{
		int res1 = r1 * 10000;
		int res2 = r2 * 10000;
		int res3 = r3 * 10000;
		int res4 = r4 * 10000;
		int res5 = r5 * 10000;
		ret = logicUser.ChangeResource(uid, -res1, -res2, -res3, -res4, -res5);
		if(ret != 0)
		{
			error_log("[change_user_resource_fail][uid=%u,r1=%d,r2=%d,r3=%d,r4=%d,r5=%d]", uid, r1, r2, r3, r4, r5);
		}
	}
	if(cash > 0)
	{
		if (isCoins)
		{
			//ret = logicPay.ChangeCash(uid, "ADNT", -cash);
			ret = logicPay.ChangePay(uid, 0, -cash, "ADNT");
			if(ret != 0)
			{
				error_log("[change_user_cash_fail][uid=%u,coins=%d]", uid, cash);
			}
		}
		else
		{
			ret = logicPay.ChangePay(uid, -cash, 0, "ADNT");
			if(ret != 0)
			{
				error_log("[change_user_cash_fail][uid=%u,cash=%d]", uid, cash);
			}
		}
	}
	return 0;
}

int CLogicAlliance::ChangeResource(unsigned allianceId, int64_t r1, int64_t r2, int64_t r3, int64_t r4, int64_t r5, int cash, const string &reason)
{
	int ret;
	DataAlliance alliance;
	if(r1 != 0 || r2 != 0 || r3 != 0 || r4 != 0 || r5 != 0)
	{
		ret = GetAlliance(allianceId, alliance);
		if(ret != 0)
		{
			return ret;
		}
		Json::Value data;
		Json::FromString(data, alliance.extra_data);
		Json::Value &resource = data["resource"];

#define CHANGE_ALLIANCE_RESOURCE(res)	\
		if(res != 0)	\
		{	\
			uint64_t old##res = 0;	\
			Json::GetUInt64(resource, #res, old##res);	\
			if(res < 0 && (unsigned)-res > old##res)	\
			{	\
				error_log("[not_enough_resource][allianceId=%u," #res "=%d,old" #res "=%llu]", allianceId, res, old##res);	\
				ERROR_RETURN_MSG(R_ERR_DATA_LIMIT, "not_enough_resource");	\
			}	\
			old##res += res;	\
			resource[#res] = Convert::UInt64ToString(old##res);	\
		}	\

		CHANGE_ALLIANCE_RESOURCE(r1);
		CHANGE_ALLIANCE_RESOURCE(r2);
		CHANGE_ALLIANCE_RESOURCE(r3);
		CHANGE_ALLIANCE_RESOURCE(r4);
		CHANGE_ALLIANCE_RESOURCE(r5);

		alliance.extra_data = Json::ToString(data);
	}

	bool hasCashData = true;
	DataAlliancePay apay;
	CLogicPay logicPay;
	if(cash != 0)
	{
		ret = logicPay.GetAlliancePay(allianceId, apay);
		if(ret != 0)
		{
			if(ret != R_ERR_NO_DATA)
			{
				return ret;
			}
			hasCashData = false;
		}
		if(cash < 0 && (unsigned)-cash > apay.coins)
		{
			error_log("[pay overspend][allianceId=%u,cash=%u,delta=%d]", allianceId, apay.coins, cash);
			ERROR_RETURN_MSG(R_ERR_DATA_LIMIT, "pay_overspend");
		}
	}

	if(r1 != 0 || r2 != 0 || r3 != 0 || r4 != 0 || r5 != 0)
	{
		ret = SetAlliance(alliance);
		if(ret != 0)
		{
			return ret;
		}
	}
	if(cash != 0)
	{
		if(hasCashData)
		{
			//ret = logicPay.ChangeCash(allianceId, reason, cash);
			ret = logicPay.ChangeAlliancePay(allianceId, cash, reason);
		}
		else
		{
			ret = logicPay.AddAlliancePay(allianceId, reason, cash);
		}
		if(ret != 0)
		{
			return ret;
		}
	}
	return 0;
}

int CLogicAlliance::SetAllianceData(unsigned uid, unsigned allianceId, const Json::Value &data)
{
	int ret;
	DataAlliance alliance;
	ret = GetAlliance(allianceId, alliance);
	if(ret != 0)
	{
		return ret;
	}
//	if(alliance.leader_uid != uid)
//	{
//		error_log("[is_not_leader][aid=%u,uid=%u,luid=%u]", allianceId, uid, alliance.leader_uid);
//		ERROR_RETURN_MSG(R_ERR_LOGIC, "is_not_leader");
//	}
	Json::Value extraData;
	Json::FromString(extraData, alliance.extra_data);
	extraData["cdata"] = data["alliancedata"];
	alliance.extra_data = Json::ToString(extraData);
	ret = SetAlliance(alliance);
	if(ret != 0)
	{
		return ret;
	}
	Json::Value updates;
	Json::Value &update = updates[0u];
	update["ts"] = Time::GetGlobalTime();
	update["s"] = "ALLIANCESETDATA";
	update["cdata"] = data["alliancedata"];
	ret = AddAllianceUpdates(uid, allianceId, updates);
	if(ret != 0)
	{
		return ret;
	}

	if(Json::IsObjectOrArray(data, "activity"))
	{
		Json::Value activity;
		activity["type"] = "technology";
		activity["time"] = Time::GetGlobalTime();
		activity["name"] = data["activity"];
		AddActivity(allianceId, activity);
	}
	return 0;
}

int CLogicAlliance::SetMemberData(unsigned uid, unsigned allianceId, const Json::Value &data)
{
	int ret;
	DataAllianceMember member;
	ret = GetMember(allianceId, uid, member);
	if(ret != 0)
	{
		return ret;
	}
	Json::Value extraData;
	Json::FromString(extraData, member.extra_data);
	extraData["cdata"] = data;
	member.extra_data = Json::ToString(extraData);
	ret = SetMember(member);
	if(ret != 0)
	{
		return ret;
	}
	return 0;
}

int CLogicAlliance::SetAllianceMemberData(unsigned uid, unsigned allianceId, int point, const Json::Value &data)
{
	if (point < 0)
	{
		error_log("[SetAllianceMemberData_param_error][point=%d]",point);
		PARAM_ERROR_RETURN();
	}
	int ret;
	DataAllianceMember member;
	ret = GetMember(allianceId, uid, member);
	if(ret != 0)
	{
		error_log("[Get member info failed!][uid=%u,ali=%u,ret=%d]",uid,allianceId,ret);
		return ret;
	}

	if ( member.curr_point - point <= 0 )
	{
		error_log("[No enough point!][uid=%u,aid=%u]",uid,allianceId);
		PARAM_ERROR_RETURN();
	}

	member.curr_point -= point;

	CLogicUser logicUser;
	DataUser user;
	ret = logicUser.GetUser(uid,user);
	if (0 != ret)
	{
		error_log("[Get user failed!][uid=%u,ret=%d]",uid, ret);
		return R_ERR_DATA;
	}

	user.alliance_tech = Json::ToString(data);

	ret = SetMember(member);
	if (0 != ret)
	{
		error_log("[Set member failed!][uid=%u,aid=%u,ret=%d]",uid,allianceId,ret);
		return ret;
	}

	ret = logicUser.SetUser(uid,user);
	if (0 != ret)
	{
		error_log("[Set user failed!][uid=%u,aid=%u,ret=%d]",uid,allianceId,ret);
		return ret;
	}
	return R_SUCCESS;
}

bool CLogicAlliance::IsLeader(unsigned uid, unsigned allianceId)
{
	int ret;
	DataAlliance alliance;
	ret = GetAllianceLimit(allianceId, alliance);
	if(ret != 0)
	{
		return false;
	}
	return alliance.leader_uid == uid;
}

bool CLogicAlliance::IsManager(unsigned uid, unsigned allianceId)
{
	int ret;
	int type;
	CDataAllianceMember dbMember;
	ret = dbMember.GetMemberType(allianceId, uid, type);
	if(ret != 0)
	{
		error_log("[GetMemberType fail][ret=%d,allianceId=%u,uid=%u]", ret, allianceId, uid);
		return false;
	}
	return type == AMT_LEADER || type == AMT_MANAGER;
}

int CLogicAlliance::Upgrade(unsigned uid, unsigned allianceId, int64_t r1, int64_t r2, int64_t r3, int64_t r4, int64_t r5, int cash, int &level)
{
	int ret;
	if(r1 < 0 || r2 < 0 || r3 < 0 || r4 < 0 || r5 < 0 || cash < 0)
	{
		error_log("[upgrade_param_error][uid=%u,r1=%lld,r2=%lld,r3=%lld,r4=%lld,r5=%lld,cash=%lld]", uid, r1, r2, r3, r4, r5, cash);
		return R_ERR_PARAM;
	}

	if(!IsManager(uid, allianceId))
	{
		error_log("[is_not_leader][aid=%u,uid=%u]", allianceId, uid);
		ERROR_RETURN_MSG(R_ERR_LOGIC, "is_not_leader");
	}

	DataAlliance alliance;
	ret = GetAllianceLimit(allianceId, alliance);
	if(ret != 0)
	{
		return ret;
	}
//	if(alliance.leader_uid != uid)
//	{
//		error_log("[is_not_leader][aid=%u,uid=%u,luid=%u]", allianceId, uid, alliance.leader_uid);
//		ERROR_RETURN_MSG(R_ERR_LOGIC, "is_not_leader");
//	}
	if(alliance.level != level)
	{
		error_log("[level_mismatch][uid=%u,aid=%u,level=%d,rlevel=%d]", uid, allianceId, level, alliance.level);
		PARAM_ERROR_RETURN();
	}
	if(level <= 0 || level >= 10)
	{
		error_log("[level_out_range][uid=%u,aid=%u,level=%d]", uid, allianceId, level);
		PARAM_ERROR_RETURN();
	}

	static uint64_t pointRequire[] = {0,1400,8600,35600,129600,411600,1107600,2742600,6282600,13902600};

	if(alliance.point < pointRequire[level])
	{
		error_log("[point_require][uid=%u,aid=%u,level=%d,point=%llu]", uid, allianceId, level, alliance.point);
		ERROR_RETURN_MSG(R_ERR_DATA_LIMIT, "point_require");
	}
	/*if(alliance.member_count < GetMemberMaxCount(level))
	{
		error_log("[member_count_require][uid=%u,aid=%u,level=%d,mc=%d]", uid, allianceId, level, alliance.member_count);
		ERROR_RETURN_MSG(R_ERR_DATA_LIMIT, "member_count_require");
	}*/
	ret = ChangeResource(allianceId, -r1, -r2, -r3, -r4, -r5, -cash, "ALUP");
	if(ret != 0)
	{
		return ret;
	}
	level++;
	alliance.level = level;
	ret = SetAllianceLimit(alliance);
	if(ret != 0)
	{
		fatal_log("[set_level_fail][uid=%u,aid=%u,level=%d,r1=%lld,r2=%lld,r3=%lld,r4=%lld,r5=%lld,cash=%d]",
				uid, allianceId, level, r1, r2, r3, r4, r5, cash);
		return ret;
	}
	Json::Value activity;
	activity["type"] = "upgrade";
	activity["time"] = Time::GetGlobalTime();
	activity["name"] = level;
	AddActivity(allianceId, activity);
	return 0;
}

int CLogicAlliance::RemoveMember(unsigned uid, unsigned allianceId, unsigned uidRemove)
{
	int ret;
	DataUser userRemove;
	CLogicUser logicUser;
	ret = logicUser.GetUserLimit(uidRemove, userRemove);
	if(ret != 0)
	{
		return ret;
	}
	if(userRemove.alliance_id != allianceId)
	{
		PARAM_ERROR_RETURN();
	}
	DataAlliance alliance;
	ret = GetAllianceLimit(allianceId, alliance);
	if(ret != 0)
	{
		return ret;
	}
	if(uidRemove != uid)
	{
		if(alliance.leader_uid != uid && !IsManager(uid, allianceId))
		{
			error_log("[is_not_leader][aid=%u,uid=%u]", allianceId, uid);
			ERROR_RETURN_MSG(R_ERR_LOGIC, "is_not_leader");
		}
	}
	ret = RemoveMember(allianceId, uidRemove);
	if(ret != 0)
	{
		return ret;
	}

	//消息通知
	DataUserBasic userBasicRemove;
	CLogicUserBasic logicBasic;
	ret = logicBasic.GetUserBasicLimitSmart(uidRemove, OpenPlatform::GetType(), userBasicRemove);
	if(ret == 0)
	{
		Json::Value activity;
		if(uidRemove == uid)
		{
			activity["type"] = "exit";
		}
		else
		{
			activity["type"] = "remove";
		}
		activity["time"] = Time::GetGlobalTime();
		activity["name"] = userBasicRemove.name;
		AddActivity(allianceId, activity);

		if(uidRemove != uid)  //被盟主踢出联盟
		{
			Json::Value updates;                                   //通知被t的人
			updates.resize(1);
			updates[(unsigned)0]["s"] = "ALLIANCEMEMBERREMOVE";
			updates[(unsigned)0]["ts"] = Time::GetGlobalTime();
			updates[(unsigned)0]["uid"] = uidRemove;
			updates[(unsigned)0]["removetime"] = Time::GetGlobalTime();
			CLogicUpdates logicUpdates;
			int r = logicUpdates.AddUpdates(uidRemove,updates);

			Json::Value message;
			message["id"] = "alliance_remove_member";
			message["name"] = alliance.name;
			CLogicMessage logicMessage;
			logicMessage.AddSysMessage(uidRemove, Json::ToString(message));
		}
	}
	return 0;
}

int CLogicAlliance::SetMemberType(unsigned uid, unsigned allianceId, unsigned memberUid, int type)
{
	if(type != AMT_NORMAL && type != AMT_MANAGER && type != AMT_GENERAL)
	{
		PARAM_ERROR_RETURN();
	}
	int ret;
	DataAlliance alliance;
	ret = GetAllianceLimit(allianceId, alliance);
	if(ret != 0)
	{
		return ret;
	}
	if(memberUid == alliance.leader_uid)
	{
		ERROR_RETURN_MSG(R_ERR_LOGIC, "is_leader");
	}
	if(alliance.leader_uid != uid)
	{
		error_log("[is_not_leader][aid=%u,uid=%u]", allianceId, uid);
		ERROR_RETURN_MSG(R_ERR_LOGIC, "is_not_leader");
	}
	int oldType;
	CDataAllianceMember dbMember;
	ret = dbMember.GetMemberType(allianceId, memberUid, oldType);
	if(ret != 0)
	{
		error_log("[GetMemberType fail][ret=%d,allianceId=%u,uid=%u]", ret, allianceId, uid);
		return ret;
	}
	if(oldType == type)
	{
		return 0;
	}
	vector<DataAllianceMember> members;
	ret = dbMember.GetMembers(allianceId, members);
	if (0 != ret)
	{
		error_log("[GetMembers faile][aid=%u.ret=%d]",allianceId, ret);
		return ret;
	}
	vector<DataAllianceMember>::iterator itr = members.begin();
	int managerNum = 0;
	for(; itr!=members.end(); ++itr)
	{
		if (itr->type == type && type == AMT_MANAGER)
		{
			managerNum++;
		}
	}
	//副盟主只有两个
	if (managerNum >= 2)
	{
		return 0;
	}

	ret = dbMember.SetMemberType(allianceId, memberUid, type);
	if(ret != 0)
	{
		error_log("[SetMemberType fail][ret=%d,allianceId=%u,uid=%u,type=%d]", ret, allianceId, uid, type);
		return ret;
	}

	//消息通知
	DataUserBasic userBasic;
	CLogicUserBasic logicBasic;
	ret = logicBasic.GetUserBasicLimitSmart(memberUid, OpenPlatform::GetType(), userBasic);
	if(ret == 0)
	{
		Json::Value activity;
		activity["type"] = "settype";
		activity["time"] = Time::GetGlobalTime();
		activity["name"] = userBasic.name;
		activity["newtype"] = type;
		activity["oldtype"] = oldType;
		AddActivity(allianceId, activity);

		if(memberUid != uid)
		{
			Json::Value message;
			message["id"] = "allience_set_member_type";
			message["name"] = alliance.name;
			message["type"] = type;
			CLogicMessage logicMessage;
			logicMessage.AddSysMessage(memberUid, Json::ToString(message));
		}
	}
	return 0;
}

int CLogicAlliance::AddAllianceUpdates(unsigned uid, unsigned allianceId, Json::Value &updates)
{
	int ret;
	CLogicUser logicUser;
	CLogicUpdates logicUpdates;
	vector<DataAllianceMember> members;
	ret = GetMembers(allianceId, members);
	if(ret != 0)
	{
		return ret;
	}
	for(vector<DataAllianceMember>::const_iterator itr = members.begin(); itr != members.end(); itr++)
	{
		if(uid != itr->uid)
		{
			DataUser user;
			ret = logicUser.GetUserLimit(itr->uid, user);
			if(ret == 0 && IsOnlineUser(user.last_active_time))
			{
				logicUpdates.AddUpdates(itr->uid, updates);
			}
		}
	}
	return 0;
}

int CLogicAlliance::ProcessRequest(const DataMessage &request, unsigned from, unsigned to, const string &type, int action, const Json::Value &data)
{
	if(type == "alliance_join")
	{
		if(action != MS_ACCEPT)
		{
			return 0;
		}
		if(!IsValidUid(request.from_uid) || !IsAllianceId(request.to_uid))
		{
			error_log("[error_request][from=%u,to=%u,type=%s,action=%d]", request.from_uid, request.to_uid, type.c_str(), action);
			return R_ERR_PARAM;
		}
		return AddMember(request.to_uid, request.from_uid);
	}
	return 0;
}

int CLogicAlliance::GetAllianceJson(unsigned uid, unsigned allianceId, Json::Value &value)
{
	int ret;
	DataUser userBy;
	CLogicUser logicUser;
	ret = logicUser.GetUserLimit(uid, userBy);
	if (ret != 0)
		return ret;

	DataAlliance alliance;
	ret = GetAlliance(allianceId, alliance);
	if(ret != 0)
	{
		return ret;
	}
	value["aid"] = alliance.alliance_id;
	value["kingdom"] = alliance.kingdom;
	value["name"] = alliance.name;
	value["flag"] = alliance.flag;
	value["leader"] = alliance.leader_uid;
	value["level"] = alliance.level;
	value["point"] = Convert::UInt64ToString(alliance.point);
	value["curr_point"] = Convert::UInt64ToString(alliance.curr_point);
	value["rank"] = alliance.rank;
	value["mc"] = alliance.member_count;
	value["enemyid"] = alliance.enemy_alliance_id;
	value["enemyname"] = alliance.enemy_alliance_name;
	value["createtime"] = alliance.create_time;
	value["desc"] = alliance.description;
	value["notice"] = alliance.notice;
	value["etime"] = alliance.remove_time;
	Json::FromString(value["activity"], alliance.activity);
	if (Json::FromString(value["data"], alliance.extra_data)) {
		if (value["data"].isMember("attackD")) {
			value["attackD"] = value["data"]["attackD"];
			value["data"].removeMember("attackD");
		}
		if (value["data"].isMember("activeD")) {
			value["activeD"] = value["data"]["activeD"];
			value["data"].removeMember("activeD");
		}
	}

	CLogicNominateAlliance logicNAlliance;
	int sign = 0;
	ret = logicNAlliance.GetAlliance(alliance.alliance_id, sign);
	value["sign"] = sign;

	CLogicPay logicPay;
	DataAlliancePay pay;
	pay.coins = 0;
	ret = logicPay.GetAlliancePay(allianceId, pay);
	value["coins"] = pay.coins;
	value["cash"] = 0;

	map<unsigned, int> interacts;
	CLogicUserInteract logicInteract;
	ret = logicInteract.GetFriendInteracts(uid, interacts);

	Json::Value &membersData = value["members"];
	membersData = Json::Value(Json::arrayValue);
	vector<DataAllianceMember> members;
	ret = GetMembers(allianceId, members);
	for(vector<DataAllianceMember>::iterator itr = members.begin(); itr != members.end(); itr++)
	{
		DataUser user;
		ret = logicUser.GetUser(itr->uid, user);
		if(0 != ret)
		{
			continue;
		}
		if (user.alliance_id != itr->alliance_id)
		{
			CDataAllianceMember dbMember;
			ret = dbMember.RemoveMember(itr->alliance_id, itr->uid);
			if(ret != 0)
			{
				error_log("[RemoveMember fail][ret=%d,uid=%u,allianceId=%u]", ret, itr->uid, itr->alliance_id);
				DB_ERROR_RETURN_MSG("remove_alliance_member_fail");
			}
			itr = members.erase(itr);
			continue;
		}

		Json::Value &member = membersData[membersData.size()];
		member["uid"] = itr->uid;
		member["type"] = itr->type;
		member["point"] = Convert::UInt64ToString(itr->point);
		member["curr_point"] = Convert::UInt64ToString(itr->curr_point);
		member["jt"] = itr->join_time;

		Json::FromString(member["data"], itr->extra_data);

		Json::FromString(member["donate"], itr->donate_data);
		Json::Value &tempDonateData = member["donate"];
		tempDonateData["rank"] = itr->donate_rank;
		tempDonateData["drank"] = itr->donate_drank;

		CLogicUserBasic logicUserBasic;
		DataUserBasic userBasic;
		ret = logicUserBasic.GetUserBasicLimitSmart(itr->uid, OpenPlatform::GetType(), userBasic);
		if(ret == 0)
		{
			member["name"] = userBasic.name;
		}

		member["level"] = user.level;
		member["online"] = (Time::GetGlobalTime() - ONLINE_TIMEOUT < user.last_active_time) ? 1 : 0;
		member["lat"] = user.last_active_time;
		member["viplevel"] = user.viplevel;
		Json::Value userState;
		Json::FromString(userState, user.user_stat);
		int member_power = 0;
		Json::GetInt(userState, "power", member_power);
		member["power"] = member_power;

		map<unsigned, int>::iterator itrInteract = interacts.find(itr->uid);
		if(itrInteract != interacts.end())
		{
			member["ic"] = itrInteract->second;
		}
		CLogicMap logicMap;
		AttackPermitType permit = APT_ALLOW;
		if (userBy.alliance_id == allianceId)
		{
			permit = APT_BAN_ALLIANCE;
		}
		else
		{
			ret = logicMap.GetAttackPermit(user, userBy, permit);
		}
		member["attackpermitted"] = permit;
	}

	//sync member count
	if((int)members.size() != alliance.member_count)
	{
		info_log("[member_count_mismatch][allianceId=%u,mc=%d,rmc=%d]",
				allianceId, alliance.member_count, (int)members.size());
		alliance.member_count = members.size();
		ret = SetAllianceLimit(alliance);
	}

	return 0;
}

int CLogicAlliance::SearchAllianceJson(const string &name, Json::Value &value)
{
	int ret;
	CDataAllianceMapping dbMapping;
	unsigned allianceId;
	ret = dbMapping.GetMapping(name, allianceId);
	if(ret != 0)
	{
		if(ret == R_ERR_NO_DATA)
		{
			ERROR_RETURN_MSG(R_ERR_NO_DATA, "alliance_not_exist");
		}
		error_log("[GetMapping fail][ret=%d,name=%s]", ret, name.c_str());
		DB_ERROR_RETURN_MSG("get_alliance_mapping_fail");
	}

	DataAlliance alliance;
	ret = GetAllianceLimit(allianceId, alliance);
	if(ret != 0)
	{
		return ret;
	}
	value["aid"] = alliance.alliance_id;
	value["name"] = alliance.name;
	value["flag"] = alliance.flag;
	value["leader_uid"] = alliance.leader_uid;
	value["level"] = alliance.level;
	value["point"] = Convert::UInt64ToString(alliance.point);
	value["rank"] = alliance.rank;
	value["mc"] = alliance.member_count;
	value["enemyid"] = alliance.enemy_alliance_id;
	value["enemyname"] = alliance.enemy_alliance_name;
	value["createtime"] = alliance.create_time;
	value["desc"] = alliance.description;
	value["notice"] = alliance.notice;
	value["membercount"] = alliance.member_count;

	Json::Value extraData;
	Json::FromString(extraData, alliance.extra_data);
	if (extraData.isMember("activeD"))
	{
		value["activeD"] = extraData["activeD"];
	}
	if (extraData.isMember("attackD"))
	{
		value["attackD"] = extraData["attackD"];
	}

	CLogicUserBasic logicUserBasic;
	DataUserBasic userBasic;
	ret = logicUserBasic.GetUserBasicLimitSmart(alliance.leader_uid, OpenPlatform::GetType(), userBasic);
	if(ret == 0)
	{
		value["leader_name"] = userBasic.name;
	}

	CLogicUser logicUser;
	DataUser user;
	ret = logicUser.GetUserLimit(alliance.leader_uid, user);
	if(ret == 0)
	{
		value["leaderlevel"] = user.level;
	}

	return 0;
}

int CLogicAlliance::GetRandomAlliancesJson(Json::Value &value)
{
	int ret;
	vector<DataAlliance> alliances;
	ret = GetRandomAlliances(GET_ALLIANCE_COUNT, alliances);
	if(ret != 0)
	{
		return ret;
	}
	CLogicUserBasic logicUserBasic;
	CLogicUser logicUser;
	for(vector<DataAlliance>::const_iterator itr = alliances.begin(); itr != alliances.end(); itr++)
	{
		Json::Value &alliance = value[value.size()];
		alliance["aid"] = itr->alliance_id;
		alliance["name"] = itr->name;
		alliance["flag"] = itr->flag;
		alliance["leader"] = itr->leader_uid;
		alliance["level"] = itr->level;
		alliance["point"] = Convert::UInt64ToString(itr->point);
		alliance["rank"] = itr->rank;
		alliance["mc"] = itr->member_count;
		alliance["enemyid"] = itr->enemy_alliance_id;
		alliance["enemyname"] = itr->enemy_alliance_name;
		alliance["createtime"] = itr->create_time;
		alliance["desc"] = itr->description;

		DataUserBasic userBasic;
		ret = logicUserBasic.GetUserBasicLimitSmart(itr->leader_uid, OpenPlatform::GetType(), userBasic);
		if(ret == 0)
		{
			alliance["leadername"] = userBasic.name;
		}

		DataUser user;
		ret = logicUser.GetUserLimit(itr->leader_uid, user);
		if(ret == 0)
		{
			alliance["leaderlevel"] = user.level;
		}
	}
	return 0;
}

static bool InteractAttackTimeCompare(const DataUserInteract &left, const DataUserInteract &right)
{
	return left.last_attack_time > right.last_attack_time;
}

int CLogicAlliance::GetAllianceEnemys(unsigned uidBy, unsigned allianceId, unsigned startIndex, unsigned count, Json::Value &value)
{
	int ret;
	if(!IsAllianceId(allianceId))
	{
		return R_ERR_PARAM;
	}
	vector<DataUserInteract> interacts;
	CLogicUserInteract dbInteract;
	ret = dbInteract.GetInteractsAttackAfter(allianceId, Time::GetGlobalTime() - ALLIANCE_ENEMY_TIME, interacts);
	if(ret != 0)
	{
		error_log("[GetInteractsAttackAfter fail][ret=%d,allianceId=%u,time=%u]",
				ret, allianceId, Time::GetGlobalTime() - ALLIANCE_ENEMY_TIME);
		DB_ERROR_RETURN_MSG("get_user_interact_fail");
	}
	vector<DataUserInteract>::iterator itr;
	itr = interacts.begin();
	while(itr != interacts.end())
	{
		if(!IsValidUid(itr->opposite_uid) || itr->attack_from <= 0)
		{
			itr = interacts.erase(itr);
		}
		else
		{
			itr++;
		}
	}
	value["total"] = (unsigned)interacts.size();
	if(interacts.size() <= startIndex)
	{
		ERROR_RETURN_MSG(R_ERR_NO_DATA, "no_more_enemys");
	}
	sort(interacts.begin(), interacts.end(), InteractAttackTimeCompare);

	CLogicUser logicUser;
	DataUser userBy;
	ret = logicUser.GetUserLimit(uidBy, userBy);
	if (ret != 0)
		return ret;

	Json::Value &enemys = value["enemys"];
	enemys = Json::Value(Json::arrayValue);
	for(size_t i = startIndex; i < interacts.size() && enemys.size() < count; i++)
	{
		Json::Value &enemy = enemys[enemys.size()];
		const DataUserInteract &interact = interacts[i];
		enemy["uid"] = interact.opposite_uid;
		enemy["attackfrom"] = interact.attack_from;
		enemy["attackto"] = interact.attack_to;
		enemy["retaliatecount"] = interact.retaliate_count;
		enemy["lastattacktime"] = interact.last_attack_time;

		CLogicUserBasic logicUserBasic;
		DataUserBasic userBasic;
		ret = logicUserBasic.GetUserBasicLimitSmart(interact.opposite_uid, OpenPlatform::GetType(), userBasic);
		if(ret == 0)
		{
			enemy["name"] = userBasic.name;
		}

		CLogicUser logicUser;
		DataUser user;
		ret = logicUser.GetUserLimit(interact.opposite_uid, user);
		if(ret == 0)
		{
			enemy["level"] = user.level;
			enemy["online"] = (Time::GetGlobalTime() - ONLINE_TIMEOUT < user.last_active_time) ? 1 : 0;
			enemy["lat"] = user.last_active_time;
		}

		if(IsAllianceId(user.alliance_id))
		{
			DataAlliance alliance;
			ret = GetAllianceLimit(user.alliance_id, alliance);
			if(ret == 0)
			{
				enemy["aid"] = alliance.alliance_id;
				enemy["aname"] = alliance.name;
				enemy["aflag"] = alliance.flag;
			}
		}
		CLogicMap logicMap;
		AttackPermitType permit = APT_ALLOW;
		logicMap.GetAttackPermit(user, userBy, permit);
		enemy["attackpermitted"] = permit;
	}
	return 0;
}

int CLogicAlliance::GetMemberMaxCount(int level)
{
	const int memberRange[10] = {20,25,30,35,40,45,50,55,60,65};
	return memberRange[level-1];
}

int CLogicAlliance::AddMemberDonate(unsigned uid, unsigned allianceId, int r1, int r2, int r3, int r4, int r5, int cash)
{
	if(r1 < 0 || r2 < 0 || r3 < 0 || r4 < 0 || r5 < 0 || cash < 0)
	{
		error_log("[donate_param_error][uid=%u,r1=%d,r2=%d,r3=%d,r4=%d,r5=%d,cash=%d]", uid, r1, r2, r3, r4, r5, cash);
		PARAM_ERROR_RETURN();
	}

	int ret;
	DataAllianceMember member;
	ret = GetMember(allianceId, uid, member);
	if(ret != 0)
	{
		return ret;
	}

	/*Json::Value data;
	Json::FromString(data, member.extra_data);
	int donate = 0;
	Json::GetInt(data, "donate", donate);
	donate += (r1 + r2 + r3 + r4 + r5) / 10000 + cash;
	data["donate"] = donate;
	member.extra_data = Json::ToString(data);*/

	Json::Value donate_data;
	if (member.donate_data.empty())
	{
		member.donate_data = "{}";
	}

	if (!Json::FromString(donate_data, member.donate_data))
	{
		error_log("Parse donate data faile!");
		return R_ERR_DATA;
	}

	unsigned temp_resource = 0;
	unsigned currCash = 0;
	unsigned donate_time = 0;
	Json::GetUInt(donate_data,"ts", donate_time);

	if ( (Time::GetGlobalTime() - donate_time) > 24*60*60) //不是当天的
	{
		donate_data["ts"] = Time::GetGlobalTime();
		donate_data["dr1"] = r1;
		donate_data["dr2"] = r2;
		donate_data["dr3"] = r3;
		donate_data["dr4"] = r4;
		donate_data["dcash"] = cash;
	}
	else
	{
		temp_resource = 0;
		Json::GetUInt(donate_data, "dr1", temp_resource);
		donate_data["dr1"] = (unsigned)(r1 + temp_resource);

		temp_resource = 0;
		Json::GetUInt(donate_data, "dr2", temp_resource);
		donate_data["dr2"] = r2 + temp_resource;

		temp_resource = 0;
		Json::GetUInt(donate_data, "dr3", temp_resource);
		donate_data["dr3"] = r3 + temp_resource;

		temp_resource = 0;
		Json::GetUInt(donate_data, "dr4", temp_resource);
		donate_data["dr4"] = r4 + temp_resource;

		currCash = 0;
		Json::GetUInt(donate_data, "dcash", currCash);
		donate_data["dcash"] = cash + currCash;
	}

	temp_resource = 0;
	Json::GetUInt(donate_data, "r1", temp_resource);
	donate_data["r1"] = r1 + temp_resource;

	temp_resource = 0;
	Json::GetUInt(donate_data, "r2", temp_resource);
	donate_data["r2"] = r2 + temp_resource;

	temp_resource = 0;
	Json::GetUInt(donate_data, "r3", temp_resource);
	donate_data["r3"] = r3 + temp_resource;

	temp_resource = 0;
	Json::GetUInt(donate_data, "r4", temp_resource);
	donate_data["r4"] = r4 + temp_resource;

	currCash = 0;
	Json::GetUInt(donate_data, "cash", currCash);
	donate_data["cash"] = cash + currCash;
	member.donate_data = Json::ToString(donate_data);

	ret = SetMember(member);
	if(ret != 0)
	{
		return ret;
	}

	ret = RankMemberDonate(allianceId);
	if (0 != ret)
	{
		error_log("RankMemberDonate error!");
	}
	return R_SUCCESS;
}

int CLogicAlliance::RankMemberDonate(unsigned allianceId)
{
	int ret = 0;
	CDataAllianceMember dbMember;
	vector<DataAllianceMember> members;
	ret = dbMember.GetMembers(allianceId, members);
	if (0 != ret)
	{
		error_log("[Get alliance member list failed!][aid=%u]", allianceId);
		return ret;
	}

	map<unsigned, DataAllianceMember > memberMap;
	vector<DataAllianceMember>::iterator itr = members.begin();
	for (itr = members.begin(); itr!=members.end(); ++itr)
	{
		memberMap[itr->uid] = *itr;
	}

	vector<DataMemberDonate> membersForDonateTotal;
	vector<DataMemberDonate> membersForTodayDonate;
	ret = ParseMemberDonate(members, membersForDonateTotal);
	if (0 != ret)
	{
		return ret;
	}
	ret = ParseMemberDonate(members, membersForTodayDonate);
	if (0 != ret)
	{
		return ret;
	}

	sort(membersForDonateTotal.begin(), membersForDonateTotal.end(), CompareDonateTotal);
	int rank = 1;
	vector<DataMemberDonate>::iterator donateItr = membersForDonateTotal.begin();
	for (; donateItr!=membersForDonateTotal.end(); ++donateItr)
	{

		if (donateItr->donate_total == 0)
		{
			memberMap[donateItr->uid].donate_rank = 0;
		}
		else
		{
			memberMap[donateItr->uid].donate_rank = rank;
		}
		++rank;
	}
	for (donateItr = membersForDonateTotal.begin(); donateItr!=membersForDonateTotal.end(); ++donateItr)
	{
		info_log("uid=%u,donate_total=%d.", donateItr->uid, donateItr->donate_total);
	}
	sort(membersForTodayDonate.begin(), membersForTodayDonate.end(), CompareTodayDonate);
	int drank = 1;
	for (donateItr = membersForTodayDonate.begin(); donateItr!=membersForTodayDonate.end(); ++donateItr)
	{
		if (donateItr->today_donate == 0)
		{
			memberMap[donateItr->uid].donate_drank = 0;
		}
		else
		{
			memberMap[donateItr->uid].donate_drank = drank;
		}
		++drank;
	}

	for (itr = members.begin(); itr!=members.end(); ++itr)
	{
		ret = SetMember(memberMap[itr->uid]);
		if (0 != ret)
		{
			error_log("[Set alliance member failed!][aid=%u,uid=%u,ret=%d]",allianceId,itr->uid,ret);
			continue;
		}
	}
	return 0;
}

int CLogicAlliance::AllianceEmail(unsigned allianceId, unsigned uid, DataEmail &data)
{
	if (!IsAllianceId(allianceId))
	{
		error_log("%u is not alliance id!", allianceId);
		PARAM_ERROR_RETURN();
	}
	int ret = 0;
	DataAlliance alliance;
	ret = GetAllianceLimit(allianceId,alliance);
	if (0 != ret)
	{
		return ret;
	}

	if (uid != alliance.leader_uid)
	{
		error_log("[is_not_leader][aid=%u,uid=%u]", allianceId, uid);
		ERROR_RETURN_MSG(R_ERR_LOGIC, "is_not_leader");
	}

	CDataAllianceMember dbMembers;
	vector<DataAllianceMember> members;
	ret = dbMembers.GetMembers(allianceId,members);
	if (0 != ret)
	{
		error_log("Get members failed! aid=%u,ret=%d",allianceId,ret);
		return ret;
	}

	CDataEmail cDataEmail;
	CDataUserBasic dbUserBasic;
	string name;

	data.opposite_uid = alliance.leader_uid;
	data.post_flag = 1;
	ret = dbUserBasic.GetUserName(alliance.leader_uid, OpenPlatform::GetType(), name);
	if (0 == ret)
	{
		data.from_name = name;
	}

	for (vector<DataAllianceMember>::iterator itr=members.begin(); itr!=members.end(); ++itr)
	{
		if (alliance.leader_uid == itr->uid)
		{
			continue;
		}
		ret = dbUserBasic.GetUserName(itr->uid, OpenPlatform::GetType(), name);
		if (0 != ret)
		{
			error_log("GetUserName faile uid=%u", itr->uid);
			continue;
		}
		data.uid = itr->uid;
		data.to_name = name;
		ret = cDataEmail.AddEmail(data);
		if (0 != ret)
		{
			error_log("AddEmail failed! ret=%d", ret);
			continue;
		}
		Json::Value updates;
		CLogicUpdates logicUpdates;
		updates[0u]["s"] = "NEWEMAIL";
		updates[0u]["ts"] = Time::GetGlobalTime();
		updates[0u]["name"] = data.from_name;
		ret = logicUpdates.AddUpdates(data.uid,updates);
		if(0 != ret)
		{
			error_log("add updates failed ,uid=%u",data.uid);
		}
	}

	return 0;
}

int CLogicAlliance::DonatePoint(int level,int r1, int r2, int r3, int r4, int r5, int &points)
{
	points = 0;
	double resource = (double)(r1 + r2 + r3 + r4 + r5)*10000;
	if(resource <= 0)
	{
		return 0;
	}

	double nPoint;
	double tempData = pow(level+1, 3.767) + 5000;
	nPoint = (double)resource / tempData;
	points = (int)(nPoint * 10);

	if(points <= 0)
	{
		points = 0;
	}
	return 0;
}
