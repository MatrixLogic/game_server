#include "LogicInc.h"
#include "LogicQQPay.h"

class CCgiMessage : public CCgiBase
{

public:

	CCgiMessage() : CCgiBase("message")
	{
		SetFeature(CF_PRINT_JSON);
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_GET_UID);
		SetFeature(CF_CHECK_SESSION);
		SetFeature(CF_CHECK_PLATFORM);
		SetFeature(CF_CHECK_HASH);
		SetFeature(CF_CHECK_VERSION);
		SetFeature(CF_APPEND_HASH);
		SetFeature(CF_PARSE_DATA);
		SetFeature(CF_CRYPT_DATA);
		SetFeature(CF_DECRYPT_DATA);
		//SetFeature(CF_COMPRESS_OUT);
	}

	CGI_ACTION_MAP_BEGIN
	CGI_SET_ACTION_MAP("send", SendMessage)
	CGI_SET_ACTION_MAP("get", GetMessage)
	CGI_SET_ACTION_MAP("remove", RemoveMessage)
	CGI_SET_ACTION_MAP("sendhelpreward", SendHelpReward)
	CGI_SET_ACTION_MAP("worldboss", GetWorldBoss)
	CGI_SET_ACTION_MAP("allianceboss", GetAllianceBoss)
	CGI_SET_ACTION_MAP("lastallianceboss", GetLastAllianceBoss)
	CGI_SET_ACTION_MAP("viewladder", ViewLadder)
	CGI_SET_ACTION_MAP("viewallserverladder", ViewAllServerLadder)
	CGI_SET_ACTION_MAP("refreshladder", RefreshLadder)
	CGI_SET_ACTION_MAP("refreshallserverladder", RefreshAllServerLadder)
	CGI_SET_ACTION_MAP("reportladder", ReportLadder)
	CGI_SET_ACTION_MAP("reportallserverladder", ReportAllServerLadder)
	CGI_SET_ACTION_MAP("gethelphist", GetHelpHist)
	CGI_SET_ACTION_MAP("dragonball", GetDragonball)
	CGI_SET_ACTION_MAP("setmarketquest", SetMarketTask)
	CGI_SET_ACTION_MAP("getmarketquest", GetMarketTask)
	CGI_SET_ACTION_MAP("viewallserverladdertop1", ViewAllServerLadderTop1)
	CGI_ACTION_MAP_END

	int SendMessage()
	{
		unsigned toUid = 0;
		if(!Json::IsArray(m_data, "targetid"))
		{
			if (!Json::GetUInt(m_data, "targetid", toUid))
				toUid = 0;
		}
		string type;
		if (!Json::GetString(m_data, "type", type))
		{
			return R_ERR_PARAM;
		}
		CLogicMessage logicMsg;
		int ret = 0;
		if (type == "message")
		{
			string message;
			if (toUid == 0 || !Json::GetString(m_data, "message", message))
				return R_ERR_PARAM;
			StringFilter::Replace(message);
			ret = logicMsg.Chat(m_uid, toUid, message);
			if (ret != 0)
				return ret;
		}
		else if (type == "request")
		{
			string way;
			if (!Json::GetString(m_data, "way", way))
				return R_ERR_PARAM;
			if (way == "add")
			{
				if (toUid == 0 || !Json::IsObject(m_data, "message"))
					return R_ERR_PARAM;
				ret = logicMsg.AddRequest(m_uid, toUid, m_data["message"]);
				if (ret != 0)
					return ret;
			}
			else if (way == "accept")
			{
				uint64_t msgid = 0;
				if (!Json::IsArray(m_data, "msgid"))
				{
					if (toUid == 0 || !Json::GetUInt64(m_data, "msgid", msgid))
						return R_ERR_PARAM;
					ret = logicMsg.ReplyRequest(m_uid, toUid, msgid, MS_ACCEPT, m_data["message"]);
					if (ret != 0)
						return ret;
				}
				else
				{
					if (!Json::IsArray(m_data, "targetid"))
					{
						return R_ERR_PARAM;
					}

					Json::Value messageId;
					Json::Value targetID;
					if (!Json::GetArray(m_data, "targetid", targetID)
						|| !Json::GetArray(m_data, "msgid", messageId))
					{
						return R_ERR_PARAM;
					}
					if (targetID.size() != messageId.size())
					{
						return R_ERR_PARAM;
					}
					for (unsigned int i=0; i<messageId.size(); i++)
					{
						if (!Json::GetUInt64(messageId, i, msgid))
						{
							continue;
						}
						if (!Json::GetUInt(targetID, i, toUid))
						{
							continue;
						}
						ret = logicMsg.ReplyRequest(m_uid, toUid, msgid, MS_ACCEPT, m_data["message"]);
						if (ret != 0)
							return ret;
						}
				}
			}
			else if (way == "reject")
			{
				uint64_t msgid = 0;
				if (!Json::IsArray(m_data, "msgid"))
				{
					if (toUid == 0 || !Json::GetUInt64(m_data, "msgid", msgid))
						return R_ERR_PARAM;
					ret = logicMsg.ReplyRequest(m_uid, toUid, msgid, MS_REJECT, m_data["message"]);
					if (ret != 0)
						return ret;
				}
				else
				{
					if (!Json::IsArray(m_data, "targetid"))
					{
						return R_ERR_PARAM;
					}

					Json::Value messageId;
					Json::Value targetID;
					if (!Json::GetArray(m_data, "targetid", targetID)
						|| !Json::GetArray(m_data, "msgid", messageId))
					{
						return R_ERR_PARAM;
					}
					if (targetID.size() != messageId.size())
					{
						return R_ERR_PARAM;
					}
					for (unsigned int i=0; i<messageId.size(); i++)
					{
						if (!Json::GetUInt64(messageId, i, msgid))
						{
							continue;
						}
						if (!Json::GetUInt(targetID, i, toUid))
						{
							continue;
						}
						ret = logicMsg.ReplyRequest(m_uid, toUid, msgid, MS_REJECT, m_data["message"]);
						if (ret != 0)
							return ret;
					}
				}
			}
			else if (way == "cancel")
			{
				uint64_t msgid = 0;
				if (!Json::GetUInt64(m_data, "msgid", msgid))
					msgid = 0;
				string mtype;
				if (!Json::GetString(m_data, "mtype", mtype))
					mtype = "alliance_join";
				ret = logicMsg.CancelRequest(m_uid, mtype, msgid);
				if (ret != 0)
					return ret;
			}
			else
			{
				return R_ERR_PARAM;
			}
		}
		else
		{
			return R_ERR_PARAM;
		}

		CGI_SEND_LOG("action=send&uid=%u&toUid=%u&type=%s", m_uid, toUid, type.c_str());
		return R_SUCCESS;
	}

	int GetMessage()
	{
		string type;
		if (!Json::GetString(m_data, "type", type))
		{
			return R_ERR_PARAM;
		}
		unsigned quid = 0;
		if (!Json::GetUInt(m_data, "quid", quid) || quid == 0)
		{
			quid = m_uid;
		}

//		if (!Json::GetUInt(m_data, "quid", quid) || quid == 0)
//		{
//			quid = m_uid;
//		}

		Json::Reader reader;
		CLogicMessage logicMsg;
		vector<DataMessage> msgVec;
		int ret = 0;
		if (type == "message")
		{
			int toward = TOWARD_RECV;
			if (!Json::GetInt(m_data, "toward", toward))
				toward = TOWARD_RECV;
			ret = logicMsg.GetChat(quid, toward, msgVec);
			if (ret != 0)
				return ret;
			Json::Value json;
			json.resize(msgVec.size());
			for (unsigned i = 0; i < msgVec.size(); i++)
			{
				json[i]["msgid"] = Convert::UInt64ToString(msgVec[i].message_id);
				json[i]["from"] = msgVec[i].from_uid;
				json[i]["to"] = msgVec[i].to_uid;
				json[i]["from_pt"] = msgVec[i].from_platform;
				json[i]["from_name"] = msgVec[i].from_user_name;
				json[i]["to_pt"] = msgVec[i].to_platform;
				json[i]["to_name"] = msgVec[i].to_user_name;
				json[i]["time"] = msgVec[i].time;
				json[i]["msg"] = msgVec[i].content;
			}
			m_jsonResult["res"] = json;
		}
		else if (type == "request")
		{
			string mtype;
			int toward;
			int status;
			Json::GetString(m_data, "mtype", mtype);
			if(!Json::GetInt(m_data, "toward", toward) ||
					!Json::GetInt(m_data, "status", status))
			{
				PARAM_ERROR_RETURN();
			}
			if (toward < 0 || toward > TOWARD_TYPE_ALL
					|| status < MS_UN_PROCESS || status > MS_STATUS_ALL)
			{
				PARAM_ERROR_RETURN();
			}

			vector<string> vecMtypes;
			if(!mtype.empty())
			{
				CBasic::StringSplitTrim(mtype, "|", vecMtypes);
			}
			set<string> mtypes(vecMtypes.begin(), vecMtypes.end());

			ret = logicMsg.GetRequestJson(quid, mtypes, toward, status, m_jsonResult["res"]);
			if(ret != 0)
			{
				return ret;
			}
		}
		else
		{
			return R_ERR_PARAM;
		}

		CGI_SEND_LOG("action=get&uid=%u&quid=%u&type=%s", m_uid, quid, type.c_str());
		return R_SUCCESS;
	}

	int RemoveMessage()
	{
		int ret;
		string type;
		uint64_t messageId;
		if (!Json::GetString(m_data, "type", type) ||
			!Json::GetUInt64(m_data, "messageid", messageId))
		{
			return R_ERR_PARAM;
		}
		CLogicMessage logicMessage;
		if(type == "message")
		{
			ret = logicMessage.RemoveChat(m_uid, messageId);
			if(ret != 0)
			{
				return ret;
			}
		}
		else
		{
			return R_ERR_PARAM;
		}
		CGI_SEND_LOG("action=remove&uid=%u&messageid=%llu&type=%s", m_uid, messageId, type.c_str());
		return R_SUCCESS;
	}

	int SendHelpReward()
	{
		int ret;
		string itemid;
		int count;
		if(!Json::GetString(m_data, "itemid", itemid) ||
			!Json::GetInt(m_data, "count", count))
		{
			PARAM_ERROR_RETURN();
		}
		CLogicUserInteract logicInteract;
		uint64_t messageId;
		ret = logicInteract.SendHelpReward(m_uid, itemid, count, messageId);
		if(ret != 0)
		{
			return ret;
		}
		m_jsonResult["messageid"] = Convert::UInt64ToString(messageId);
		CGI_SEND_LOG("action=sendhelpreward&uid=%u&itemid=%s&count=%d&messageid=%llu",
				m_uid, itemid.c_str(), count, messageId);
		return R_SUCCESS;
	}

	int GetWorldBoss()
	{
		CLogicWorldBoss logicWorldBoss;
		int ret = logicWorldBoss.GetWorldBossInfo(m_uid, m_jsonResult["worldboss"]);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=worldboss&uid=%u",m_uid);
		return R_SUCCESS;
	}

	int GetAllianceBoss()
	{
		CLogicAllianceBoss logicAllianceBoss;
		int ret = logicAllianceBoss.GetAllianceBossInfo(m_uid, m_jsonResult["allianceboss"]);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=allianceboss&uid=%u",m_uid);
		return R_SUCCESS;
	}
	int GetLastAllianceBoss()        //上次屠魔排名以及最后一击
	{
		CLogicAllianceBoss logicAllianceBoss;
		int ret = logicAllianceBoss.GetLastAllianceBossInfo(m_uid, m_jsonResult["lastallianceboss"]);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=lastallianceboss&uid=%u",m_uid);
		return R_SUCCESS;
	}

	int ViewLadder()
	{
		CLogicLadder logicLadder;
		int ret = logicLadder.ViewLadder(m_uid, m_jsonResult);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=viewladder&uid=%u",m_uid);
		return R_SUCCESS;
	}

	int ViewAllServerLadder()
	{
		string uid;
		String::Format(uid, "%u", m_uid);
		string url = "action=viewladder";
		url.append("&uid=").append(uid);

		string level;
		if (!Json::GetString(m_data, "level", level))
		{
			PARAM_ERROR_RETURN_MSG("level");
		}
		url.append("&level=").append(level);

		CLogicBaseMatch logicBaseMatch;
		int ret = logicBaseMatch.RequestBaseMatch(url,m_jsonResult);
		if (0 != ret)
		{
			return ret;
		}

		CGI_SEND_LOG("action=viewallserverladder&uid=%u",m_uid);
		return R_SUCCESS;

	}

	int RefreshLadder()
	{
		CLogicLadder logicLadder;
		int ret = logicLadder.Refresh(m_uid, m_jsonResult);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=refreshladder&uid=%u",m_uid);
		return R_SUCCESS;
	}

	int RefreshAllServerLadder()
	{
		string uid;
		String::Format(uid, "%u", m_uid);
		string url = "action=refreshladder";
		url.append("&uid=").append(uid);

		string level;
		if (!Json::GetString(m_data, "level", level))
		{
			PARAM_ERROR_RETURN_MSG("level");
		}
		url.append("&level=").append(level);

		CLogicBaseMatch logicBaseMatch;
		int ret = logicBaseMatch.RequestBaseMatch(url,m_jsonResult);
		if (0 != ret)
		{
			return ret;
		}

		CGI_SEND_LOG("action=refreshallserverladder&uid=%u",m_uid);
		return R_SUCCESS;
	}

	int GetDragonball()
	{
		CLogicDragonball logicBall;
		int ret = logicBall.GetDragonballs(m_jsonResult["dragonballs"]);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=dragonball&uid=%u",m_uid);
		return R_SUCCESS;
	}

	int ReportLadder()
	{
		CLogicLadder logicLadder;
		int ret = logicLadder.ReportResult(m_uid, m_data["data"]);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=reportladder&uid=%u",m_uid);
		return R_SUCCESS;
	}

	int ReportAllServerLadder()
	{
		string uid;
		String::Format(uid, "%u", m_uid);
		string url = "action=reportladder";
		url.append("&uid=").append(uid);

		if (!m_data.isMember("data"))
		{
			PARAM_ERROR_RETURN_MSG("data");
		}
		string data = Json::ToString(m_data["data"]);
		url.append("&data=").append(Crypt::UrlEncode(data));

		string level;
		if (!Json::GetString(m_data, "level", level))
		{
			PARAM_ERROR_RETURN_MSG("level");
		}
		url.append("&level=").append(level);

		CLogicBaseMatch logicBaseMatch;
		Json::Value result;
		int ret = logicBaseMatch.RequestBaseMatch(url,result);
		if (0 != ret)
		{
			//error_log("[RequestBaseMatch fail][url=%s]",url.c_str());
			return ret;
		}

		CGI_SEND_LOG("action=reportallserverladder&uid=%u",m_uid);
		return R_SUCCESS;
	}

	int GetHelpHist()
	{
		CLogicHelpHist logiclasthelp;
		int ret = logiclasthelp.GetHelpHist(m_uid, m_jsonResult["helplist"]);
		if (0 != ret)
		{
			return ret;
		}
		return R_SUCCESS;
	}

	int SetMarketTask()
	{
		int ret = 0;
		string openid;
		if(!Json::GetString(m_data, "openid", openid))
		{
			PARAM_ERROR_RETURN();
		}
		if (!Json::IsArray(m_data, "quest"))
		{
			PARAM_ERROR_RETURN();
		}
		Json::Value quest;
		if (!Json::GetArray(m_data, "quest", quest))
		{
			PARAM_ERROR_RETURN();
		}

		CLogicMarketTask logicMarketTask;
		int flag = 0;
		string contractid;
		string sttaskid;
		for (int i=0; i<quest.size(); ++i)
		{
			if (!Json::GetString(quest[i], "marketquestid", contractid))
			{
				error_log("get marketquestid fail,i=%d", i);
				continue;
			}
			if (!Json::GetString(quest[i], "param", sttaskid))
			{
				error_log("get taskid fail,i=%d", i);
				continue;
			}
			if (!Json::GetInt(quest[i], "finish", flag))
			{
				error_log("get finish fail,i=%d", i);
				continue;
			}
			ret = logicMarketTask.SetMarketTask(openid, contractid, sttaskid, flag);
			if (0 != ret)
			{
				error_log("SetMarketTask fail,ret=%d", ret);
				continue;
			}
			if (1 == flag)
			{
				CLogicQQPay logicQQPay;
				string pf = "qzone";
				string pfkey;
				if(!Json::GetString(m_data, "pfkey", pfkey))
				{
					PARAM_ERROR_RETURN();
				}
				string openkey;
				if(!Json::GetString(m_data, "openkey", openkey))
				{
					PARAM_ERROR_RETURN();
				}
				string zoneid;
				string appid;
				if(!Json::GetString(m_data, "appid", appid))
				{
					PARAM_ERROR_RETURN();
				}

				ret = logicQQPay.InformTaskCompleted(pf, pfkey, openid, openkey, zoneid, appid, contractid);
				if(0 != ret)
				{
					return ret;
				}
			}
		}
		return 0;
	}

	int GetMarketTask()
	{
		int ret = 0;
		string openid;
		if(!Json::GetString(m_data, "openid", openid))
		{
			PARAM_ERROR_RETURN();
		}

		CLogicMarketTask logicMarketTask;
		vector<DataMarketTask> data;
		ret = logicMarketTask.GetMarketTask(openid, data);
		if (0 != ret)
		{
			return ret;
		}
		Json::Value questRet = Json::Value(Json::arrayValue);
		vector<DataMarketTask>::iterator itr = data.begin();

		for (; itr!=data.end(); ++itr)
		{
			Json::Value &temp = questRet[questRet.size()];
			temp["marketquestid"] = itr->contractid;
			temp["finish"] = itr->flag;
			temp["param"] = itr->taskid;
		}
		m_jsonResult["quest"] = questRet;
		return 0;
	}

	int ViewAllServerLadderTop1()
	{
		string url = "action=viewtop1";
		CLogicBaseMatch logicBaseMatch;
		int ret = logicBaseMatch.RequestBaseMatch(url, m_jsonResult);
		if (0 != ret)
		{
			return ret;
		}
		return 0;
	}
};

CGI_MAIN(CCgiMessage)
