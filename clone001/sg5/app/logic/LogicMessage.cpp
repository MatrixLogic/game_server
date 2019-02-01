#include "LogicMessage.h"

#define MSG_LIVE_TIME (14*24*3600)
#define CHAT_LIVE_TIME (7*24*3600)
#define GET_CHAT_MAX 50
#define REQUEST_LIVE_TIME (24*3600*30)
#define MAX_ALLIANCE_REQUEST_MSG 100

bool compareUserMsg(const DataUserMessage &umsg1, const DataUserMessage &umsg2)
{
	return umsg1.time > umsg2.time;
}

int CLogicMessage::GetMessageId(uint64_t &msgid)
{
	CLogicIdCtrl logicIdCtrl;
	int ret = logicIdCtrl.GetNextId(KEY_MESSAGE_ID_CTRL, msgid);
	if(ret != 0)
	{
		error_log("[GetNextId fail][ret=%d]",ret);
		DB_ERROR_RETURN_MSG("get_message_id_fail");
	}
	return 0;
}

int CLogicMessage::SetMessageContent(uint64_t messageId, const string &content)
{
	int ret;
	CDataMessage dbMessage;
	ret = dbMessage.SetMessageContent(messageId, content);
	if(ret != 0)
	{
		error_log("[SetMessageContent fail][ret=%d,messageId=%llu,content=%s]", ret, messageId, content.c_str());
		DB_ERROR_RETURN_MSG("set_message_fail");
	}
	return 0;
}

int CLogicMessage::AddMessage(uint64_t &msgid, const DataUserBasic &user, UserMsgStatus status,
		MessageType type, const string &content, MessageStatus mstatus)
{
	int ret = 0;
	if ( (ret = GetMessageId(msgid)) != 0)
		return ret;

	DataUserMessage userMsg;
	DataMessage msg;

	userMsg.message_id = msgid;
	userMsg.opposite_uid = 0;
	userMsg.status = status;
	userMsg.time = Time::GetGlobalTime();
	userMsg.toward = TOWARD_RECV;
	userMsg.type = type;
	userMsg.uid = user.uid;

	msg.content = content;
	msg.from_platform = PT_UNKNOW;
	msg.from_uid = 0;
	msg.message_id = msgid;
	msg.reply_id = 0;
	msg.status = mstatus;
	msg.time = Time::GetGlobalTime();
	msg.to_photo = user.figure_url;
	msg.to_platform = user.platform;
	msg.to_uid = user.uid;
	msg.to_user_name = user.name;
	msg.type = type;

	CDataMessage msgDB;
	ret = msgDB.AddMessage(msg);
	if (ret != 0)
	{
		error_log("[AddMessage fail][ret=%d,uid=%u,msgid=%ul]",ret,user.uid,msgid);
		DB_ERROR_RETURN_MSG("add_msg_fail");
	}

	CLogicUserMessage userMsgDB;
	ret = userMsgDB.AddUserMessage(userMsg);
	if (ret != 0)
	{
		error_log("[AddUserMessage fail][ret=%d,uid=%u,msgid=%ul]",ret,user.uid,msgid);
		DB_ERROR_RETURN_MSG("add_msg_fail");
	}
	return 0;
}

int CLogicMessage::AddMessage(const DataUserBasic &user, UserMsgStatus status,
		MessageType type, const string &content, MessageStatus mstatus)
{
	uint64_t msgid;
	return AddMessage(msgid, user, status, type, content, mstatus);
}

