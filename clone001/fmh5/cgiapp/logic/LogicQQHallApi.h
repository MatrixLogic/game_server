/*
 * LogicQQHallApi.h
 *
 *  Created on: 2016-4-1
 *      Author: oulong
 */

#ifndef LOGICQQHALLAPI_H_
#define LOGICQQHALLAPI_H_

#include "LogicInc.h"
#include <stdexcept>

class QQHallApi
{
public:
	QQHallApi(CCgiBase& cgi, const Json::Value& jsonData);
	virtual ~QQHallApi() {}

	//1. openreport_login
	//2. openreport_common
	//3. openreport_quit
	//4. openreport_rank
	virtual const char* Name() const = 0;

	//构造get请求数据
	//virtual int MakeGetData(const std::string& uri, std::string& data);
	virtual int MakeRequestData(const std::string& uri, std::string& data);

	void SetFormat(const std::string& format);
	void SetUserIP(const std::string& ip);
	void SetCharset(const std::string& charset);

	std::string OpenId() const;
protected:
	int MakeSignVlaue(
			  const std::string& method
			, const std::string& uri
			, const std::map<string, string>& params
			, std::string& sign) const;
protected:
	std::map<string, string> m_params;
};

class QQUserApi : public QQHallApi
{
public:
	QQUserApi(CCgiBase& cgi, const Json::Value& jsonData)
		: QQHallApi(cgi, jsonData)
	{

	}

	virtual ~QQUserApi() {}

	/*
	 * 仅“用户 基础数据 ”中的“登录 数据 ”、“游戏 时 长”需要 分配 1、2，
	 * 其余从 11 开始 分配 。
	 * 要求 同一个 游戏， 每条数据的 ID 需要 唯
	 */
	virtual int Infotype() const = 0;

	virtual int MakeRequestData(const std::string& uri, std::string& data);
};

//登陆数据
class QQHallLoginSdk : public QQUserApi
{
public:
	QQHallLoginSdk(CCgiBase& cgi, const Json::Value& jsonData);

	const char* Name() const { return "openreport_login"; }
	int Infotype() const { return 1; }
};

//openreport_common
class QQHallCommon : public QQUserApi
{
public:
	QQHallCommon(CCgiBase& cgi, const Json::Value& jsonData)
		: QQUserApi(cgi, jsonData)
	{
	}

	virtual ~QQHallCommon() {}
	const char* Name() const { return "openreport_common"; }
};

//个性化数据
class QQHallGenerallApi : public QQHallCommon
{
public:
	QQHallGenerallApi(CCgiBase& cgi, const Json::Value& jsonData);

	int Infotype() const { return 12; }
};

//公会
//公会名称
class QQHallUnionApi : public QQHallCommon
{
public:
	QQHallUnionApi(CCgiBase& cgi, const Json::Value& jsonData);
	int Infotype() const { return 13; }
};

//--------------------------------------------------------------------------
//全量用户排名信息上报接口
class QQAllRankApi : public QQHallApi
{
public:
	QQAllRankApi(CCgiBase& cgi, const Json::Value& alldata, const Json::Value& rankData);
	virtual ~QQAllRankApi() {}

	const char* Name() const { return "openreport_rank"; }

protected:
	/**
	 * rankinfos 组织格式：
	 * rankinfos=ranktype_rank_value1_value2_value3_value4|ranktype_
	 * rank_value1_value2|ranktype_rank_value1_value2_value3_value4_valu
	 * e5_value6 （数组间用 ‘|’ 分隔，数组元素内部用 ‘_’ 分隔！）
	 */
	std::string MakeRankinfos(const Json::Value& data);
};

//----------------------------------------------------------------------------
//排名榜单接口
struct QQReportPlatformInfo
{
	int zoneId;
	std::string appId;
	std::string appKey;
	PlatformType type;

	QQReportPlatformInfo() : zoneId(0) {}
};

class QQRsyncRankListApi
{
public:
	QQRsyncRankListApi(QQReportPlatformInfo& platformInfo);
	virtual ~QQRsyncRankListApi() {}

	//构造请求数据
	int MakeRequestData(const Json::Value& data, std::string& result);

