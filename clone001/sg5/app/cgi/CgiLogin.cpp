
#include "LogicInc.h"

class CCgiLogin : public CCgiBase
{
public:
	CCgiLogin() : CCgiBase("login")
	{
		SetFeature(CF_PRINT_JSON);
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_CHECK_PLATFORM);
		SetFeature(CF_CHECK_HASH);
		SetFeature(CF_CHECK_VERSION);
		SetFeature(CF_APPEND_HASH);
		SetFeature(CF_PARSE_DATA);
		SetFeature(CF_CRYPT_DATA);
		SetFeature(CF_DECRYPT_DATA);
		//SetFeature(CF_COMPRESS_OUT);
	}

	CGI_DIRECT_PROCESS(Login)

	int Login()
	{
		//解析参数
		string openid;
		if (!Json::GetString(m_data, "openid", openid))
		{
			PARAM_ERROR_RETURN_MSG("check_openid_fail");
		}
		string openkey;
		Json::GetString(m_data, "openkey", openkey);
		string iopenid;
		Json::GetString(m_data, "iopenid", iopenid);
		int isWhite = 0;
		Json::GetInt(m_data, "white", isWhite);
		string promotionId;
		Json::GetString(m_data, "spreadid", promotionId);
		unsigned spreadid = atoi(promotionId.c_str());
		if (openid.empty())
		{
			error_log("[openid is empty][platform=%d,openid=%s,openkey=%s]",
					OpenPlatform::GetType(), openid.c_str(),openkey.c_str());
			PARAM_ERROR_RETURN_MSG("check_openid_fail");
		}

		//GM flag
		CLogicAdmin logicAdmin;
		unsigned gmFlag = 0;
		logicAdmin.CheckGM(openid, gmFlag);
		/*
		if((gmFlag & gm_th) && m_ipstr != "218.17.157.202")
		{
			error_log("check_ip_fail,openid=%s,ip=%s",openid.c_str(),m_ipstr.c_str());
			PARAM_ERROR_RETURN_MSG("check_ip_fail");
		}
		*/
		m_jsonResult["gm"] = (gmFlag & gm_admin)?1:0;
		m_jsonResult["th"] = (gmFlag & gm_th)?1:0;

		//  获取UID
		OPUserInfo userinfo;
		unsigned uid = 0;
		if (WhiteList::Check(openid, openid))
		{
//			error_log("[invalid white user][openid=%s,openkey=%s]", openid.c_str(), openkey.c_str());
//			REFUSE_RETURN_MSG("check_white_user_fail");
			isWhite = 1;
		}
		if (isWhite == 1 )
		{
			//  白名单
			//openkey = openid;
//			if (!WhiteList::Check(openid, openkey))
//			{
//				error_log("[invalid white user][openid=%s,openkey=%s]", openid.c_str(), openkey.c_str());
//				REFUSE_RETURN_MSG("check_white_user_fail");
//			}
			//uid = Convert::StringToUInt(openid, 0);
//			if (!IsValidUid(uid))
//			{
//				error_log("[invalid white user][openid=%s,openkey=%s]", openid.c_str(), openkey.c_str());
//				REFUSE_RETURN_MSG("check_white_user_fail");
//			}
			int ret = 0;

			map<string, string> params;
			string pf;
			if (Json::GetString(m_data, "pf", pf) && !pf.empty())
				params["pf"] = pf;
			params["userip"] = m_ipstr;
			OpenPlatform::GetPlatform()->SetParameter(params);
			CLogicUserBasic logicUserBasic;
			bool isNewUser = false;
			OPFriendList friendList;
			ret = logicUserBasic.LoginPlatformUser(OpenPlatform::GetType(), userinfo, friendList, iopenid, uid,isNewUser);
			if(ret != 0)
				return ret;
			info_log("[white user login][uId=%d,openid=%s,openkey=%s]", uid, openid.c_str(), openkey.c_str());
		}
		else
		{
			// 非白名单
			int ret = 0;

			map<string, string> params;
			string pf;
			if (Json::GetString(m_data, "pf", pf) && !pf.empty())
				params["pf"] = pf;
			params["userip"] = m_ipstr;
			OpenPlatform::GetPlatform()->SetParameter(params);

			ret = OpenPlatform::GetPlatform()->GetUserInfo(userinfo, openid, openkey);
			if (ret != 0)
			{
				if(OpenPlatform::GetType() == PT_TEST && ret == R_ERR_DATA_LIMIT)
				{
					ERROR_RETURN_MSG(R_ERR_DATA_LIMIT, "名字中包含非法字符");
				}

				error_log("[GetUserInfo fail][ret=%d,openid=%s,openkey=%s]",
						ret,openid.c_str(),openkey.c_str());
				PT_ERROR_RETURN_MSG("get_user_info_fail");
			}
			if (userinfo.Name.empty())
			{
				error_log("[GetUserInfo name empty][openid=%s,openkey=%s,name=%s,pic=%s]",
						openid.c_str(), openkey.c_str(), userinfo.Name.c_str(), userinfo.FigureUrl.c_str());
			}

			if((OpenPlatform::GetType() == PT_PENGYOU
				|| OpenPlatform::GetType() == PT_QZONE
				|| OpenPlatform::GetType() == PT_TXWEIBO
				|| OpenPlatform::GetType() == PT_3366
				|| OpenPlatform::GetType() == PT_qqgame)
				&& (gmFlag & gm_th))
			{
				userinfo.ExtraIntInfo["bIsVip"] = 1;
				userinfo.ExtraIntInfo["bIsYearVip"] = 1;
				userinfo.ExtraIntInfo["nVipLevel"] = 7;

				userinfo.ExtraIntInfo["is_blue_vip"] = 1;
				userinfo.ExtraIntInfo["is_blue_year_vip"] = 1;
				userinfo.ExtraIntInfo["blue_vip_level"] = 7;
				userinfo.ExtraIntInfo["is_super_blue_vip"] = 1;
			}

			OPFriendList friendList;
			ret = OpenPlatform::GetPlatform()->GetAppFriendList(friendList, openid, openkey);
			if(ret != 0)
			{
				error_log("[GetAppFriendList fail][ret=%d,openid=%s,openkey=%s]",
						ret,openid.c_str(),openkey.c_str());
			}
			CLogicUserBasic logicUserBasic;
			bool isNewUser = false;
			ret = logicUserBasic.LoginPlatformUser(OpenPlatform::GetType(), userinfo, friendList, iopenid, uid,isNewUser);
			if(ret != 0)
				return ret;
			// 推广登记
//			if(OpenPlatform::GetType() == PT_TEST) //测试用，现网暂时屏蔽
//			{

				if(spreadid == 0 && OpenPlatform::GetType() == PT_TEST && isNewUser)
				{
					spreadid=10000001;

				}
				if(spreadid != 0 && isNewUser)
				{
					CLogicPromotionMapping logicPromotionMapping;
					DataPromotionMapping dataPromotionMapping;
					dataPromotionMapping.uid = uid;
					dataPromotionMapping.pid = spreadid;
					dataPromotionMapping.usetime = 0;
					dataPromotionMapping.useflag = 0;
					logicPromotionMapping.AddPromotionMapping(dataPromotionMapping);
				}
				if(isNewUser)
				{
					SPREAD_LOG("%s|%u|%u|%s|",openid.c_str(),uid,spreadid,iopenid.c_str());
				}
//			}
		}

		if (OpenPlatform::GetType() == 7)
		{
			size_t found = openkey.find('|');
			if (found != string::npos)
			{
				openkey = openkey.substr(0, found);
			}
		}
		//  分配KEY
		string skey = Session::CreateSessionKey(uid);
		string sessionData;
		String::Format(sessionData, "{\"rip\":%u,\"lvt\":%u,\"skey\":\"%s\",\"oid\":\"%s\",\"okey\":\"%s\",\"pt\":%d}",
				m_remoteip, Time::GetGlobalTime(), skey.c_str(), openid.c_str(), openkey.c_str(), OpenPlatform::GetType());
		Session::SetValue(uid, SESSION_DATA, sessionData);
		//m_skey = skey;

		int encode = CCGIIn::GetCGIInt("encode");

		CLogicUserBasic logicUserBasic;
		Json::Value friends, blacklist;
		CLogicUser data;
		DataUser userDB;

		int ret = logicUserBasic.GetFriendsJson(uid, OpenPlatform::GetType(), friends, (encode==1?true:false));
		if(ret != 0)
			return ret;

		ret = data.GetUserLimit(uid, userDB);
		if(ret != 0)
			return ret;

		ret = logicUserBasic.GetBlacksJson(uid, OpenPlatform::GetType(), blacklist);
		if(ret != 0)
			return ret;

		m_jsonResult["uid"] = uid;
		m_jsonResult["name"] = userinfo.Name;
		m_jsonResult["cur_pt"] = (int)OpenPlatform::GetType();
		m_jsonResult["currenttime"] = Time::GetGlobalTime();
		m_jsonResult["skey"] = skey;
		m_jsonResult["tcp_server"] = Config::GetValue(CONFIG_TCP_SERVER);
		m_jsonResult["all_server"] = Config::GetValue(CONFIG_ALL_SERVER);
		int serveridb;
		Config::GetDomain(serveridb);
		m_jsonResult["serverid"] = serveridb;
		m_jsonResult["defaultkingdom"] = Config::GetValue(CONFIG_DEFAULT_KINGDOM);
		m_jsonResult["friends"] = friends;
		m_jsonResult["black"] = blacklist;
		//读档
		Json::Value archive;
		CLogicArchive logicArchive;
		int result = logicArchive.Load(uid, uid, "build", m_data, m_jsonResult);
		if (result != 0)
			return result;

		CLogicNominateAlliance nAlliance;
		ret = nAlliance.CheckAlliance(uid);

		CGI_SEND_LOG("uid=%u&platform=%d&openid=%s&openkey=%s&iopenid=%s&isWhite=%d",
				uid, OpenPlatform::GetPlatform()->GetPlatformByPF(), openid.c_str(), openkey.c_str(), iopenid.c_str(), isWhite);
		return R_SUCCESS;
	}

};

CGI_MAIN(CCgiLogin)
