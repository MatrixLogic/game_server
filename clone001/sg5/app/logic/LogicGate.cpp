#include "LogicGate.h"
#include <fstream>

map<int, GateRank> CLogicGate::gateRankMap;

int CLogicGate::GetGate(unsigned uid, Json::Value &data)
{
	CDataGate bdDB;
	vector<DataGate> datas;
	int ret = bdDB.GetGate(uid, datas);
	if (ret != 0 && ret != R_ERR_NO_DATA)
	{
		error_log("[GetGate fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("db_get_gate_fail");
	}
	if (ret == R_ERR_NO_DATA)
	{
		data.resize(0);
		return 0;
	}
	Json::Reader reader;
	data.resize(datas.size());
	for (unsigned i = 0; i < datas.size(); i++)
	{
		if (!reader.parse(datas[i].data, data[i]))
		{
			error_log("[parse fail][uid=%u,i=%u]",uid,i);
			DB_ERROR_RETURN_MSG("db_gate_data_error");
		}
	}
	return 0;
}

int CLogicGate::UpdateGate(unsigned uid, const Json::Value &data)
{
	if (!data.isArray())
	{
		error_log("[gate type error][uid=%u,type=%d]",uid,data.type());
		DATA_ERROR_RETURN_MSG("data_gate_error");
	}
	int ret = 0;
	Json::FastWriter writer;
	Json::Reader reader;
	CDataGate gateDB;
	vector<unsigned> ids;
	ret = gateDB.GetGateIds(uid, ids);
	if (ret != 0 && ret != R_ERR_NO_DATA)
	{
		error_log("[GetGate fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("get_gate_fail");
	}
	for (unsigned i = 0; i < data.size(); i++)
	{
		unsigned id = 0;
		int w  = 0;
		if (!Json::GetUInt(data[i], "id", id) || !Json::GetInt(data[i], "w", w))
		{
			error_log("[gate data error][uid=%u,index=%u]",uid,i);
			DATA_ERROR_RETURN_MSG("data_gate_error");
		}
		if (w == 0)
		{
			unsigned j = 0;
			for (; j < ids.size(); j++)
			{
				if (id == ids[j]) break;
			}
			if (j < ids.size()) continue;
		}
		ret = gateDB.ReplaceGate(uid, id, writer.write(data[i]));
		if (ret != 0)
		{
			error_log("[ReplaceGate fail][uid=%u,id=%u,ret=%d]",uid,id,ret);
			DB_ERROR_RETURN_MSG("db_set_gate_fail");
		}
	}
	return 0;
}

int CLogicGate::GetGateRank(unsigned uid, int gateId, Json::Value &json)
{
	const vector<GateRankData>& gate = GetGateRankVec(gateId);
	unsigned self = 0;
	for (unsigned i = 0; i < gate.size(); i++)
	{
		if (gate[i].uid == uid)
		{
			self = i + 1;
			break;
		}
	}
	Json::Value rank;
	unsigned size = gate.size() >= 11? 11 : gate.size();
	rank.resize(size);
	for (unsigned i = 0; i < size; i++)
	{
		rank[i]["uid"] = gate[i].uid;
		rank[i]["best_score"] = gate[i].score;
		rank[i]["name"] = gate[i].name;
		rank[i]["rank"] = (i + 1);
	}
	json["rank"] = rank;
	json["self"] = self;

	return 0;
}

int CLogicGate::Load(unsigned uid, unsigned start, unsigned end, Json::Value &json)
{
	if (start > end)
	{
		json["gate"].resize(0);
		return 0;
	}
	if (end == 0)
		end = (unsigned)-1;
	CDataGate db;
	vector<DataGate> datas;
	int ret = db.GetGate(uid, start, end, datas);
	if (ret != 0 && ret != R_ERR_NO_DATA)
	{
		error_log("[GetGate fail][uid=%u,ret=%d,start=%u,end=%u]",uid,ret,start,end);
		DB_ERROR_RETURN_MSG("db_get_gate_fail");
	}
	if (ret == R_ERR_NO_DATA)
	{
		json["gate"].resize(0);
		return 0;
	}
	Json::Reader reader;
	json["gate"].resize(datas.size());
	for (unsigned i = 0; i < datas.size(); i++)
	{
		if (!reader.parse(datas[i].data, json["gate"][i]))
		{
			error_log("[parse fail][uid=%u,i=%u,start=%u,end=%u]",uid,i,start,end);
			DB_ERROR_RETURN_MSG("db_gate_data_error");
		}
	}
	return 0;
}

const vector<GateRankData>& CLogicGate::GetGateRankVec(int gateId)
{
	static vector<GateRankData> nullRank;

	string gatePath = Config::GetValue(CONFIG_GATE_RAND_DIR);
	if (gatePath.empty())
	{
		error_log("[gate rank dir error][]");
		return nullRank;
	}
	if (gatePath[gatePath.length() - 1] != '/')
		gatePath.append("/");
	gatePath.append("top_").append(CTrans::ITOS(gateId)).append(".txt");
	ifstream fin(gatePath.c_str());
	if (!fin)
	{
		error_log("[read rank fail][path=%s]",gatePath.c_str());
		return nullRank;
	}

	map<int, GateRank>::iterator it = gateRankMap.find(gateId);
	bool needRead = false;
	if (it == gateRankMap.end())
	{
		GateRank gateRankTmp;
		gateRankTmp.lastReadTime = 0;
		gateRankMap[gateId] = gateRankTmp;
		needRead = true;
	}
	else
	{
		if (Time::GetGlobalTime() - (it->second).lastReadTime > 600)
			needRead = true;
	}
	it = gateRankMap.find(gateId);
	if (it == gateRankMap.end())
	{
		return nullRank;
	}
	if (needRead)
	{
		int count = 0;
		(it->second).gateRankVec.clear();
		string lineBuf;
		while (getline(fin, lineBuf))
		{
			if (count >= 1000)
				break;
			if (lineBuf.empty())
				continue;
			vector<string> items;
			CBasic::StringSplit(lineBuf, "\t", items);
			if (items.size() < 3)
				continue;
			GateRankData rankData;
			rankData.uid = CTrans::STOI(items[0]);
			rankData.score = CTrans::STOI(items[2]);
			rankData.name = items[1];
			(it->second).gateRankVec.push_back(rankData);
			count++;
			lineBuf.clear();
		}
		(it->second).lastReadTime = Time::GetGlobalTime();
	}
	fin.close();

	return (it->second).gateRankVec;
}
