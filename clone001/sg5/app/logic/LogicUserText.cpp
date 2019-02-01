#include "LogicUserText.h"

#define RELATION_UPDATE_TIME (7*24*60*60)

int CLogicUserText::SetUserData(unsigned uid, const Json::Value &data)
{
	int ret;
	CDataUserText dbUserData;
	ret = dbUserData.SetData(uid, Json::ToString(data));
	if(ret != 0)
	{
		error_log("[SetData fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("set_user_text_fail");
	}
	return 0;
}

int CLogicUserText::GetUserData(unsigned uid, Json::Value &data)
{
	int ret;
	CDataUserText dbUserData;
	string sData;
	ret = dbUserData.GetData(uid, sData);
	if(ret == 0)
	{
		if (sData.empty())
			sData = "{}";
		Json::FromString(data, sData);
	}
	else if(ret == R_ERR_NO_DATA)
	{
		ret = dbUserData.AddData(uid, "{}");
		if(ret != 0)
		{
			error_log("[AddData fail][ret=%d,uid=%u]", ret, uid);
			DB_ERROR_RETURN_MSG("add_user_text_fail");
		}
		data = Json::Value(Json::objectValue);
	}
	else
	{
		error_log("[GetData fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("get_user_text_fail");
	}
	return 0;
}

int CLogicUserText::RemoveUserData(unsigned uid)
{
	int ret;
	CDataUserText dbUserData;
	ret = dbUserData.RemoveData(uid);
	if(ret != 0)
	{
		error_log("[RemoveData fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("remove_user_text_fail");
	}
	return 0;
}