int CLogicMessage::AddMessage(uint64_t &msgid, const DataUserBasic &user, UserMsgStatus status,
		const DataUserBasic &toUser, UserMsgStatus toStatus,
		MessageType type, const string &content,
		MessageStatus mstatus, uint64_t reply_id)
{
	int ret = 0;
	if ( (ret = GetMessageId(msgid)) != 0)
		return ret;

	DataMessage msg;
	msg.content = content;
	msg.from_photo = user.figure_url;
	msg.from_platform = user.platform;
	msg.from_uid = user.uid;
	msg.from_user_name = user.name;
	msg.message_id = msgid;
	msg.reply_id = reply_id;
	msg.status = mstatus;
	msg.time = Time::GetGlobalTime();
	msg.to_photo = toUser.figure_url;
	msg.to_platform = toUser.platform;
	msg.to_uid = toUser.uid;
	msg.to_user_name = toUser.name;
	msg.type = type;

	CDataMessage msgDB;
	ret = msgDB.AddMessage(msg);
	if (ret != 0)
	{
		error_log("[AddMessage fail][ret=%d,uid=%u,msgid=%ul,toUid=%u]",ret,user.uid,msgid,toUser.uid);
		DB_ERROR_RETURN_MSG("add_msg_fail");
	}

	CLogicUserMessage userMsgDB;
	if(IsLegalUid(msg.from_uid))
	{
		DataUserMessage userMsg1;
		userMsg1.message_id = msgid;
		userMsg1.opposite_uid = toUser.uid;
		userMsg1.status = status;
		userMsg1.time = Time::GetGlobalTime();
		userMsg1.toward = TOWARD_SEND;
		userMsg1.type = type;
		userMsg1.uid = user.uid;
		ret = userMsgDB.AddUserMessage(userMsg1);
		if (ret != 0)
		{
			error_log("[AddUserMessage1 fail][ret=%d,uid=%u,msgid=%ul,toUid=%d]",ret,user.uid,msgid,toUser.uid);
			DB_ERROR_RETURN_MSG("add_msg_fail");
		}
	}

	if(IsLegalUid(msg.to_uid))
	{
		DataUserMessage userMsg2;
		userMsg2.message_id = msgid;
		userMsg2.opposite_uid = user.uid;
		userMsg2.status = toStatus;
		userMsg2.time = Time::GetGlobalTime();
		userMsg2.toward = TOWARD_RECV;
		userMsg2.type = type;
		userMsg2.uid = toUser.uid;
		ret = userMsgDB.AddUserMessage(userMsg2);
		if (ret != 0)
		{
			error_log("[AddUserMessage2 fail][ret=%d,uid=%u,msgid=%ul,toUid=%d]",ret,user.uid,msgid,toUser.uid);
			DB_ERROR_RETURN_MSG("add_msg_fail");
		}
	}
	return 0;
}

int CLogicMessage::AddMessage(const DataUserBasic &user, UserMsgStatus status,
		const DataUserBasic &toUser, UserMsgStatus toStatus,
		MessageType type, const string &content,
		MessageStatus mstatus, uint64_t reply_id)
{
	uint64_t msgid;
	return AddMessage(msgid, user, status, toUser, toStatus, type, content, mstatus, reply_id);
}

int CLogicMessage::AddSysMessage(const DataUserBasic &user, const string &content)
{
	return AddMessage(user, UMS_UN_READ, MSG_SYS, content, MS_UN_PROCESS);
}

int CLogicMessage::AddSysMessage(unsigned uid, const string &content)
{
	DataUserBasic user;
	user.uid = uid;
	user.platform = OpenPlatform::GetType();
	return AddMessage(user, UMS_UN_READ, MSG_SYS, content, MS_UN_PROCESS);
}

