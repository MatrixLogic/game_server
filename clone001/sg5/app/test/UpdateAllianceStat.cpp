#include "LogicInc.h"
#undef printf

#define ALLIANCE_REMOVE (-2)
#define SEVEN_DAY_SECONDD (7 * 24 * 60 * 60)

static bool AlliancePointCompare(const DataAlliance &left, const DataAlliance &right)
{
	return left.point > right.point;
}
static bool CompaerMemberPoint(const DataAllianceMember &left, const DataAllianceMember &right)
{
	return left.point > right.point;
}

class UpdateAlliance
{
public:
	int InitAllianceList(vector<unsigned>& db);
	int RankAlliance(void);
	int CleanDonate(void);
	int AutoChangeLeader(void);
	int UpdataeDegree(void);
	int GetRankAllianceJson(Json::Value &result);
	int GetAllianceEnemy(void);
	int UpdateAllianceRank(void);
	int RewardFastAlliance(void);

public:
	void GetAllianceList(vector<DataAlliance> &alliances);

private:
	vector<DataAlliance> m_AllianceList;
	set<unsigned> m_ActiveAlliances;
};

class UpdateAllianceMember
{
public:
	UpdateAllianceMember();
	int Init(unsigned allianceId);
	int Rank(void);
	int CheckLeaderLastLoginTs();
	int CheckManagerLastLoginTs();
	int CheckGeneralLastLoginTs();
	int CheckMemberLastLoginTs();
	int GetLeader(unsigned &uid);
private:
	unsigned m_Leader;
	vector<DataAllianceMember> m_Manager;
	vector<DataAllianceMember> m_General;
	vector<DataAllianceMember> m_Members;
};

int UpdateAlliance::InitAllianceList(vector<unsigned>& db)
{
	if(db.empty())
	{
		int ret = 0;
		CLogicIdCtrl logicIdCtrl;
		uint64_t u64Id;
		ret = logicIdCtrl.GetCurrentId(KEY_ALLIANCE_ID_CTRL, u64Id);
		if(ret != 0)
		{
			printf("get alliance id fail %d\n", ret);
			return ret;
		}
		unsigned allianceId = 0;
		if (!Config::GetUIntValue(allianceId, CONFIG_AID_START))
		{
			allianceId = ALLIANCE_ID_START;
		}

		CLogicAlliance logicAlliance;
		for(; allianceId <= (unsigned)u64Id; allianceId++)
		{
			DataAlliance alliance;
			ret = logicAlliance.GetAlliance(allianceId, alliance);
			if (0 == ret)
			{
				if (alliance.remove_time != 0 && alliance.remove_time > Time::GetGlobalTime())
				{
					ret = logicAlliance.RemoveAlliance(allianceId, true);
					if (0 != ret)
					{
						printf("[RemoveAlliance fail][aid=%u,ret=%d]\n",allianceId, ret);
					}
					else
					{
						printf("[RemoveAlliance success][aid=%u,removeTs=%u,nowTs=%u]\n",
								allianceId, alliance.remove_time, Time::GetGlobalTime());
					}
					continue;
				}
				m_AllianceList.push_back(alliance);
				m_ActiveAlliances.insert(alliance.alliance_id);
			}
			else if (R_ERR_NO_DATA != ret)
			{
				printf("[GetAlliance fail][aid=%u,ret=%d]\n",allianceId,ret);
			}
		}
	}
	else
	{
		for(int i=0;i<db.size();++i)
		{
			int ret = 0;
			CLogicIdCtrl logicIdCtrl;
			uint64_t u64Id;
			ret = logicIdCtrl.GetCurrentId(KEY_ALLIANCE_ID_CTRL, u64Id, db[i]);
			if(ret != 0)
			{
				printf("get alliance id fail %d\n", ret);
				return ret;
			}
			unsigned allianceId = ALLIANCE_ID_START + 500000*(db[i]-1);

			CLogicAlliance logicAlliance;
			for(; allianceId <= (unsigned)u64Id; allianceId++)
			{
				DataAlliance alliance;
				ret = logicAlliance.GetAlliance(allianceId, alliance);
				if (0 == ret)
				{
					if (alliance.remove_time != 0 && alliance.remove_time > Time::GetGlobalTime())
					{
						ret = logicAlliance.RemoveAlliance(allianceId, true);
						if (0 != ret)
						{
							printf("[RemoveAlliance fail][aid=%u,ret=%d]\n",allianceId, ret);
						}
						else
						{
							printf("[RemoveAlliance success][aid=%u,removeTs=%u,nowTs=%u]\n",
									allianceId, alliance.remove_time, Time::GetGlobalTime());
						}
						continue;
					}
					m_AllianceList.push_back(alliance);
					m_ActiveAlliances.insert(alliance.alliance_id);
				}
				else if (R_ERR_NO_DATA != ret)
				{
					printf("[GetAlliance fail][aid=%u,ret=%d]\n",allianceId,ret);
				}
			}
		}
	}
	return 0;
}

