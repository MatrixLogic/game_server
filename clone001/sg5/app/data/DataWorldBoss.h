#ifndef _DATA_WORLD_BOSS_H_
#define _DATA_WORLD_BOSS_H_

#include "Kernel.h"

#define WORLDBOSS_CHALLENGERS_MAX 1000
struct WorldBossChallenger
{
	unsigned uid;
	unsigned damage;	//伤害
	char     name[56];	//攻击者名字
};
struct WorldBossData
{
	unsigned fullBlood;
	unsigned blood;
	unsigned ts;
	unsigned standts;
	unsigned challNum;
	unsigned showNum;
	WorldBossChallenger challengers[WORLDBOSS_CHALLENGERS_MAX];
};

class CDataWorldBoss
{
public:
	CDataWorldBoss();
	int Init(const string &path, const vector<int> &vMinBloods,const vector<int> &vMaxBlood, semdat sem=sem_worldboss);
	int LoadWorldBoss(unsigned uid, unsigned bossId, unsigned &blood, unsigned &number, int &selfRank, WorldBossChallenger &self,
			vector<WorldBossChallenger> &top, WorldBossChallenger &last,unsigned &fullBlood);
	int AttackWorldBoss(unsigned uid, unsigned bossId, unsigned damage, const string &name,
			bool &dying, unsigned &blood, unsigned &number, int &selfRank, WorldBossChallenger &self,
			vector<WorldBossChallenger> &top, WorldBossChallenger &last, vector<WorldBossChallenger> &lucks
			, vector<WorldBossChallenger> &dam);
	int ViewWorldBoss(unsigned uid, unsigned bossId, unsigned &blood, unsigned &fullBlood,unsigned &number, int &selfRank, WorldBossChallenger &self,
			vector<WorldBossChallenger> &top, WorldBossChallenger &last);
	int ViewWorldBoss( unsigned bossId, unsigned &blood, unsigned &fullblood, unsigned &standts, unsigned ts);
	int SetWorldBoss( unsigned bossId, unsigned blood, unsigned fullblood, unsigned standts, unsigned ts);

//	unsigned GetWorldBossTotalBlood(unsigned bossId)
//	{
//		if (!IsValidWorldBossId(bossId))
//			return R_ERR_NO_DATA;
//		return m_worldbossBlood[bossId - WORLD_BOSS_ID_MIN];
//	}

protected:
	CShareMemory m_sh[WORLD_BOSS_ID_MAX - WORLD_BOSS_ID_MIN + 1];
	bool m_init;
	//unsigned m_worldbossBlood[WORLD_BOSS_ID_MAX - WORLD_BOSS_ID_MIN + 1];
	unsigned m_minWorldbossBlood[WORLD_BOSS_ID_MAX - WORLD_BOSS_ID_MIN + 1];
	unsigned m_maxWorldbossBlood[WORLD_BOSS_ID_MAX - WORLD_BOSS_ID_MIN + 1];
};

#endif