int CLogicMessage::GetNewSysMessage(unsigned uid, vector<DataMessage> &msgVec)
{
	unsigned startTime = Time::GetGlobalTime() - MSG_LIVE_TIME;
	CLogicUserMessage userMsgDB;
	vector<DataUserMessage> userMsgVec;
	int ret = userMsgDB.GetUserMessage(uid, MSG_SYS, UMS_UN_READ, startTime, userMsgVec);
	if (ret != 0)
	{
		error_log("[GetUserMessage fail][ret=%d,uid=%u]",ret,uid);
		DB_ERROR_RETURN_MSG("get_user_msg_fail");
	}
	CDataMessage msgDB;
	for (unsigned i = 0; i < userMsgVec.size(); i++)
	{
		DataMessage msg;
		ret = msgDB.GetMessage(userMsgVec[i].message_id, msg);
		if (ret != 0)
		{
			error_log("[GetMessage fail][ret=%d,uid=%u,msgid=%ul]",ret,uid,userMsgVec[i].message_id);
		}
		else
		{
			msgVec.push_back(msg);
			userMsgDB.SetUserMessageStatus(uid, userMsgVec[i].message_id, UMS_READED);
		}
	}
	return 0;
}

int CLogicMessage::Chat(unsigned uid, unsigned toUid, const string &message)
{
	int ret = 0;
	DataUserBasic user;
	DataUserBasic toUser;
	CLogicUserBasic logicUser;
	user.uid = uid;
	user.platform = 0;
	toUser.uid = toUid;
	toUser.platform = 0;
	if (IsValidUid(uid))
	{
		ret = logicUser.GetUserBasicLimitSmart(uid, OpenPlatform::GetType(), user);
		if (ret != 0)
			return ret;
	}
	if (IsValidUid(toUid))
	{
		ret = logicUser.GetUserBasicLimitSmart(toUid, OpenPlatform::GetType(), toUser);
		if (ret != 0)
			return ret;
	}

	ret = AddMessage(user, UMS_READED, toUser, UMS_UN_READ, MSG_CHAT, message, MS_UN_PROCESS);
	if (ret != 0)
		return ret;

	return 0;
}

int CLogicMessage::GetChat(unsigned uid, int toward, vector<DataMessage> &msgVec)
{
	if (toward < 0 || toward > TOWARD_TYPE_ALL)
	{
		PARAM_ERROR_RETURN();
	}
	int ret = 0;
	unsigned startTime = Time::GetGlobalTime() - CHAT_LIVE_TIME;
	CLogicUserMessage userMsgDB;
	vector<DataUserMessage> userMsgVec;
	if (toward == TOWARD_TYPE_ALL)
	{
		ret = userMsgDB.GetUserMessageForTime(uid, MSG_CHAT, startTime, userMsgVec);
	}
	else
	{
		ret = userMsgDB.GetUserMessageForTime(uid, MSG_CHAT, toward, startTime, userMsgVec);
	}
	if (ret != 0)
	{
		error_log("[GetUserMessage fail][ret=%d,uid=%u,toward=%d]",ret,uid,toward);
		DB_ERROR_RETURN_MSG("get_user_msg_fail");
	}
	vector<DataUserMessage>::iterator itr = userMsgVec.begin();
	while(itr != userMsgVec.end())
	{
		if(itr->status == UMS_REMOVE)
		{
			itr = userMsgVec.erase(itr);
		}
		else
		{
			itr++;
		}
	}
	if (userMsgVec.size() > GET_CHAT_MAX)
	{
		sort(userMsgVec.begin(), userMsgVec.end(), compareUserMsg);
	}
	CDataMessage msgDB;
	unsigned vsize = userMsgVec.size() > GET_CHAT_MAX ? GET_CHAT_MAX : userMsgVec.size();
	for (unsigned i = 0; i < vsize; i++)
	{
		if (!IsValidUid(userMsgVec[i].opposite_uid))
			continue;
		DataMessage msg;
		ret = msgDB.GetMessage(userMsgVec[i].message_id, msg);
		if (ret != 0)
		{
			error_log("[GetMessage fail][ret=%d,uid=%u,msgid=%ul]",ret,uid,userMsgVec[i].message_id);
		}
		else
		{
			msgVec.push_back(msg);
			if (userMsgVec[i].status == UMS_UN_READ)
			{
				userMsgDB.SetUserMessageStatus(uid, userMsgVec[i].message_id, UMS_READED);
			}
		}
	}
	return 0;
}

