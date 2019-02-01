#include "LogicQuest.h"

int CLogicQuest::GetQuest(unsigned uid, Json::Value &data)
{
	CDataQuest bdDB;
	vector<DataQuest> datas;
	int ret = bdDB.GetQuest(uid, datas);
	if (ret != 0 && ret != R_ERR_NO_DATA)
	{
		error_log("[GetQuest fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("db_get_quest_fail");
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
			DB_ERROR_RETURN_MSG("db_quest_data_error");
		}
	}
	return 0;
}

int CLogicQuest::UpdateQuest(unsigned uid, const Json::Value &data)
{
	if (!data.isArray())
	{
		error_log("[quest type error][uid=%u,type=%d]",uid,data.type());
		DATA_ERROR_RETURN_MSG("data_quest_error");
	}
	int ret = 0;
	Json::FastWriter writer;
	CDataQuest bdDB;
	for (unsigned i = 0; i < data.size(); i++)
	{
		unsigned id = 0;
		if (!Json::GetUInt(data[i], "id", id))
		{
			error_log("[quest data error][uid=%u,index=%u]",uid,i);
			DATA_ERROR_RETURN_MSG("data_quest_error");
		}
//		(data[i].isMember("c")
//						&& data[i]["c"].isIntegral()
//						&& data[i]["c"].asInt() == 9 )||
		if ((data[i].isMember("status")
				&& data[i]["status"].isIntegral()
				&& data[i]["status"].asInt() == 9))
		{
			ret = bdDB.RemoveQuest(uid, id);
			if (ret != 0)
			{
				error_log("[RemoveQuest fail][uid=%u,id=%u,ret=%d]",uid,id,ret);
				DB_ERROR_RETURN_MSG("db_rm_quest_fail");
			}
		}
		else
		{
			ret = bdDB.ReplaceQuest(uid, id, writer.write(data[i]));
			if (ret != 0)
			{
				error_log("[ReplaceQuest fail][uid=%u,id=%u,ret=%d]",uid,id,ret);
				DB_ERROR_RETURN_MSG("db_set_quest_fail");
			}
		}
	}
	return 0;
}
