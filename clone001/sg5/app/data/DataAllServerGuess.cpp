#include "DataInc.h"

CGuessAllServerData::CGuessAllServerData(){
	m_init = false;
}


int CGuessAllServerData::Init(const string &path, int type, semdat sem)
{

	if(m_init){
		return 0;
	}

	if(path.empty()){
		error_log("GuessData no path,check config please");
		return R_ERROR;
	}

	//debug_log("[Init guess][path=%s]",path.c_str());
	int semgroup = type;
	int semserver = 0;
	Config::GetDomain(semserver);
	if(!semserver)
		Config::GetDB(semserver);
	if(!m_sh.CreateOrOpen(path.c_str(),sizeof(GuessData), SEM_ID(sem,semgroup,semserver))){
		error_log("GuessData create share memory failed,%s,%m",path.c_str());
		return R_ERROR;
	}

	CAutoLock lock(&m_sh, true);
	if(!m_sh.HasInit())
	{
		GuessData *pData = (GuessData *)m_sh.GetAddress();
		memset(pData, 0, sizeof(GuessData));
		m_sh.SetInitDone();
	}
	m_init = true;

	return 0;
}

int CGuessAllServerData::Apply(unsigned uid,unsigned gid,unsigned coins){
	GuessData *pData = (GuessData *)m_sh.GetAddress();
	if(pData == NULL){
		error_log("get GuessData address failed");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);

	for(int i=0;i<GUESS_GROUP_COUNT;++i){
		if(pData->guessor[i].gid == gid){
			GuessItem* pSupports = pData->guessor[i].supports;
			bool flag = true;
			for(unsigned j=0; j < pData->guessor[i].numOfApply;++j){
				if(pSupports[j].uid == uid){
					pSupports[j].coins += coins;
					flag = false;
					break;
				}
			}

			if(flag){
				if(pData->guessor[i].numOfApply >= MAX_GUESS_COUNT){
					error_log("CGuessData::Apply supportor is full");
					return R_ERR_LOGIC;
				}

				pSupports[pData->guessor[i].numOfApply].uid = uid;
				pSupports[pData->guessor[i].numOfApply].coins = coins;
				++pData->guessor[i].numOfApply;
			}

			pData->guessor[i].coins += coins;

			return 0;
		}
	}

	error_log("CGuessData::Apply target id %u not found",gid);
	return R_ERR_LOGIC;
}

int CGuessAllServerData::View(unsigned uid,vector<GuessViewData>& dataVec){
	GuessData *pData = (GuessData *)m_sh.GetAddress();
	if(pData == NULL){
		error_log("get GuessData address failed");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);

	dataVec.clear();
	for(int i=0;i<GUESS_GROUP_COUNT;++i){
		GuessViewData tmpData;
		memset(&tmpData,0,sizeof(tmpData));
		tmpData.gid = pData->guessor[i].gid;
		snprintf(tmpData.gname,sizeof(tmpData.gname),"%s",pData->guessor[i].gname);
		tmpData.coins = pData->guessor[i].coins;
		tmpData.numOfApply = pData->guessor[i].numOfApply;

		GuessItem* pSupports = pData->guessor[i].supports;
		for(unsigned j=0; j < pData->guessor[i].numOfApply;++j){
			if(pSupports[j].uid == uid){
				tmpData.ucoins = pSupports[j].coins;
				break;
			}
		}

		dataVec.push_back(tmpData);
	}

	return 0;
}

int CGuessAllServerData::SetInitValues(const vector<GuessViewData>& dataVec){
	GuessData *pData = (GuessData *)m_sh.GetAddress();
	if(pData == NULL){
		error_log("get GuessData address failed");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);

	if(dataVec.size() != GUESS_GROUP_COUNT){
		error_log("CGuessData::SetInitValues failed,count not match:%u",dataVec.size());
		return R_ERR_LOGIC;
	}

	memset(pData, 0, sizeof(GuessData));
	for(int i = 0; i < GUESS_GROUP_COUNT;++i){
		pData->guessor[i].gid = dataVec[i].gid;
		snprintf(pData->guessor[i].gname,sizeof(pData->guessor[i].gname),"%s",dataVec[i].gname);
	}

	return 0;
}

GuessData* CGuessAllServerData::GetData(){
	GuessData *pData = (GuessData *)m_sh.GetAddress();
	if(pData == NULL){
		error_log("get GuessData address failed");
	}

	return pData;
}

int CGuessAllServerData::GetSupportCount(unsigned gid,unsigned& count){
	GuessData *pData = (GuessData *)m_sh.GetAddress();
	if(pData == NULL){
		error_log("get GuessData address failed");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);

	count = 0;
	if(gid == 0) return 0;
	for(int i=0;i<GUESS_GROUP_COUNT;++i){
		if(pData->guessor[i].gid == gid){
			count = pData->guessor[i].coins;
			break;
		}
	}

	return 0;
}
