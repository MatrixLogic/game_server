/*
 * LogicQQHallApi.cpp
 *
 *  Created on: 2016-4-1
 *      Author: oulong
 */


#include "LogicQQHallApi.h"

//-------------------------------------------------------------------
//QQV3BaseParams
QQHallApi::QQHallApi(CCgiBase& cgi, const Json::Value& jsonData)
{
	std::string szOpenId = jsonData["openid"].asString();
	if (szOpenId.empty())
	{
		error_log("[qq report][no openid]");
		throw std::invalid_argument("param_error_openid");
	}
	m_params.insert(make_pair("openid", szOpenId));

	std::string szOpenKey = jsonData["openkey"].asString();
	if (szOpenKey.empty())
	{
		error_log("[qq report][no openkey]");
		throw std::invalid_argument("param_error_openkey");
	}
	m_params.insert(make_pair("openkey", szOpenKey));

	std::string szPf = jsonData["pf"].asString();
	if (szPf.empty())
	{
		error_log("[qq report][no pf]");
		throw std::invalid_argument("param_error_pf");
	}
	m_params.insert(make_pair("pf", szPf));

	std::string appid = "";
	if (NULL == OpenPlatform::GetPlatform()
	|| OpenPlatform::GetPlatform()->GetAppId().empty())
	{
		error_log("[qq report][not_config_plathform_appid]");
		throw std::logic_error("not_config_plathform_appid");
	}
	else
	{
		appid = OpenPlatform::GetPlatform()->GetAppId();
	}
	m_params.insert(make_pair("appid", appid));

	unsigned uid = jsonData["uid"].asInt();
	if (0 == uid)
	{
		error_log("[qq report][not_uid]");
		throw std::logic_error("not_uid");
	}
	m_params.insert(make_pair("roleid", CTrans::UTOS(uid)));

	//获取zoneid
	string serverid;
	unsigned zoneid;
	if (Config::GetValue(serverid, CONFIG_SERVER_ID))
	{
		Convert::StringToUInt(zoneid, serverid);
		unsigned domain = MainConfig::GetMergedDomain(zoneid);
		if(domain != zoneid)
		{
			error_log("[qq report][is_merge_zone]");
			throw std::invalid_argument("is_merge_zone");
		}

		m_params.insert(make_pair("zoneid", serverid));
	}
	else
	{
		error_log("[qq report][no zoneid]");
		throw std::invalid_argument("param_error_zoneid");
	}
}


int QQHallApi::MakeSignVlaue(const std::string& method
		, const std::string& uri
		, const std::map<string, string>& params
		, std::string& sign) const
{
	//Step 1. 构造源串----------------------------------------------
	std::string m = method;
	std::transform(m.begin(), m.end(), m.begin(), ::toupper);
	//>>1. 将请求的URI路径进行URL编码
	std::string data = m + "&" + Crypt::UrlEncodeForTX(uri);
	//>>2. 将除“sig”外的所有参数按key进行字典升序排列。
	std::string szParams;
	for (std::map<string, string>::const_iterator iter = params.begin();
			iter != params.end();
			++ iter)
	{
		szParams +=  "&" + iter->first + "=" + iter->second;
	}
	//>>3. 将第2步中排序后的参数(key=value)用&拼接起来，并进行URL编码。
	//>>4. 将HTTP请求方式（GET或者POST）以及第1步和第3步中的字符串用&拼接起来。
	szParams.erase(szParams.begin());
	data += "&" + Crypt::UrlEncodeForTX(szParams);

	//Step 2. 构造密钥
	//得到密钥的方式：在应用的appkey末尾加上一个字节的“&”，即appkey&，
	std::string key = "euUqWcUiPtlKEqxM&";
	if (OpenPlatform::GetPlatform())
	{
		key = OpenPlatform::GetPlatform()->GetAppKey() + "&";
	}
	else
	{
		PARAM_ERROR_RETURN_MSG("[qq_report][get_playtform_failed]");
	}

	//Step 3. 生成签名值
	//1. 使用HMAC-SHA1加密算法，使用Step2中得到的密钥对Step1中得到的源串加密。
	//2. 然后将加密后的字符串经过Base64编码。
	string bsig = Crypt::HmacSha1(data, key);

	if (! Crypt::Base64Encode(sign, bsig))
	{
		return R_ERROR;
	}

	return R_SUCCESS;
}

