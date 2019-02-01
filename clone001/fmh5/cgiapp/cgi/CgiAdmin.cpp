#include "LogicInc.h"

string CreateCode()
{
	string skey = "";
	int range = 62;
	for (int i = 0; i < CODE_SIZE ; i++ )
	{
		int c = Math::GetRandomInt(range);
		if(c >= 36)
		{
			c -= 36;
			//skey += CTrans::ITOS(c) ;
			skey += (char)('a' + c);
		}else if(c >= 26)
		{	c -= 26;
			skey += (char)('0' + c);
		}else
		{
			skey += (char)('A' + c);
		}
	}
	return skey;
}
class CCgiAdmin : public CCgiBase
{
public:
	CCgiAdmin() : CCgiBase("admin")
	{
		SetFeature(CF_PRINT_JSON);
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_CHECK_PLATFORM);
	}

	CGI_SIMPLE_ACTION_MAP_BEGIN
	CGI_SET_ACTION_MAP("login", Login)
	CGI_SET_ACTION_MAP("logincheck", LoginCheck)
	CGI_SET_ACTION_MAP("autologin", AutoLogin)
	CGI_SET_ACTION_MAP("modifypwd", ModifyPwd)
	CGI_SET_ACTION_MAP("broadcast", BroadCast)
	CGI_SET_ACTION_MAP("forbidchat", ForbidChat)
	CGI_SET_ACTION_MAP("getexchargenumber", GetExchangeCode)
	CGI_SET_ACTION_MAP("changeblocktsbyopenid", changeBlockTsByOpenId)
	CGI_SET_ACTION_MAP("queryblocktsbyopenid", QueryBlockTsByOpenId)
	CGI_SET_ACTION_MAP("addgamemanager",AddGM)
	CGI_SET_ACTION_MAP("delgamemanager",DelGM)
	CGI_SET_ACTION_MAP("add_th",Add_th)
	CGI_SET_ACTION_MAP("del_th",Del_th)
	CGI_SET_ACTION_MAP("add_ip",Add_ip)
	CGI_SET_ACTION_MAP("del_ip",Del_ip)
	CGI_SET_ACTION_MAP("addResourceOrEquip", addResourceOrEquip)
	CGI_SET_ACTION_MAP("GetActTime",GetActTime)
	CGI_SET_ACTION_MAP("SetActTime",SetActTime)
	CGI_SET_ACTION_MAP("GetForbidTime",GetForbidTime)
	CGI_SET_ACTION_MAP("SetForbidTime",SetForbidTime)


	CGI_ACTION_MAP_END


	int AddTsCheck(const string &name, const int &ts)
	{
		m_tscheck[name] = ts;
		return 0;
	}

	int CheckTs(const string &name,const int &ts)
	{
		if(m_tscheck.count(name) && m_tscheck[name] == ts)
			return 0;
		error_log("name=%s,ts=%d,real_ts=%d,wrong!",name.c_str(), ts,m_tscheck.count(name)?m_tscheck[name]:0);
		SESS_ERROR_RETURN_MSG("TS error");
	}

	int loginIP(const string &name, const string &ip)
	{
		m_ipcheck[name] = m_ipstr;
		return 0;
	}
	int checkIP(const string &name)
	{
		if(m_ipcheck.count(name) && m_ipcheck[name] == m_ipstr)
			return 0;
		error_log("name=%s,ip=%s,real_ip=%s,wrong!",name.c_str(), m_ipstr.c_str(),m_ipcheck.count(name)?m_ipcheck[name].c_str():"NULL");
		SESS_ERROR_RETURN_MSG("IP error");
	}

	int checkTry(const string &name)
	{
		if(m_trycheck.count(name))
		{
			if(m_trycheck[name].second + 86400 > Time::GetGlobalTime())
			{
				if(m_trycheck[name].first > 5)
					return R_ERR_LOGIC;
			}
			else
				m_trycheck.erase(name);
		}

		return 0;
	}
	int addTry(const string &name)
	{
		if(m_trycheck.count(name))
			m_trycheck[name] = pair<unsigned, unsigned>(m_trycheck[name].first+1, Time::GetGlobalTime());
		else
			m_trycheck[name] = pair<unsigned, unsigned>(1, Time::GetGlobalTime());

		return 0;
	}
	
	int Login()
	{
		int ret = 0;
		int ts = Time::GetGlobalTime();
		string name = CCGIIn::GetCGIStr("username");
		string password = CCGIIn::GetCGIStr("password");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		if(checkTry(name))
		{
			REFUSE_RETURN_MSG("Passwd error");
		}
		
		string skey;
		int level = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.Login(name, password, skey);
			if (ret != 0)
			{
				addTry(name);
				return ret;
			}
			m_jsonResult["username"] = name;
			m_jsonResult["skey"] = skey;
			m_jsonResult["ts"] = ts;
		}
		else
		{
			CLogicAdmin logicAdmin;
			ret = logicAdmin.Login(name, password, skey);
			if (ret != 0)
			{
				addTry(name);
				return ret;
			}

			level = logicAdmin.GetLevel(name);
			m_jsonResult["username"] = name;
			m_jsonResult["skey"] = skey;
			m_jsonResult["level"] = level;
			m_jsonResult["ts"] = ts;
		}

		loginIP(name, m_ipstr);
		AddTsCheck(name,ts);

		CGI_SEND_LOG("action=login&name=%s&skey=%s&level=%u", name.c_str(), skey.c_str(),level);
		return 0;
	}

	int LoginCheck(){
			string name = CCGIIn::GetCGIStr("username");
			string skey = CCGIIn::GetCGIStr("skey");

			CLogicAdmin logicAdmin;
			int ret = logicAdmin.LoginCheck(name,skey);
			if(ret!=0)
			{
				return ret;
			}

			ret = checkIP(name);if(ret)	return ret;

			return 0 ;
		}

	int AutoLogin()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		if (name.empty() || skey.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int level = logicAdmin.AutoLogin(name, skey);
		m_jsonResult["level"] = level;

		CGI_SEND_LOG("action=autologin&level=%u", level);
		return 0;
	}

	int ModifyPwd()
	{
		string name = CCGIIn::GetCGIStr("username");
		string password = CCGIIn::GetCGIStr("password");
		string newpwd = CCGIIn::GetCGIStr("newpwd");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || newpwd.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		
		if(checkTry(name))
		{
			REFUSE_RETURN_MSG("Passwd error");
		}
		
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.ChangePwd(name, password, newpwd);
			if (0 != ret) 
			{
				addTry(name);
				return ret;
			}
		}
		else
		{
			CLogicAdmin logicAdmin;
			ret = logicAdmin.ModifyPassword(name, password, newpwd);
			if (ret != 0)
			{
				addTry(name);
				return ret;
			}
		}
		CGI_SEND_LOG("action=modifypwd&name=%s", name.c_str());
		return 0;
	}

	int BroadCast()
	{
		int ret = 0;
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");

		int ts =  CCGIIn::GetCGIInt("ts");
		CLogicAdmin logicAdmin;
		ret = logicAdmin.CheckSession(name, skey);
		if (ret != 0)
			return ret;
		ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_9);
		if (ret != 0)
			return ret;
		ret = checkIP(name);if(ret)	return ret;
		ret = CheckTs(name,ts);if(ret)return ret;
		ts = Time::GetGlobalTime();
		AddTsCheck(name,ts);
		m_jsonResult["ts"] = ts;

		string message = CCGIIn::GetCGIStr("content");
		int repeats = CCGIIn::GetCGIInt("repeats");
		unsigned interval = CCGIIn::GetCGIInt("interval");
		string serverid = CCGIIn::GetCGIStr("serverid");

		if (serverid.empty())
			Config::GetValue(serverid, CONFIG_SERVER_ID);

		vector<string> rlt;
		String::Split(serverid, '-', rlt);
		if (rlt.size() >= 2) {
			unsigned begin = Convert::StringToUInt(rlt[0]);
			unsigned end = Convert::StringToUInt(rlt[1]);
			if (end < begin) {
				error_log("BroadCast error!");
				return ret;
			}

			for (unsigned i = begin; i <= end; ++i) {
				string serverstr = Convert::UIntToString(i);
				logicAdmin.BroadCastByTool(message, repeats, interval,serverstr);
			}
		} else {
			logicAdmin.BroadCastByTool(message, repeats, interval, serverid);
		}
		CGI_SEND_LOG("action=broadcast&repeats = %d&interval = %d&message=%s",repeats,interval,message.c_str());
		return 0;
	}

	int ForbidChat()
	{
		CLogicAdmin logicAdmin;
		int ret = 0;
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		ret = logicAdmin.CheckSession(name, skey);
		if (ret != 0)
			return ret;
		ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_1);
		if (ret != 0)
			return ret;

		unsigned uid = CCGIIn::GetCGIInt("uid");
		string deadline = CCGIIn::GetCGIStr("deadline");
		string serverid = CCGIIn::GetCGIStr("serverid");

		unsigned forbidts= (unsigned) CTime::ParseDate(deadline);
		//unsigned block_time = (unsigned) CTime::ParseDate(blockts);
		if (forbidts == 0) {
			PARAM_ERROR_RETURN_MSG("time_format_error");
		}
		//info_log("uid = %u,forbidts = %u,deadline =%s",uid,forbidts,deadline.c_str());

		ret  = logicAdmin.AddForbidUser(uid,forbidts,serverid);
		if(ret != 0)
		{
			return ret;
		}
		return 0;
	}
	int GetExchangeCode()
	{
		Json::Value data;
		Json::FastWriter writer;
		string type = CCGIIn::GetCGIStr("type");
		if(type.empty())
			type="00";
		unsigned t = atoi(type.c_str());
		if(t >= CODE_TYPE)
			return R_ERR_PARAM;
		int count = CCGIIn::GetCGIInt("count",0,100000,10,10);
		if(!count)
			count = 10;
		int ret = 0;
		DataExchangeCode dataExchangeCode;
		CLogicExchangeCode logicExchangeCode;
		string code;
		int i = 0;
		dataExchangeCode.uid = ADMIN_UID;
		dataExchangeCode.type = t;
		dataExchangeCode.gentime = Time::GetGlobalTime();
		dataExchangeCode.deadline = dataExchangeCode.gentime + (86400 * 99);
		dataExchangeCode.usetime = 0;
		for(i = 0; i < count; i++)
		{
			code = CreateCode();
			dataExchangeCode.code = code;
			ret = logicExchangeCode.AddExchangeCode(dataExchangeCode);
			if(ret != 0)
			{
				continue;
			}
			data[i]=type + code;
		}
		m_jsonResult["data"] = data;
		return 0;
	}

	int AddGM()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		if (name.empty() || skey.empty() || openid.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicAdmin logicAdmin;
		int ret = logicAdmin.CheckSession(name, skey);
		if (ret != 0)
			return ret;
		ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_9);
		if (ret != 0)
			return ret;

		ret = logicAdmin.SetFlag(openid,gm_admin);
		if (0 != ret)
		{
			REFUSE_RETURN_MSG("ChageGMList_fail");
		}
		CLogicAdmin::Log(name, "add_GM", openid,time(0),skey,"");
		return 0;
	}

	int DelGM()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		if (name.empty() || skey.empty() || openid.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicAdmin logicAdmin;
		int ret = logicAdmin.CheckSession(name, skey);
		if (ret != 0)
			return ret;
		ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_9);
		if (ret != 0)
			return ret;

		ret = logicAdmin.SetFlag(openid,gm_none);
		if (0 != ret)
		{
			REFUSE_RETURN_MSG("ChageGMList_fail");
		}
		CLogicAdmin::Log(name, "del_GM", openid,88,skey,"");
		return 0;
	}

	int Add_th()
	{
		if(!OpenPlatform::IsOurPlatform())
			return -1;

		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || openid.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_ALL);
			if (ret != 0)
				return ret;
		}

		ret = logicAdmin.SetFlag(openid,gm_th);
		if (0 != ret)
		{
			REFUSE_RETURN_MSG("ChageTHList_fail");
		}

		if (1 != custom)
		{
			CLogicAdmin::Log(name, "add_th", openid, time(0), skey, "");
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "add_th", openid, time(0), skey, "");
		}

		CLogicAdmin::AddCheckLog(name,m_ipstr,"运营号",openid,0,0,0);

		return 0;

	}

	int Del_th()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || openid.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_ALL);
			if (ret != 0)
				return ret;
		}

		ret = logicAdmin.SetFlag(openid,gm_none);
		if (0 != ret)
		{
			REFUSE_RETURN_MSG("ChageTHList_fail");
		}
		if (1 != custom)
		{
			CLogicAdmin::Log(name, "del_th", openid,99,skey,"");
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "del_th", openid, 99, skey, "");
		}
		return 0;
	}

	int Add_ip()
	{
		if(!OpenPlatform::IsOurPlatform())
			return -1;

		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || openid.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_9);
			if (ret != 0)
				return ret;
		}

		ret = logicAdmin.SetFlag(openid,gm_ip);
		if (0 != ret)
		{
			REFUSE_RETURN_MSG("ChageIPList_fail");
		}

		if (1 != custom)
		{
			CLogicAdmin::Log(name, "add_ip", openid, time(0), skey, "");
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "add_ip", openid, time(0), skey, "");
		}

		return 0;
	}

	int Del_ip()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || openid.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_9);
			if (ret != 0)
				return ret;
		}

		ret = logicAdmin.SetFlag(openid,gm_none);
		if (0 != ret)
		{
			REFUSE_RETURN_MSG("ChageIPList_fail");
		}
		if (1 != custom)
		{
			CLogicAdmin::Log(name, "del_ip", openid,99,skey,"");
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "del_ip", openid, 99, skey, "");
		}

		CLogicAdmin::AddCheckLog(name,m_ipstr,"解除ip限制",openid,0,0,0);

		return 0;
	}

	int changeBlockTsByOpenId()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		string blockts = CCGIIn::GetCGIStr("blockts");
		string close_reason = CCGIIn::GetCGIStr("resonforclose");
		if (name.empty() || skey.empty() || openid.empty() || close_reason.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicAdmin logicAdmin;
		int ret = logicAdmin.CheckSession(name, skey);
		if (ret != 0)
			return ret;
		ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_9);
		if (ret != 0)
			return ret;

		unsigned block_time = (unsigned)CTime::ParseDate(blockts);
		ret = logicAdmin.ChangeBlockTs(openid,block_time,close_reason);
		if (0 != ret)
		{
			REFUSE_RETURN_MSG("ChangeBlockTs");
		}
		return 0;
	}
	int QueryBlockTsByOpenId()
	{
		string openid = CCGIIn::GetCGIStr("openid");
		if (openid.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicAdmin logicAdmin;

		int ret = logicAdmin.GetBlockTs(openid,m_jsonResult);
		if (0 != ret)
		{
			REFUSE_RETURN_MSG("QueryBlockTsByOpenId");
		}
		return 0;
	}

	int addResourceOrEquip()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		int ts =  CCGIIn::GetCGIInt("ts");
		string reason = CCGIIn::GetCGIStr("reason");
		string equipid =  CCGIIn::GetCGIStr("equip_id");
		unsigned type = CCGIIn::GetCGIInt("type");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		int count = CCGIIn::GetCGIInt("count");

		if (name.empty() || skey.empty() || reason.empty() || 0 == uid || 0 == type)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		unsigned equip_id = 0;

		if (!equipid.empty())
		{
			equip_id =  CTrans::STOI(equipid.c_str());
		}

		//校验
		CLogicAdmin logicAdmin;
		int ret = logicAdmin.CheckSession(name, skey);
		if (ret != 0)
		{
			return ret;
		}
		ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_9);
		if (ret != 0)
		{
			return ret;
		}
		ret = checkIP(name);if(ret)	return ret;
		ret = CheckTs(name,ts);if(ret)return ret;
		ts = Time::GetGlobalTime();
		AddTsCheck(name,ts);
		m_jsonResult["ts"] = ts;

		//校验完成，开始实现功能
		ret = logicAdmin.AddResourceOrEquip(uid, type, count, reason, equip_id);
		if (ret)
		{
			return ret;
		}

		string t = CTrans::ITOS((type-1>= e_asyn_max)?equip_id:(type-1)) + ":" + CTrans::ITOS(count);
		CLogicAdmin::Log(name, "Add_ResourceOrEquip", reason, uid, t);

		return 0;
	}

	int SetActTime()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		int ts =  CCGIIn::GetCGIInt("ts");

		unsigned id = CCGIIn::GetCGIInt("id");
		unsigned bts = CCGIIn::GetCGIInt("bts");
		unsigned ets = CCGIIn::GetCGIInt("ets");
		unsigned ver = CCGIIn::GetCGIInt("ver");

		if (name.empty() || skey.empty() || id == 0 || bts == 0 || ets == 0 || ver == 0)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		//校验
		CLogicAdmin logicAdmin;
		int ret = logicAdmin.CheckSession(name, skey);
		if (ret != 0)
		{
			return ret;
		}
		ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_9);
		if (ret != 0)
		{
			return ret;
		}
		ret = checkIP(name);if(ret)	return ret;
		ret = CheckTs(name,ts);if(ret)return ret;
		ts = Time::GetGlobalTime();
		AddTsCheck(name,ts);
		m_jsonResult["ts"] = ts;

		m_jsonResult["act"] = logicAdmin.SetActTime(id, bts, ets, ver);

		string value = CTrans::ITOS(id) + " " + CTrans::ITOS(bts) + " " + CTrans::ITOS(ets) + " " + CTrans::ITOS(ver);
		CLogicAdmin::Log(name, "SetActTime", "", 0, value, "");

		return 0;
	}
	int GetActTime()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		if (name.empty() || skey.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int ret = logicAdmin.CheckSession(name, skey);
		if (ret != 0)
		{
			return ret;
		}
		ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_9);
		if (ret != 0)
		{
			return ret;
		}

		m_jsonResult["act"] = logicAdmin.GetActTime();

		return 0;
	}

	int GetForbidTime()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");

		unsigned uid = CCGIIn::GetCGIInt("uid");
		if (uid == 0)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		//校验
		CLogicAdmin logicAdmin;
		int ret = logicAdmin.CheckSession(name, skey);
		if (ret != 0)
		{
			return ret;
		}
		ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_0);
		if (ret != 0)
		{
			return ret;
		}

		Admin::RequestForbidTS * msg = new Admin::RequestForbidTS;
		msg->set_uid(uid);
		CSG17Packet packet;
		packet.cmd = PROTOCOL_ADMIN;
		packet.m_msg = msg;
		ret = BattleSocket::Send(Config::GetZoneByUID(uid), &packet);
		if(ret)
		{
			ERROR_RETURN_MSG(5, "send system error");
		}
		CSG17Packet reply;
		ret = BattleSocket::Receive(Config::GetZoneByUID(uid), &reply);
		if(ret)
		{
			ERROR_RETURN_MSG(5, "recv system error");
		}
		Admin::ReplyForbidTS* rmsg = (Admin::ReplyForbidTS*) reply.m_msg;

		m_jsonResult["forbid_ts"] = rmsg->forbid_ts();
		m_jsonResult["reason"] = rmsg->reason();

		return 0;
	}
	int SetForbidTime()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");

		unsigned uid = CCGIIn::GetCGIInt("uid");
		unsigned forbid_ts = CCGIIn::GetCGIInt("forbid_ts");
		string reason = CCGIIn::GetCGIStr("reason");

		if (uid == 0 || reason.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		//校验
		CLogicAdmin logicAdmin;
		int ret = logicAdmin.CheckSession(name, skey);
		if (ret != 0)
		{
			return ret;
		}
		ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_1);
		if (ret != 0)
		{
			return ret;
		}

		Admin::SetForbidTS * msg = new Admin::SetForbidTS;
		msg->set_uid(uid);
		msg->set_forbid_ts(forbid_ts);
		msg->set_reason(reason);
		CSG17Packet packet;
		packet.cmd = PROTOCOL_ADMIN;
		packet.m_msg = msg;
		ret = BattleSocket::Send(Config::GetZoneByUID(uid), &packet);
		if(ret)
		{
			ERROR_RETURN_MSG(5, "send system error");
		}

		string value = CTrans::ITOS(forbid_ts);
		CLogicAdmin::Log(name, "SetForbidTime", "", uid, value, "");

		return 0;
	}

public:
	Json::Value ExportData;
	static map<string,string> m_ipcheck;
	static map<string,int> m_tscheck;
	static map<string,pair<unsigned, unsigned> > m_trycheck;

};
map<string,string> CCgiAdmin::m_ipcheck;
map<string,int> CCgiAdmin::m_tscheck;
map<string,pair<unsigned, unsigned> > CCgiAdmin::m_trycheck;
CGI_MAIN(CCgiAdmin)
