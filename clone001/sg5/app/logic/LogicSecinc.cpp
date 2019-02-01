#include "LogicSecinc.h"

int CLogicSecinc::AddSecinc(unsigned uid, unsigned eqid, unsigned amount)
{
	if (amount == 0) return 0;
	uint64_t tempid = 0;
	CLogicIdCtrl logicIdCtrl;
	int ret = logicIdCtrl.GetNextId(KEY_SECINC_ID_CTRL, tempid);
	if (ret != 0)
	{
		error_log("[GetNextId fail][ret=%d,uid=%u,eqid=%d,amount=%d]",
				ret, uid, eqid, amount);
		DB_ERROR_RETURN_MSG("get_secinc_id_fail");
	}
	DataSecinc secinc;
	secinc.uid = uid;
	secinc.sid = tempid;
	Json::Value data;
	data["sid"] = secinc.sid;
	data["d"]["t"] = 1;
	data["d"]["id"] = eqid;
	data["d"]["q"] = amount;
	Json::FastWriter writer;
	secinc.data = writer.write(data);
	CDataSecinc secincDb;
	ret = secincDb.AddSecinc(uid, secinc.sid, secinc.data);
	if (ret != 0)
	{
		error_log("[AddSecinc fail][ret=%d,uid=%u,eqid=%d,amount=%d]",
				ret, uid, eqid, amount);
		DB_ERROR_RETURN_MSG("add_secinc_fail");
	}
	return 0;
}

int CLogicSecinc::GetSecinc(unsigned uid, Json::Value &data)
{
	CDataSecinc secincDb;
	vector<DataSecinc> datas;
	int ret = secincDb.GetSecinc(uid, datas);
	if (ret != 0 && ret != R_ERR_NO_DATA)
	{
		error_log("[GetSecinc fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("get_secinc_fail");
	}
	unsigned size = datas.size();
	data.resize(size);
	Json::Reader reader;
	for (unsigned i = 0; i < size; i++)
	{
		reader.parse(datas[i].data, data[i]);
	}
	return 0;
}

int CLogicSecinc::ProcessSecinc(unsigned uid, const Json::Value &data)
{
	if (!data.isArray())
	{
		error_log("[secinc type error][uid=%u,type=%d]",uid,data.type());
		DATA_ERROR_RETURN_MSG("secinc_type_error");
	}
	CDataSecinc secincDb;
	unsigned size = data.size();
	for (unsigned i = 0; i < size; i++)
	{
		if (!data[i].isIntegral())
		{
			error_log("[secinc data error][uid=%u,i=%u]",uid,i);
			DATA_ERROR_RETURN_MSG("secinc_data_error");
		}
		unsigned sid = data[i].asUInt();
		int ret = secincDb.RemoveSecinc(uid, sid);
		if (ret != 0)
		{
			error_log("[RemoveSecinc fail][uid=%u,sid=%u]",uid,sid);
			DB_ERROR_RETURN_MSG("remove_secinc_fail");
		}
	}
	return 0;
}