int CLogicMessage::RemoveChat(unsigned uid, uint64_t messageId)
{
	int ret;
	CLogicUserMessage dbUserMessage;
	ret = dbUserMessage.SetUserMessageStatus(uid, messageId, UMS_REMOVE);
	if(ret != 0)
	{
		error_log("[SetUserMessageStatus fail][ret=%d,uid=%u,messageid=%llu]", ret, uid, messageId);
		DB_ERROR_RETURN_MSG("set_message_fail");
	}
	return 0;
}

int CLogicMessage::AddRequest(uint64_t &messageId, unsigned uid, unsigned toUid, const Json::Value &content)
{
	int ret = 0;
	if(uid == toUid)
	{
		error_log("[self_request][uid=%u,content=%s]", uid, Json::ToString(content).c_str());
		LOGIC_ERROR_RETURN_MSG("self_request");
	}
	DataUserBasic user;
	DataUserBasic toUser;
	CLogicUserBasic logicUser;
	if (IsValidUid(uid))
	{
		logicUser.GetUserBasicLimitSmart(uid, OpenPlatform::GetType(), user);
		if (ret != 0)
			return ret;
	}
	else
	{
		user.uid = uid;
		user.platform = OpenPlatform::GetType();
	}
	if (IsValidUid(toUid))
	{
		ret = logicUser.GetUserBasicLimitSmart(toUid, OpenPlatform::GetType(), toUser);
		if (ret != 0)
			return ret;
	}
	else
	{
		toUser.uid = toUid;
		toUser.platform = OpenPlatform::GetType();
	}

	string type;
	if (Json::GetString(content, "type", type) && type == "alliance_join")
	{
		unsigned int aid;
		if (Json::GetUInt(content, "aid", aid))
		{
			ret = CheckAllianceJoinMsg(aid);
			if (0 != ret)
			{
				return ret;
			}
		}
	}

	Json::FastWriter writer;
	ret = AddMessage(messageId, user, UMS_READED, toUser, UMS_UN_READ, MSG_REQUEST, writer.write(content), MS_UN_PROCESS);
	if (ret != 0)
		return ret;

	return 0;
}

int CLogicMessage::AddRequest(unsigned uid, unsigned toUid, const Json::Value &content)
{
	uint64_t messageId;
	return AddRequest(messageId, uid, toUid, content);
}


int CLogicMessage::ReplyRequest(unsigned uid, unsigned toUid, uint64_t message_id, MessageStatus mstatus, const Json::Value &content)
{
	int ret;
	if(uid == toUid)
	{
		error_log("[self_reply][uid=%u,messagid=%llu]", uid, message_id);
		LOGIC_ERROR_RETURN_MSG("self_reply");
	}
	CDataMessage msgDB;
	DataMessage oldMsg;
	ret = msgDB.GetMessage(message_id, oldMsg);
	if (ret != 0)
	{
		error_log("[GetMessage fail][uid=%u,toUid=%u,msgid=%llu,ret=%d]",uid,toUid,message_id,ret);
		DB_ERROR_RETURN_MSG("get_msg_id_fail");
	}
	DataUserBasic user;
	DataUserBasic toUser;
	CLogicUserBasic logicUser;
	ret = logicUser.GetUserBasicLimitSmart(uid, OpenPlatform::GetType(), user);
	if (ret != 0)
		return ret;
	if(toUid == 0)
	{
		toUid = oldMsg.from_uid;
		if(uid == toUid)
		{
			error_log("[self_reply][uid=%u,messagid=%llu]", uid, message_id);
			LOGIC_ERROR_RETURN_MSG("self_reply");
		}
	}
	ret = logicUser.GetUserBasicLimitSmart(toUid, OpenPlatform::GetType(), toUser);
	if (ret != 0)
		return ret;
	Json::FastWriter writer;
	ret = AddMessage(user, UMS_READED, toUser, UMS_UN_READ, MSG_REQUEST, writer.write(content), mstatus, message_id);
	if (ret != 0)
		return ret;
	msgDB.SetMessageStatus(message_id, mstatus);

	Json::Reader reader;
	Json::Value oldJson;
	reader.parse(oldMsg.content, oldJson);
	CLogicAlliance logicAllian;
	string type;
	Json::GetString(oldJson, "type", type);
	ret = logicAllian.ProcessRequest(oldMsg, toUid, uid, type, mstatus, oldJson);
	if(ret == 0)
	{
		CLogicUserInteract logicInteract;
		ret = logicInteract.ProcessRequest(oldMsg, toUid, uid, type, mstatus, oldJson);
	}
	if (ret == 0)
	{
		if (type == "alliance_join")
		{
			Json::Value replyJson;
			replyJson["id"] = (mstatus == MS_ACCEPT?"acceptReq" : "rejectReq");
			replyJson["fid"] = uid;
			replyJson["fname"] = user.name;
			string content2 = writer.write(replyJson);
			AddSysMessage(toUid, content2);
		}
		else if (type == "truce")
		{
			Json::Value replyJson;
			replyJson["id"] = (mstatus == MS_ACCEPT?"acceptTruce" : "rejectTruce");
			replyJson["fid"] = uid;
			replyJson["fname"] = user.name;
			string content2 = writer.write(replyJson);
			AddSysMessage(toUid, content2);
		}
	}

	return ret;
}