int QQHallApi::MakeRequestData(const std::string& uri, std::string& data)
{
	int ret = 0;
	std::string szSign;
	if ((ret = MakeSignVlaue("GET", uri, m_params, szSign)) != R_SUCCESS)
	{
		error_log("[report qq][make_get_data error.2 ret=%d]", ret);
		return ret;
	}

	data.clear();
	for (std::map<string, string>::const_iterator it = m_params.begin();
			it != m_params.end();
			++it)
	{
		data +=  it->first + "=" + Crypt::UrlEncodeForTX(it->second) + "&";
	}

	data += "sig=" + Crypt::UrlEncodeForTX(szSign);


	return R_SUCCESS;
}

void QQHallApi::SetFormat(const std::string& format)
{
	if (! format.empty())
	{
		m_params.insert(make_pair("format", format));
	}
}

void QQHallApi::SetUserIP(const std::string& ip)
{
	if (! ip.empty())
	{
		m_params.insert(make_pair("userip", ip));
	}
}

void QQHallApi::SetCharset(const std::string& charset)
{
	if (! charset.empty())
	{
		m_params.insert(make_pair("charset", charset));
	}
}

std::string QQHallApi::OpenId() const
{
	std::map<string, string>::const_iterator it = m_params.find("openid");
	if (it != m_params.end())
	{
		return it->second;
	}

	return "";
}

//@end QQHallApi

//--------------------------------------------------------------------
int QQUserApi::MakeRequestData(const std::string& uri, std::string& data)
{
	if (Infotype() == 0)
	{
		error_log("[qq report][not over infotype]");
		return R_ERROR;
	}
	m_params.insert(make_pair("infotype", CTrans::ITOS(Infotype())));

	return QQHallApi::MakeRequestData(uri, data);
}

//---------------------------------------------------------------------
//Login
QQHallLoginSdk::QQHallLoginSdk(CCgiBase& cgi, const Json::Value& jsonData)
	: QQUserApi(cgi, jsonData)
{
	std::string szHeadUrl = jsonData["user_headurl"].asString();
//	if (szHeadUrl.empty())
//	{
//		//error_log("[qq_report_login][no_headurl]");
//		//throw std::invalid_argument("param_error_headurl");
//	}
	m_params.insert(make_pair("headurl", szHeadUrl));

	std::string szName = jsonData["user_name"].asString();
	if (szName.empty())
	{
		error_log("[qq_report_login][no_name]");
		throw std::invalid_argument("param_error_name");
	}
	m_params.insert(make_pair("name", szName));

	std::string szZoneName = jsonData["zone_name"].asString();
	if (szZoneName.empty())
	{
		error_log("[qq_report_login][no_zone_name]");
		throw std::invalid_argument("param_error_zone_name");
	}
	m_params.insert(make_pair("buf1", szZoneName));

//	int nGender = CCGIIn::GetCGIInt("gender");
//	if (nGender != 1 && nGender != 2)
//	{
//		error_log("[qq_report_login][gender=%d]", nGender);
//		throw std::invalid_argument("param_error_gender");
//	}
//	m_params.insert(make_pair("gender", CTrans::ITOS(nGender)));
}
//-------------------------------------------------------------------

