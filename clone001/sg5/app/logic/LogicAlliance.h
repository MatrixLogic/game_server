/*
 * LogicAlliance.h
 *
 *  Created on: 2011-9-19
 *      Author: dada
 */

#ifndef LOGICALLIANCE_H_
#define LOGICALLIANCE_H_

#include "LogicInc.h"

enum AllianceStatus
{
	AS_ACTIVE = 0,
	AS_REMOVE = 1
};

enum AllianceMemberType
{
	AMT_NORMAL = 0,
	AMT_LEADER = 1,
	AMT_MANAGER = 2, //副盟主
	AMT_GENERAL = 3  //大将军
};

class CLogicAlliance
{
public:
	int AddAlliance(unsigned uid, string &name, int flag, const string &description,
			int cash, int coins, bool useResource, DataAlliance &alliance);
	int SetAlliance(const DataAlliance &alliance);
	int SetAllianceLimit(const DataAlliance &alliance);
	int GetAlliance(unsigned allianceId, DataAlliance &alliance);
	int GetAllianceLimit(unsigned allianceId, DataAlliance &alliance);
	int RemoveAlliance(unsigned allianceId);
	int RemoveAlliance(unsigned allianceId, bool isReal);

	int AddMember(unsigned allianceId, unsigned uid);
	int SetMember(const DataAllianceMember &member);
	int GetMember(unsigned allianceId, unsigned uid, DataAllianceMember &member);
	int GetMembers(unsigned allianceId, vector<DataAllianceMember> &members);
	int RemoveMember(unsigned allianceId, unsigned uid);

	int ChangeLeader(unsigned allianceId, unsigned newLeaderUid);
	int AddPoint(unsigned allianceId, unsigned uid, int point);
	int AddAttackPoint(unsigned allianceId, unsigned uid, const Json::Value &data, int defenceLevel);
	int GetRandomAlliances(int count, vector<DataAlliance> &alliances);
	int AddActivity(DataAlliance &alliance, const Json::Value &activity);
	int AddActivity(unsigned &allianceId, const Json::Value &activity);
	int AddLastAttack(unsigned allianceId, unsigned uid, const Json::Value &attackData);
	bool IsInSameAlliance(unsigned uid, unsigned targetUid);
	int Donate(unsigned uid, unsigned allianceId, int r1, int r2, int r3, int r4, int r5, int cash, bool isCoins=true);
	int ChangeResource(unsigned allianceId, int64_t r1, int64_t r2, int64_t r3, int64_t r4, int64_t r5, int cash, const string &reason);
	int SetAllianceData(unsigned uid, unsigned allianceId, const Json::Value &data);
	int SetMemberData(unsigned uid, unsigned allianceId, const Json::Value &data);
	bool IsLeader(unsigned uid, unsigned allianceId);
	bool IsManager(unsigned uid, unsigned allianceId);
	int Upgrade(unsigned uid, unsigned allianceId, int64_t r1, int64_t r2, int64_t r3, int64_t r4, int64_t r5, int cash, int &level);
	int RemoveMember(unsigned uid, unsigned allianceId, unsigned uidRemove);
	int SetMemberType(unsigned uid, unsigned allianceId, unsigned memberUid, int type);
	int AddAllianceUpdates(unsigned uid, unsigned allianceId, Json::Value &updates);
	int AddMemberDonate(unsigned uid, unsigned allianceId, int r1, int r2, int r3, int r4, int r5, int cash);
	int SetAllianceMemberData(unsigned uid, unsigned allianceId, int point, const Json::Value &data);
	int RankMemberDonate(unsigned allianceId);
	int AllianceEmail(unsigned allianceId, unsigned uid, DataEmail &data);
	int CompeteLaeder(unsigned allianceId, unsigned uid);

	int ProcessRequest(const DataMessage &request, unsigned from, unsigned to, const string &type, int action, const Json::Value &data);

	int GetAllianceJson(unsigned uid, unsigned allianceId, Json::Value &value);
	int SearchAllianceJson(const string &name, Json::Value &value);
	int GetRandomAlliancesJson(Json::Value &value);
	int GetAllianceEnemys(unsigned uidBy, unsigned allianceId, unsigned startIndex, unsigned count, Json::Value &value);

	int GetMemberMaxCount(int level);
	int DonatePoint(int level,int r1, int r2, int r3, int r4, int r5, int &points);
};

#endif /* LOGICALLIANCE_H_ */