int CLogicMessage::CancelRequest(unsigned uid, const string &mtype, uint64_t message_id)
{
	int ret  = 0;
	CDataMessage msgDB;
	if (message_id == 0)
	{
		unsigned startTime = Time::GetGlobalTime() - REQUEST_LIVE_TIME;
		CLogicUserMessage userMsgDB;
		vector<DataUserMessage> userMsgVec;
		ret = userMsgDB.GetUserMessageForTime(uid, MSG_REQUEST, TOWARD_SEND, startTime, userMsgVec);
		if (ret == R_ERR_NO_DATA)
			return 0;
		if (ret != 0)
		{
			error_log("[GetUserMessage fail][ret=%d,uid=%u,toward=%d]",ret,uid,TOWARD_SEND);
			DB_ERROR_RETURN_MSG("get_user_msg_fail");
		}
		Json::Reader reader;
		for (unsigned i = 0; i < userMsgVec.size(); i++)
		{
			DataMessage msg;
			ret = msgDB.GetMessage(userMsgVec[i].message_id, msg);
			if (ret != 0)
			{
				error_log("[GetMessage fail][ret=%d,uid=%u,msgid=%ul]",ret,uid,userMsgVec[i].message_id);
			}
			else
			{
				if (msg.status == MS_UN_PROCESS)
				{
					Json::Value json;
					reader.parse(msg.content, json);
					if (json.isMember("type")
							&& json["type"].isString()
							&& json["type"].asString() == mtype)
					{
						msgDB.SetMessageStatus(userMsgVec[i].message_id, MS_CANCEL);
					}
				}
			}
		}
	}
	else
	{
		ret = msgDB.SetMessageStatus(message_id, MS_CANCEL);
		if (ret != 0)
		{
			error_log("[SetMessageStatus fail][uid=%u,msgid=%llu,ret=%d]",uid,message_id,ret);
			DB_ERROR_RETURN_MSG("set_msg_status_fail");
		}
	}

	return 0;
}