QQHallGenerallApi::QQHallGenerallApi(CCgiBase& cgi, const Json::Value& jsonData)
	: QQHallCommon(cgi, jsonData)
{
	//总战力
	if (! jsonData.isMember("user_total_power"))
	{
		error_log("[no_total_fight]");
		throw std::invalid_argument("qq_report_no_total_fight");
	}
	int nTotalValue = jsonData["user_total_power"].asInt();

	m_params.insert(make_pair("value1", CTrans::ITOS(nTotalValue)));

	//用户
	if (! jsonData.isMember("user_level"))
	{
		error_log("[no_user_level]");
		throw std::invalid_argument("qq_report_no_user_level");
	}
	int nUserLevel = jsonData["user_level"].asInt();

	m_params.insert(make_pair("value2", CTrans::ITOS(nUserLevel)));

	//主城等级
	if (! jsonData.isMember("main_city_level"))
	{
		error_log("[no_main_level]");
		throw std::invalid_argument("qq_report_no_main_level");
	}
	int nMainLevel = jsonData["main_city_level"].asInt();

	m_params.insert(make_pair("value3", CTrans::ITOS(nMainLevel)));

	//英雄数量
	if (! jsonData.isMember("heros_num"))
	{
		error_log("[no_heros_number]");
		throw std::invalid_argument("[QQHallHeroNumberApi]qq_report_no_heros_number");
	}
	int nHeros = jsonData["heros_num"].asInt();

	m_params.insert(make_pair("value4", CTrans::ITOS(nHeros)));

	//成就
	std::string szAchives = jsonData["user_achives"].asString();
	if (szAchives.empty())
	{
		error_log("[no_achievement]");
		throw std::invalid_argument("[QQHallAchievementApi]qq_report_no_achives");
	}

	m_params.insert(make_pair("buf1", szAchives));

	//最强英雄
	std::string szHeroName = jsonData["powerful_hero_name"].asString();
//	if (szHeroName.empty())
//	{
//		error_log("[no_hero_id]");
//		throw std::invalid_argument("[QQHallPowerfulHeroApi]qq_report_no_hero_id");
//	}
	m_params.insert(make_pair("buf2", szHeroName));

	std::string szHeroPhoto = jsonData["powerful_hero_headurl"].asString();
//	if (szHeroPhoto.empty())
//	{
//		error_log("[no_hero_photo]");
//		throw std::invalid_argument("[QQHallPowerfulHeroApi]qq_report_no_hero_photo");
//	}
	m_params.insert(make_pair("buf3", szHeroPhoto));


	if (! jsonData.isMember("powerful_hero_power"))
	{
		error_log("[no_hero_power]");
		throw std::invalid_argument("[QQHallPowerfulHeroApi]qq_report_no_hero_power");
	}
	int nHeroPower = jsonData["powerful_hero_power"].asInt();
	m_params.insert(make_pair("value5", CTrans::ITOS(nHeroPower)));
}

//---
QQHallUnionApi::QQHallUnionApi(CCgiBase& cgi, const Json::Value& jsonData)
	: QQHallCommon(cgi, jsonData)
{
	std::string szName = jsonData["union_name"].asString();
	if (szName.empty())
	{
		error_log("[no_union_name]");
		throw std::invalid_argument("[QQHallUnionApi]qq_report_no_name");
	}

	m_params.insert(make_pair("buf1", szName));

	int power = jsonData["union_power"].asInt();
	m_params.insert(make_pair("value1", CTrans::ITOS(power)));
}
//------------------------------------------------------------------------------

QQAllRankApi::QQAllRankApi(CCgiBase& cgi, const Json::Value& alldata, const Json::Value& rankData)
	: QQHallApi(cgi, alldata)
{
	if (! rankData.isArray())
	{
		error_log("[rank_params]");
		throw std::invalid_argument("[QQAllRankApi]qq_report_rank_params_error");
	}

	std::string szRankinfos;
	for (int i = 0; i < (int)rankData.size(); ++i)
	{
		if (i != 0)
		{
			szRankinfos += "|";
		}

		szRankinfos += MakeRankinfos(rankData[i]);
	}

	m_params.insert(make_pair("rankinfos", szRankinfos));
}

