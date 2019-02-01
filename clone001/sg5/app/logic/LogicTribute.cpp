#include "LogicTribute.h"

#define TRIBUTE_AMOUNT_MAX 8

int CLogicTribute::InitializeTribute(unsigned uid)
{
	const Json::Value *pInitJson = 0;
	int ret = CLogicArchive::GetInitArchive(pInitJson);
	if (ret != 0)
		return ret;
	if (!pInitJson->isMember("tribute"))
	{
		return 0;
	}
	const Json::Value &tribute = (*pInitJson)["tribute"];
	if (!tribute.isArray())
	{
		error_log("[init tribute error][uid=%u]",uid);
		DB_ERROR_RETURN_MSG("init_tribute_error");
	}
	Json::FastWriter writer;
	CDataTribute tributeDb;
	for (unsigned i = 0; i < tribute.size(); i++)
	{
		if (!tribute[i].isMember("id") || !tribute[i]["id"].isIntegral())
		{
			error_log("[init tribute error][uid=%u,i=%u]",uid,i);
			DB_ERROR_RETURN_MSG("init_tribute_error");
		}
		unsigned id = tribute[i]["id"].asUInt();
		ret = tributeDb.AddTribute(uid, id, writer.write(tribute[i]));
		if (ret != 0)
		{
			error_log("[init tribute error][uid=%u,id=%u]",uid,id);
			DB_ERROR_RETURN_MSG("init_tribute_error");
		}
	}
	return 0;
}

int CLogicTribute::ProcessTribute(unsigned uid, const Json::Value &tributes)
{
	if (!tributes.isArray())
	{
		error_log("[tributes type error][uid=%u,type=%d]",uid,tributes.type());
		DATA_ERROR_RETURN_MSG("data_tribute_error");
	}
	int ret = 0;
	CDataTribute tributeDB;
	for (unsigned i = 0; i < tributes.size(); i++)
	{
		unsigned id = 0;
		if (!Json::GetUInt(tributes[i], "id", id))
		{
			error_log("[tributes data error][uid=%u,index=%u]",uid,i);
			DATA_ERROR_RETURN_MSG("data_tribute_error");
		}
		if (tributes[i].isMember("status")
				&& tributes[i]["status"].isIntegral()
				&& tributes[i]["status"].asInt() == 9)
		{
			ret = tributeDB.RemoveTribute(uid, id);
			if (ret != 0)
			{
				error_log("[RemoveTribute fail][uid=%u,id=%llu,ret=%d]",uid,id,ret);
				DB_ERROR_RETURN_MSG("rm_tribute_fail");
			}
		}
	}

	return 0;
}

