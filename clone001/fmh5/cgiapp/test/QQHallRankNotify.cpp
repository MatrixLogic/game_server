/*
 * QQHallRankNotify.cpp
 *
 *  Created on: 2016-4-5
 *      Author: dawx62fac
 */

#include "LogicInc.h"
#include "Kernel.h"


//个人战力
int SyncUserDehpRank(const Json::Value& data, unsigned zoneid);
//个人等级
int SyncUserPowerRank(const Json::Value& data, unsigned zoneid);
//维京战绩
int SyncGateRank(const Json::Value& data, unsigned zoneid);
//最强英雄
int SyncForteHeroRank(const Json::Value& data, unsigned zoneid);
//公会排行
//int SyncUnionRank(const Json::Value& data, unsigned zoneid);
//公会战力
//int SyncUnionDehpRank(const Json::Value& data, unsigned zoneid);

//推送数据
int SyncRankData(const std::string& data, QQRsyncRankListApi& api);

int GetRankDataFromFile(const std::string& path, Json::Value& data);


#define DEFINE_SYNC_FUNC(NAME, API) 											\
int Sync##NAME##Rank(const Json::Value& rankData, QQReportPlatformInfo &info) 	\
{															   					\
	try																			\
	{																			\
		int ret = 0;															\
		API api(info);															\
		std::string data;														\
		if ((ret = api.MakeRequestData(rankData, data)) == 0)					\
		{																		\
			SyncRankData(data, api);											\
		}																		\
		else																	\
		{																		\
			std::cout << "[" << __FUNCTION__  << "]"							\
					  << "make get data error. ret: "							\
					  << ret << std::endl;										\
		}																		\
	}																			\
	catch (std::exception& e)													\
	{																			\
		std::cout << "[Sync" << __FUNCTION__ << "Rank]" 						\
			<< e.what() << std::endl;											\
	}																			\
																				\
	return R_SUCCESS;															\
}

DEFINE_SYNC_FUNC(UserDehp, QQRsyncUserDehpApi)
DEFINE_SYNC_FUNC(UserPower, QQRsyncUserPowerApi)
DEFINE_SYNC_FUNC(Gate, QQRsyncGateApi)
DEFINE_SYNC_FUNC(ForteHero, QQRsyncForteHeroApi)
DEFINE_SYNC_FUNC(Ladder, QQRsyncLadderApi)
//DEFINE_SYNC_FUNC(Union, QQRsyncUnionApi)
//DEFINE_SYNC_FUNC(UnionDehp, QQRsyncUnionDehpApi)


bool InitPlatformInfo(const char* path, QQReportPlatformInfo& info)
{
	CMarkupSTL xmlConf;
	if (! xmlConf.Load(path))
	{
		return false;
	}

	if(!xmlConf.FindElem("app"))
	{
		return false;
	}
	xmlConf.IntoElem();


	while(xmlConf.FindElem("param"))
	{
		string name = xmlConf.GetAttrib("name");
		if (name == "appid")
		{
			info.appId = xmlConf.GetAttrib("value");
		}
		else if (name == "appkey")
		{
			info.appKey = xmlConf.GetAttrib("value");
		}
	}

	if (info.appId.empty() || info.appKey.empty())
	{
		std::cout << "appid or appkey init failed" << std::endl;
		return false;
	}

	//---------------------------------------------------------
	//获取区号
	set<int> db;
	string serverid;
	unsigned zoneid;
	if (Config::GetValue(serverid, CONFIG_SERVER_ID))
	{
		Convert::StringToUInt(zoneid, serverid);
		unsigned domain = MainConfig::GetMergedDomain(zoneid);
		if(domain != zoneid)
		{
			cout<<domain<<"!="<<zoneid<<endl;
			return false;
		}
	}

	info.zoneId = zoneid;
	info.type = PT_UNKNOW;  //无效随便填

	return true;
}

