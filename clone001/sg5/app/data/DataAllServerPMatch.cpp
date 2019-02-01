#include "DataPersonMatch.h"
#include "DataInc.h"
#include <iostream>
#include <fstream>

using namespace std;

CAllServerPersonBaseMatch::CAllServerPersonBaseMatch() {
	m_init = false;
}

int CAllServerPersonBaseMatch::Init(const string &path, semdat sem) {
	if (m_init) {
		return 0;
	}
	if (path.empty()) {
		error_log("[init_personmatch_fail][path=%s]", path.c_str());
		return R_ERROR;
	}

	int semgroup = 0;
	int semserver = 0;
	Config::GetDomain(semserver);
	if (!semserver)
		Config::GetDB(semserver);
	if (!m_sh.CreateOrOpen(path.c_str(), sizeof(PersonMatchData),
			SEM_ID(sem,semgroup,semserver))) {
		error_log("[init_personmatch_fail][path=%s]", path.c_str());
		return R_ERROR;
	}
	CAutoLock lock(&m_sh, true);
	if (!m_sh.HasInit()) {
		PersonMatchData *pData = (PersonMatchData *) m_sh.GetAddress();
		memset(pData, 0, sizeof(PersonMatchData));
		m_sh.SetInitDone();
	}
	m_init = true;
	return 0;
}

