/*
 * DataRiver.cpp
 *
 *  Created on: 2011-6-21
 *      Author: dada
 */

#include "DataRiver.h"

#define ACTIVITY 1
#define NOT_ACTIVITY 0

CDataRiver::CDataRiver() {
	m_init = false;
}

int CDataRiver::Init(const string &path, semdat sem) {
	if (m_init) {
		return 0;
	}

	int semgroup = 0;
	int semserver = 0;
	Config::GetDomain(semserver);
	if(!semserver)
		Config::GetDB(semserver);
	if (!m_sh.CreateOrOpen(path.c_str(), sizeof(DataRiver),
			SEM_ID(sem,semgroup,semserver))) {
		error_log("[init_map_data_fail][path=%s]", path.c_str());
		return R_ERROR;
	}
	CAutoLock lock(&m_sh, true);
	if (!m_sh.HasInit()) {
		DataRiver *pTable = (DataRiver *) m_sh.GetAddress();
		memset(pTable, 0, sizeof(*pTable));
		m_sh.SetInitDone();
	}
	m_init = true;
	return 0;
}

int CDataRiver::AddShip(DataShip &ship) {
	if (!IsValidUid(ship.uid)) {
		error_log("[overflow][uid=%u]", ship.uid);
		return R_ERR_DATA_LIMIT;
	}

	DataRiver *pTable = (DataRiver *) m_sh.GetAddress();
	if (pTable == NULL) {
		error_log("[map_set_user_fail][uid=%u]", ship.uid);
		return R_ERR_DB;
	}

	CAutoLock lock(&m_sh, true);

	int head = pTable->head;
	while (head != 0) {
		if (pTable->Ships[head].ship.uid == ship.uid) {
			pTable->Ships[head].ship = ship;
			return 0;
		}
		head = pTable->Ships[head].last;
	}

	unsigned uid = ship.uid;
	uid = uid % SHIP_USER_COUNT_CONTENT;
	int i = 0;
	for (int i = 0; i < SHIP_USER_COUNT_COUNT; ++i) {
		if (!pTable->Ships[uid].ship.uid)
			break;
		uid += SHIP_USER_COUNT_CONTENT;
	}
	if (i == SHIP_USER_COUNT_COUNT) {
		for (uid = SHIP_USER_COUNT_COUNT * SHIP_USER_COUNT_CONTENT;
				uid < SHIP_USER_COUNT; ++uid) {
			if (!pTable->Ships[uid].ship.uid)
				break;
		}
		if (uid == SHIP_USER_COUNT) {
			error_log("[full][uid=%u]", uid);
			return R_ERR_DATA_LIMIT;
		}
	}

	pTable->Ships[uid].ship = ship;
	if (!pTable->Ships[pTable->head].ship.uid) {
		pTable->head = uid;
		pTable->Ships[uid].last = 0;
		pTable->Ships[uid].next = 0;
	} else {
		pTable->Ships[uid].last = pTable->head;
		pTable->Ships[uid].next = 0;
		pTable->Ships[pTable->head].next = uid;
		pTable->head = uid;
	}

	return 0;
}

int CDataRiver::RemoveShip(unsigned uid) {
	if (!IsValidUid(uid)) {
		error_log("[overflow][uid=%u]", uid);
		return R_ERR_DATA_LIMIT;
	}

	DataRiver *pTable = (DataRiver *) m_sh.GetAddress();
	if (pTable == NULL) {
		error_log("[map_set_user_fail][uid=%u]", uid);
		return R_ERR_DB;
	}

	CAutoLock lock(&m_sh, true);

	int head = pTable->head;
	while (head != 0) {
		if (pTable->Ships[head].ship.uid == uid)
			break;
		head = pTable->Ships[head].last;
	}
	if (head == 0)
		return R_ERR_NO_DATA;

	uid = head;
	pTable->Ships[uid].ship.uid = NOT_ACTIVITY;
	if (pTable->head == uid) {
		pTable->head = pTable->Ships[uid].last;
	}
	if (pTable->Ships[uid].next != 0) {
		pTable->Ships[pTable->Ships[uid].next].last = pTable->Ships[uid].last;
	}
	if (pTable->Ships[uid].last != 0) {
		pTable->Ships[pTable->Ships[uid].last].next = pTable->Ships[uid].next;
	}
	pTable->Ships[uid].last = 0;
	pTable->Ships[uid].next = 0;
	return 0;
}

int CDataRiver::GetShips(vector<DataShip> &ships) {
	DataRiver *pTable = (DataRiver *) m_sh.GetAddress();
	if (pTable == NULL) {
		error_log("[map_set_user_fail]");
		return R_ERR_DB;
	}

	CAutoLock lock(&m_sh, true);
	int head = pTable->head;
	while (head != 0) {
		if (pTable->Ships[head].ship.uid) {
			ships.push_back(pTable->Ships[head].ship);
		}
		head = pTable->Ships[head].last;
	}
	return 0;
}

int CDataRiver::GetOneShip(unsigned uid,DataShip &ship) {
	DataRiver *pTable = (DataRiver *) m_sh.GetAddress();
	if (pTable == NULL) {
		error_log("[map_set_user_fail][uid=%u]", uid);
		return R_ERR_DB;
	}

	CAutoLock lock(&m_sh, true);

	int head = pTable->head;
	while (head != 0) {
		if (pTable->Ships[head].ship.uid == uid) {
			ship = pTable->Ships[head].ship;
			return 0;
		}
		head = pTable->Ships[head].last;
	}
	return R_ERR_NO_DATA;
}