int main(int argc, char* argv[])
{
	if (!Kernel::Init(APP_CONFIG_PATH))
	{
		cout << "init fail" << endl;
		return 0;
	}
	Config::SetDomain(0);

	//
	QQReportPlatformInfo info;
	const char* path = "/data/release/sgonline/pi/conf/platform/pengyou_platform.xml";
	if (! InitPlatformInfo(path, info))
	{
		error_log("Init platform info failed, path=%s", path);
		std::cout << "Init platform info error. path=" << path << std::endl;
		return 0;
	}

	Json::Value jsonUserRank;
	int ret = 0;
	if ((ret = GetRankDataFromFile("../webroot/data/ranklist", jsonUserRank)) == 0)
	{

		if (jsonUserRank.isMember("dehp") && jsonUserRank["dehp"].isArray())
		{
			//个人战力排名
			SyncUserDehpRank(jsonUserRank["dehp"], info);
		}
		else
		{
			std::cout << "rank dehp data error" << std::endl;
		}

		if (jsonUserRank.isMember("power") && jsonUserRank["power"].isArray())
		{
			//个人等级排名
			SyncUserPowerRank(jsonUserRank["power"], info);
		}
		else
		{
			std::cout << "rank power data error" << std::endl;
		}

		if (jsonUserRank.isMember("gate") && jsonUserRank["gate"].isArray())
		{
			//维京战役
			SyncGateRank(jsonUserRank["gate"], info);
		}
		else
		{
			std::cout << "rank gate data error" << std::endl;
		}

		if (jsonUserRank.isMember("mcity") && jsonUserRank["mcity"].isArray())
		{
			//最强英雄
			SyncForteHeroRank(jsonUserRank["mcity"], info);
		}
		else
		{
			std::cout << "rank mcity data error" << std::endl;
		}

		if (jsonUserRank.isMember("ladder") && jsonUserRank["ladder"].isArray())
		{
			//维京战役
			SyncLadderRank(jsonUserRank["ladder"], info);
		}
		else
		{
			std::cout << "rank ladder data error" << std::endl;
		}
	}
	else
	{
		std::cerr << "Read user rank data error" << std::endl;
	}

//	Json::Value jsonUnionData;
//	if ((ret = GetRankDataFromFile("../webroot/data/alliance_rank", jsonUnionData)) == 0)
//	{
//		//
//		SyncUnionRank(jsonUnionData, zoneid);
//	}
//	else
//	{
//		std::cerr << "Read union rank data error" << std::endl;
//	}
//
//	Json::Value jsonUnionDehpData;
//	if ((ret = GetRankDataFromFile("../webroot/data/alliance_rank_dehp", jsonUnionDehpData)) == 0)
//	{
//		//
//		SyncUnionDehpRank(jsonUnionDehpData, zoneid);
//	}
//	else
//	{
//		std::cerr << "Read union dehp rank data error" << std::endl;
//	}

	return 0;
}

int GetRankDataFromFile(const std::string& path, Json::Value& result)
{
	int ret = 0;
	std::string ranklist_buff;
	ret = File::Read(path,ranklist_buff);
	if (0 != ret)
	{
		cout << "Read file faile path=" << path << endl;
		return R_ERROR;
	}
	Json::Reader reader;
	if (!reader.parse(ranklist_buff, result))
	{
		cout << "Parse Json error!" << reader.getFormatedErrorMessages() << endl;
		return R_ERROR;
	}

	return R_SUCCESS;
}

int SyncRankData(const std::string& data, QQRsyncRankListApi& api)
{
	std::string url = "http://openrsync.minigame.qq.com/cgi-bin/rsync_ranklist";

	string response;
	if (!Network::HttpPostRequest(response, url, data) || response.empty())
	{
		error_log("[http request fail][url=%s,response=%s, Ranktype:%d]",
				url.c_str(), response.c_str(), api.Ranktype());
		return -1;
	}

	Json::Value value;
	if(!Json::Reader().parse(response, value))
	{
		error_log("[http request fail][url=%s,response=%s, Ranktype:%d]",
						url.c_str(), response.c_str(), api.Ranktype());
		return -1;
	}

	//debug_log("ret: %s", Json::ToString(value).c_str());

	std::cout << "QQReport Sync Ranklist" << " response: " << response.c_str() << std::endl;

	int ret = 0;
	if (!Json::GetInt(value, "result", ret) || ret != 0)
	{
		error_log("[http request fail][url=%s,response=%s, Ranktype:%d]",
						url.c_str(), response.c_str(), api.Ranktype());
		return -1;
	}

	return 0;
}




