/*
 * DataNationalWarRgt.h
 *
 *  Created on: 2013-08-13
 *      Author: Administrator
 */

#ifndef CDATANATIONALWARREGISTRATION_H___
#define CDATANATIONALWARREGISTRATION_H___
#include "Kernel.h"

#define CITY_NUM  18

struct CDataCitiesInState{
	int state;          					//0无联盟占领，1有联盟占领
	int chall_state;                    	//0无联盟挑战状态，1有联盟挑战状态，2战斗进行中状态
	unsigned OccAlliance_id;
	char Occname[32];                       //占领盟
	unsigned ChallAlliance_id;
	char Challname[32];
	unsigned Occ_InviteAlliance_id[2];      //防守方辅助联盟
	char Occname0[32];
	char Occname1[32];
	unsigned Chall_InviteAlliance_id[2];    //挑战方辅助联盟
	char Challname0[32];
	char Challname1[32];
	unsigned Occ_ts;					    //占领城市的时间
	unsigned chall_ts;
	unsigned chall_uid;
	int city_id;

};

struct CDataAllCityState{
	CDataCitiesInState Cityofnumber[CITY_NUM];
};


class CDataNationalWarRgt {
public:
	CDataNationalWarRgt();
	virtual ~CDataNationalWarRgt();
	int Init(const std::string &path, semdat sem=sem_nationalwarrgt);
	int GetCityState( CDataCitiesInState vecCityState[]);
	int UpdateCityState(const CDataCitiesInState &CityState,int &state);     //宣战成功
	int InviteSuccess(const CDataCitiesInState &CityState,int &state);    //邀请成功
	int CleanNationalWarRgt(void);

private:
	bool m_init;
	CShareMemory m_sh;
};

#endif /* CDATANATIONALWARREGISTRATION_H___ */