int UpdateAlliance::CleanDonate(void)
{
	vector<DataAlliance>::iterator aItr = m_AllianceList.begin();
	for (; aItr!=m_AllianceList.end(); aItr++)
	{
		//重置每日捐献数值
		int ret = 0;
		CDataAllianceMember dbmember;
		vector<DataAllianceMember> members;
		ret = dbmember.GetMembers(aItr->alliance_id,members);
		if (0 != ret)
		{
			printf("[CleanDonate][Get member failed][ret=%d,aid=%u]\n",ret,aItr->alliance_id);
			return ret;
		}
		vector<DataAllianceMember>::iterator itr = members.begin();
		Json::Value donateData;
		for (; itr!=members.end(); ++itr)
		{
			itr->donate_drank = 0;
			if (itr->donate_data.empty())
			{
				itr->donate_data = "{}";
			}
			if (!Json::FromString(donateData, itr->donate_data))
			{
				printf("[CleanDonate][Parse donate data faile][uid=%u]\n", itr->uid);
			}
			donateData["dr1"] = 0;
			donateData["dr2"] = 0;
			donateData["dr3"] = 0;
			donateData["dr4"] = 0;
			donateData["dcash"] = 0;
			itr->donate_data = Json::ToString(donateData);
			ret = dbmember.SetMember(*itr);
			if (0 != ret)
			{
				printf("CleanDonate SetMember faile!uid=%u.\n", itr->uid);
			}
		}
	}

	return 0;
}

int UpdateAlliance::AutoChangeLeader(void)
{
	//联盟盟主如连续7天没有上线，系统自动弹劾盟主.
	int ret = 0;
	CDataUser dbUser;
	DataUser user;
	CLogicAlliance logicAlliance;
	vector<DataAlliance>::iterator aItr = m_AllianceList.begin();
	for (; aItr!=m_AllianceList.end(); aItr++)
	{
		UpdateAllianceMember UpMember;
		ret = UpMember.Init(aItr->alliance_id);
		if (0 != ret)
		{
			continue;
		}
		UpMember.Rank();
		ret = UpMember.CheckLeaderLastLoginTs();
		if (0 != ret)
		{
			ret = UpMember.CheckManagerLastLoginTs();
			if (0 != ret)
			{
				ret = UpMember.CheckGeneralLastLoginTs();
				if (0 != ret)
				{
					UpMember.CheckMemberLastLoginTs();
				}
			}
		}

		unsigned new_leader = 0;
		UpMember.GetLeader(new_leader);
		if (0 == new_leader)
		{
			printf("[AutoChangeLeader][all member last login seven days ago][aid=%u]\n", aItr->alliance_id);
			ret = logicAlliance.RemoveAlliance(aItr->alliance_id, true);
			if (0 != ret)
			{
				printf("[AutoChangeLeader][remove alliance fail][aid=%u,ret=%d]\n", aItr->alliance_id,ret);
			}
			aItr = m_AllianceList.erase(aItr);
			continue;
		}
		if (new_leader != aItr->leader_uid)
		{
			printf("[AutoChangeLeader][need change leader][aid=%u,old_leader=%u,new_leader=%u]\n",
					aItr->alliance_id, aItr->leader_uid, new_leader);
			ret = logicAlliance.ChangeLeader(aItr->alliance_id, new_leader);
			if (0 != ret)
			{
				printf("[AutoChangeLeader][ChangeLeader fail][aid=%u,ret=%d,new_leader=%u]\n",
						aItr->alliance_id,ret,new_leader);
			}
			else
			{
				aItr->leader_uid = new_leader;
				printf("[AutoChangeLeader][change leader success][aid=%u,newLaeder=%u]\n",
						aItr->alliance_id,new_leader);
			}
		}
	}

	return 0;
}

