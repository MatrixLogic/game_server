#include "DataPersonMatch.h"
#include "DataInc.h"
#include <iostream>
#include <fstream>

using namespace std;

CPersonBaseMatch::CPersonBaseMatch()
{
	m_init = false;
}

int CPersonBaseMatch::Init(const string &path, semdat sem)
{
	if(m_init)
	{
		return 0;
	}
	if (path.empty())
	{
		error_log("[init_personmatch_fail][path=%s]", path.c_str());
		return R_ERROR;
	}

	int semgroup = 0;
	int semserver = 0;
	Config::GetDomain(semserver);
	if(!semserver)
		Config::GetDB(semserver);
	if(!m_sh.CreateOrOpen(path.c_str(), sizeof(PersonMatchData), SEM_ID(sem,semgroup,semserver)))
	{
		error_log("[init_personmatch_fail][path=%s]", path.c_str());
		return R_ERROR;
	}
	CAutoLock lock(&m_sh, true);
	if(!m_sh.HasInit())
	{
		PersonMatchData *pData = (PersonMatchData *)m_sh.GetAddress();
		memset(pData, 0, sizeof(PersonMatchData));
		m_sh.SetInitDone();
	}
	m_init = true;
	return 0;
}

int CPersonBaseMatch::View(unsigned uid, int &stage, PersonMatchPlayer& person,vector<PersonMatchPlayer> &playoff){
	PersonMatchData *pdata = (PersonMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	stage = pdata->stage;
	playoff.push_back(pdata->champion);
	playoff.push_back(pdata->top2[0]);
	playoff.push_back(pdata->top2[1]);
	playoff.push_back(pdata->top4[0]);
	playoff.push_back(pdata->top4[1]);
	playoff.push_back(pdata->top4[2]);
	playoff.push_back(pdata->top4[3]);
	playoff.push_back(pdata->top8[0]);
	playoff.push_back(pdata->top8[1]);
	playoff.push_back(pdata->top8[2]);
	playoff.push_back(pdata->top8[3]);
	playoff.push_back(pdata->top8[4]);
	playoff.push_back(pdata->top8[5]);
	playoff.push_back(pdata->top8[6]);
	playoff.push_back(pdata->top8[7]);

	memset(&person,0,sizeof(person));
	for(unsigned i = 0; i < pdata->numOfApply; ++i){
		if(pdata->mems[i].uid == uid){
			memcpy(&person,&(pdata->mems[i]),sizeof(person));
			break;
		}
	}

	return 0;
}

unsigned CPersonBaseMatch::getInstId(PersonMatchData *pdata){
	unsigned id = pdata->instance_ndx++;
	bool flag = false;
	do{
		int i;
		for(i = 0; i < 8; ++i){
			if(pdata->rival[i].instanceid == id){
				flag = true;
				id = pdata->instance_ndx++;
				break;
			}
		}
		if(i == 8){
			flag = false;
		}
	}while(flag);

	return id;
}

int CPersonBaseMatch::Apply(unsigned uid, const string &name, unsigned &instanceid){
	PersonMatchData *pdata = (PersonMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}

	CAutoLock lock(&m_sh, true);
	if (pdata->stage != PMS_APPLY)
		return R_ERR_LOGIC;

	for(unsigned int i=0;i<pdata->numOfApply;++i){
		if(pdata->mems[i].uid == uid){
			error_log("user %u has applyed",uid);
			return 0;
		}
	}

	if(pdata->numOfApply > PERSONMATCH_PLAYERS_MAX - 1){
		error_log("full members");
		return R_ERR_LOGIC;
	}

	pdata->mems[pdata->numOfApply].uid = uid;
	snprintf(pdata->mems[pdata->numOfApply].name,PERSONMATCH_NAME_LEN_MAX,"%s",name.c_str());
	pdata->mems[pdata->numOfApply].instanceid = getInstId(pdata);
	if(!IsValidPersonMatchId(pdata->mems[pdata->numOfApply].instanceid)){
		error_log("invalid person inst id:%u",pdata->mems[pdata->numOfApply].instanceid);
		return R_ERR_LOGIC;
	}
	instanceid = pdata->mems[pdata->numOfApply].instanceid;

	++pdata->numOfApply;
	
	return 0;
}

int CPersonBaseMatch::ReportResult(unsigned uid, int order, int damage){
	if (order < 1 || order > 6 || damage < 0 || damage > 100)
		return R_ERR_LOGIC;
	
	PersonMatchData *pdata = (PersonMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}

	CAutoLock lock(&m_sh, true);


	unsigned int i;	
	if(pdata->stage == PMS_REGULAR && order >= 1 && order <= 3){
		for(i = 0; i < pdata->numOfApply; ++i){
			if(pdata->mems[i].uid == uid){
				pdata->mems[i].damage[order - 1] = damage;
				pdata->mems[i].flag[order - 1] = 1;
				return 0;
			}
		}
	}
	else if(pdata->stage == PMS_PLAYOFF8 && order == 4){
		for(i=0;i<8;i++){
			if(pdata->top8[i].uid == uid){
				pdata->top8[i].damage[0] = damage;
				pdata->top8[i].flag[0] = 1;
				return 0;
			}
		}		
	}
	else if(pdata->stage == PMS_PLAYOFF4 && order == 5){
		for(i=0;i<4;i++){
			if(pdata->top4[i].uid == uid){
				pdata->top4[i].damage[0] = damage;
				pdata->top4[i].flag[0] = 1;
				return 0;
			}
		}		
	}
	else if(pdata->stage == PMS_PLAYOFF2 && order == 6){
		for(i=0;i<2;i++){
			if(pdata->top2[i].uid == uid){
				pdata->top2[i].damage[0] = damage;
				pdata->top2[i].flag[0] = 1;
				return 0;
			}
		}		
	}

	return 0;
}

int CPersonBaseMatch::getInstPath(string & path){
	time_t nt = time(NULL) + (8 * 3600);
	struct tm* pTm = gmtime(&nt);
	char buf[512] = {0};
	string instPath = Config::GetValue(PERSON_INST_PATH);

	snprintf(buf,sizeof(buf),"%s.%d",instPath.c_str(),pTm->tm_wday);

	path = buf;

	return 0;
}


int CPersonBaseMatch::StartPlayoff4(){

	PersonMatchData *pdata = (PersonMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	if (pdata->stage != PMS_PLAYOFF8)
		return R_ERR_LOGIC;

	CAutoLock lock(&m_sh, true);
	pdata->stage = PMS_PLAYOFF4;

	int idx;
	memset(pdata->top4,0,sizeof(pdata->top4));

	CGuessData guessData;
	if(guessData.Init(Config::GetValue(GUESS_DATA_PATH),2) != 0){
		cerr << "guess data init fail" << endl;
		return R_ERR_LOGIC;
	}

	idx = (pdata->top8[0].damage[0] >= pdata->top8[1].damage[0] ? 0 : 1);
	if(pdata->top8[0].damage[0] == pdata->top8[1].damage[0]){
		unsigned s1=0,s2=0;
		guessData.GetSupportCount(pdata->top8[0].uid,s1);
		guessData.GetSupportCount(pdata->top8[1].uid,s2);
		idx = s1 > s2 ? 0 : 1;
		if(s1 == s2){
			int temp = random() % 2;
			if(temp == 0)
			{
				if(pdata->top8[0].uid != 0 && pdata->top8[1].uid == 0)
				{
					idx = 0;
				}
				else
					idx = 1;

			}
			if(temp == 1)
			{
				if(pdata->top8[1].uid != 0 && pdata->top8[0].uid == 0)
				{
					idx = 1;
				}
				else
				    idx = 0;
			}
			//idx = random() % 2;
		}
	}
	pdata->top4[0].uid = pdata->top8[idx].uid;
	snprintf(pdata->top4[0].name,PERSONMATCH_NAME_LEN_MAX,"%s",pdata->top8[idx].name);
	pdata->top4[0].instanceid = pdata->top8[idx].instanceid;

	
	idx = (pdata->top8[2].damage[0] >= pdata->top8[3].damage[0] ? 2 : 3);
	if(pdata->top8[2].damage[0] == pdata->top8[3].damage[0]){
		unsigned s1=0,s2=0;
		guessData.GetSupportCount(pdata->top8[2].uid,s1);
		guessData.GetSupportCount(pdata->top8[3].uid,s2);
		idx = s1 > s2 ? 2 : 3;
		if(s1 == s2){
			int temp = random() % 2;
			if(temp == 0)
			{
					if(pdata->top8[2].uid != 0 && pdata->top8[3].uid == 0)
					{
						idx = 2;
					}
					else
						idx = 3;

			}
			if(temp == 1)
			{
				if(pdata->top8[3].uid != 0 && pdata->top8[2].uid == 0)
				{
					idx = 3;
				}
				else
					idx = 2;

			}
			//idx = 2 + random() % 2;
		}
	}
	pdata->top4[1].uid = pdata->top8[idx].uid;
	snprintf(pdata->top4[1].name,PERSONMATCH_NAME_LEN_MAX,"%s",pdata->top8[idx].name);
	pdata->top4[1].instanceid = pdata->top8[idx].instanceid;

	idx = (pdata->top8[4].damage[0] >= pdata->top8[5].damage[0] ? 4 : 5);
	if(pdata->top8[4].damage[0] == pdata->top8[5].damage[0]){
		unsigned s1=0,s2=0;
		guessData.GetSupportCount(pdata->top8[4].uid,s1);
		guessData.GetSupportCount(pdata->top8[5].uid,s2);
		idx = s1 > s2 ? 4 : 5;
		if(s1 == s2){
			int temp = random() % 2;
			if(temp == 0)
			{
				if(pdata->top8[4].uid != 0 && pdata->top8[5].uid == 0)
				{
					idx = 4;
				}
				else
					idx = 5;

			}
			if(temp == 1)
			{
				if(pdata->top8[5].uid != 0 && pdata->top8[4].uid == 0)
				{
					idx = 5;
				}
				else
				    idx = 4;

			}
			//idx = 4 + random() % 2;
		}
	}
	pdata->top4[2].uid = pdata->top8[idx].uid;
	snprintf(pdata->top4[2].name,PERSONMATCH_NAME_LEN_MAX,"%s",pdata->top8[idx].name);
	pdata->top4[2].instanceid = pdata->top8[idx].instanceid;

	idx = (pdata->top8[6].damage[0] >= pdata->top8[7].damage[0] ? 6 : 7);
	if(pdata->top8[6].damage[0] == pdata->top8[7].damage[0]){
		unsigned s1=0,s2=0;
		guessData.GetSupportCount(pdata->top8[6].uid,s1);
		guessData.GetSupportCount(pdata->top8[7].uid,s2);
		idx = s1 > s2 ? 6 : 7;
		if(s1 == s2){
			int temp = random() % 2;
			if(temp == 0)
			{
				if(pdata->top8[6].uid != 0 && pdata->top8[7].uid == 0)
				{
					idx = 6;
				}
				else
					idx = 7;

			}
			if(temp == 1)
			{
				if(pdata->top8[7].uid != 0 && pdata->top8[6].uid == 0)
				{
					idx = 7;
				}
				else
				    idx = 6;

			}
			//idx = 6 + random() % 2;
		}
	}
	pdata->top4[3].uid = pdata->top8[idx].uid;
	snprintf(pdata->top4[3].name,PERSONMATCH_NAME_LEN_MAX,"%s",pdata->top8[idx].name);
	pdata->top4[3].instanceid = pdata->top8[idx].instanceid;

	/*
	string instPath = Config::GetValue(PERSON_INST_PATH);
	ofstream ofile(instPath.c_str(),ios::trunc);

	for(int i = 0; i < 4; ++i){
		ofile << pdata->top4[i].uid << " " << pdata->top4[i].instanceid << endl;
	}
	ofile.close();
	*/
	return 0;
}

int CPersonBaseMatch::StartPlayoff2(){
	PersonMatchData *pdata = (PersonMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	if (pdata->stage != PMS_PLAYOFF4)
		return R_ERR_LOGIC;

	CAutoLock lock(&m_sh, true);
	pdata->stage = PMS_PLAYOFF2;

	int idx;
	memset(pdata->top2,0,sizeof(pdata->top2));

	CGuessData guessData;
	if(guessData.Init(Config::GetValue(GUESS_DATA_PATH),2) != 0){
		cerr << "guess data init fail" << endl;
		return R_ERR_LOGIC;
	}

	idx = (pdata->top4[0].damage[0] >= pdata->top4[1].damage[0] ? 0 : 1);
	if(pdata->top4[0].damage[0] == pdata->top4[1].damage[0]){
		unsigned s1=0,s2=0;
		guessData.GetSupportCount(pdata->top4[0].uid,s1);
		guessData.GetSupportCount(pdata->top4[1].uid,s2);
		idx = s1 > s2 ? 0 : 1;
		if(s1 == s2){
			int temp = random() % 2;
			if(temp == 0)
			{
				if(pdata->top4[0].uid != 0 && pdata->top4[1].uid == 0)
				{
					idx = 0;
				}
				else
					idx = 1;

			}
			if(temp == 1)
			{
				if(pdata->top4[1].uid != 0 && pdata->top4[0].uid == 0)
				{
					idx = 1;
				}
				else
				    idx = 0;

			}
			//idx = random() % 2;
		}
	}
	pdata->top2[0].uid = pdata->top4[idx].uid;
	snprintf(pdata->top2[0].name,PERSONMATCH_NAME_LEN_MAX,"%s",pdata->top4[idx].name);
	pdata->top2[0].instanceid = pdata->top4[idx].instanceid;

	idx = (pdata->top4[2].damage[0] >= pdata->top4[3].damage[0] ? 2 : 3);
	if(pdata->top4[2].damage[0] == pdata->top4[3].damage[0]){
		unsigned s1=0,s2=0;
		guessData.GetSupportCount(pdata->top4[2].uid,s1);
		guessData.GetSupportCount(pdata->top4[3].uid,s2);
		idx = s1 > s2 ? 2 : 3;
		if(s1 == s2){
			int temp = random() % 2;
			if(temp == 0)
			{
				if(pdata->top4[2].uid != 0 && pdata->top4[3].uid == 0)
				{
					idx = 2;
				}
				else
					idx = 3;

			}
			if(temp == 1)
			{
				if(pdata->top4[3].uid != 0 && pdata->top4[2].uid == 0)
				{
					idx = 3;
				}
				else
				    idx = 2;

			}
			//idx = 2 + random() % 2;
		}
	}
	pdata->top2[1].uid = pdata->top4[idx].uid;
	snprintf(pdata->top2[1].name,PERSONMATCH_NAME_LEN_MAX,"%s",pdata->top4[idx].name);
	pdata->top2[1].instanceid = pdata->top4[idx].instanceid;

	return 0;
}

int CPersonBaseMatch::FinishMatch(const PersonMatchData **p)
{
	PersonMatchData *pdata = (PersonMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	if (pdata->stage != PMS_PLAYOFF2)
		return R_ERR_LOGIC;

	*p = pdata;
	
	CAutoLock lock(&m_sh, true);
	pdata->stage = PMS_MATCH_FINISH;

	CGuessData guessData;
	if(guessData.Init(Config::GetValue(GUESS_DATA_PATH),2) != 0){
		cerr << "guess data init fail" << endl;
		return R_ERR_LOGIC;
	}

	int idx = (pdata->top2[0].damage[0] >= pdata->top2[1].damage[0] ? 0 : 1);
	if(pdata->top2[0].damage[0] == pdata->top2[1].damage[0]){
		unsigned s1=0,s2=0;
		guessData.GetSupportCount(pdata->top2[0].uid,s1);
		guessData.GetSupportCount(pdata->top2[1].uid,s2);
		idx = s1 > s2 ? 0 : 1;
		if(s1 == s2){
			int temp = random() % 2;
			if(temp == 0)
			{
				if(pdata->top2[0].uid != 0 && pdata->top2[1].uid == 0)
				{
					idx = 0;
				}
				else
					idx = 1;

			}
			if(temp == 1)
			{
				if(pdata->top2[1].uid != 0 && pdata->top2[0].uid == 0)
				{
					idx = 1;
				}
				else
				    idx = 0;

			}
			//idx = random() % 2;
		}
	}
	pdata->champion.uid = pdata->top2[idx].uid;
	pdata->champion.damage[0] = pdata->top2[idx].damage[0];
	snprintf(pdata->champion.name,PERSONMATCH_NAME_LEN_MAX,"%s",pdata->top2[idx].name);
	pdata->champion.instanceid = pdata->top2[idx].instanceid;

	return 0;
}

int CPersonBaseMatch::GetRival(int idx,PersonMatchPlayer& rival){
	PersonMatchData *pdata = (PersonMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}

	if(idx < 0 || idx > 7){
		return R_ERR_PARAM;
	}

	memset(&rival,0,sizeof(rival));
	memcpy(&rival,&(pdata->rival[idx]),sizeof(rival));

	return 0;
}

int CPersonBaseMatch::Watch(){
	PersonMatchData *pdata = (PersonMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);

	cout << "stage:" << ((int)pdata->stage) << endl;
	cout << "instance_ndx:" << pdata->instance_ndx << endl;
	cout << "numOfApply:" << pdata->numOfApply << endl;
	cout << "rivals:" << endl;
	for(int i = 0; i<8;++i){
		cout << pdata->rival[i].uid << "," << pdata->rival[i].instanceid << "," << pdata->rival[i].name << endl;
	}
	cout << endl;
		
	cout << "champion:" << pdata->champion.uid << "," << pdata->champion.name << "," << (int)pdata->champion.damage[0] << endl<<endl;
	cout << "top2:" << endl;
	cout << pdata->top2[0].uid << "," << pdata->top2[0].name << "," << (int)(pdata->top2[0].damage[0]) << endl;
	cout << pdata->top2[1].uid << "," << pdata->top2[1].name << "," << (int)(pdata->top2[1].damage[0]) << endl<<endl;
	cout << "top4:" << endl;
	cout << pdata->top4[0].uid << "," << pdata->top4[0].name << "," << (int)(pdata->top4[0].damage[0]) << endl;
	cout << pdata->top4[1].uid << "," << pdata->top4[1].name << "," << (int)(pdata->top4[1].damage[0]) << endl;
	cout << pdata->top4[2].uid << "," << pdata->top4[2].name << "," << (int)(pdata->top4[2].damage[0]) << endl;
	cout << pdata->top4[3].uid << "," << pdata->top4[3].name << "," << (int)(pdata->top4[3].damage[0]) << endl<<endl;
	cout << "top8:" << endl;
	cout << pdata->top8[0].uid << "," << pdata->top8[0].name << "," << (int)(pdata->top8[0].damage[0]) << "," << (int)(pdata->top8[0].damage[1]) << endl;
	cout << pdata->top8[1].uid << "," << pdata->top8[1].name << "," << (int)(pdata->top8[1].damage[0]) << "," << (int)(pdata->top8[1].damage[1])<< endl;
	cout << pdata->top8[2].uid << "," << pdata->top8[2].name << "," << (int)(pdata->top8[2].damage[0]) << "," << (int)(pdata->top8[2].damage[1])<< endl;
	cout << pdata->top8[3].uid << "," << pdata->top8[3].name << "," << (int)(pdata->top8[3].damage[0]) << "," << (int)(pdata->top8[3].damage[1])<< endl;
	cout << pdata->top8[4].uid << "," << pdata->top8[4].name << "," << (int)(pdata->top8[4].damage[0]) << "," << (int)(pdata->top8[4].damage[1])<< endl;
	cout << pdata->top8[5].uid << "," << pdata->top8[5].name << "," << (int)(pdata->top8[5].damage[0]) << "," << (int)(pdata->top8[5].damage[1])<< endl;
	cout << pdata->top8[6].uid << "," << pdata->top8[6].name << "," << (int)(pdata->top8[6].damage[0]) << "," << (int)(pdata->top8[6].damage[1])<< endl;
	cout << pdata->top8[7].uid << "," << pdata->top8[7].name << "," << (int)(pdata->top8[7].damage[0]) << "," << (int)(pdata->top8[7].damage[1])<< endl<<endl;

	return 0;
}

int CPersonBaseMatch::GetStage(int& stage){
	PersonMatchData *pdata = (PersonMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);

	stage = pdata->stage;

	return 0;
}

PersonMatchData * CPersonBaseMatch::GetPersonMatchData(){
	PersonMatchData *pdata = (PersonMatchData *)m_sh.GetAddress();
	return pdata;
}

int CPersonBaseMatch::EndStage()
{
	PersonMatchData *pdata = (PersonMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	pdata->stage = PMS_MATCH_FINISH;

	return 0;
}

