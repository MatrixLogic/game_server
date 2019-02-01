/*
 * CgiPointsRank.cpp
 *
 *  Created on: 2012-10-17
 *      Author: Administrator
 */

#include "LogicInc.h"
#include "LogicQQPay.h"
#include "stdlib.h"
#define EXCHANGE_CODE_NOT_EXISIT 3
#define EXCHANGE_CODE_USED 2
#define USER_EXCHANGED_BEFORE 1
#define EXCHANGED_SUCCESS 0;
#define EXCHANGE_CODE_OUT_OF_DATE 4
class CCgiActivity : public CCgiBase
{
public:

	CCgiActivity() : CCgiBase("activity")
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
		SetFeature(CF_COMPRESS_OUT);
		srand((int)time(NULL));       //初始随机数
	}

	CGI_ACTION_MAP_BEGIN
	CGI_SET_ACTION_MAP("getqixijiedata", GetQixijieData)
	CGI_SET_ACTION_MAP("updateqixijiedata", UpdateQixijieData)
	CGI_SET_ACTION_MAP("gethundreddaysactivityrank", GetHundredDaysActivityRank)
	CGI_SET_ACTION_MAP("getpointsrank", GetPointsRank)
	CGI_SET_ACTION_MAP("updatepointsrank", UpdatePointsRank)
	CGI_SET_ACTION_MAP("exchange", Exchange)
	CGI_SET_ACTION_MAP("getbarbairanrank", GetBarbairanRank)
	CGI_SET_ACTION_MAP("ranklist", GetTopTenUser)
	CGI_SET_ACTION_MAP("viewMysticalShop", ViewMysticalShop)
	CGI_SET_ACTION_MAP("buyMysticalShop", BuyMysticalShop)
	CGI_SET_ACTION_MAP("getgodfb", GetGodFB)
	CGI_SET_ACTION_MAP("viewboat", ViewBoat)
	CGI_SET_ACTION_MAP("getboat", GetBoat)
	CGI_SET_ACTION_MAP("setboat", SetBoat)
	CGI_SET_ACTION_MAP("updateboatlog", UpdateBoatAttackLog)
	CGI_SET_ACTION_MAP("qqpanel", ReqQQPanel)
	CGI_ACTION_MAP_END

	int GetQixijieData()
	{
		int ret = 0;
		CLogicQixijie qixiRank;
		int unsigned uid;
		if (!Json::GetUInt(m_data, "userid", uid) || uid == 0)
		{
			uid = m_uid;
		}
	    ret = qixiRank.GetQixiActivityRankJson(uid,m_jsonResult["qixiactivityRank"]);
		if(ret != 0)
		{
			error_log("get ranklist failed uid = %u",uid);
			return ret;
		}
		CGI_SEND_LOG("action=getqixiactivityrank&uid=%u", m_uid);
		return R_SUCCESS;

	}

	int UpdateQixijieData()
	{
		int ret = 0;
		CLogicQixijie qixiRank;
		int unsigned uid;
		if (!Json::GetUInt(m_data, "userid", uid) || uid == 0 )
		{
			uid = m_uid;
		}
		ret = qixiRank.UpdateQixiActivityRank(uid,m_data);
		if(ret != 0)
		{
			error_log("update ranklist failed uid = %u",uid);
			return ret;
		}
		ret = qixiRank.GetQixiActivityRankJson(uid,m_jsonResult["qixiactivityRank"]);
		if(ret != 0)
		{
			error_log("get ranklist failed uid = %u",uid);
			return ret;
		}
		int prize = m_jsonResult["qixiactivityRank"][15]["PrizesResults"].asInt();
		unsigned now = Time::GetGlobalTime();
		string result = Json::ToString(m_jsonResult["qixiactivityRank"]);
		//EQUIPMENT_LOG("uid=%u,prizeid=%u,ts=%d,act=%s,code=%s,data=%s",uid,prize,now,"qixijie","Cgiactivity",result.c_str());
		CGI_SEND_LOG("action=updateqixiactivityrank&uid=%u", m_uid);
		return R_SUCCESS;

	}

	int GetHundredDaysActivityRank()
	{
		int ret = 0;
		CLogicHundredDaysActivityRank pointsRank;

		 ret = pointsRank.GetHundredDaysActivityRankJson(m_uid,m_jsonResult["hdaysactivityRank"]);
		 if(ret != 0)
		 {
			 error_log("get ranklist failed uid = %u",m_uid);
			 return ret;
		 }
		CGI_SEND_LOG("action=gethundreddaysactivityrank&uid=%u", m_uid);
		return R_SUCCESS;

	}


	int GetPointsRank()
	{
		int ret = 0;
		CLogicPointsRank pointsRank;
		int unsigned uid;
		if (!Json::GetUInt(m_data, "userid", uid) || uid == 0)
		{
			uid = m_uid;
		}
		 ret = pointsRank.GetPointsRankJson(uid,m_jsonResult["pointsRank"]);
		 if(ret != 0)
		 {
			 error_log("get ranklist failed uid = %u",uid);
			 return ret;
		 }
		CGI_SEND_LOG("action=getpointsrank&uid=%u", m_uid);
		return R_SUCCESS;
	}

	int UpdatePointsRank()
	{
		int unsigned  uid;
		DataPoints points;
		CLogicUserBasic logicUserBasic;
		DataUserBasic dataUserBasic;
		int ret = 0;
		CLogicPointsRank pointsRank;
		if(!Json::GetUInt(m_data, "userid", uid) || uid == 0)
		{
			uid = m_uid;
		}
		if(!Json::GetUInt(m_data, "points", points.points))
		{
			PARAM_ERROR_RETURN_MSG("check_points_fail");
		}
		logicUserBasic.GetUserBasicLimit(uid,OpenPlatform::GetType(),dataUserBasic);
		points.uid = uid;
		memcpy(points.name,dataUserBasic.name.c_str(),sizeof(points.name) - 1);
		ret = pointsRank.UpdatePointsRank(uid,points);
		if(ret != 0)
		{
			return ret;
		}
		CGI_SEND_LOG("action=updatepointsrank&uid=%u", m_uid);
		return R_SUCCESS;
	}
	int Exchange()
	{
		unsigned uid;
		string exchangeCode;
		if(!Json::GetUInt(m_data, "userid", uid) || uid == 0)
		{
			uid = m_uid;
		}
		if(!Json::GetString(m_data, "exchangecode", exchangeCode) || exchangeCode.empty())
		{
			//uid = m_uid;
			return R_ERR_PARAM;
		}

		int length = exchangeCode.length();
		if (length <= CODE_SIZE)
		{
			error_log("[exchangeCode length error][length=%d]",length);
			return R_ERR_PARAM;
		}
		int type_len = exchangeCode.length() - CODE_SIZE;
		if (type_len <= 0)
			return R_ERR_PARAM;
		int iType = atoi(exchangeCode.substr(0,type_len).c_str());
		CLogicExchangeCode logicChangeCode;
		DataExchangeCode dataExchangeCode;
		CLogicPromotionMapping logicPromotionMapping;
		DataPromotionMapping dataPromotionMapping;
		dataExchangeCode.code = exchangeCode.substr(type_len,length - type_len);
		//error_log("[exchangeCode][length=%d,typelen=%d,code=%s]",length,type_len,dataExchangeCode.code.c_str());
		unsigned now = Time::GetGlobalTime();
		int ret = logicChangeCode.GetExchangeCode(dataExchangeCode);
		if(ret != 0)
		{
			m_jsonResult["result"] = EXCHANGE_CODE_NOT_EXISIT;
			return 0;
		}
//		if(dataExchangeCode.deadline < now)
//		{
//			m_jsonResult["result"] = EXCHANGE_CODE_OUT_OF_DATE;
//			return 0;
//
//		}
		dataPromotionMapping.uid = uid;
		dataPromotionMapping.useflag = 0;
		ret = logicPromotionMapping.GetPromotionMapping(dataPromotionMapping);
		if(!(ret == R_SUCCESS || ret == R_ERR_NO_DATA))
		{
			return ret;
		}
		if(dataExchangeCode.usetime != 0)  // 该兑换码已被使用
		{
			m_jsonResult["result"] = EXCHANGE_CODE_USED;
			return 0;
		}
		if(ret == R_SUCCESS)   // 推广用户
		{
			if(dataPromotionMapping.useflag & iType)    //用户以领取过奖励
			{
				m_jsonResult["result"] = USER_EXCHANGED_BEFORE;
				return 0;
			}
			dataPromotionMapping.useflag |= iType;
			dataPromotionMapping.usetime = Time::GetGlobalTime();
			logicPromotionMapping.SetPromotionMapping(dataPromotionMapping);
		}else{                      //非推广用户
			dataPromotionMapping.useflag |= iType; //登记领取类型标记
			dataPromotionMapping.usetime = now;
			dataPromotionMapping.pid = 0;
			logicPromotionMapping.AddPromotionMapping(dataPromotionMapping);
		}
		dataExchangeCode.usetime = now;
		dataExchangeCode.uid = uid;
		logicChangeCode.SetExchangeCode(dataExchangeCode);
		m_jsonResult["result"] = EXCHANGED_SUCCESS;
		m_jsonResult["type"] = iType;
		return 0;
	}
	int GetBarbairanRank()
	{

		unsigned uid = 0;
		if (!Json::GetUInt(m_data, "userid", uid) || uid == 0)
		{
			uid = m_uid;
		}
		CLogicUser logicUser;
		DataUser user;
		int ret = logicUser.GetUserLimit(uid,user);
		if(0 != ret)
		{
			error_log("get user limit failed uid=%u, ret=%d",uid, ret);
			return ret;
		}
		int i = 0;
		CLogicBarbarianInvasion  logicBarbarianInvasion;
		DataPlayersPoints playersRank;
		memset(&playersRank,0x00,sizeof(DataPlayersPoints));
		DataTeamItem teamRank[512];
		memset(teamRank,0x00,sizeof(teamRank));
		int teamNum = 0;
		ret = logicBarbarianInvasion.GetTeamRank(teamRank,teamNum);
		if(0 != ret)
		{
			error_log("get TeamPlayersRank failed uid=%u",uid);
			return ret;
		}
		if(IsAllianceId(user.alliance_id))
		{
			ret = logicBarbarianInvasion.GetTeamPlayersRank(playersRank,user.alliance_id);
			if(0 != ret)
			{
				error_log("get TeamPlayersRank failed uid=%u",uid);
				return ret;
			}
			for(i = 0; i < playersRank.playerNum; i++)
			{
				m_jsonResult["barbairanplayersrank"][i]["uid"]= playersRank.players[i].uid;
				m_jsonResult["barbairanplayersrank"][i]["points"]= playersRank.players[i].points;
				m_jsonResult["barbairanplayersrank"][i]["name"]= playersRank.players[i].name;
			}
		}
		for(i = 0; i < teamNum; i++)
		{
			m_jsonResult["barbairanteamrank"][i]["aid"]= teamRank[i].aid;
			m_jsonResult["barbairanteamrank"][i]["teampoints"]= teamRank[i].teamPoints;
			m_jsonResult["barbairanteamrank"][i]["name"]= teamRank[i].aname;
		}
		return 0;
	}

	int GetTopTenUser()
	{
		CLogicTopTenUser logicTopTenUser;
		DataTopTenUser topUser;
		int ret = logicTopTenUser.GetInfo(topUser);
		if(0 != ret)
		{
			return ret;
		}
		Json::Value data;
		Json::Value dataLevel = Json::Value(Json::arrayValue);
		Json::Value dataDehp = Json::Value(Json::arrayValue);
		Json::Value dataLoot = Json::Value(Json::arrayValue);
		CDataUserBasic dbUser;
		string userName;
		CLogicHero logicHero;
		Json::Value heroData;
		for (int i=0; i<10; i++)
		{
			ret = dbUser.GetUserName(topUser.userLevel[i].uid, OpenPlatform::GetType(), userName);
			if(0 == ret)
			{
				Json::Value &tempData = dataLevel[dataLevel.size()];
				tempData["uid"] = topUser.userLevel[i].uid;
				tempData["lvl"] = topUser.userLevel[i].level;
				tempData["name"] = userName;
			}
			userName.clear();
			ret = dbUser.GetUserName(topUser.userPlunder[i].uid, OpenPlatform::GetType(), userName);
			if (0 == ret)
			{
				Json::Value &tempData = dataLoot[dataLoot.size()];
				tempData["uid"] = topUser.userPlunder[i].uid;
				tempData["loot"] = topUser.userPlunder[i].plunder;
				tempData["name"] = userName;
			}
			heroData.clear();
			ret = logicHero.GetHero(topUser.heroPower[i].uid, heroData);
			if (0 == ret)
			{
				ret = dbUser.GetUserName(topUser.heroPower[i].uid, OpenPlatform::GetType(), userName);
				if (0 == ret)
				{
					Json::Value &tempData = dataDehp[dataDehp.size()];
					tempData["uid"] = topUser.heroPower[i].uid;
					tempData["power"] = topUser.heroPower[i].power;
					tempData["name"] = userName;
					for (int j=0; j<heroData.size(); ++j)
					{
						int def = 0;
						if (Json::GetInt(heroData[j], "def", def) && (1 == def))
						{
							tempData["id"] = heroData[j]["id"];
						}
					}
				}
			}
			userName.clear();
		}
		m_jsonResult["loot"] = dataLoot;
		m_jsonResult["hero"] = dataDehp;
		m_jsonResult["user"] = dataLevel;
		return 0;
	}
	int ViewMysticalShop()
	{

		unsigned uid = 0;
		if (!Json::GetUInt(m_data, "userid", uid) || uid == 0)
		{
			uid = m_uid;
		}

		CLogicUser logicUser;
		DataUser user;
		int ret = logicUser.GetUserLimit(uid,user);
		if(0 != ret)
		{
			error_log("get user limit failed uid=%u, ret=%d",uid, ret);
			return ret;
		}
		int i = 0;
		CLogicMysticalShop logicMysticalShop;
		DataEquipItem dataEquipItem[512];
		DataEquipSells dataEquipSells[512];
		memset(&dataEquipItem,0x00,sizeof(DataEquipItem));
		memset(&dataEquipSells,0x00,sizeof(DataEquipItem));

		// 初始化商店
		ret = logicMysticalShop.InitMysticalShop();
		int equipNum = 0;
		ret = logicMysticalShop.GetTopEquipNum(dataEquipItem, equipNum);
		if(0 != ret)
		{
			error_log("get TopEquipNum failed uid=%u",uid);
			return ret;
		}
		string rangeTime = Config::GetValue(CONFIG_MYSTICALSHOP_RANGE_TS);
		Json::Value value;
		Json::Reader().parse(rangeTime, value);
		m_jsonResult["mysticalTime"] = value;

		//int equipNum = 3;
		for(i = 0; i < equipNum; i++)
		{

			m_jsonResult["mysticalshop"][i]["eid"]= dataEquipItem[i].eid;
			m_jsonResult["mysticalshop"][i]["price"]= dataEquipItem[i].price;
			m_jsonResult["mysticalshop"][i]["num"]= dataEquipItem[i].nums;
		}

		// 添加信息栏
		int buyNums;
		ret = logicMysticalShop.GetInfobuys(dataEquipSells, buyNums);
		for (int i=0; i<buyNums; i++)
		{
			m_jsonResult["mysticalBuys"][i]["uid"] = dataEquipSells[i].uid;
			m_jsonResult["mysticalBuys"][i]["eid"] = dataEquipSells[i].buy_Equip.eid;
			m_jsonResult["mysticalBuys"][i]["time"] = dataEquipSells[i].buy_time;
			m_jsonResult["mysticalBuys"][i]["price"] = dataEquipSells[i].buy_Equip.price;
			m_jsonResult["mysticalBuys"][i]["name"] = dataEquipSells[i].name;
		}

		return 0;
	}

	int BuyMysticalShop()
	{
		unsigned uid = 0;
		unsigned eid = 0;
		string name = "";

		Json::GetUInt(m_data, "uid", uid);
		Json::GetUInt(m_data, "eid", eid);
		Json::GetString(m_data, "name", name);

		CLogicMysticalShop logicMysticalShop;
		int ret = logicMysticalShop.PayMysticalEquip(uid, name, eid);
		if (0  == ret)
		{
			m_jsonResult["mysticalshopBuyType"] = 0;
		}
		else
		{
			m_jsonResult["mysticalshopBuyType"] = -1;
		}
		return 0;
	}
	int GetGodFB()
	{
		int ret = 0;
		unsigned uid = 0;
		if(!Json::GetUInt(m_data, "uid", uid))
		{
			PARAM_ERROR_RETURN_MSG("uid");
		}
		CLogicWeapon logicWeapon;
		ret = logicWeapon.GetWeapon(uid, m_jsonResult);
		if (0 != ret)
		{
			return ret;
		}
		return 0;
	}

	int GetBoat()
	{
		int ret = 0;
		CLogicRiver logicRiver;
		unsigned userid;
		if(Json::GetUInt(m_data, "userid", userid))
		{
			Json::Value boats;
			logicRiver.GetShipJson(userid, boats);
			if (0 != ret)
				return ret;
			m_jsonResult["boats"][(unsigned)0] = boats;
			CGI_SEND_LOG("action=GetBoat&uid=%u&userid=%u",m_uid,userid);
		}
		else
		{
			ret = logicRiver.GetAllShipJson(m_jsonResult["boats"]);
			if (0 != ret)
				return ret;
		}
		return 0;
	}

	int SetBoat()
	{
		int ret = 0;
		unsigned uid;
		if(!Json::GetUInt(m_data, "uid", uid))
		{
			PARAM_ERROR_RETURN_MSG("uid");
		}
		if (!m_data.isMember("data"))
		{
			PARAM_ERROR_RETURN_MSG("data");
		}

		CLogicRiver logicRiver;
		ret = logicRiver.SetShip(uid, m_data["data"]);
		if (0 != ret)
		{
			return ret;
		}
		unsigned userid = 0;
		Json::GetUInt(m_data["data"], "uid", userid);
		CGI_SEND_LOG("action=SetBoat&uid=%u&userid=%u",m_uid,userid);
		return 0;
	}

	int ViewBoat()
	{
		int ret = 0;
		unsigned uid;
		if(!Json::GetUInt(m_data, "uid", uid))
		{
			PARAM_ERROR_RETURN_MSG("uid");
		}

		CLogicRiver logicRiver;
		ret = logicRiver.GetAllShipJson(m_jsonResult["boats"]);
		if (0 != ret)
		{
			return ret;
		}

		ret = logicRiver.GetAttackLog(uid, m_jsonResult["log"]);
		if (0 != ret)
		{
			return ret;
		}
		return 0;
	}

	int UpdateBoatAttackLog()
	{
		int ret = 0;
		unsigned uid;
		if(!Json::GetUInt(m_data, "uid", uid))
		{
			PARAM_ERROR_RETURN_MSG("uid");
		}

		if (!m_data.isMember("log"))
		{
			PARAM_ERROR_RETURN_MSG("log");
		}
		CLogicRiver logicRiver;
		ret = logicRiver.SetAttackLog(uid, m_data["log"]);
		if (0 != ret)
		{
			return ret;
		}
		CGI_SEND_LOG("action=UpdateBoatAttackLog&uid=%u&userid=%u",m_uid,uid);
		return 0;
	}

	int ReqQQPanel()
	{
		string openid;
		if(!Json::GetString(m_data, "openid", openid))
		{
			PARAM_ERROR_RETURN();
		}
		string openkey;
		if(!Json::GetString(m_data, "openkey", openkey))
		{
			PARAM_ERROR_RETURN();
		}
		string pf = "qzone";
		string pfkey;
		if(!Json::GetString(m_data, "pfkey", pfkey))
		{
			PARAM_ERROR_RETURN();
		}
		string appid;
		if(!Json::GetString(m_data, "appid", appid))
		{
			PARAM_ERROR_RETURN();
		}

		CLogicQQPay logicQQPay;
		int ret = logicQQPay.ReqQQPanel(pf, pfkey, openid, openkey, m_ipstr, appid);
		if(ret == 0)
			m_jsonResult["qqpanel"] = 1;
		else
			m_jsonResult["qqpanel"] = 0;
		CGI_SEND_LOG("action=ReqQQPanel&uid=%u&ret=%u",m_uid,ret);
		return 0;
	}
};

CGI_MAIN(CCgiActivity)