std::string QQAllRankApi::MakeRankinfos(const Json::Value& data)
{
	std::string result;

	int nRank = data["rank"].asInt();

	int nRankType = data["ranktype"].asInt();
	if (nRankType < 10001)
	{
		error_log("[no_valid_ranktype]");
		throw std::invalid_argument("[QQAllRankApi]qq_report_no_valid_ranktype");
	}

	result += CTrans::ITOS(nRankType) + "_" + CTrans::ITOS(nRank);

	for (int i = 1; i <= 10; i++)
	{
		std::string key = "value" + CTrans::ITOS(i);

		if (! data.isMember(key.c_str())) break;
		int nVal = data[key].asInt();
		std::string szVal = CTrans::ITOS(nVal);
		result += "_" + szVal;
	}

	return result;
}

//-----------

//--------------------------------------------------------------------------------
//@rsync_ranklist
QQRsyncRankListApi::QQRsyncRankListApi(QQReportPlatformInfo& platformInfo)
{
	m_platInfo = platformInfo;
//	//---AppId

	std::string appId = platformInfo.appId;
//	if(m_platInfo.appId)
//	{
//		error_log("[qq report][can_not_find_plathform_appid]");
//		throw std::logic_error("not_plathform_appid");
//	}
	m_params.insert(make_pair("appid", appId));
	//@end

	//---AppKey
//	std::string appKey = OpenPlatform::GetPlatform()->GetAppKey();
//	if (appKey.empty())
//	{
//		error_log("[qq report][can_not_find_plathform_key]");
//		throw std::logic_error("not_plathform_key");
//	}
	//@end

	//---ts
	unsigned ts = Time::GetGlobalTime();
	m_params.insert(make_pair("ts", CTrans::UTOS(ts)));
	//@end

	//---rsynckey md5(appid.appkey.ts)
	std::string data = appId + "." + m_platInfo.appKey + "." + CTrans::UTOS(ts);
	std::string rsynckey = Crypt::Md5Encode(data);
	m_params.insert(make_pair("rsynckey", rsynckey));
	//@end

	m_params.insert(make_pair("zoneid", CTrans::ITOS(m_platInfo.zoneId)));
}

//----
int QQRsyncUserRankBaseApi::MakeRanklists(const Json::Value& rankData)
{/*
	if (! rankData.isArray())
	{
		error_log("[qq report][rank_list_data_format_error]");
		return R_ERR_DATA;
	}

	CLogicUserBasic logicUserBasic;
	DataUserBasic dataUserBase;

	//排名
	std::string ranklists ;
	for (int i = 0; i < (int)rankData.size() && i < 100; ++i)
	{
		unsigned uid = rankData[i]["uid"].asUInt();
		int ret = 0;
		if ((ret = logicUserBasic.GetUserBasic(uid, m_platInfo.type, dataUserBase)) != 0)
		{
			error_log("[qq report][get_user_basic_error.uid=%u,playtype=%d,ret=%d]"
					, uid, m_platInfo.type,ret);
			continue;
		}

		std::map<string, string> mapVals;
		if (0 != MakeValues(rankData[i], mapVals))
		{
			error_log("[qq report][make_values.uid=%u,playtype=%d,ret=%d]"
								, uid, m_platInfo.type,ret);
			continue;
		}

		ranklists += "|";


		ranklists += dataUserBase.open_id
				+ "_" + CTrans::UTOS(uid)
				+ "_" + CTrans::ITOS(i + 1);

		if (mapVals.size() > 0)
		{
			for (int i = 1; i <= 10; i++)
			{
				std::string key = "value" + CTrans::ITOS(i);
				std::map<std::string, std::string>::iterator it = mapVals.find(key);
				if (it == mapVals.end())
				{
					break;
				}

				ranklists += "_" + it->second;
			}
		}
	}
	if (ranklists.size() > 0)
	{
		ranklists.erase(ranklists.begin());
	}
	m_params.insert(make_pair("ranklists", ranklists));
	//@end
*/
	return R_SUCCESS;
}

