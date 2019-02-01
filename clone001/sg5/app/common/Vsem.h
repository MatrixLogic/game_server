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
};

#define SEM_START 1000000000
#define SEM_ID(semdat,semgroup,semserver) (SEM_START+semserver*1000000+semdat*1000+(semgroup++))

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

class CVsem: public ILock {
public:
	CVsem();
	~CVsem();

	bool CreateOrOpen(const char *name, int sem = 0);
	bool Close() {
		return true;
	}
	bool Create(const char *name, int sem = 0);
	bool Open(const char *name, int sem = 0);

	bool Lock();
	bool Unlock();

private:

	int m_sem;
	int _sem;
};

#endif //__VSEM_H__
