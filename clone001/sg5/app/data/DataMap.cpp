/*
 * DataMap.cpp
 *
 *  Created on: 2011-6-21
 *      Author: dada
 */

#include "DataMap.h"
#include "DataInc.h"

#define MAX_USER_GROUP 200
#define MAX_USER_LIMIT 10

struct MapDataItem {
	int32_t MaxCount[MAX_USER_LIMIT];	//当前组最大可添加数
	int32_t CurrConut[MAX_USER_LIMIT];
	/*int32_t Reserve2;
	 int32_t Reserve3;*/
	uint32_t UserListHead[MAX_USER_LIMIT];	//以可添加关系数作为索引
};

struct MapUserData {
	unsigned Valid;	//是否有效
	uint16_t Group;	//所属组
	//uint16_t Count;	//当前关系数
	//uint16_t Limit;	//最大关系数
	uint16_t Type;  //用户类型
	uint32_t LastUid;
	uint32_t NextUid;
};

struct MapTable {
	MapDataItem MapData[MAX_USER_GROUP];	//分组列表
	MapUserData UserData[MAP_USER_COUNT];	//用户数据表
};

CDataMap::CDataMap() {
	m_init = false;
}

int CDataMap::Init(const string &path, semdat sem) {
	if (m_init) {
		return 0;
	}

	int semgroup = 0;
	int semserver = 0;
	Config::GetDomain(semserver);
	if(!semserver)
		Config::GetDB(semserver);
	if (!m_sh.CreateOrOpen(path.c_str(), sizeof(MapTable), SEM_ID(sem,semgroup,semserver))) {
		error_log("[init_map_data_fail][path=%s]", path.c_str());
		return R_ERROR;
	}
	CAutoLock lock(&m_sh, true);
	if (!m_sh.HasInit()) {
		MapTable *pTable = (MapTable *) m_sh.GetAddress();
		memset(pTable, 0, sizeof(*pTable));
		m_sh.SetInitDone();
	}
	m_init = true;
	return 0;
}

int CDataMap::SetUser(unsigned uid, int group, int type) {
	if (!IsValidUid(uid)) {
		error_log("[overflow][uid=%u,group=%d]", uid, group);
		return R_ERR_DATA_LIMIT;
	}

	MapTable *pTable = (MapTable *) m_sh.GetAddress();
	if (pTable == NULL) {
		error_log("[map_set_user_fail][uid=%u,group=%d,count=%d,limit=%d]", uid,
				group);
		return R_ERR_DB;
	}

	CAutoLock lock(&m_sh, true);

	unsigned count = uid;
	uid = uid % MAP_USER_COUNT_CONTENT;
	int i = 0;
	MapUserData *pUserData;
	for (; i < MAP_USER_COUNT_COUNT; ++i) {
		pUserData = &(pTable->UserData[uid]);

		if (pUserData->Valid == 0 || pUserData->Valid == count)
			break;
		uid += MAP_USER_COUNT_CONTENT;
	}
	if (i == MAP_USER_COUNT_COUNT) {
		for (uid = MAP_USER_COUNT_COUNT * MAP_USER_COUNT_CONTENT;
				uid < MAP_USER_COUNT; ++uid) {
			pUserData = &(pTable->UserData[uid]);

			if (pUserData->Valid == 0 || pUserData->Valid == count)
				break;
		}
		if (uid == MAP_USER_COUNT) {
			error_log("[full][uid=%u,count=%d]", uid, count);
			return R_ERR_DATA_LIMIT;
		}
	}

	if (pUserData->Valid == 0) {	//新用户
		pUserData->Group = group;
		pUserData->Type = type;
		MapDataItem *pDataItem = &(pTable->MapData[group]);
		unsigned nextUid = pDataItem->UserListHead[type];
		pUserData->NextUid = nextUid;
		if (nextUid != 0) {
			pTable->UserData[nextUid].LastUid = uid;
		}
		pUserData->LastUid = 0;
		pDataItem->UserListHead[type] = uid;
		pUserData->Valid = count;
		pDataItem->MaxCount[type]++;

	} else {	//已有用户
		if (pUserData->Type == type && pUserData->Group == group) {
			return 0;
		}

		MapDataItem *pDataItem = &(pTable->MapData[pUserData->Group]);
		if (pUserData->LastUid != 0) {
			pTable->UserData[pUserData->LastUid].NextUid = pUserData->NextUid;
		} else {
			pDataItem->UserListHead[pUserData->Type] = pUserData->NextUid;
		}
		if (pUserData->NextUid != 0) {
			pTable->UserData[pUserData->NextUid].LastUid = pUserData->LastUid;
		}
		pDataItem->MaxCount[pUserData->Type]--;
		if (pDataItem->MaxCount[pUserData->Type] < 0) {
			pDataItem->MaxCount[pUserData->Type] = 0;
		}

		pDataItem = &(pTable->MapData[group]);
		pUserData->LastUid = 0;
		unsigned headUid = pDataItem->UserListHead[type];
		pUserData->NextUid = headUid;
		if (headUid != 0) {
			pTable->UserData[headUid].LastUid = uid;
		}
		pDataItem->UserListHead[type] = uid;
		pUserData->Group = group;
		pUserData->Type = type;
		pDataItem->MaxCount[type]++;
	}

	return 0;
}

