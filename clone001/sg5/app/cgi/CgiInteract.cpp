/*
 * CgiInteract.cpp
 *
 *  Created on: 2011-12-20
 *      Author: dada
 */

#include "LogicInc.h"
#include "PengyouPlatform.h"

class CCgiInteract : public CCgiBase
{

public:

	CCgiInteract() : CCgiBase("interact")
	{
		SetFeature(CF_PRINT_JSON);
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_CHECK_PLATFORM);
		SetFeature(CF_GET_UID);
		SetFeature(CF_CHECK_SESSION);
	}

	CGI_SIMPLE_ACTION_MAP_BEGIN
	CGI_SET_ACTION_MAP("GetRequestFilter", GetRequestFilter)
	CGI_SET_ACTION_MAP("SendRequest", SendRequest)
	CGI_SET_ACTION_MAP("GetRequest", GetRequest)
	CGI_SET_ACTION_MAP("ProcessRequest", ProcessRequest)
	CGI_SET_ACTION_MAP("RequestItem", RequestItem)
	CGI_ACTION_MAP_END

	int GetRequestFilter()
	{
		int ret;
		string type = CCGIIn::GetCGIStr("type");
		if(type.empty())
		{
			PARAM_ERROR_RETURN();
		}
		CLogicUserInteract logicUserInteract;
		ret = logicUserInteract.GetRequestFilter(m_uid, type, m_jsonResult["filter"]);
		if(ret != 0)
		{
			return 0;
		}
		string to = CCGIIn::GetCGIStr("to");
		if(!to.empty())
		{
			vector<string> toUsers;
			CBasic::StringSplitTrim(to, ",", toUsers);
			UidList toUids;
			for(vector<string>::iterator itr = toUsers.begin(); itr != toUsers.end(); itr++)
			{
				unsigned toUid;
				if(Convert::StringToUInt(toUid, *itr))
				{
					toUids.push_back(toUid);
				}
			}
			if(!toUids.empty())
			{
				ret = logicUserInteract.FilterRequestUsers(toUids, m_jsonResult["filter"], m_jsonResult["to"]);
			}
		}
		CGI_SEND_LOG("action=GetRequestFilter&uid=%u&type=%s&count=%u", m_uid, type.c_str(), m_jsonResult["filter"].size());
		return 0;
	}

	int SendRequest()
	{
		int ret;
		string type = CCGIIn::GetCGIStr("type");
		if(type.empty())
		{
			PARAM_ERROR_RETURN();
		}
		string data = CCGIIn::GetCGIStr("data");
		if(data.empty())
		{
			PARAM_ERROR_RETURN();
		}
		map<string, string> userRequests;	// OpenId -> RequestId
		string request = CCGIIn::GetCGIStr("request");
		if(request.empty())
		{
			PARAM_ERROR_RETURN();
		}
		vector<string> requests;
		CBasic::StringSplitTrim(request, "|", requests);
		for(vector<string>::iterator itr = requests.begin(); itr != requests.end(); itr++)
		{
			vector<string> requestItems;
			CBasic::StringSplit(*itr, ",", requestItems);
			if(requestItems.size() == 2)
			{
				userRequests[Crypt::UrlDecode(requestItems[0])] = Crypt::UrlDecode(requestItems[1]);
			}
			else
			{
				error_log("[request_item_error][uid=%u,request=%s]", m_uid, itr->c_str());
			}
		}
		if(userRequests.size() == 0)
		{
			error_log("[requests_data_error][uid=%u,requests=%s]", m_uid, request.c_str());
			PARAM_ERROR_RETURN();
		}
		CLogicUserInteract logicUserInteract;
		ret = logicUserInteract.SendRequest(m_uid, type, data, userRequests);
		if(ret != 0)
		{
			return ret;
		}
		CGI_SEND_LOG("action=SendRequest&uid=%u&type=%s&data=%s&count=%u",
				m_uid, type.c_str(), data.c_str(), userRequests.size());
		return 0;
	}

	int GetRequest()
	{
		string mtype = CCGIIn::GetCGIStr("mtype");
		int toward = CCGIIn::GetCGIInt("toward");
		int status = CCGIIn::GetCGIInt("status");
		if(toward == CCGIIn::CGI_INT_ERR ||
				status == CCGIIn::CGI_INT_ERR)
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

		CLogicMessage logicMessage;
		int ret = logicMessage.GetRequestJson(m_uid, mtypes, toward, status, m_jsonResult["requests"]);
		if(ret != 0)
		{
			return ret;
		}
		CGI_SEND_LOG("action=GetRequest&uid=%u&mtype=%s&toward=%d&status=%d&count=%u",
				m_uid, mtype.c_str(), toward, status, (unsigned)m_jsonResult["requests"].size());
		return R_SUCCESS;
	}

	int ProcessRequest()
	{
		int ret;
		string way = CCGIIn::GetCGIStr("way");
		string sMessageId = CCGIIn::GetCGIStr("messageid");
		if(way.empty() || sMessageId.empty())
		{
			PARAM_ERROR_RETURN();
		}
		uint64_t messageId;
		if(!Convert::StringToUInt64(messageId, sMessageId))
		{
			PARAM_ERROR_RETURN();
		}
		CLogicMessage logicMessage;
		if(way == "accept" || way == "reject")
		{
			ret = logicMessage.ReplyRequest(m_uid, 0, messageId, ((way == "accept") ? MS_ACCEPT : MS_REJECT), m_data["message"]);
			if (ret != 0)
			{
				return ret;
			}
		}
		else if (way == "cancel")
		{
			ret = logicMessage.CancelRequest(m_uid, "", messageId);
			if (ret != 0)
			{
				return ret;
			}
		}
		else
		{
			PARAM_ERROR_RETURN();
		}
		CGI_SEND_LOG("action=ProcessRequest&uid=%u&messageId=%llu&way=%s", m_uid, messageId, way.c_str());
		return R_SUCCESS;
	}

	int RequestItem()
	{
		int ret;
		string itemid = CCGIIn::GetCGIStr("itemid");
		if(itemid.empty())
		{
			PARAM_ERROR_RETURN();
		}
		int count = CCGIIn::GetCGIInt("count");
		if(count == CCGIIn::CGI_INT_ERR || count <= 0)
		{
			PARAM_ERROR_RETURN();
		}
		CLogicUserInteract logicInteract;
		uint64_t messageId;
		unsigned waitTime;
		ret = logicInteract.RequestItem(m_uid, itemid, count, messageId, waitTime);
		if(ret != 0)
		{
			if(string("request_item_time_limit") == GetErrorMessage())
			{
				m_jsonResult["wait"] = waitTime;
			}
			return ret;
		}
		m_jsonResult["messageid"] = Convert::UInt64ToString(messageId);
		CGI_SEND_LOG("action=RequestGift&uid=%u&itemid=%s&count=%d&messageid=%llu", m_uid, itemid.c_str(), count, messageId);
		return R_SUCCESS;
	}

};

CGI_MAIN(CCgiInteract)
