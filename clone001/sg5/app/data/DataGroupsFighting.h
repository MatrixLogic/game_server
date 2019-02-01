#ifndef _DATA_GROUPS_FIGHTING_H_
#define _DATA_GROUPS_FIGHTING_H_

#include "Kernel.h"

#define BATTLEFIELD 18
#define MAXNUM_OF_PEOPLE 1000


//个人战阵属性
struct WarProperties{
	int typeLevels[3];
	int type;
	unsigned maxHp;
	int level;
	int attack;
	int attackRate;
	int critRate;
	unsigned defence;
	int dodgeRate;
};

struct EveryoneFormation
{
	unsigned uid;
	unsigned alliance_id;
	unsigned fullmorale;              //士气
	unsigned morale;
	int state;               //1为运动状态 .2为据点状态
	int position;            //玩家所处的据点id
	int target;              //玩家要去的据点id
	int role;              //1为观看状态， 2为在战场状态
	unsigned ts;           //进出据点的时间
	int camp;                //阵营  1为offensive  2为defence
	struct WarProperties personalWarData;
};

struct AllCombatData{
	int Map_id;                         //地图id
	int FortifiedNum;                   //地图对应的据点数
	unsigned Data_id;					//数据序列号
	unsigned Senduid;					//改变数据的uid
	struct EveryoneFormation All_people[MAXNUM_OF_PEOPLE];
};

class CDataGroupsFigthing
{
public:
	CDataGroupsFigthing();
	virtual ~CDataGroupsFigthing();
	int Init(const std::string &path,int cityid, semdat sem=sem_groupsfighting);
	int GetAllCombatData(vector <EveryoneFormation> &All_data,int cityindex,unsigned &Data_id,unsigned &Senduid,vector <EveryoneFormation> &Self_data);
	int UpdatasFightingData(vector <EveryoneFormation> &All_data,int cityindex,unsigned &Data_id,unsigned &Senduid,int &map_id);

	int AddFullmorale(int cityindex,unsigned uid);
	int CleanGroupsfigthingData(int cityindex);

protected:
	CShareMemory m_sh[BATTLEFIELD];
	bool m_init;

};

#endif