int CLogicTribute::GetTribute(unsigned uid, Json::Value &tributes)
{
	CDataTribute tributeDB;
	vector<DataTribute> datas;
	int ret = tributeDB.GetTribute(uid, datas);
	if (ret != 0)
	{
		error_log("[GetTribute fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("get_tribute_fail");
	}
	Json::Reader reader;
	tributes.resize(datas.size());
	for (unsigned i = 0; i < datas.size(); i++)
	{
		reader.parse(datas[i].data, tributes[i]);
	}
	return 0;
}

int CLogicTribute::Tribute(unsigned uid, unsigned tributeUid, const Json::Value &tribute)
{
	int level = 0;
	int tributeLevel = 0;
	if (!Json::GetInt(tribute, "level", level) || !Json::GetInt(tribute, "tribute_level", tributeLevel))
	{
		error_log("[tribute data error][uid=%u,tributeUid=%u]",uid,tributeUid);
		PARAM_ERROR_RETURN_MSG("tribute_data_error");
	}
	DataUserBasic triUserBasic;
	CLogicUserBasic logicUserBasic;
	int ret = logicUserBasic.GetUserBasicLimit(tributeUid, OpenPlatform::GetType(), triUserBasic);
	if (ret != 0)
		return ret;

	vector<unsigned> ids;
	CDataTribute tributeDB;
	ret = tributeDB.GetTributeIds(uid, ids);
	if (ret != 0)
	{
		error_log("[GetTributeIds fail][uid=%u,tributeUid=%u,ret=%d]",uid,tributeUid,ret);
		DB_ERROR_RETURN_MSG("get_tribute_fail");
	}
	unsigned maxid = 0;
	for (unsigned i = 0; i < ids.size(); i++)
	{
		if (ids[i] > maxid)
			maxid = ids[i];
	}
	DataTribute tributeData;
	tributeData.uid = uid;
	tributeData.id = maxid + 1;
	Json::Value triJson;
	triJson["id"] = tributeData.id;
	int r = Math::GetRandomInt(100);
	triJson["coins"] = 0;
	triJson["c"] = 0;
	switch(r/20)
	{
	case 0: triJson["c"] = 50;break;
	case 1: triJson["c"] = 100;break;
	case 2: triJson["coins"] = 1;break;
	case 3: triJson["coins"] = 2;break;
	default: triJson["coins"] = 4;break;
	}
	triJson["uid"] = triUserBasic.uid;
	triJson["name"] = triUserBasic.name;
	triJson["pic"] = triUserBasic.figure_url;
	Json::FastWriter writer;
	tributeData.data = writer.write(triJson);
	ret = tributeDB.AddTribute(uid, tributeData.id, tributeData.data);
	if (ret != 0)
	{
		error_log("[AddTribute fail][uid=%u,id=%u,ret=%d]",uid,tributeData.id,ret);
		DB_ERROR_RETURN_MSG("add_tribute_fail");
	}

	Json::Value triList;
	ret = GetTributeList(uid, triList);
	if (ret == 0 && triList.size() <= 100)
	{
		triList.append(tributeUid);
		CLogicUserData logicUserData;
		logicUserData.SetTributeList(uid, triList);
	}
	return 0;
}

int CLogicTribute::IsEnableTribute(const DataUser &user, unsigned tributeUid, bool &enable)
{
	unsigned uid = user.uid;
	if (uid == ADMIN_UID)
	{
		enable = false;
		return 0;
	}
	int maxTribute = GetTributeLimit(uid, user.invite_count);
	if (maxTribute <= 0)
	{
		enable = false;
		return 0;
	}
	vector<unsigned> ids;
	CDataTribute tributeDB;
	int ret = tributeDB.GetTributeIds(uid, ids);
	if (ret != 0 && ret != R_ERR_NO_DATA)
		return ret;
	if (ids.size() >= (unsigned)maxTribute)
	{
		enable = false;
		return 0;
	}
	Json::Value triList;
	ret = GetTributeList(uid, triList);
	if (ret != 0)
		return ret;
	if (triList.size() >= (unsigned)(maxTribute + 4 + 1))
	{
		enable = false;
		return 0;
	}
	if (triList.size() > 1)
	{
		for (unsigned i = 1; i < triList.size(); i++)
		{
			if (triList[i].asUInt() == tributeUid)
			{
				enable = false;
				return 0;
			}
		}
	}
	enable = true;
	return 0;
}

int CLogicTribute::DaliyTribute(unsigned uid, int invite_count)
{
	int maxTribute = GetTributeLimit(uid, invite_count);
	if (maxTribute <= 0)
		return 0;
	vector<unsigned> ids;
	CDataTribute tributeDB;
	int ret = tributeDB.GetTributeIds(uid, ids);
	if (ret != 0)
	{
		error_log("[GetTributeIds fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("get_tribute_fail");
	}
	if (ids.size() >= (unsigned)maxTribute)
		return 0;
	unsigned maxid = 0;
	for (unsigned i = 0; i < ids.size(); i++)
	{
		if (ids[i] > maxid)
			maxid = ids[i];
	}
	DataTribute tributeData;
	tributeData.uid = uid;
	tributeData.id = maxid + 1;
	Json::Value triJson;
	triJson["id"] = tributeData.id;
	triJson["coins"] = 2;
	triJson["c"] = 0;
	Json::FastWriter writer;
	tributeData.data = writer.write(triJson);
	ret = tributeDB.AddTribute(uid, tributeData.id, tributeData.data);
	if (ret != 0)
	{
		error_log("[AddTribute fail][uid=%u,id=%u,ret=%d]",uid,tributeData.id,ret);
		DB_ERROR_RETURN_MSG("add_tribute_fail");
	}
	if (ids.size() + 1 < (unsigned)maxTribute)
	{
		tributeData.id = maxid + 2;
		Json::Value triJson2;
		triJson2["id"] = tributeData.id;
		triJson2["coins"] = 0;
		triJson2["c"] = 100;
		tributeData.data = writer.write(triJson2);
		ret = tributeDB.AddTribute(uid, tributeData.id, tributeData.data);
		if (ret != 0)
		{
			error_log("[AddTribute fail][uid=%u,id=%u,ret=%d]",uid,tributeData.id,ret);
			DB_ERROR_RETURN_MSG("add_tribute_fail");
		}
	}
	return 0;
}

int CLogicTribute::GetTributeLimit(unsigned uid, int invite_count)
{
	int mainBdLvl = 0;
	Json::Value mainBd;
	CLogicBuilding logicBd;
	int ret = logicBd.GetBuilding(uid, 1, mainBd);
	if (ret == 0)
	{
		if (mainBd.isMember("l") && mainBd["l"].isIntegral())
		{
			mainBdLvl = mainBd["l"].asInt();
		}
	}
	int maxTribute = invite_count + 2;
	if (maxTribute > mainBdLvl)
		maxTribute = mainBdLvl;
	if (maxTribute < 2)
		maxTribute = 2;
	return maxTribute;
}

int CLogicTribute::GetTributeList(unsigned uid, Json::Value &tributeList)
{
	DataUserData userData;
	CLogicUserData logicUserData;
	int ret = logicUserData.GetUserData(uid, userData);
	if (ret != 0)
		return ret;
	if (userData.tribute_list.empty())
		userData.tribute_list = "[]";
	Json::Reader reader;
	reader.parse(userData.tribute_list, tributeList);
	if (tributeList.empty())
	{
		tributeList[(unsigned)0] = Time::GetGlobalTime();
		logicUserData.SetTributeList(uid, tributeList);
	}
	else
	{
		unsigned ts = tributeList[(unsigned)0].asUInt();
		if (CTime::GetDayInterval(ts, Time::GetGlobalTime()) != 0)
		{
			tributeList.resize(1);
			tributeList[(unsigned)0] = Time::GetGlobalTime();
			logicUserData.SetTributeList(uid, tributeList);
		}
	}
	return 0;
}