int UpdateAlliance::UpdataeDegree(void)
{
	int ret = 0;
	CLogicAlliance logicAlliance;
	vector<DataAllianceMember> members;
	vector<DataAlliance>::iterator aItr = m_AllianceList.begin();
	for (; aItr != m_AllianceList.end(); aItr++)
	{
		int activeDegree = 0;
		int attackDegree = 0;
		CLogicUser logicUer;
		DataUser user;
		members.clear();
		ret = logicAlliance.GetMembers(aItr->alliance_id, members);
		if (0 == ret)
		{
			int activeUser = 0;
			for (vector<DataAllianceMember>::iterator itr = members.begin(); itr != members.end(); ++itr)
			{
				ret = logicUer.GetUserLimit(itr->uid, user);
				if (0 != ret)
				{
					continue;
				}
				if (user.last_active_time > Time::GetGlobalTime() - 3*24*60*60)
				{
					activeUser++;
				}
			}

			if (aItr->member_count > 0)
			{
				activeDegree = (int)((double)activeUser / (double)aItr->member_count * 100);
			}
		}

		vector<DataUserInteract> interacts;
		CDataUserInteract dbInteract;
		int ret = dbInteract.GetInteractsAttackAfter(aItr->alliance_id, Time::GetGlobalTime() - 3*24*60*60, interacts);
		if(ret == 0)
		{
			if (aItr->member_count > 0)
			{
				attackDegree = (int)((double)interacts.size() / (double)aItr->member_count / 3 * 100);
			}
		}
		DataAlliance alliance;
		ret = logicAlliance.GetAlliance(aItr->alliance_id, alliance);
		if(ret != 0)
		{
			continue;
		}
		Json::Value extraData;
		Json::FromString(extraData, alliance.extra_data);
		extraData["activeD"] = activeDegree;
		extraData["attackD"] = attackDegree;
		alliance.extra_data = Json::ToString(extraData);
		ret = logicAlliance.SetAlliance(alliance);
		if(ret != 0)
		{
			continue;
		}
	}
	return 0;
}

int UpdateAlliance::RankAlliance(void)
{
	//按等级大小排序，同等级的按战功大小排序
	map<int, vector<DataAlliance> > allianceMap;
	allianceMap.clear();
	vector<DataAlliance>::iterator itr = m_AllianceList.begin();

	for (; itr!=m_AllianceList.end(); ++itr)
	{
		allianceMap[itr->level-1].push_back(*itr);
	}

	int i = 0;
	for (i=0; i<10; ++i)
	{
		sort(allianceMap[i].begin(), allianceMap[i].end(), AlliancePointCompare);
	}

	m_AllianceList.clear();
	for (i=10-1; i>=0; --i)
	{
		for (itr = allianceMap[i].begin(); itr!=allianceMap[i].end(); ++itr)
		{
			m_AllianceList.push_back(*itr);
		}
	}
	return 0;
}

