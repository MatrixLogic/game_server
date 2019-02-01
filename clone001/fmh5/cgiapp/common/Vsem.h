#ifndef __VSEM_H__
#define __VSEM_H__

#include "ILock.h"

//sem id
enum semdat {
	sem_arena = 1,
	sem_barbarian,
	sem_basematch,
	sem_broadcast,
	sem_dragonball,
	sem_fightarena,
	sem_guess,
	sem_ladder,
	sem_map,
	sem_match,
	sem_mysticalshop,
	sem_nominatealliance,
	sem_payrank,
	sem_personmatch,
	sem_pointsrank,
	sem_pointsrankallserver,
	sem_river,
	sem_toptenuser,
	sem_world,
	sem_worldboss,
	sem_worldnpc,
	sem_basematchallserver,
	sem_personmatchallserver_1,
	sem_personmatchallserver_2,
	sem_personmatchallserver_3,
	sem_guessallserver,
	sem_guessallserver_1,
	sem_guessallserver_2,
	sem_guessallserver_3,
	sem_ladderallserver_1,
	sem_ladderallserver_2,
	sem_ladderallserver_3,
	sem_alliance,
	sem_hundreddaysactivity,
	sem_world_map,
	sem_new_user_rec,
	sem_qixijie,
	sem_nationalwarrgt,
	sem_groupsfighting,
	sem_gm,
	sem_noise,
	sem_recharge_alliance,
	sem_alliance_battle,
	sem_barbarian_king,
	sem_xml,
	sem_kingdom,
	sem_allserver_rank,
	sem_viprebates,
	sem_alliance_point,
	sem_world_attack,
	sem_openarea_activity,
	sem_new_world,
	sem_new_lottery,
	sem_consume_rank,
	sem_new_world_battle,
	sem_all_server_new_world_battle,
	sem_restriction,
	sem_battle,
	sem_allserver_map,
	sem_allserver_map_buffer,
	sem_brave_new_world,
	sem_new_match,
	sem_brave_new_world_battle,
	sem_predator_active,
	sem_dragon_scale_gift_bag,
};

#define SEM_START 2000000000
#define SEM_ID(semdat,semgroup,semserver) (SEM_START+semserver%10000*10000+semdat%100*100+(semgroup++)%100)

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

class CVsem: public ILock {
public:
	CVsem();
	~CVsem();

	bool CreateOrOpen(const char *name, int sem = 0, unsigned vnum = 1);
	bool Close() { return true; }
	bool Create(const char *name, int sem = 0, unsigned vnum = 1);
	bool Open(const char *name, int sem = 0);

	bool Lock(int vnum = 1);
	bool Unlock(int vnum = 1);

private:

	int m_sem;
	int _sem;
};

#endif //__VSEM_H__
