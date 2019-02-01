#include "DataDragonball.h"
#include "DataInc.h"

CDataDragonball::CDataDragonball()
{
	m_init = false;
}

int CDataDragonball::Init(const string &path, semdat sem)
{
	if(m_init)
		return 0;
	if (path.empty())
	{
		error_log("[init_dragonball_fail][path=%s]", path.c_str());
		return R_ERROR;
	}

	int semgroup = 0;
	int semserver = 0;
	Config::GetDomain(semserver);
	if(!semserver)
		Config::GetDB(semserver);
	if(!m_sh.CreateOrOpen(path.c_str(), sizeof(DragonballData), SEM_ID(sem,semgroup,semserver)))
	{
		error_log("[init_dragonball_fail][path=%s]", path.c_str());
		return R_ERROR;
	}
	CAutoLock lock(&m_sh, true);
	if(!m_sh.HasInit())
	{
		DragonballData *pdata = (DragonballData *)m_sh.GetAddress();
		memset(pdata, 0, sizeof(*pdata));
		m_sh.SetInitDone();
	}
	m_init = true;
	return 0;
}

int CDataDragonball::SetDragonball(int ballId, unsigned uid, const string &name, unsigned now,
		unsigned &preUid, unsigned &preTs, string &preName)
{
	if (!IsValidDragonballId(ballId))
	{
		return R_ERR_REFUSE;
	}
	DragonballData *pdata = (DragonballData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}

	DataUser dataUser;
	CDataUser userDB;
	if(userDB.GetUser(uid,dataUser) != 0){
		return R_ERR_DB;
	}

	CAutoLock lock(&m_sh, true);
	int index = DragonballIdToIndex(ballId);

	preUid = (pdata->ball[index]).holderUid;
	preTs = (pdata->ball[index]).ts;
	preName = (pdata->ball[index]).name;
	(pdata->ball[index]).holderUid = uid;
	(pdata->ball[index]).ts = now;
	snprintf((pdata->ball[index]).name, sizeof((pdata->ball[index]).name), "%s", name.c_str());
	(pdata->ball[index]).allianceId = dataUser.alliance_id;

	if(dataUser.alliance_id == 0){
		snprintf((pdata->ball[index]).aname, sizeof((pdata->ball[index]).aname), "%s", "");
	}
	else{
		DataAlliance dataAlliance;
		CDataAlliance dbAlliance;
		if(dbAlliance.GetAllianceLimit(dataUser.alliance_id,dataAlliance) != 0){
			return R_ERR_DB;
		}
		snprintf((pdata->ball[index]).aname, sizeof((pdata->ball[index]).aname), "%s", dataAlliance.name.c_str());
	}
	return 0;
}