int CLogicMessage::CancelRequest(unsigned uid, TowardType toward)
{
	int ret  = 0;
	CDataMessage msgDB;
	unsigned startTime = Time::GetGlobalTime() - REQUEST_LIVE_TIME;
	CLogicUserMessage userMsgDB;
	vector<DataUserMessage> userMsgVec;
	if (toward == TOWARD_TYPE_ALL)
	{
		ret = userMsgDB.GetUserMessageForTime(uid, MSG_REQUEST, startTime, userMsgVec);
	}
	else
	{
		ret = userMsgDB.GetUserMessageForTime(uid, MSG_REQUEST, toward, startTime, userMsgVec);
	}
	if (ret == R_ERR_NO_DATA)
		return 0;
	if (ret != 0)
	{
		error_log("[GetUserMessage fail][ret=%d,uid=%u,toward=%d]",ret,uid,toward);
		DB_ERROR_RETURN_MSG("get_user_msg_fail");
	}
	for (unsigned i = 0; i < userMsgVec.size(); i++)
	{
		DataMessage msg;
		ret = msgDB.GetMessage(userMsgVec[i].message_id, msg);
		if (ret != 0)
		{
			error_log("[GetMessage fail][ret=%d,uid=%u,msgid=%ul]",ret,uid,userMsgVec[i].message_id);
		}
		else
		{
			if (msg.status == MS_UN_PROCESS)
			{
				msgDB.SetMessageStatus(userMsgVec[i].message_id, MS_CANCEL);
			}
		}
	}

	return 0;
}

int CLogicMessage::GetRequest(unsigned uid, const set<string> &mtypes, int toward, int status, vector<DataMessage> &msgVec)
{
	if (toward < 0 || toward > TOWARD_TYPE_ALL
			|| status < MS_UN_PROCESS || status > MS_STATUS_ALL)
	{
		PARAM_ERROR_RETURN();
	}
	int ret  = 0;
	unsigned startTime = Time::GetGlobalTime() - REQUEST_LIVE_TIME;
	CLogicUserMessage userMsgDB;
	vector<DataUserMessage> userMsgVec;
	if (toward == TOWARD_TYPE_ALL)
	{
		ret = userMsgDB.GetUserMessageForTime(uid, MSG_REQUEST, startTime, userMsgVec);
	}
	else
	{
		ret = userMsgDB.GetUserMessageForTime(uid, MSG_REQUEST, toward, startTime, userMsgVec);
	}
	if (ret != 0)
	{
		error_log("[GetUserMessage fail][ret=%d,uid=%u,toward=%d]",ret,uid,toward);
		DB_ERROR_RETURN_MSG("get_user_msg_fail");
	}
	bool judgeType = !mtypes.empty();
	CDataMessage msgDB;
	Json::Reader reader;
	for (unsigned i = 0; i < userMsgVec.size(); i++)
	{
		DataMessage msg;
		ret = msgDB.GetMessage(userMsgVec[i].message_id, msg);
		if (ret != 0)
		{
			error_log("[GetMessage fail][ret=%d,uid=%u,msgid=%ul]",ret,uid,userMsgVec[i].message_id);
		}
		else
		{
			if (status == MS_STATUS_ALL || msg.status == status)
			{
				if(judgeType)
				{
					Json::Value messageData;
					Json::FromString(messageData, msg.content);
					string currType;
					if(Json::GetString(messageData, "type", currType))
					{
						if(mtypes.find(currType) != mtypes.end())
						{
							msgVec.push_back(msg);
						}
					}
				}
				else
				{
					msgVec.push_back(msg);
				}
			}
		}
	}

	return 0;
}

int CLogicMessage::GetRequest(unsigned uid, const string &mtype, int toward, int status, vector<DataMessage> &msgVec)
{
	set<string> mtypes;
	if(!mtype.empty())
	{
		mtypes.insert(mtype);
	}
	return GetRequest(uid, mtypes, toward, status, msgVec);
}

