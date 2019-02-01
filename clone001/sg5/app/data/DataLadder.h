#ifndef _DATA_LADDER_H_
#define _DATA_LADDER_H_

#include "Kernel.h"

#define IsValidLadderRank(r) (r >= 1 && r <= 1000)
#define LadderRankToIndex(r) (r - 1)
#define LADDER_ITEM_NUMBER 1000
struct LadderItem
{
	unsigned uid;
	char     flag;
	char     level;
	char     name[64];
	char     pic[256];
	unsigned upts;
	unsigned ext1;
	unsigned ext2;
};
struct LadderData
{
	LadderItem ladder[LADDER_ITEM_NUMBER];
};

enum LadderError
{
	LE_SUCCESS,
	LE_SYS_ERROR,
	LE_PARAM_ERROR,
	LE_CHALL_SELF,
	LE_DEFENCER_OUT_OF_RANK,
	LE_CHALL_BACK_RANK,
	LE_ATTACKER_MISTACK,
	LE_DEFENCER_MISTACK
};

class CDataLadder
{
public:
	CDataLadder();
	int Init(const string &path, semdat sem=sem_ladder);
	int GetRank(unsigned uid, int &rank);
	int GetFivePlayers(int rank1, int rank2, int rank3, int rank4, int rank5,
			LadderItem &player1, LadderItem &player2, LadderItem &player3, LadderItem &player4, LadderItem &player5);
	int Win(int &rank1, unsigned uid1, int level1, const string &name1, const string &pic1,
			int &rank2, unsigned uid2, int level2, const string &name2, const string &pic2,
			unsigned upts, LadderError &error);
	int GetTopOne(LadderItem &data);

protected:
	CShareMemory m_sh;
	bool m_init;
};

#endif
