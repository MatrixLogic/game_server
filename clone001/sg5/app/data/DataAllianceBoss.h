#ifndef _DATA_ALLIANCE_BOSS_H_
#define _DATA_ALLIANCE_BOSS_H_

#include "Kernel.h"
#include "DataAlliancePay.h"

#define ALLIANCEBOSS_CHALLENGERS_MAX 100
#define ALLIANCEBOSS_ALLIANCE_MAX 1000
struct AllianceBossChallenger
{
	unsigned uid;
	unsigned damage;	//伤害
	char     name[56];	//攻击者名字
};
struct AllianceBossData
{
	unsigned fullBlood;
	unsigned blood;
	unsigned ts;                                           
	unsigned ts_start;                   //屠魔开启时间
	unsigned standts;
	unsigned challNum;
	unsigned showNum;
	unsigned vip_grade;                //开启vip等级
	unsigned alliance_id;              //帮会id
	AllianceBossChallenger challengers[ALLIANCEBOSS_CHALLENGERS_MAX];
};

struct AllianceAttackNumber
{
	AllianceBossData Alliancebossdata[ALLIANCEBOSS_ALLIANCE_MAX];
};

class CDataAllianceBoss
{
public:
	CDataAllianceBoss();
	virtual ~CDataAllianceBoss();
	int Init(const string &path, const vector<int> &vMinBloods,const vector<int> &vMaxBlood, semdat sem=sem_alliance);
	int LoadAllianceBossOpener(unsigned uid, unsigned bossId, unsigned &blood,
					unsigned &fullBlood,unsigned &vip_grade,unsigned &alliance_id,unsigned &ts);
	int LoadAllianceBoss(unsigned uid, unsigned bossId, unsigned &blood, unsigned &number, int &selfRank, AllianceBossChallenger &self,
				vector<AllianceBossChallenger> &top, AllianceBossChallenger &last,unsigned &fullBlood,unsigned &alliance_id);
	int AttackAllianceBoss(unsigned uid, unsigned bossId, unsigned damage, const string &name,
			bool &dying, unsigned &blood, unsigned &number, int &selfRank, AllianceBossChallenger &self,
			vector<AllianceBossChallenger> &top, AllianceBossChallenger &last, vector<AllianceBossChallenger> &lucks
			, vector<AllianceBossChallenger> &dam,unsigned &alliance_id,unsigned &vip);
	int ViewAllianceBoss(unsigned uid, unsigned bossId, unsigned &blood, unsigned &fullBlood,unsigned &number, int &selfRank, AllianceBossChallenger &self,
			vector<AllianceBossChallenger> &top, AllianceBossChallenger &last,unsigned &alliance_id);
	int LoadLastAllianceBoss(unsigned uid, unsigned bossId, unsigned &blood, unsigned &number, int &selfRank, AllianceBossChallenger &self,
			vector<AllianceBossChallenger> &top, AllianceBossChallenger &last,unsigned &fullBlood,unsigned &alliance_id);

//	unsigned GetWorldBossTotalBlood(unsigned bossId)
//	{
//		if (!IsValidWorldBossId(bossId))
//			return R_ERR_NO_DATA;
//		return m_worldbossBlood[bossId - WORLD_BOSS_ID_MIN];
//	}

protected:
	CShareMemory m_sh;
	bool m_init;
	//unsigned m_worldbossBlood[WORLD_BOSS_ID_MAX - WORLD_BOSS_ID_MIN + 1];
	unsigned m_minWorldbossBlood[4];
	unsigned m_maxWorldbossBlood[4];
};

#endif