int CLogicMessage::GetRequestJson(unsigned uid, const set<string> &mtypes, int toward, int status, Json::Value &requests)
{
	int ret;
	vector<DataMessage> messages;
	ret = GetRequest(uid, mtypes, toward, status, messages);
	if(ret != 0)
	{
		return ret;
	}
	for(vector<DataMessage>::iterator itr = messages.begin(); itr != messages.end(); itr++)
	{
		ret = AutoRejectRequest(*itr);
		if (0 == ret)
		{
			continue;
		}
		Json::Value &request = requests[requests.size()];
		request["msgid"] = Convert::UInt64ToString(itr->message_id);
		request["from"] = itr->from_uid;
		request["to"] = itr->to_uid;
		request["from_pt"] = itr->from_platform;
		String::Utf8Regularize(itr->from_user_name);
		request["from_name"] = itr->from_user_name;
		request["from_photo"] = itr->from_photo;
		request["to_pt"] = itr->to_platform;
		String::Utf8Regularize(itr->to_user_name);
		request["to_name"] = itr->to_user_name;
		request["to_photo"] = itr->to_photo;
		request["time"] = itr->time;
		Json::FromString(request["msg"], itr->content);
		request["type"] = itr->type;
		request["status"] = itr->status;
	}
	return 0;
}

int CLogicMessage::AddPlatformRequest(uint64_t &messageId, unsigned fromUid, const OPUserInfo &toUser, const Json::Value &content)
{
	int ret;
	CLogicUserBasic logicUserBasic;
	DataUserBasic fromUser;
	if(IsValidUid(fromUid))
	{
		ret = logicUserBasic.GetUserBasicLimitSmart(fromUid, OpenPlatform::GetType(), fromUser);
		if (ret != 0)
		{
			return ret;
		}
	}
	DataUserBasic toUserBasic;
	toUserBasic.uid = MUT_NON_APP_USER;
	toUserBasic.name = toUser.Name;
	toUserBasic.figure_url = toUser.FigureUrl;
	toUserBasic.platform = OpenPlatform::GetType();
	ret = AddMessage(messageId, fromUser, UMS_READED, toUserBasic, UMS_UN_READ, MSG_REQUEST, Json::ToString(content), MS_UN_PROCESS);
	if (ret != 0)
	{
		return ret;
	}

	return 0;
}

int CLogicMessage::SetMessagesTarget(vector<uint64_t> messageIds, unsigned toUid)
{
	int ret;
	CLogicUserBasic logicUserBasic;
	DataUserBasic toUser;
	ret = logicUserBasic.GetUserBasicLimitSmart(toUid, OpenPlatform::GetType(), toUser);
	if(ret != 0)
	{
		return ret;
	}
	int count = 0;
	CDataMessage dbMessage;
	CLogicUserMessage dbUserMessage;
	DataMessage message;
	for(vector<uint64_t>::iterator itr = messageIds.begin(); itr != messageIds.end(); itr++)
	{
		ret = dbMessage.GetMessage(*itr, message);
		if(ret != 0)
		{
			error_log("[GetMessage fail][ret=%d,messageId=%llu]", ret, *itr);
		}
		else if(IsLegalUid(message.to_uid))
		{
			if(message.to_uid != toUid)
			{
				error_log("[conflict_to_user][messageId=%llu,to_uid=%u,new_to_uid=%u]", *itr, message.to_uid, toUid);
			}
		}
		else
		{
			message.to_uid = toUid;
			message.to_platform = toUser.platform;
			message.to_user_name = toUser.name;
			message.to_photo = toUser.figure_url;
			ret = dbMessage.SetMessage(message);
			if (ret != 0)
			{
				error_log("[SetMessage fail][ret=%d,msgid=%llu,toUid=%u]", ret, *itr, toUid);
			}
			else
			{
				DataUserMessage userMessageFrom;
				ret = dbUserMessage.GetUserMessage(message.from_uid, message.message_id, userMessageFrom);
				if(ret != 0)
				{
					error_log("[AddUserMessage fail][ret=%d,msgid=%llu,fromUid,toUid=%u]",
							ret, *itr, message.from_uid, toUid);
				}
				else
				{
					userMessageFrom.opposite_uid = toUid;
					ret = dbUserMessage.SetUserMessage(userMessageFrom);
					if(ret != 0)
					{
						error_log("[SetUserMessage fail][ret=%d,msgid=%llu,fromUid,toUid=%u]",
								ret, *itr, message.from_uid, toUid);
					}
				}

				DataUserMessage userMessageTo;
				userMessageTo.message_id = message.message_id;
				userMessageTo.uid = toUid;
				userMessageTo.opposite_uid = message.from_uid;
				userMessageTo.status = UMS_UN_READ;
				userMessageTo.type = message.type;
				userMessageTo.toward = TOWARD_RECV;
				userMessageTo.time = message.time;
				ret = dbUserMessage.AddUserMessage(userMessageTo);
				if(ret != 0)
				{
					error_log("[AddUserMessage fail][ret=%d,msgid=%llu,toUid=%u]", ret, *itr, toUid);
				}
				else
				{
					count++;
				}
			}
		}
	}

	return 0;
}