int UpdateAlliance::GetAllianceEnemy(void)
{
	vector<DataUserInteract> interacts;
	CDataUserInteract dbInteract;
	vector<DataAlliance>::iterator aItr = m_AllianceList.begin();
	for (; aItr != m_AllianceList.end(); aItr++)
	{
		int ret = dbInteract.GetInteractsAttackAfter(aItr->alliance_id, Time::GetGlobalTime() - ALLIANCE_ENEMY_TIME, interacts);
		if(ret != 0)
		{
			printf("[GetAllianceEnemy][get alliance interacts fail][aid=%u,ret=%d]\n", aItr->alliance_id, ret);
			continue;
		}

		int maxAttack = 0;
		int maxDefence = 0;
		unsigned enemyAllianceId = 0;
		vector<DataUserInteract>::iterator itrInteract = interacts.begin();
		for(; itrInteract != interacts.end(); itrInteract++)
		{
			if(IsAllianceId(itrInteract->opposite_uid) &&
				m_ActiveAlliances.find(itrInteract->opposite_uid) != m_ActiveAlliances.end() &&
				(itrInteract->attack_from > maxDefence ||
				(itrInteract->attack_from == maxDefence && itrInteract->attack_to > maxAttack)))
			{
				maxAttack = itrInteract->attack_to;
				maxDefence = itrInteract->attack_from;
				enemyAllianceId = itrInteract->opposite_uid;
			}
		}

		if (IsAllianceId(enemyAllianceId))
		{
			CLogicAlliance logicAlliance;
			DataAlliance alliance;
			ret = logicAlliance.GetAlliance(aItr->alliance_id,alliance);
			if (0 == ret)
			{
				alliance.enemy_alliance_id = enemyAllianceId;
				DataAlliance enemyAlliance;
				ret = logicAlliance.GetAlliance(alliance.enemy_alliance_id,enemyAlliance);
				if ( 0 == ret)
				{
					alliance.enemy_alliance_name = enemyAlliance.name;
				}
				ret = logicAlliance.SetAlliance(alliance);
				if (0 != ret)
				{
					printf("[set alliance enemy fail][aid=%u,eaid=%u,ret=%d]\n",
							alliance.alliance_id,alliance.enemy_alliance_id,ret);
				}
			}
		}

	}
	return 0;
}

int UpdateAlliance::GetRankAllianceJson(Json::Value &reslt)
{
	int ret = 0;
	vector<DataAlliance>::iterator aItr = m_AllianceList.begin();
	int rank = 0;
	for (; aItr != m_AllianceList.end(); aItr++)
	{
		++rank;
		Json::Value &allianceData = reslt[reslt.size()];
		allianceData["aid"] = aItr->alliance_id;
		allianceData["kingdom"] = aItr->kingdom;
		allianceData["name"] = aItr->name;
		allianceData["flag"] = aItr->flag;
		allianceData["leader"] = aItr->leader_uid;
		allianceData["level"] = aItr->level;
		allianceData["point"] = Convert::UInt64ToString(aItr->point);
		allianceData["rank"] = rank;
		allianceData["mc"] = aItr->member_count;
		allianceData["enemyid"] = aItr->enemy_alliance_id;
		allianceData["enemyname"] = aItr->enemy_alliance_name;
		allianceData["createtime"] = aItr->create_time;
		allianceData["desc"] = aItr->description;
		allianceData["notice"] = aItr->notice;
		allianceData["ts"] = Time::GetGlobalTime();
		CLogicUserBasic logicUserBasic;
		DataUserBasic userBasic;
		ret = logicUserBasic.GetUserBasicLimitSmart(aItr->leader_uid, PT_TEST, userBasic);
		if(ret == 0)
		{
			allianceData["leadername"] = userBasic.name;
			allianceData["openid"] = userBasic.open_id;
		}
		CLogicUser logicUser;
		DataUser user;
		ret = logicUser.GetUserLimit(aItr->leader_uid, user);
		if(ret == 0)
		{
			allianceData["leaderlevel"] = user.level;
		}
	}
	return 0;
}

