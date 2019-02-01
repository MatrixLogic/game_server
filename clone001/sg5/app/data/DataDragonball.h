#ifndef _DATA_DRAGONBALL_H_
#define _DATA_DRAGONBALL_H_

#include "Kernel.h"

#define DRAGONBALL_ID_MIN 1
#define DRAGONBALL_ID_MAX 7
#define INVALID_BALL_ID   10
#define IsValidDragonballId(id) (id >= DRAGONBALL_ID_MIN && id <= DRAGONBALL_ID_MAX)
#define DragonballIdToIndex(id) (id - DRAGONBALL_ID_MIN)

#define MAX_HOLD_TIME (2*60*60)
#define ALLIANCE_DRAGONBALL_COUNT 4

struct Dragonball
{
	unsigned holderUid;
	unsigned allianceId;
	unsigned ts;
	char     name[64];
	char	 aname[64];
};

struct AllianceBall{
	unsigned allianceId;
	unsigned count;
	unsigned ts;
};

struct DragonballData
{
	Dragonball ball[7];
	AllianceBall aBall;
};

class CDataDragonball
{
public:
	CDataDragonball();
	int Init(const string &path, semdat sem=sem_dragonball);
	int SetDragonball(int ballId, unsigned uid, const string &name, unsigned now,
			unsigned &preUid, unsigned &preTs, string &preName);
	int ViewAllDragonball(DragonballData &balls);
	int WhichDragonballHold(unsigned uid, unsigned &ballId, unsigned &ts);
	int GetAllianceDragons(map<unsigned,AllianceBall>& allianceBalls);
	int GetAllianceWithBalls(AllianceBall& aBall);
	int SetAlliance(AllianceBall& aBall);
	int SetAlliance2(AllianceBall& aBall);
	int ResetAlliance();
	int GetDragonballStatus(int ballId,Dragonball &ballData);
protected:
	CShareMemory m_sh;
	bool m_init;
};

#endif
