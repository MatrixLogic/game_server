/*
 * LogicUserMessage.cpp
 *
 *  Created on: 2013-4-18
 *      Author: Administrator
 */

#include "LogicUserMessage.h"

int CLogicUserMessage::AddUserMessage(const DataUserMessage &data)
{
	int ret = 0;
	if (IsValidUid(data.uid))
	{
		CDataUserMessage userMsgDB;
		ret = userMsgDB.AddUserMessage(data);
		if (0 != ret)
		{
			error_log("[AddUserMessage fail][uid=%u,massId=%ul,ret=%d]", data.uid,data.message_id,ret);
		}
		return ret;
	}
	else if (IsAllianceId(data.uid))
	{
		CDataAllianceMessage AllianceMsgDB;
		ret = AllianceMsgDB.AddAllianceMessage(data);
		if (0 != ret)
		{
			error_log("[AddAllianceMessage fail][aid=%u,massId=%ul,ret=%d]", data.uid,data.message_id,ret);
		}
		return ret;
	}

	return R_ERR_REFUSE;
}

int CLogicUserMessage::SetUserMessage(const DataUserMessage &data)
{
	int ret = 0;
	if (IsValidUid(data.uid))
	{
		CDataUserMessage userMsgDB;
		ret = userMsgDB.SetUserMessage(data);
		if (0 != ret)
		{
			error_log("[SetUserMessage fail][uid=%u,massId=%ul,ret=%d]", data.uid,data.message_id,ret);
		}
		return ret;
	}
	else if (IsAllianceId(data.uid))
	{
		CDataAllianceMessage AllianceMsgDB;
		ret = AllianceMsgDB.SetAllianceMessage(data);
		if (0 != ret)
		{
			error_log("[SetAllianceMessage fail][aid=%u,massId=%ul,ret=%d]", data.uid,data.message_id,ret);
		}
		return ret;
	}
	return R_ERR_REFUSE;
}

int CLogicUserMessage::SetUserMessageStatus(unsigned uid, uint64_t message_id, int status)
{
	int ret = 0;
	if (IsValidUid(uid))
	{
		CDataUserMessage userMsgDB;
		ret = userMsgDB.SetUserMessageStatus(uid, message_id, status);
		if (0 != ret)
		{
			error_log("[SetUserMessageStatus fail][uid=%u,massId=%ul,status=%d,ret=%d]",uid,message_id,status,ret);
		}
		return ret;
	}
	else if (IsAllianceId(uid))
	{
		CDataAllianceMessage AllianceMsgDB;
		ret = AllianceMsgDB.SetAllianceMessageStatus(uid, message_id, status);
		if (0 != ret)
		{
			error_log("[SetAllianceMessageStatus fail][aid=%u,massId=%ul,status=%d,ret=%d]",uid,message_id,status,ret);
		}
		return ret;
	}
	return R_ERR_REFUSE;
}

int CLogicUserMessage::GetUserMessage(unsigned uid, uint64_t message_id, DataUserMessage &data)
{
	int ret = 0;
	if (IsValidUid(uid))
	{
		CDataUserMessage userMsgDB;
		ret = userMsgDB.GetUserMessage(uid,message_id,data);
		if (0 != ret)
		{
			error_log("[GetUserMessage fail][uid=%u,massId=%ul,ret=%d]", uid, message_id,ret);
		}
		return ret;
	}
	else if (IsAllianceId(uid))
	{
		CDataAllianceMessage AllianceMsgDB;
		ret = AllianceMsgDB.GetAllianceMessage(uid, message_id, data);
		if (0 != ret)
		{
			error_log("[GetAllianceMessage fail][uid=%u,massId=%ul,ret=%d]", uid, message_id,ret);
		}
		return ret;
	}
	return R_ERR_REFUSE;
}

int CLogicUserMessage::GetUserMessage(unsigned uid, int type, int status,unsigned startTime, vector<DataUserMessage> &datas)
{
	int ret = 0;
	if (IsValidUid(uid))
	{
		CDataUserMessage userMsgDB;
		ret = userMsgDB.GetUserMessage(uid,type,status,startTime,datas);
		if (0 != ret)
		{
			error_log("[GetUserMessage fail][uid=%u,type=%d,statu=%d,startTime=%u,ret=%d]",
					uid,type,status,startTime,ret);
		}
		return ret;
	}
	else if (IsAllianceId(uid))
	{
		CDataAllianceMessage AllianceMsgDB;
		ret = AllianceMsgDB.GetAllianceMessage(uid,type,status,startTime,datas);
		if (0 != ret)
		{
			error_log("[GetAllianceMessage fail][aid=%u,type=%d,statu=%d,startTime=%u,ret=%d]",
						uid,type,status,startTime,ret);
		}
		return ret;
	}
	return R_ERR_REFUSE;
}

int CLogicUserMessage::GetUserMessageForTime(unsigned uid, int type, unsigned startTime, vector<DataUserMessage> &datas)
{
	int ret = 0;
	if (IsValidUid(uid))
	{
		CDataUserMessage userMsgDB;
		ret = userMsgDB.GetUserMessageForTime(uid,type,startTime,datas);
		if (0 != ret)
		{
			error_log("[GetUserMessageForTime fail][uid=%u,type=%d,startTime=%u,ret=%d]",
					uid,type,startTime,ret);
		}
		return ret;
	}
	else if (IsAllianceId(uid))
	{
		CDataAllianceMessage AllianceMsgDB;
		ret = AllianceMsgDB.GetAllianceMessageForTime(uid,type,startTime,datas);
		if(0 != ret)
		{
			error_log("[GetAllianceMessageForTime fail][aid=%u,type=%d,startTime=%u,ret=%d]",
						uid,type,startTime,ret);
		}
		return ret;
	}
	return R_ERR_REFUSE;
}

int CLogicUserMessage::GetUserMessageForTime(unsigned uid, int type, int toward, unsigned startTime, vector<DataUserMessage> &datas)
{
	int ret = 0;
	if (IsValidUid(uid))
	{
		CDataUserMessage userMsgDB;
		ret = userMsgDB.GetUserMessageForTime(uid,type,toward,startTime,datas);
		if (0 != ret)
		{
			error_log("[GetUserMessageForTime fail][uid=%u,type=%d,toward=%d,startTime=%u,ret=%d]",
						uid,type,toward,startTime,ret);
		}
		return ret;
	}
	else if (IsAllianceId(uid))
	{
		CDataAllianceMessage AllianceMsgDB;
		ret = AllianceMsgDB.GetAllianceMessageForTime(uid,type,toward,startTime,datas);
		if(0 != ret)
		{
			error_log("[GetAllianceMessageForTime fail][aid=%u,type=%d,toward=%d,startTime=%u,ret=%d]",
						uid,type,toward,startTime,ret);
		}
		return ret;
	}
	return R_ERR_REFUSE;
}