int CDataDragonball::ViewAllDragonball(DragonballData &balls)
{
	DragonballData *pdata = (DragonballData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	memcpy(&balls, pdata, sizeof(DragonballData));
	return 0;
}

int CDataDragonball::WhichDragonballHold(unsigned uid, unsigned &ballId, unsigned &ts)
{
	ballId = ts = 0;
	DragonballData *pdata = (DragonballData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	for (int i = DRAGONBALL_ID_MIN; i <= DRAGONBALL_ID_MAX; i++)
	{
		if ((pdata->ball[DragonballIdToIndex(i)]).holderUid && uid &&
				(pdata->ball[DragonballIdToIndex(i)]).ts &&
				(pdata->ball[DragonballIdToIndex(i)]).holderUid == uid)
		{
			ballId = i;
			ts = (pdata->ball[DragonballIdToIndex(i)]).ts;
			return 0;
		}
	}
	return -1;
}

int CDataDragonball::GetAllianceDragons(map<unsigned,AllianceBall>& allianceBalls){

	allianceBalls.clear();

	DragonballData *pdata = (DragonballData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	for (int i = DRAGONBALL_ID_MIN; i <= DRAGONBALL_ID_MAX; i++)
	{
		Dragonball* pBall = &(pdata->ball[DragonballIdToIndex(i)]);
		if(pBall->allianceId == 0){
			continue;
		}

		if(allianceBalls.find(pBall->allianceId) == allianceBalls.end()){
			AllianceBall aBall;
			aBall.allianceId = pBall->allianceId;
			aBall.count = 0;
			aBall.ts = 0;

			allianceBalls[pBall->allianceId] = aBall;
		}

		allianceBalls[pBall->allianceId].count++;
		if(pBall->ts > allianceBalls[pBall->allianceId].ts){
			allianceBalls[pBall->allianceId].ts = pBall->ts;
		}
	}

	return 0;
}

int CDataDragonball::GetAllianceWithBalls(AllianceBall& allianceBalls){
	map<unsigned,AllianceBall> ballMap;
	int ret = GetAllianceDragons(ballMap);
	if(ret != 0){
		return -1;
	}

	for(map<unsigned,AllianceBall>::iterator it = ballMap.begin(); it != ballMap.end();++it ){
		AllianceBall& aBall = it->second;
		if(aBall.count >= ALLIANCE_DRAGONBALL_COUNT){
			allianceBalls.allianceId = aBall.allianceId;
			allianceBalls.count = aBall.count;
			allianceBalls.ts = aBall.ts;

			return 1;
		}
	}

	return 0;
}

int CDataDragonball::SetAlliance(AllianceBall& aBall){
	aBall.allianceId = 0;
	AllianceBall newABall;
	int ret = GetAllianceWithBalls(newABall);
	if(ret < 0){
		return ret;
	}

	DragonballData *pdata = (DragonballData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);

	if(ret == 0){
		aBall.allianceId = pdata->aBall.allianceId;
		aBall.count = pdata->aBall.count;
		aBall.ts = pdata->aBall.ts;

		memset(&(pdata->aBall),0,sizeof(AllianceBall));
		return 0;
	}

	if(ret == 1){
		if(pdata->aBall.allianceId != 0 && pdata->aBall.allianceId != newABall.allianceId){
			aBall.allianceId = pdata->aBall.allianceId;
			aBall.count = pdata->aBall.count;
			aBall.ts = pdata->aBall.ts;
		}

		if(pdata->aBall.allianceId != newABall.allianceId){
			pdata->aBall.ts = newABall.ts;
		}
		pdata->aBall.allianceId = newABall.allianceId;
		pdata->aBall.count = newABall.count;
	}

	return 0;
}

int CDataDragonball::SetAlliance2(AllianceBall& aBall){
	aBall.allianceId = 0;
	AllianceBall newABall;
	int ret = GetAllianceWithBalls(newABall);
	if(ret < 0){
		return ret;
	}

	DragonballData *pdata = (DragonballData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);

	aBall.allianceId = pdata->aBall.allianceId;
	aBall.count = pdata->aBall.count;
	aBall.ts = pdata->aBall.ts;

	if(ret == 0){
		memset(&(pdata->aBall),0,sizeof(AllianceBall));
		return 0;
	}

	if(ret == 1){
		pdata->aBall.ts = newABall.ts;
		pdata->aBall.allianceId = newABall.allianceId;
		pdata->aBall.count = newABall.count;
	}

	return 0;
}

int  CDataDragonball::ResetAlliance(){
	DragonballData *pdata = (DragonballData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);

	memset(&(pdata->aBall),0,sizeof(AllianceBall));

	return 0;
}

int CDataDragonball::GetDragonballStatus(int ballId,Dragonball &ballData)
{
	if (!IsValidDragonballId(ballId))
	{
		return R_ERR_REFUSE;
	}
	DragonballData *pdata = (DragonballData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	memcpy(&ballData, &pdata->ball[DragonballIdToIndex(ballId)],sizeof(Dragonball));
	return 0;
}