int UpdateAlliance::UpdateAllianceRank(void)
{
	int ret = 0;
	CLogicAlliance logicAlliance;
	DataAlliance alliance;
	vector<DataAlliance>::iterator aItr = m_AllianceList.begin();
	int rank = 0;
	for (; aItr != m_AllianceList.end(); aItr++)
	{
		++rank;
		ret = logicAlliance.GetAllianceLimit(aItr->alliance_id, alliance);
		if (0 != ret)
		{
			printf("[UpdateAllianceRank][GetAllianceLimit fail][aid=%u,ret=%d]\n",aItr->alliance_id,ret);
			continue;
		}
		if (alliance.rank == rank)
		{
			continue;
		}
		alliance.rank = rank;
		ret = logicAlliance.SetAllianceLimit(alliance);
		if (0 != ret)
		{
			printf("[UpdateAllianceRank][SetAllianceLimit fail][aid=%u,ret=%d]\n",aItr->alliance_id,ret);
		}
	}
	return 0;
}


void UpdateAlliance::GetAllianceList(vector<DataAlliance> &alliances)
{
	alliances = m_AllianceList;
	return ;
}

int UpdateAlliance::RewardFastAlliance(void)
{
	unsigned ui_open_ts = 0;
	if (!Config::GetUIntValue(ui_open_ts, CONFIG_OPEN_TS) || 0 == ui_open_ts)
	{
		printf("Get CONFIG_OPEN_TS from config fail!ui_open_ts=%u.\n", ui_open_ts);
		return R_ERR_DATA;
	}

	if (!Time::IsToday(ui_open_ts + 7 * 24 * 60 * 60))
	{
		return R_ERR_LOGIC;
	}

	int result = 0;
	CLogicAlliance logicAlliance;
	DataAlliance alliance;
	unsigned RecReward = 0;
	unsigned memCoinReward = 0;
	unsigned leaderCoinReward = 0;
	vector<uint64_t> uidList;
	vector<DataAlliance>::iterator itr = m_AllianceList.begin();
	for(; itr != m_AllianceList.end(); itr++)
	{
		 unsigned aid = itr->alliance_id;
		 vector<DataAllianceMember> vVembers;
		 result = logicAlliance.GetMembers(aid,vVembers);
		 if(result != 0)
		 {
			 printf("[get aillance member failed][aid=%u,ret=%d]\n", aid, result);
			 continue;
		 }
		 unsigned level = itr->level;
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

		 CLogicUser logicUser;
		 DataUser user;
		 CLogicPay logicPay;

		 for(unsigned index = 0; index < vVembers.size();index++)
		 {
			 result = logicUser.GetUserLimit(vVembers[index].uid, user);
			 if(result != 0)
			 {
				 printf("Get user fail,uid=%u,result=%d.\n", vVembers[index].uid, result);
				 continue;
			 }

			 if(user.uid != alliance.leader_uid)
			 {
				 result = logicPay.ChangePay(user.uid,0,memCoinReward,"ALLIANCE_RUSH_REWARD",1);
				 if(result != 0)
				 {
					 printf("change cash failed,uid=%u,coins=%u,result=%d.\n",user.uid, memCoinReward,result);
				 }
			 }
			 else
			 {
				 result = logicPay.ChangePay(user.uid,0,leaderCoinReward,"ALLIANCE_RUSH_REWARD",1);
				 if(result != 0)
				 {
					 printf("change cash failed,uid=%u,coins=%u,ret=%d.\n",user.uid,leaderCoinReward,result);
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
				 printf("Set user fail,uid=%u,result=%d", vVembers[index].uid, result);
				 continue;
			 }

			 printf("uid=%u,memcash=%u,leadercoins=%u,recreward=%u.\n",
					 vVembers[index].uid,memCoinReward,leaderCoinReward,RecReward);
		 }
		 vVembers.clear();
	}

	CLogicEmail logicEmail;
	DataEmail email;
	email.attach_flag = 0;
	email.post_ts = Time::GetGlobalTime();
	email.title = "联盟冲级奖励";
	email.text = "联盟冲级奖励已发放完毕（无需提取附件，系统已自动发放）,如有疑问，请联系GM";
	email.uid = ADMIN_UID;
	result = logicEmail.AddEmail(email,uidList,(PlatformType)2);
	if(result != 0)
	{
		printf("send email fail,ret=%d\n", result);
	}

	return 0;
}

UpdateAllianceMember::UpdateAllianceMember()
{
	 m_Leader = 0;
}

int UpdateAllianceMember::Init(unsigned allianceId)
{
	int ret = 0;
	CDataAllianceMember dbAllianceMember;
	vector <DataAllianceMember> members;
	ret = dbAllianceMember.GetMembers(allianceId,members);
	if (0 != ret)
	{
		printf("[UpdateAllianceMember init][GetMembers fail][aid=%u,ret=%d]\n",allianceId,ret);
		return ret;
	}
	vector <DataAllianceMember>::iterator itr = members.begin();
	for (; itr != members.end(); ++itr)
	{
		if (itr->type == AMT_LEADER)
		{
			m_Leader = itr->uid;
		}
		else if (itr->type == AMT_MANAGER)
		{
			m_Manager.push_back(*itr);
		}
		else if (itr->type == AMT_GENERAL)
		{
			m_General.push_back(*itr);
		}
		else if (itr->type == AMT_NORMAL)
		{
			m_Members.push_back(*itr);
		}
		else
		{
			printf("[UpdateAllianceMember init][not this type][type=%d]\n",itr->type);
		}
	}
	return 0;
}

int UpdateAllianceMember::Rank(void)
{
	sort(m_Manager.begin(),m_Manager.end(),CompaerMemberPoint);
	sort(m_General.begin(),m_General.end(),CompaerMemberPoint);
	sort(m_Members.begin(),m_Members.end(),CompaerMemberPoint);
	return 0;
}

int UpdateAllianceMember::CheckLeaderLastLoginTs()
{
	int ret = 0;
	CDataUser dbUser;
	DataUser user;
	ret = dbUser.GetUserLimit(m_Leader,user);
	if (0 != ret)
	{
		printf("[CheckLeaderLastLoginTs][get user fail][uid=%u,ret=%d]\n",m_Leader, ret);
		return ret;
	}
	if (user.last_active_time + SEVEN_DAY_SECONDD < Time::GetGlobalTime())
	{
		m_Leader = 0;
		return R_ERROR;
	}
	return 0;
}

int UpdateAllianceMember::CheckManagerLastLoginTs()
{
	int ret = 0;
	CDataUser dbUser;
	DataUser user;
	vector<DataAllianceMember>::iterator itr = m_Manager.begin();
	for (;itr != m_Manager.end(); ++itr)
	{
		ret = dbUser.GetUserLimit(itr->uid, user);
		if (0 != ret)
		{
			continue;
		}
		if (	user.last_active_time + SEVEN_DAY_SECONDD > Time::GetGlobalTime()
			&& 0 == m_Leader)
		{
			m_Leader = itr->uid;
			return 0;
		}
	}
	return R_ERROR;
}

int UpdateAllianceMember::CheckGeneralLastLoginTs()
{
	int ret = 0;
	CDataUser dbUser;
	DataUser user;
	vector<DataAllianceMember>::iterator itr = m_General.begin();
	for (;itr != m_General.end(); ++itr)
	{
		ret = dbUser.GetUserLimit(itr->uid, user);
		if (0 != ret)
		{
			continue;
		}
		if (	user.last_active_time + SEVEN_DAY_SECONDD > Time::GetGlobalTime()
			&& 0 == m_Leader)
		{
			m_Leader = itr->uid;
			return 0;
		}
	}
	return R_ERROR;
}

int UpdateAllianceMember::CheckMemberLastLoginTs()
{
	int ret = 0;
	CDataUser dbUser;
	DataUser user;
	vector<DataAllianceMember>::iterator itr = m_Members.begin();
	for (;itr != m_Members.end(); ++itr)
	{
		ret = dbUser.GetUserLimit(itr->uid, user);
		if (0 != ret)
		{
			continue;
		}
		if (	user.last_active_time + SEVEN_DAY_SECONDD > Time::GetGlobalTime()
			&& 0 == m_Leader)
		{
			m_Leader = itr->uid;
			return 0;
		}
	}
	return R_ERROR;
}

int UpdateAllianceMember::GetLeader(unsigned &uid)
{
	uid = m_Leader;
	return 0;
}

int main(int argc, char *argv[])
{
	Kernel::Init(APP_CONFIG_PATH);
	Config::SetDomain(0);
	int ret;

	vector<unsigned> db;
	for(int i=1;i<argc;++i)
		db.push_back(atoi(argv[i]));
	UpdateAlliance UpAlliance;
	ret = UpAlliance.InitAllianceList(db);
	if (0 != ret)
	{
		printf("[InitAllianceList fail][ret=%d]\n", ret);
		return ret;
	}
	ret = UpAlliance.RankAlliance();
	if (0 != ret)
	{
		printf("[RankAlliance fail][ret=%d]\n", ret);
		return ret;
	}

	ret = UpAlliance.RewardFastAlliance();
	if (0 == ret)
	{
		printf("[RewardFastAlliance success!][ret=%d]\n", ret);
	}

	ret = UpAlliance.AutoChangeLeader();
	if (0 != ret)
	{
		printf("[AutoChangeLeader fail][ret=%d]\n", ret);
	}
	ret = UpAlliance.CleanDonate();
	if (0 != ret)
	{
		printf("[CleanDonate fail][ret=%d]\n", ret);
	}
	ret = UpAlliance.UpdataeDegree();
	if (0 != ret)
	{
		printf("[UpdataeDegree fail][ret=%d]\n", ret);
	}
	ret = UpAlliance.GetAllianceEnemy();
	if (0 != ret)
	{
		printf("[GetAllianceEnemy fail][ret=%d]\n", ret);
	}
	ret = UpAlliance.UpdateAllianceRank();
	if (0 != ret)
	{
		printf("[UpdateAllianceRank fail][ret=%d]\n", ret);
	}

	Json::Value allianceRankPoint = Json::Value(Json::arrayValue);
	ret = UpAlliance.GetRankAllianceJson(allianceRankPoint);
	if (0 != ret)
	{
		printf("[GetRankAllianceJson fail][ret=%d]\n", ret);
		return ret;
	}

	string allianceRankPath_top10 = Config::GetValue("alliance_rank");
	string allianceRankPath_point = Config::GetValue("alliance_rank_point");
 	if(allianceRankPath_top10.empty() || allianceRankPath_point.empty() )
	{
		allianceRankPath_point = "../webroot/data/alliance_rank_point";
		allianceRankPath_top10 = "../webroot/data/alliance_rank";
	}
 	Json::Value allianceRankTop10;
 	for(int i=0;i<10;i++)
 	{
 		allianceRankTop10[i] = allianceRankPoint[i];
 	}
	ret = File::Write(allianceRankPath_top10, Json::ToString(allianceRankTop10));
	if(ret != 0)
	{
		printf("write alliance rank %s fail %d\n", allianceRankPath_top10.c_str(), ret);
	}
	ret = File::Write(allianceRankPath_point, Json::ToString(allianceRankPoint));
	if(ret != 0)
	{
		printf("write alliance rank1 %s fail %d\n", allianceRankPath_point.c_str(), ret);
	}
	printf("update alliances stat ok\n");
	return 0;
}
