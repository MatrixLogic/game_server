#include "LogicHelpHistory.h"

#define MAX_HELP_HISTORY_NUM 500

static bool HelpHistoryTimeCompare(const DataHelpHistory &left, const DataHelpHistory &right)
{
	return left.ts > right.ts;
}

int CLogicHelpHist::AddHelpHist(unsigned uid, const Json::Value &data)
{
	if (!data.isArray())
	{
		error_log("[help history type error][uid=%u,type=%d]",uid,data.type());
		DATA_ERROR_RETURN_MSG("data_help_history_error");
	}

	CDataHelpHist db;
	int ret = 0;

	ret = CheckHelpHistory(uid,data.size());
	if (0 != ret)
	{
		error_log("[Can not add help history][uid=%u,ret=%d]");
		return ret;
	}

	Json::FastWriter writer;
	for (unsigned i = 0; i < data.size(); i++)
	{
		if (!data[i].isObject() || !data[i].isMember("ts")
				|| !data[i]["ts"].isIntegral())
		{
			error_log("[help history data error][uid=%u,i=%d]",uid,i);
			DATA_ERROR_RETURN_MSG("data_help_history_error");
		}
		string helpType;
		if (!Json::GetString(data[i], "s", helpType))
			return R_ERROR;
		if (helpType != "BH" && helpType != "FB" && helpType != "BHWATER")
		{
			return R_ERROR;
		}

		unsigned ts = data[i]["ts"].asUInt();
		ret = db.AddHelpHist(uid, ts, writer.write(data[i]));
		if (ret != 0)
		{
			error_log("[AddHelpHist fail][uid=%u,i=%d,ret=%d]",uid,i,ret);
			DATA_ERROR_RETURN_MSG("db_add_help_histofy_fail");
		}
	}
	return R_SUCCESS;
}

int CLogicHelpHist::AddTributeHist(unsigned uid, unsigned uidBy, unsigned ts)
{
	CDataUserBasic dbUserbasic;
	string name;
	int ret = dbUserbasic.GetUserName(uidBy, OpenPlatform::GetType(), name);
	if (0 != ret)
	{
		return ret;
	}

	ret = CheckHelpHistory(uid,1);
	if (0 != ret)
	{
		error_log("[Can not add help history][uid=%u,ret=%d]");
		return ret;
	}

	Json::Value data;
	data["name"] = name;
	data["ts"] = ts;
	data["user"] = uidBy;
	data["s"] = "tribute";

	CDataHelpHist db;

	Json::FastWriter writer;
	ret = db.AddHelpHist(uid, ts, writer.write(data));
	if (ret != 0)
	{
		error_log("[AddTributeHist fail][uid=%u,i=%d,ret=%d]",uid,ret);
		return ret;
	}
	return R_SUCCESS;
}

int CLogicHelpHist::GetHelpHist(unsigned uid, Json::Value &data)
{
	CDataHelpHist db;
	vector<DataHelpHistory> datas;
	int ret = db.GetHelpHist(uid, datas);
	if (ret == R_ERR_NO_DATA)
	{
		data.resize(0);
		return 0;
	}
	if (ret != 0)
	{
		error_log("[get help histry fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("db_get_help_history_fail");
	}

	data.resize(datas.size());
	Json::Reader reader;
	for (unsigned i = 0; i < datas.size(); i++)
	{
		if (!reader.parse(datas[i].data, data[i]))
		{
			error_log("[parse help histofy fail][uid=%u,i=%d]",uid,i);
			DATA_ERROR_RETURN_MSG("db_help_histofy_data_error");
		}
	}
	return R_SUCCESS;
}

int CLogicHelpHist::CheckHelpHistory(unsigned uid, int addNum)
{
	int ret = 0;
	CDataHelpHist db;
	vector<DataHelpHistory> datas;

	ret = db.GetHelpHist(uid, datas);
	if (0 != ret)
	{
		if (R_ERR_NO_DATA == ret || datas.empty())
		{
			return R_SUCCESS;
		}
		else
		{
			return R_ERROR;
		}
	}

	if ( (datas.size() + addNum) < MAX_HELP_HISTORY_NUM )
	{
		return 0;
	}

	sort(datas.begin(), datas.end(), HelpHistoryTimeCompare);

	for (int i=0; i<addNum; ++i)
	{
		int item = datas.size() - 1 - i;
		if (item < 0)
		{
			return R_ERROR;
		}

		ret = db.RemoveHelpHistBeforeTs(uid,datas[item].ts);
		if (0 != ret)
		{
			error_log("[Remove help history failed!][uid=%u]",uid);
			return R_ERROR;
		}
	}
	return R_SUCCESS;
}