int CAllServerPersonBaseMatch::View(unsigned uid, int &stage,
		PersonMatchPlayer& person, vector<PersonMatchPlayer> &playoff) {
	PersonMatchData *pdata = (PersonMatchData *) m_sh.GetAddress();
	if (pdata == NULL) {
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

	memset(&person, 0, sizeof(person));
	for (unsigned i = 0; i < pdata->numOfApply; ++i) {
		if (pdata->mems[i].uid == uid) {
			memcpy(&person, &(pdata->mems[i]), sizeof(person));
			break;
		}
	}

	return 0;
}

unsigned CAllServerPersonBaseMatch::getInstId(PersonMatchData *pdata) {
	unsigned id = pdata->instance_ndx++;
	bool flag = false;
	do {
		int i;
		for (i = 0; i < 8; ++i) {
			if (pdata->rival[i].instanceid == id) {
				flag = true;
				id = pdata->instance_ndx++;
				break;
			}
		}
		if (i == 8) {
			flag = false;
		}
	} while (flag);

	return id;
}

int CAllServerPersonBaseMatch::Apply(unsigned uid, const string &name,
		unsigned &instanceid) {
	PersonMatchData *pdata = (PersonMatchData *) m_sh.GetAddress();
	if (pdata == NULL) {
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}

	CAutoLock lock(&m_sh, true);
	if (pdata->stage != PMS_APPLY)
		return R_ERR_LOGIC;

	for (unsigned int i = 0; i < pdata->numOfApply; ++i) {
		if (pdata->mems[i].uid == uid) {
			error_log("user %u has applyed", uid);
			return 0;
		}
	}

	if (pdata->numOfApply > PERSONMATCH_PLAYERS_MAX - 1) {
		error_log("full members");
		return R_ERR_LOGIC;
	}

	pdata->mems[pdata->numOfApply].uid = uid;
	snprintf(pdata->mems[pdata->numOfApply].name, PERSONMATCH_NAME_LEN_MAX,
			"%s", name.c_str());
	pdata->mems[pdata->numOfApply].instanceid = getInstId(pdata);
	if (!IsValidAllServerPersonMatchId(pdata->mems[pdata->numOfApply].instanceid)) {
		error_log("invalid person inst id:%u",
				pdata->mems[pdata->numOfApply].instanceid);
		return R_ERR_LOGIC;
	}
	instanceid = pdata->mems[pdata->numOfApply].instanceid;

	++pdata->numOfApply;

	return 0;
}

int CAllServerPersonBaseMatch::ReportResult(unsigned uid, int order,
		int damage) {
	if (order < 1 || order > 6 || damage < 0 || damage > 100)
		return R_ERR_LOGIC;

	PersonMatchData *pdata = (PersonMatchData *) m_sh.GetAddress();
	if (pdata == NULL) {
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}

	CAutoLock lock(&m_sh, true);

	unsigned int i;
	if (pdata->stage == PMS_REGULAR && order >= 1 && order <= 3) {
		for (i = 0; i < pdata->numOfApply; ++i) {
			if (pdata->mems[i].uid == uid) {
				pdata->mems[i].damage[order - 1] = damage;
				pdata->mems[i].flag[order - 1] = 1;
				return 0;
			}
		}
	} else if (pdata->stage == PMS_PLAYOFF8 && order == 4) {
		for (i = 0; i < 8; i++) {
			if (pdata->top8[i].uid == uid) {
				pdata->top8[i].damage[0] = damage;
				pdata->top8[i].flag[0] = 1;
				return 0;
			}
		}
	} else if (pdata->stage == PMS_PLAYOFF4 && order == 5) {
		for (i = 0; i < 4; i++) {
			if (pdata->top4[i].uid == uid) {
				pdata->top4[i].damage[0] = damage;
				pdata->top4[i].flag[0] = 1;
				return 0;
			}
		}
	} else if (pdata->stage == PMS_PLAYOFF2 && order == 6) {
		for (i = 0; i < 2; i++) {
			if (pdata->top2[i].uid == uid) {
				pdata->top2[i].damage[0] = damage;
				pdata->top2[i].flag[0] = 1;
				return 0;
			}
		}
	}

	return 0;
}

int CAllServerPersonBaseMatch::SetCurrInstId(unsigned instId) {
	PersonMatchData *pdata = (PersonMatchData *) m_sh.GetAddress();
	if (pdata == NULL) {
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	pdata->instance_ndx = instId;
	return 0;
}

int CAllServerPersonBaseMatch::GetNextInstId(unsigned &instId) {
	PersonMatchData *pdata = (PersonMatchData *) m_sh.GetAddress();
	if (pdata == NULL) {
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	unsigned id = pdata->instance_ndx++;
	bool flag = false;
	do {
		int i;
		for (i = 0; i < 8; ++i) {
			if (pdata->rival[i].instanceid == id) {
				flag = true;
				id = pdata->instance_ndx++;
				break;
			}
		}
		if (i == 8) {
			flag = false;
		}
	} while (flag);

	instId = id;
	return 0;
}

int CAllServerPersonBaseMatch::GetStage(int &stage) {
	PersonMatchData *pdata = (PersonMatchData *) m_sh.GetAddress();
	if (pdata == NULL) {
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	stage = pdata->stage;
	return 0;
}

int CAllServerPersonBaseMatch::SetRivals(PersonMatchPlayer rival[8]) {
	PersonMatchData *pdata = (PersonMatchData *) m_sh.GetAddress();
	if (pdata == NULL) {
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	memcpy(pdata->rival, rival, sizeof(pdata->rival));
	return 0;
}

int CAllServerPersonBaseMatch::NextTurn(void) {
	PersonMatchData *pdata = (PersonMatchData *) m_sh.GetAddress();
	if (pdata == NULL) {
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	memset(pdata, 0, sizeof(*pdata));
	pdata->stage = PMS_APPLY;
	return 0;
}

int CAllServerPersonBaseMatch::StartRegular(void) {
	PersonMatchData *pdata = (PersonMatchData *) m_sh.GetAddress();
	if (pdata == NULL) {
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	if (pdata->stage != PMS_APPLY)
		return R_ERR_LOGIC;

	CAutoLock lock(&m_sh, true);
	memset(pdata->top8, 0, sizeof(pdata->top8));
	memset(pdata->top4, 0, sizeof(pdata->top4));
	memset(pdata->top2, 0, sizeof(pdata->top2));
	memset(&(pdata->champion), 0, sizeof(pdata->champion));

	pdata->stage = PMS_REGULAR;

	static int arr[21][3] = { { 0, 1, 2 }, { 0, 1, 3 }, { 0, 1, 4 },
			{ 0, 1, 5 }, { 0, 1, 6 }, { 0, 1, 7 }, { 1, 2, 3 }, { 1, 2, 4 }, {
					1, 2, 5 }, { 1, 2, 6 }, { 1, 2, 7 }, { 2, 3, 4 },
			{ 2, 3, 5 }, { 2, 3, 6 }, { 2, 3, 7 }, { 3, 4, 5 }, { 3, 4, 6 }, {
					3, 4, 7 }, { 4, 5, 6 }, { 4, 5, 7 }, { 5, 6, 7 } };

	srandom(time(NULL));

	for (unsigned i = 0; i < pdata->numOfApply; ++i) {
		int idx = random() % 21;
		pdata->mems[i].rivals[0] = arr[idx][0];
		pdata->mems[i].rivals[1] = arr[idx][1];
		pdata->mems[i].rivals[2] = arr[idx][2];
	}

	return 0;
}

static bool PersonCompare(const PersonMatchPlayer &left,
		const PersonMatchPlayer &right) {
	return (left.damage[0] + left.damage[1] + left.damage[2])
			> (right.damage[0] + right.damage[1] + right.damage[2]);
}

#define SET_TOP8_POS(idx,ddx) \
	pdata->top8[idx].uid = personVector[ddx].uid;\
	snprintf(pdata->top8[idx].name,PERSONMATCH_NAME_LEN_MAX,"%s",personVector[ddx].name);\
	pdata->top8[idx].damage[1] = personVector[ddx].damage[0] + personVector[ddx].damage[1] + personVector[ddx].damage[2];\
	pdata->top8[idx].instanceid = personVector[ddx].instanceid;

int CAllServerPersonBaseMatch::StartGuess(void) {
	PersonMatchData *pdata = (PersonMatchData *) m_sh.GetAddress();
	if (pdata == NULL) {
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	if (pdata->stage != PMS_REGULAR)
		return R_ERR_LOGIC;
	CAutoLock lock(&m_sh, true);
	pdata->stage = PMS_GUESS;
	vector<PersonMatchPlayer> personVector;
	for (unsigned i = 0; i < (pdata->numOfApply < 8 ? 8 : pdata->numOfApply);
			++i) {
		personVector.push_back(pdata->mems[i]);
	}

	sort(personVector.begin(), personVector.end(), PersonCompare);

	memset(pdata->top8, 0, sizeof(pdata->top8));
	memset(pdata->top4, 0, sizeof(pdata->top4));
	memset(pdata->top2, 0, sizeof(pdata->top2));
	memset(&(pdata->champion), 0, sizeof(pdata->champion));

	SET_TOP8_POS(0, 0)
	SET_TOP8_POS(1, 7)

	SET_TOP8_POS(2, 3)
	SET_TOP8_POS(3, 4)

	SET_TOP8_POS(4, 1)
	SET_TOP8_POS(5, 6)

	SET_TOP8_POS(6, 2)
	SET_TOP8_POS(7, 5)

	return 0;
}

int CAllServerPersonBaseMatch::StartPlayoff8(void) {
	PersonMatchData *pdata = (PersonMatchData *) m_sh.GetAddress();
	if (pdata == NULL) {
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}

	if (pdata->stage != PMS_GUESS)
		return R_ERR_LOGIC;

	CAutoLock lock(&m_sh, true);
	pdata->stage = PMS_PLAYOFF8;

	return 0;
}

int CAllServerPersonBaseMatch::StartPlayoff4(PersonMatchPlayer *top4) {
	PersonMatchData *pdata = (PersonMatchData *) m_sh.GetAddress();
	if (pdata == NULL) {
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	if (pdata->stage != PMS_PLAYOFF8)
		return R_ERR_LOGIC;

	CAutoLock lock(&m_sh, true);
	pdata->stage = PMS_PLAYOFF4;

	int idx;
	memcpy(pdata->top4, top4, sizeof(pdata->top4));
	return 0;
}

int CAllServerPersonBaseMatch::StartPlayoff2(PersonMatchPlayer *top2) {
	PersonMatchData *pdata = (PersonMatchData *) m_sh.GetAddress();
	if (pdata == NULL) {
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	if (pdata->stage != PMS_PLAYOFF4)
		return R_ERR_LOGIC;

	CAutoLock lock(&m_sh, true);
	pdata->stage = PMS_PLAYOFF2;

	memcpy(pdata->top2, top2, sizeof(pdata->top2));
	return 0;
}

int CAllServerPersonBaseMatch::FinishMatch(PersonMatchPlayer *champion) {
	PersonMatchData *pdata = (PersonMatchData *) m_sh.GetAddress();
	if (pdata == NULL) {
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	if (pdata->stage != PMS_PLAYOFF2)
		return R_ERR_LOGIC;

	CAutoLock lock(&m_sh, true);
	pdata->stage = PMS_MATCH_FINISH;

	CGuessData guessData;
	if (guessData.Init(Config::GetValue(GUESS_DATA_PATH), 2) != 0) {
		cerr << "guess data init fail" << endl;
		return R_ERR_LOGIC;
	}

	memcpy(&(pdata->champion), champion, sizeof(PersonMatchPlayer));
	return 0;
}

int CAllServerPersonBaseMatch::Watch(void) {
	PersonMatchData *pdata = (PersonMatchData *) m_sh.GetAddress();
	if (pdata == NULL) {
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);

	cout << "stage:" << ((int) pdata->stage) << endl;
	cout << "instance_ndx:" << pdata->instance_ndx << endl;
	cout << "numOfApply:" << pdata->numOfApply << endl;
	cout << "mems:" << endl;
	for (unsigned i = 0; i < PERSONMATCH_PLAYERS_MAX && i <= pdata->numOfApply;
			i++) {
		if (IsValidUid(pdata->mems[i].uid)) {
			cout << i << "," << pdata->mems[i].uid << "," << pdata->mems[i].name
					<< "," << pdata->mems[i].instanceid << endl;
		}
	}
	cout << "rivals:" << endl;
	for (int i = 0; i < 8; ++i) {
		cout << pdata->rival[i].uid << "," << pdata->rival[i].instanceid << ","
				<< pdata->rival[i].name << endl;
	}
	cout << endl;

	cout << "champion:" << pdata->champion.uid << "," << pdata->champion.name
			<< "," << (int) pdata->champion.damage[0] << endl << endl;
	cout << "top2:" << endl;
	cout << pdata->top2[0].uid << "," << pdata->top2[0].name << ","
			<< (int) (pdata->top2[0].damage[0]) << endl;
	cout << pdata->top2[1].uid << "," << pdata->top2[1].name << ","
			<< (int) (pdata->top2[1].damage[0]) << endl << endl;
	cout << "top4:" << endl;
	cout << pdata->top4[0].uid << "," << pdata->top4[0].name << ","
			<< (int) (pdata->top4[0].damage[0]) << endl;
	cout << pdata->top4[1].uid << "," << pdata->top4[1].name << ","
			<< (int) (pdata->top4[1].damage[0]) << endl;
	cout << pdata->top4[2].uid << "," << pdata->top4[2].name << ","
			<< (int) (pdata->top4[2].damage[0]) << endl;
	cout << pdata->top4[3].uid << "," << pdata->top4[3].name << ","
			<< (int) (pdata->top4[3].damage[0]) << endl << endl;
	cout << "top8:" << endl;
	cout << pdata->top8[0].uid << "," << pdata->top8[0].name << ","
			<< (int) (pdata->top8[0].damage[0]) << ","
			<< (int) (pdata->top8[0].damage[1]) << endl;
	cout << pdata->top8[1].uid << "," << pdata->top8[1].name << ","
			<< (int) (pdata->top8[1].damage[0]) << ","
			<< (int) (pdata->top8[1].damage[1]) << endl;
	cout << pdata->top8[2].uid << "," << pdata->top8[2].name << ","
			<< (int) (pdata->top8[2].damage[0]) << ","
			<< (int) (pdata->top8[2].damage[1]) << endl;
	cout << pdata->top8[3].uid << "," << pdata->top8[3].name << ","
			<< (int) (pdata->top8[3].damage[0]) << ","
			<< (int) (pdata->top8[3].damage[1]) << endl;
	cout << pdata->top8[4].uid << "," << pdata->top8[4].name << ","
			<< (int) (pdata->top8[4].damage[0]) << ","
			<< (int) (pdata->top8[4].damage[1]) << endl;
	cout << pdata->top8[5].uid << "," << pdata->top8[5].name << ","
			<< (int) (pdata->top8[5].damage[0]) << ","
			<< (int) (pdata->top8[5].damage[1]) << endl;
	cout << pdata->top8[6].uid << "," << pdata->top8[6].name << ","
			<< (int) (pdata->top8[6].damage[0]) << ","
			<< (int) (pdata->top8[6].damage[1]) << endl;
	cout << pdata->top8[7].uid << "," << pdata->top8[7].name << ","
			<< (int) (pdata->top8[7].damage[0]) << ","
			<< (int) (pdata->top8[7].damage[1]) << endl << endl;

	return 0;
}

int CAllServerPersonBaseMatch::GetTop8Data(vector<GuessViewData> &dataVec) {
	PersonMatchData *pdata = (PersonMatchData *) m_sh.GetAddress();
	if (pdata == NULL) {
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	for (int i = 0; i < 8; i++) {
		GuessViewData tmpData;
		memset(&tmpData, 0, sizeof(GuessViewData));
		tmpData.gid = pdata->top8[i].uid;
		snprintf(tmpData.gname, sizeof(tmpData.gname), "%s",
				pdata->top8[i].name);
		dataVec.push_back(tmpData);
	}
	return 0;

}

int CAllServerPersonBaseMatch::GetRival(int idx, PersonMatchPlayer& rival) {
	PersonMatchData *pdata = (PersonMatchData *) m_sh.GetAddress();
	if (pdata == NULL) {
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}

	if (idx < 0 || idx > 7) {
		return R_ERR_PARAM;
	}

	CAutoLock lock(&m_sh, true);
	memset(&rival, 0, sizeof(rival));
	memcpy(&rival, &(pdata->rival[idx]), sizeof(rival));

	return 0;
}

PersonMatchData* CAllServerPersonBaseMatch::GetPersonMatchData(
		CShareMemory* sh) {
	PersonMatchData *pdata = (PersonMatchData *) m_sh.GetAddress();
	if (pdata == NULL) {
		error_log("[GetAddress fail][]");
		return NULL;
	}
	sh = &m_sh;
	return pdata;
}