	/**
	 * 上报的排名类型标识，由产品分配，从10001开始
	 */
	virtual int Ranktype() const = 0;
protected:
	/**
	 * ranklists 组织格式：
	 * ranklists=openid_roleid_rank_value1_value2_value3_value4|open
	 * id_roleid_rank_value1_value2|openid_roleid_rank_value1_value2_val
	 * ue3_value4_value5_value6
	 */
	virtual int MakeRanklists(const Json::Value& data) = 0;
protected:
	std::map<string, string> m_params;

	QQReportPlatformInfo m_platInfo;
};

//个人排名类
class QQRsyncUserRankBaseApi : public QQRsyncRankListApi
{
public:
	QQRsyncUserRankBaseApi(QQReportPlatformInfo& platformInfo)
	 : QQRsyncRankListApi(platformInfo)
	{}

	virtual ~QQRsyncUserRankBaseApi() {}
protected:
	int MakeRanklists(const Json::Value& data);
	virtual int MakeValues(const Json::Value& data, std::map<string, string>& map);
};

//个人等级
class QQRsyncUserPowerApi : public QQRsyncUserRankBaseApi
{
public:
	QQRsyncUserPowerApi(QQReportPlatformInfo& platformInfo)
		: QQRsyncUserRankBaseApi(platformInfo) {}

	int MakeValues(const Json::Value& data, std::map<string, string>& map);

	int Ranktype() const { return 10001; }
};

//个人战力
class QQRsyncUserDehpApi : public QQRsyncUserRankBaseApi
{
public:
	QQRsyncUserDehpApi(QQReportPlatformInfo& platformInfo)
		: QQRsyncUserRankBaseApi(platformInfo) {}

	int MakeValues(const Json::Value& data, std::map<string, string>& map);

	int Ranktype() const { return 10002; }
};

//最强英雄
class QQRsyncForteHeroApi : public QQRsyncUserRankBaseApi
{
public:
	QQRsyncForteHeroApi(QQReportPlatformInfo& platformInfo)
		: QQRsyncUserRankBaseApi(platformInfo) {}

	int MakeValues(const Json::Value& data, std::map<string, string>& map);

	int Ranktype() const { return 10004; }
};

//维京战役
class QQRsyncGateApi : public QQRsyncUserRankBaseApi
{
public:
	QQRsyncGateApi(QQReportPlatformInfo& platformInfo)
		: QQRsyncUserRankBaseApi(platformInfo) {}

	int MakeValues(const Json::Value& data, std::map<string, string>& map);

	int Ranktype() const { return 10005; }
};

//竞技场
class QQRsyncLadderApi : public QQRsyncUserRankBaseApi
{
public:
	QQRsyncLadderApi(QQReportPlatformInfo& platformInfo)
		: QQRsyncUserRankBaseApi(platformInfo) {}

	int MakeValues(const Json::Value& data, std::map<string, string>& map);

	int Ranktype() const { return 10003; }
};

//--------------------------------------------------------------------

//公会排名类
class QQRsyncUnionRankBaseApi : public QQRsyncRankListApi
{
public:
	QQRsyncUnionRankBaseApi(QQReportPlatformInfo& platformInfo)
	 : QQRsyncRankListApi(platformInfo)
	{}

	virtual ~QQRsyncUnionRankBaseApi() {}
protected:
	int MakeRanklists(const Json::Value& data);
};


//
////个人等级
//QQ_RSYNC_CONSTRUCT(UserPower, 	QQRsyncUserRankBaseApi, 	10001);
////个人战力
//QQ_RSYNC_CONSTRUCT(UserDehp, 	QQRsyncUserRankBaseApi, 	10002);
////最强英雄
//QQ_RSYNC_CONSTRUCT(ForteHero, 	QQRsyncUserRankBaseApi, 	10004);
////维京战役
//QQ_RSYNC_CONSTRUCT(Gate, 		QQRsyncUserRankBaseApi, 	10005);
////公会
//QQ_RSYNC_CONSTRUCT(Union, 	QQRsyncUnionRankBaseApi, 	10005);
////公会战力
//QQ_RSYNC_CONSTRUCT(UnionDehp,	QQRsyncUnionRankBaseApi, 	10006);

#endif /* LOGICQQHALLAPI_H_ */