int CLogicMessage::CheckAllianceJoinMsg(unsigned aid)
{
	//Check alliance members
	CLogicAlliance alliancelogic;
	DataAlliance alliance;
	int ret = alliancelogic.GetAllianceLimit(aid, alliance);
	if(ret != 0)
	{
		return ret;
	}
	int limitCount = alliancelogic.GetMemberMaxCount(alliance.level);
	if(alliance.member_count >= limitCount)
	{
		ERROR_RETURN_MSG(R_ERR_DATA_LIMIT, "alliance_member_count_limit");
	}

	//Check message total
	CLogicUserMessage userMsgDB;
	unsigned startTime = Time::GetGlobalTime() - REQUEST_LIVE_TIME;
	vector<DataUserMessage> userMsgVec;
	ret = userMsgDB.GetUserMessageForTime(aid, MSG_REQUEST, startTime, userMsgVec);
	if (0 != ret)
	{
		error_log("GetUserMessageForTime faile uid=%u,ret=%d", aid,ret);
		return 0;
	}

	CDataMessage msgDB;
	Json::Reader reader;
	int messageTotal = 0;
	for (unsigned i = 0; i < userMsgVec.size(); i++)
	{
		DataMessage msg;
		ret = msgDB.GetMessage(userMsgVec[i].message_id, msg);
		if (ret != 0)
		{
			error_log("[GetMessage fail][ret=%d,uid=%u,msgid=%ul]",ret,aid,userMsgVec[i].message_id);
		}
		else
		{
			if (msg.status == 0)
			{
				Json::Value messageData;
				Json::FromString(messageData, msg.content);
				string currType;
				if(Json::GetString(messageData, "type", currType) && currType == "alliance_join")
				{
					messageTotal++;
				}
			}
		}
	}

	//alliance request message over MAX_ALLIANCE_REQUEST_MSG return can not add message.
	if (messageTotal >= MAX_ALLIANCE_REQUEST_MSG)
	{
		ERROR_RETURN_MSG(R_ERR_DATA_LIMIT, "alliance_member_count_limit");
	}
	return 0;
}

int CLogicMessage::AutoRejectRequest(DataMessage &message)
{
	if ( (Time::GetGlobalTime() - message.time) < (12 * 60 * 60)
		|| message.type != 2)
	{
		return R_ERROR;
	}
	int ret = 0;
	CLogicAlliance logicAlliance;
	DataAlliance alliance;
	ret = logicAlliance.GetAlliance(message.to_uid, alliance);
	if (0 != ret)
	{
		error_log("Get Alliance failed,aid=%u,ret=%d", message.to_uid, ret);
		return ret;
	}

	Json::Value messageData;
	Json::FromString(messageData, message.content);
	ret = ReplyRequest(alliance.leader_uid, message.from_uid, message.message_id, MS_REJECT, messageData);
	if (0 != ret)
	{
		error_log("ReplyRequest failed,leader=%u,targetId=%u,messageId=%u",alliance.leader_uid,message.from_uid,message.message_id);
		return ret;
	}

	return 0;
}

