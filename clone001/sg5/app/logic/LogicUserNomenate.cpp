#include "LogicUserNomenate.h"

int CLogicUserNomenate::InitializeUserNomenate(unsigned uid, DataUserNomenate *pData)
{
	DataUserNomenate userNomenate;
	userNomenate.uid = uid;
	userNomenate.rec_list = "";
	userNomenate.add_list = "";
	userNomenate.black_list = "";
	userNomenate.friend_list = "";
	userNomenate.update_time = 0;

	CDataUserNomenate cuserNomenate;
	int ret = cuserNomenate.addNomenate(userNomenate);
	if (ret != 0)
	{
		error_log("[AddUserNomenate fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("add_user_data_fail");
	}
	if (pData)
	{
		pData->uid = userNomenate.uid;
		pData->rec_list = userNomenate.rec_list;
		pData->add_list = userNomenate.add_list;
		pData->black_list = userNomenate.black_list;
		pData->friend_list = userNomenate.friend_list;
		pData->update_time = userNomenate.update_time;
	}
	return 0;
}
int CLogicUserNomenate::AddUserNomenate(const DataUserNomenate &data)
{
	CDataUserNomenate cuserNomenate;
	int ret = cuserNomenate.addNomenate(data);
	if(ret != 0)
	{
		error_log("[AddUserNomenate fail][uid=%u,ret=%d]",data.uid,ret);
		DB_ERROR_RETURN_MSG("get_user_data_fail");
	}
	if (ret == R_ERR_NO_DATA)
	{
		ret = InitializeUserNomenate((unsigned)data.uid, (DataUserNomenate *)&data);
		if (ret != 0)
			return ret;
	}
	ret = JoinNewUserRecList(data.uid);
	if (0 != ret)
	{
		error_log("[JoinNewUserRecList faile][ret=%d,uid=%u]",ret);
	}
	return 0;
}
int CLogicUserNomenate::GetUserNomenate(unsigned uid, DataUserNomenate &data)
{
	CDataUserNomenate cuserNomenate;
	int ret = cuserNomenate.getNomenate(uid, data);
	if (ret != 0 && ret != R_ERR_NO_DATA)
	{
		printf("[GetUserNomenate fail][uid=%u,ret=%d]",uid,ret);
		error_log("[GetUserNomenate fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("get_user_data_fail");
	}
	if (ret == R_ERR_NO_DATA)
	{
		ret = InitializeUserNomenate(uid, &data);
		if (ret != 0)
			return ret;
	}
	return 0;
}
int CLogicUserNomenate::SetUserNomenate(unsigned uid, const DataUserNomenate &data)
{
	CDataUserNomenate cuserNomenate;
	int ret = cuserNomenate.setNomenate(uid, data);
	if (ret != 0)
	{
		error_log("[SetUserNomenate fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("set_user_data_fail");
	}
	return 0;
}
int CLogicUserNomenate::DeleteUserNomenate(unsigned uid)
{
	CDataUserNomenate cuserNomenate;
	int ret = cuserNomenate.deleteNomenate(uid);
	if(ret != 0)
	{
		error_log("[DelUserNomenate fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("set_user_data_fail");
	}
	return 0;
}
int CLogicUserNomenate::setFriendRecList(unsigned uid, const string &rec_list)
{
	CDataUserNomenate cuserNomenate;
	int ret = cuserNomenate.setRecList(uid, rec_list);
	if(ret != 0)
	{
		error_log("[setFriendRecList fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("set_user_data_fail");
	}
	return 0;
}
int CLogicUserNomenate::getFriendRecList(unsigned uid, string &rec_list)
{
	CDataUserNomenate cuserNomenate;
	int ret = cuserNomenate.getRecList(uid, rec_list);
	if(ret != 0)
	{
		error_log("[getFriendRecList fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("set_user_data_fail");
	}
	return 0;
}
int CLogicUserNomenate::setFriendAddList(unsigned uid, const string &add_list)
{
	CDataUserNomenate cuserNomenate;
	int ret = cuserNomenate.setAddList(uid, add_list);
	if(ret != 0)
	{
		error_log("[setFriendAddList fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("set_user_data_fail");
	}
	return 0;
}
int CLogicUserNomenate::getFriendAddList(unsigned uid, string &add_list)
{
	CDataUserNomenate cuserNomenate;
	int ret = cuserNomenate.getAddList(uid, add_list);
	if(ret != 0)
	{
		error_log("[getFriendAddList fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("get_user_data_fail");
	}
	return 0;
}
int CLogicUserNomenate::setFriendBlackList(unsigned uid, const string &black_list)
{
	CDataUserNomenate cuserNomenate;
	int ret = cuserNomenate.setBlackList(uid, black_list);
	if(ret != 0)
	{
		error_log("[setFriendBlackList fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("set_user_data_fail");
	}
	return 0;
}
int CLogicUserNomenate::getFriendBlackList(unsigned uid, string &black_list)
{
	CDataUserNomenate cuserNomenate;
	int ret = cuserNomenate.getBlackList(uid, black_list);
	if(ret != 0)
	{
		error_log("[getFriendBlackList fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("get_user_data_fail");
	}
	return 0;
}
int CLogicUserNomenate::setFriendList(unsigned uid, const string &friend_list)
{
	CDataUserNomenate cuserNomenate;
	int ret = cuserNomenate.setFriendList(uid, friend_list);
	if(ret != 0)
	{
		error_log("[setFriendList fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("set_user_data_fail");
	}
	return 0;
}
int CLogicUserNomenate::getFriendList(unsigned uid, string &friend_list)
{
	CDataUserNomenate cuserNomenate;
	int ret = cuserNomenate.getFriendList(uid, friend_list);
	if(ret != 0)
	{
		error_log("[getFriendList fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("get_user_data_fail");
	}
	return 0;
}

int CLogicUserNomenate::GetNewUserRecList(string &list)
{
	int ret = 0;
	CDataNewUserRecList *p_data = GetCDataNewUserRecList();
	if (NULL == p_data)
	{
		return R_ERR_DATA;
	}
	unsigned rec_list[MAX_REC_LIST_COUNT];
	ret = p_data->GetRecList(rec_list,MAX_REC_LIST_COUNT);
	if (0 != ret)
	{
		error_log("[GetRecList fail][ret=%d]",ret);
		return ret;
	}

	int index = rand() % MAX_REC_LIST_COUNT;
	index = (index==0) ? index+1 : index;
	int count = 0;
	for (unsigned i=index; count < 26 && i != index-1; i++)
	{
		if (i >= MAX_REC_LIST_COUNT)
		{
			i = 0;
		}
		if(IsValidUid(rec_list[i]))
		{
			string tem_uid;
			String::Format(tem_uid, "%u", rec_list[i]);
			list.append("|").append(tem_uid);
			count++;
		}
	}
	return 0;
}

int CLogicUserNomenate::JoinNewUserRecList(unsigned uid)
{
	int ret = 0;
	CDataNewUserRecList *p_data = GetCDataNewUserRecList();
	if (NULL == p_data)
	{
		return R_ERR_DATA;
	}

	ret = p_data->SetRecList(uid);
	if (0 != ret)
	{
		error_log("[SetRecList fail][ret=%d]",ret);
		return ret;
	}
	return 0;
}

CDataNewUserRecList * CLogicUserNomenate::GetCDataNewUserRecList(void)
{
	GET_MEM_DATA_SEM(CDataNewUserRecList, CONFIG_NEW_USER_REC_LIST_PATH,sem_new_user_rec)
}

