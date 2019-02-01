/*
 * IOpenPlatform
 *
 *  Created on: 2011-5-16
 *      Author: dada
 */

#ifndef __IOPENPLATFORM_H__
#define __IOPENPLATFORM_H__

#include "Common.h"
#include "Kernel.h"

enum PlatformType
{
	PT_UNKNOW 		= -1,	//未知
	PT_TEST 		= 0,	//测试平台
	PT_FACEBOOK 	= 1,	//Facebook平台中文版
	PT_PENGYOU		= 2, 	//朋友平台
	PT_RENREN 		= 3,	//人人平台
	PT_QZONE 		= 4,	//QZONE平台
	PT_SINAWB 		= 5,	//新浪微博
	PT_WEIYOUXI 	= 6,	//新浪微游戏
	PT_KUAIWAN 		= 7,	//快玩平台
	PT_MIXI 		= 8,	//Mixi平台
	PT_TXWEIBOAPP 	= 9,	//腾讯微博应用平台
	PT_MOBAGE 		= 10,	//日本Yahoo!Mobage平台
	PT_TXWEIBO 		= 11,	//腾讯微博
	PT_KAIXIN 		= 12,	//开心网
	PT_FETION 		= 13,	//飞信
	PT_VN 			= 14,	//英文版自建平台
	PT_3366 		= 15,	//3366
	PT_qqgame		= 16,	//qq游戏大厅
	PT_TW			= 17, 	//台湾BSG
	PT_EN			= 18,	//FB英文版
	PT_DO			= 19,	//英文版联运
	PT_4399			= 20,	//4399联运
	PT_C9			= 21,	//C9
	PT_SOGOU		= 22,	//搜狗游戏联运平台
	PT_7477			= 23,   //7477联运
	PT_TX_C9		= 24,   //tx-c9
	PT_KW			= 25,  	//mix for PT_7477
	PT_7K			= 26,	//mix for PT_7477
	PT_360UU		= 27,	//mix for PT_7477
	PT_QD			= 28,	//mix for PT_7477
	PT_V1			= 29,	//mix for PT_7477
	PT_66YOU		= 30,	//mix for PT_7477
	PT_51IGAME		= 31,	//mix for PT_7477
	PT_HUANLEYUAN	= 32,	//mix for PT_7477
	PT_BAIDU	    = 33,	//baidu联运
	PT_51WAN	    = 34,	//
	PT_YUNQU	    = 35,	//
	PT_XUNLEI	    = 36,	//
	PT_TIEXUE	    = 37,	//
	PT_FENGHUANG    = 38,	//
	PT_7k7k		    = 39,	//
	PT_360		    = 40,	//
	PT_YLT		    = 41,	//
	PT_CYXC		    = 42,	//

	PT_MAX,
};

enum Gender
{
	GD_UNKNOW = -1,	//未知
	GD_FEMALE = 0,	//女
	GD_MALE = 1	//男
};

//开放平台用户基本信息
struct OPUserInfo
{

public:

	string OpenId;	//平台ID
	string Name;	//姓名
	int Gender;	//性别： -1 - 未知; 0 - 女; 1 - 男
	string FigureUrl;	//头像URL
	string Nation;	//国家
	string Province;	//省份
	string City;	//城市
	map<string, int> ExtraIntInfo;	//其他数值信息
	map<string, string> ExtraStrInfo;	//其他字符串信息
	Common::Login msg;

public:

	OPUserInfo()
	{
		Gender = GD_UNKNOW;
	}

	void Clear()
	{
		OpenId.clear();
		Name.clear();
		Gender = GD_UNKNOW;
		FigureUrl.clear();
		Nation.clear();
		Province.clear();
		City.clear();
		ExtraIntInfo.clear();
		ExtraStrInfo.clear();
	}

};

//开放平台好友列表
typedef vector<string> OPFriendList;

//开放平台接口
class IOpenPlatform
{

public:

	//初始化
	//输入参数：appid - 平台应用ID
	//			appkey - 平台应用KEY
	//			params - 其他参数
	//返回值：0 - 成功; 否则失败，错误信息见GetErrorMessage()
	virtual int Initialize(const string &appid, const string &appkey, map<string, string> &params);

	//获取平台类型
	virtual PlatformType GetPlatformType() {return m_type;}
	virtual void SetPlatformType(PlatformType type) {m_type = type;}
	virtual PlatformType GetPlatformByPF(){return m_type;}

	//获取平台应用ID
	virtual const string &GetAppId() { return m_appId; }

	//获取平台应用KEY
	virtual const string &GetAppKey() { return m_appKey; }

	//获取用户信息
	//输入参数：openid - 用户平台ID
	//			openkey - 用户KEY
	//输出参数：userInfo - 用户信息
	//返回值：0 - 成功; 否则失败，错误信息见GetErrorMessage()
	virtual int GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey) = 0;

	//获取应用好友列表
	//输入参数：openid - 用户平台ID
	//			openkey - 用户KEY
	//输出参数：friendList - 应用好友OpenId列表
	//返回值：0 - 成功; 否则失败，错误信息见GetErrorMessage()
	virtual int GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey) = 0;

	//获取全部好友列表
	virtual int GetFriendList(vector<OPUserInfo> &friendList, const string &openid, const string &openkey);

	virtual void SetParameter(const map<string, string> &params);

	//获取错误信息
	virtual string GetErrorMessage() = 0;

	//获取配置参数
	virtual string GetConfig(const string &name);
	virtual int Is_Login(const string &openid, const string &openkey,const string &pf);

	virtual void ReplyCharge(){}
	virtual void SetReply(string& reply){}
public:

	IOpenPlatform():m_type(PT_UNKNOW) {}
	virtual ~IOpenPlatform() {}
protected:

	string m_appId;
	string m_appKey;
	map<string, string> m_config;

private:
	PlatformType m_type;
};

#endif	//__IOPENPLATFORM_H__
