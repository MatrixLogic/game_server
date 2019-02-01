/*
 * LogicNeighbour.cpp
 *
 *  Created on: 2011-6-9
 *      Author: dada
 */

#include "LogicNeighbour.h"

int CLogicNeighbour::GetNeighbours(unsigned uid, UidList &neighbours)
{
	int ret;
	CDataNeighbour dbNeighbour;
	string sNeighbours;
	ret = dbNeighbour.GetNeighbours(uid, sNeighbours);
	if(ret == R_ERR_NO_DATA)
	{
		ret = dbNeighbour.AddNeighbours(uid, "|");
		if(ret != 0)
		{
			error_log("[AddNeighbours fail][ret=%d,uid=%u]", ret, uid);
			DB_ERROR_RETURN_MSG("set_neighbour_fail");
		}
		neighbours.clear();
		return 0;
	}
	else if(ret != 0)
	{
		error_log("[GetNeighbours fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("get_neighbour_fail");
	}
	neighbours.clear();
	vector<string> vecNeighbours;
	CBasic::StringSplitTrim(sNeighbours, "|", vecNeighbours);
	for(vector<string>::iterator itr = vecNeighbours.begin(); itr != vecNeighbours.end(); itr++)
	{
		unsigned neighbourId;
		if(Convert::StringToUInt(neighbourId, *itr))
		{
			neighbours.push_back(neighbourId);
		}
		else
		{
			error_log("[parse neighbour fail][uid=%u,neighbour=%s]", uid, itr->c_str());
		}
	}
	return 0;
}

int CLogicNeighbour::SetNeighbours(unsigned uid, const UidList &neighbours)
{
	int ret;
	UidList neighboursOld;
	ret = GetNeighbours(uid, neighboursOld);
	if(ret != 0)
	{
		return ret;
	}
	bool changed = false;
	for(UidList::const_iterator itr = neighbours.begin(); itr != neighbours.end(); itr++)
	{
		if(uid != *itr && find(neighboursOld.begin(), neighboursOld.end(), *itr) == neighboursOld.end())
		{
			//new neighbour
			AddNeighbour(*itr, uid);
			changed = true;
		}
	}
	for(UidList::const_iterator itr = neighboursOld.begin(); itr != neighboursOld.end(); itr++)
	{
		if(uid != *itr && find(neighbours.begin(), neighbours.end(), *itr) == neighbours.end())
		{
			//missed neighbour
			RemoveNeighbour(*itr, uid);
			changed = true;
		}
	}
	if(!changed)
	{
		return 0;
	}

	string sNeighbours = "|";
	string sNeighbour;
	for(UidList::const_iterator itr = neighbours.begin(); itr != neighbours.end(); itr++)
	{
		String::Format(sNeighbour, "%u|", *itr);
		sNeighbours.append(sNeighbour);
	}

	CDataNeighbour dbNeighbour;
	ret = dbNeighbour.SetNeighbours(uid, sNeighbours);
	if(ret != 0)
	{
		error_log("[SetNeighbours fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("set_neighbour_fail");
	}
	return 0;
}

bool CLogicNeighbour::IsNeighbour(unsigned uid, unsigned nid)
{
	int ret;
	CDataNeighbour dbNeighbour;
	string sNeighbours;
	ret = dbNeighbour.GetNeighbours(uid, sNeighbours);
	if(ret == R_ERR_NO_DATA)
	{
		return false;
	}
	else if(ret != 0)
	{
		error_log("[GetNeighbours fail][ret=%d,uid=%u]", ret, uid);
		return false;
	}
	string sNeighbour;
	String::Format(sNeighbour, "|%u|", nid);
	if(sNeighbours.find(sNeighbour) == string::npos)
	{
		return false;
	}
	return true;
}

int CLogicNeighbour::AddNeighbour(unsigned uid, unsigned nid)
{
	int ret;
	CDataNeighbour dbNeighbour;
	string sNeighbours;
	ret = dbNeighbour.GetNeighbours(uid, sNeighbours);
	if(ret != 0)
	{
		error_log("[GetNeighbours fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("get_neighbour_fail");
	}

	string sAppend;
	String::Format(sAppend, "%u|", nid);
	string sFind = string("|") + sAppend;
	if(sNeighbours.find(sFind) != string::npos)
	{
		error_log("[ASSERT(neighbour_not_pair)][error=AddNeighbour,uid=%u,nid=%u]", uid, nid);
		return 0;
	}

	sNeighbours.append(sAppend);
	ret = dbNeighbour.SetNeighbours(uid, sNeighbours);
	if(ret != 0)
	{
		error_log("[SetNeighbours fail][ret=%d,uid=%u,nid=%u]", ret, uid, nid);
		DB_ERROR_RETURN_MSG("set_neighbour_fail");
	}
	return 0;
}

int CLogicNeighbour::RemoveNeighbour(unsigned uid, unsigned nid)
{
	int ret;
	CDataNeighbour dbNeighbour;
	string sNeighbours;
	ret = dbNeighbour.GetNeighbours(uid, sNeighbours);
	if(ret != 0)
	{
		error_log("[GetNeighbours fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("get_neighbour_fail");
	}

	string sFind;
	String::Format(sFind, "|%u|", nid);
	if(sNeighbours.find(sFind) == string::npos)
	{
		error_log("[ASSERT(neighbour_not_pair)][error=RemoveNeighbour,uid=%u,nid=%u]", uid, nid);
		return 0;
	}

	CBasic::StringReplace(sNeighbours, sFind, "|");
	ret = dbNeighbour.SetNeighbours(uid, sNeighbours);
	if(ret != 0)
	{
		error_log("[SetNeighbours fail][ret=%d,uid=%u,nid=%u]", ret, uid, nid);
		DB_ERROR_RETURN_MSG("set_neighbour_fail");
	}
	return 0;
}

int CLogicNeighbour::GetUserInfoJson(const Json::Value &uidList, Json::Value &result)
{
	if (uidList.isNull() || !uidList.isArray() || uidList.empty())
	{
		PARAM_ERROR_RETURN_MSG("param_error");
	}
	CLogicUserBasic logicUserBasic;
	DataUserBasic userBasic;
	unsigned size = uidList.size();
	result.resize(size);
	for (unsigned i = 0; i < size; i++)
	{
		if (!uidList[i].isIntegral()) continue;
		unsigned uid = uidList[i].asUInt();
		userBasic.uid = uid;
		userBasic.figure_url.clear();
		userBasic.name.clear();
		userBasic.open_id.clear();
		logicUserBasic.GetUserBasicLimitSmart(uid, OpenPlatform::GetType(), userBasic);
		result[i]["uid"] = userBasic.uid;
		result[i]["openid"] = userBasic.open_id;
		result[i]["name"] = userBasic.name;
		result[i]["pic"] = userBasic.figure_url;
	}

	return 0;
}