int QQRsyncUserRankBaseApi::MakeValues(const Json::Value& data, std::map<string, string>& map)
{
	if (! data.isMember("name")) return R_ERROR;
	map.insert(std::make_pair("value1", data["name"].asString()));

	return R_SUCCESS;
}

//---
int QQRsyncUserPowerApi::MakeValues(const Json::Value& data, std::map<string, string>& map)
{
	if (0 != QQRsyncUserRankBaseApi::MakeValues(data, map)) return R_ERROR;
	if (! data.isMember("power")) return R_ERROR;
	map.insert(std::make_pair("value2", CTrans::ITOS(data["power"].asInt())));

	return R_SUCCESS;
}

//---
int QQRsyncUserDehpApi::MakeValues(const Json::Value& data, std::map<string, string>& map)
{
	if (0 != QQRsyncUserRankBaseApi::MakeValues(data, map)) return R_ERROR;
	if (! data.isMember("dehp")) return R_ERROR;
	map.insert(std::make_pair("value2", CTrans::ITOS(data["dehp"].asInt())));

	return R_SUCCESS;
}

//---
int QQRsyncForteHeroApi::MakeValues(const Json::Value& data, std::map<string, string>& map)
{
	if (0 != QQRsyncUserRankBaseApi::MakeValues(data, map)) return R_ERROR;
	if (! data.isMember("mcity")) return R_ERROR;
	map.insert(std::make_pair("value2", CTrans::ITOS(data["mcity"].asInt())));

	return R_SUCCESS;
}

int QQRsyncGateApi::MakeValues(const Json::Value& data, std::map<string, string>& map)
{
	if (0 != QQRsyncUserRankBaseApi::MakeValues(data, map)) return R_ERROR;
	if (! data.isMember("gate")) return R_ERROR;
	map.insert(std::make_pair("value2", CTrans::ITOS(data["gate"].asInt())));

	return R_SUCCESS;
}

int QQRsyncLadderApi::MakeValues(const Json::Value& data, std::map<string, string>& map)
{
	return QQRsyncUserRankBaseApi::MakeValues(data, map);
}

//公会类的排名
int QQRsyncUnionRankBaseApi::MakeRanklists(const Json::Value& rankData)
{
	//
	if (! rankData.isArray())
	{
		error_log("[qq report][rank_list_data_format_error]");
		return R_ERR_DATA;
	}

	//PlatformType platType = OpenPlatform::GetPlatform()->GetPlatformType();
	//排名
	int rank = 1;
	std::string ranklists ;
	for (int i = 0; i < (int)rankData.size(); ++i)
	{
		unsigned aid = rankData[i]["aid"].asUInt();

		//TODO  计算公会的openid值
		std::string openid = "achive_open_id";

		ranklists += "|";


		ranklists += openid
				+ "_" + CTrans::UTOS(aid)
				+ "_" + CTrans::ITOS(rank);
	}
	if (ranklists.size() > 0)
	{
		ranklists.erase(ranklists.begin());
	}
	m_params.insert(make_pair("ranklists", ranklists));
	//@end

	return R_SUCCESS;
}

//---
int QQRsyncRankListApi::MakeRequestData(const Json::Value& data, std::string& result)
{
	if (0 == Ranktype())
	{
		error_log("[qq report][no openid]");
		return -1;
	}
	m_params.insert(make_pair("ranktype", CTrans::ITOS(Ranktype())));

	int ret = 0;
	if ((ret = MakeRanklists(data)) != R_SUCCESS)
	{
		return ret;
	}

	result.clear();
	for (std::map<string, string>::const_iterator it = m_params.begin();
			it != m_params.end();
			++it)
	{
		result +=  "&" + it->first + "=" + Crypt::UrlEncodeForTX(it->second);
	}

	if (result.size() > 0)
	{
		result.erase(result.begin());
	}

	return R_SUCCESS;
}
//@end