/*int CDataMap::SetCount(unsigned uid, int count)
 {
 int uid_min = UID_MIN;
 Config::GetIntValue(uid_min, "uid_min");

 if(uid < uid_min || uid >= (uid_min + MAP_USER_COUNT))
 {
 error_log("[uid overflow][uid=%u]", uid);
 return R_ERR_DATA_LIMIT;
 }
 uid -= uid_min;

 MapTable *pTable = (MapTable *)m_sh.GetAddress();
 if(pTable == NULL)
 {
 error_log("[map_set_count_fail][uid=%u,count=%d]", uid, count);
 return R_ERR_DB;
 }

 CAutoLock lock(&m_sh, true);
 MapUserData *pUserData = &(pTable->UserData[uid]);
 if(count == pUserData->Count)
 {
 return 0;
 }

 MapDataItem *pDataItem = &(pTable->MapData[pUserData->Group]);
 if(pUserData->LastUid != 0)
 {
 pTable->UserData[pUserData->LastUid].NextUid = pUserData->NextUid;
 }
 else
 {
 int lastCount = pUserData->Limit - pUserData->Count;
 if(lastCount < 0)
 {
 lastCount = 0;
 }
 pDataItem->UserListHead[lastCount] = pUserData->NextUid;
 }
 if(pUserData->NextUid != 0)
 {
 pTable->UserData[pUserData->NextUid].LastUid = pUserData->LastUid;
 }

 int currCount = pUserData->Limit - count;
 if(currCount < 0)
 {
 currCount = 0;
 }
 if(currCount > pDataItem->MaxCount)
 {
 pDataItem->MaxCount = currCount;
 }
 pUserData->LastUid = 0;
 unsigned headUid = pDataItem->UserListHead[currCount];
 pUserData->NextUid = headUid;
 if(headUid != 0)
 {
 pTable->UserData[headUid].LastUid = uid;
 }
 pDataItem->UserListHead[currCount] = uid;
 pUserData->Count = count;
 return 0;
 }*/

int CDataMap::GetUser(int group, unsigned &uid, const int type) {
	MapTable *pTable = (MapTable *) m_sh.GetAddress();
	if (pTable == NULL) {
		error_log("[map_get_user_fail][group=%d]", group);
		return R_ERR_DB;
	}

	CAutoLock lock(&m_sh, true);
	MapDataItem *pDataItem = &(pTable->MapData[group]);
	unsigned nextUid = pDataItem->UserListHead[type];
	if (pDataItem->CurrConut[type] > pDataItem->MaxCount[type]) {
		pDataItem->CurrConut[type] = 1;
	}
	if (nextUid != 0) {
		for (int n = 0; n < pDataItem->CurrConut[type] && nextUid != 0; n++) {
			uid = nextUid;
			nextUid = pTable->UserData[uid].NextUid;
		}
		if (nextUid != 0) {
			uid = nextUid;
		}
		uid = pTable->UserData[uid].Valid;
		pDataItem->CurrConut[type]++;
		return 0;
	}

	return R_ERR_DATA;
}
