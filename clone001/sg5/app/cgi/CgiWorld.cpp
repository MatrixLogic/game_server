/*
 * CgiWorld.cpp
 *
 *  Created on: 2012-8-21
 *      Author: Administrator
 */

#include "LogicInc.h"
class CCgiWorld: public CCgiBase {
public:
	CCgiWorld() : CCgiBase("world")
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
	}

	CGI_ACTION_MAP_BEGIN
	CGI_SET_ACTION_MAP("move", Move)
	CGI_SET_ACTION_MAP("getuserinfo", GetUserInfo)
	CGI_SET_ACTION_MAP("setuserinfo", SetUserInfo)
	CGI_SET_ACTION_MAP("occupycity", OccupyCity)
	CGI_SET_ACTION_MAP("movecity", MoveCity)
	CGI_SET_ACTION_MAP("getcollect", GetCollect)
	CGI_SET_ACTION_MAP("addcollect", AddCollect)
	CGI_SET_ACTION_MAP("delcollect", DelCollect)
	CGI_SET_ACTION_MAP("getworldnpc", GetWorldNpc)
	CGI_SET_ACTION_MAP("querywnpcdrwd", QueryWorldNpcReward)

	CGI_ACTION_MAP_END

	~CCgiWorld()
	{
	}

	int Move()
	{
		int ret = 0;
		CLogicUser logicUser;
		DataUser user;
		ret = logicUser.GetUserLimit(m_uid,user);
		if(0 != ret){
			error_log("get user limit failed %u",m_uid);
			return ret;
		}
		if(user.mainpos == 0)
		{
			CLogicWorld logicWorld;
			WorldPoint worldPoint;
			worldPoint.uid = m_uid;
			worldPoint.map_flag = USER_MAIN_CITY;
			ret = logicWorld.AddUser(worldPoint,user.mainpos);
			if(0 != ret)
			{
				error_log("add world user failed %u",m_uid);
				return ret;
			}
			ret = logicUser.SetMainpos(m_uid,user.mainpos);
			if(0 != ret){
				error_log("set user limit failed %u",m_uid);
				return ret;
			}
		}
		CLogicWorld clogicworld;
		if(!m_data["world_blocks"].isArray())
		{
			return R_ERR_PARAM;
		}
		ret = clogicworld.Move(m_data["world_blocks"],m_jsonResult["world_pos"]);

		return ret;
	}
	int GetUserInfo()
	{
		int ret = 0;
		unsigned world_pos;
		string signature;
		string figure_url;
		CLogicUserBasic clogicUserBasic;
		CLogicWorld logicWorld;
		WorldPoint worldPoint;
		CLogicUser lguser;
		DataUser user;
		CLogicHero logicUser;
		Json::Value hero;
		CLogicAlliance logicAlliance;
		DataAlliance alliance;
		Json::Reader reader;
		Json::Value baseExtraData;
		CLogicBaseExtra logicBaseExtra;
		DataBaseExtra baseExtra;
		Json::Value stats;
		Json::Value soldiers;
		bool isMainPos = false;
		if(!Json::GetUInt(m_data,"world_pos",world_pos))
		{
			return R_ERR_PARAM;
		}

		string name;
		ret = logicWorld.GetUserInfo(world_pos,worldPoint);
		if(ret != 0)
		{
			error_log(" get world user info failed,uid = %u",worldPoint.uid);
			return ret;
		}
		if(IsValidUid(worldPoint.uid))
		{
			ret = clogicUserBasic.GetWorldUserInfo(worldPoint.uid,OpenPlatform::GetType(),signature,figure_url,name);
			if(ret != 0)
			{
				error_log(" get world user info failed,uid = %u",worldPoint.uid);
				return ret;
			}
			ret = lguser.GetUser(worldPoint.uid,user);
			if(ret != 0)
			{
				error_log(" get  user info failed,uid = %u",worldPoint.uid);
				return ret;
			}
			reader.parse(user.user_stat,stats);
			if (IsAllianceId(user.alliance_id))
			{
				ret = logicAlliance.GetAlliance(user.alliance_id, alliance);
				if(ret != 0)
					return ret;

			}
			else
			{
				alliance.alliance_id = ALLIANCE_ID_NULL;
				alliance.flag = 0;
			}

			ret = logicBaseExtra.GetBaseExtra(worldPoint.uid,world_pos,baseExtra);
			if(!(0 == ret || ret == R_ERR_NO_DATA))
			{
				error_log(" get BaseExtra hero failed,uid = %u",worldPoint.uid);
				return ret;
			}
			m_jsonResult["world_user"]["lastcollectts"] = baseExtra.last_collect_ts;
			unsigned now = Time::GetGlobalTime();

			// 获取派驻武将
			//if(baseExtra.)
//			if(Time::GetGlobalTime() < baseExtra.protected_time)
//			{
//				LOGIC_ERROR_RETURN_MSG("ban_attack_world_24_protected");
//			}
			if (IsOnlineUser(baseExtra.last_breath_time) && baseExtra.uid == baseExtra.last_breath_uid && m_uid !=  baseExtra.uid)
			{
				//result["online"] = 1;
				//APT_BAN_BEING_ATTACK		= 7,	//对方被攻击
				m_jsonResult["world_user"]["attackpermit"] = APT_BAN_ONLINE;
				//return 0;
			}else if (baseExtra.being_attack_flag == BEING_ATTACK && IsBeingAttacked(baseExtra.last_breath_time) && baseExtra.uid != baseExtra.last_breath_uid)
			{
				//LOGIC_ERROR_RETURN_MSG("being_attacked");
				m_jsonResult["world_user"]["attackpermit"] = APT_BAN_BEING_ATTACK;
			}else{
				m_jsonResult["world_user"]["attackpermit"] = APT_ALLOW;
			}
			ret = logicUser.GetHeroAccredited(worldPoint.uid,world_pos,hero);
			if(ret != 0)
			{
				error_log(" get accredited hero failed,uid = %u",worldPoint.uid);
				return ret;
			}

			CLogicBuilding logicBuilding;
			Json::Value building;
			isMainPos = user.mainpos == world_pos ?true:false;
			ret = logicBuilding.GetBuilding(worldPoint.uid,world_pos,building,isMainPos);
			int i = 0;
			int iCount = building.size();
			for(; i < iCount;i++)
			{
				if((building[i]["t"].asInt() == 6 || building[i]["t"].asInt() == 17) && building[i].isMember("m") && !building[i]["m"].empty())
				{
					soldiers.append(building[i]["m"]);
				}
			}

		}
		if(IsValidUid(worldPoint.uid))
		{
			m_jsonResult["world_user"]["protected_time"] = baseExtra.protected_time;
			m_jsonResult["world_user"]["occupy_time"] = baseExtra.occupy_time;
			m_jsonResult["world_user"]["aid"] = alliance.alliance_id;
			m_jsonResult["world_user"]["alliance_name"] = alliance.name;
			m_jsonResult["world_user"]["signature"] = signature;
			m_jsonResult["world_user"]["pic"] = figure_url;
			m_jsonResult["world_user"]["power"] = stats["power"];
			m_jsonResult["world_user"]["mcity"] = stats["mcity"];
		}else{
			m_jsonResult["world_user"]["protected_time"] = 0;
			m_jsonResult["world_user"]["occupy_time"] = 0;
			m_jsonResult["world_user"]["aid"] = 0;
			m_jsonResult["world_user"]["alliance_name"] = "";
			m_jsonResult["world_user"]["signature"] = "";
			m_jsonResult["world_user"]["pic"] = "";
		}
		m_jsonResult["world_user"]["userid"] = worldPoint.uid;
		m_jsonResult["world_user"]["m"] = soldiers;
		m_jsonResult["world_user"]["AccreditedHeros"] = hero;
		m_jsonResult["world_user"]["cityname"] = worldPoint.cityname;
		m_jsonResult["world_user"]["name"] = name;
		m_jsonResult["world_user"]["range"] = worldPoint.influence_zone;
		m_jsonResult["world_user"]["defense"] = Convert::UInt64ToString(worldPoint.defensive_power);
		return 0;
	}
	int SetUserInfo()
	{
		int ret = 0;
		int unsigned uid;
		int unsigned world_pos;
		string signature;
		string figure_url;
		CLogicUserBasic clogicUserBasic;
		CLogicWorld logicWorld;
		if(!Json::GetString(m_data,"signature",signature))
		{
			return R_ERR_PARAM;
		}
		uid = m_uid;
		if(!Json::GetUInt(m_data,"world_pos",world_pos) )
		{
			return R_ERR_PARAM;
		}
		ret = clogicUserBasic.SetWorldUserInfo(uid,OpenPlatform::GetType(),signature);
		if(ret != 0)
		{
			error_log(" set world user info failed,uid = %u",uid);
			return ret;
		}
		return 0;

	}
	int OccupyCity() {
		int ret = 0;
		int unsigned uid;
		int unsigned world_pos;
		unsigned occupy;
		unsigned bNPC;
		string figure_url;
		Json::Value userFlag;
		Json::Reader reader;
		string sUserFlag;
		DataUser dataUser;
		CLogicWorld logicWorld;
		CLogicUpdates logicUpdates;
		Json::Value updates;
		CLogicBaseExtra logicBaseExtra;
		Json::Value worldPosList;
		WorldPoint worldpoint;
		vector<DataBaseExtra> vDataBaseExtra;
		DataBaseExtra defendBaseExtra;
		CLogicBuilding logicBuilding;
		CLogicUserBasic logicUserBasic;
		DataUserBasic userBasic;
		CLogicWorldNpc logicWorldNpc;
		int world_type = 0;
		unsigned defenduid = 0;
		uid = m_uid;
		if (!Json::GetUInt(m_data, "userid", defenduid))
		{
			error_log("miss defend uid = %u",defenduid);
			return R_ERR_PARAM;
		}
		if (!Json::GetUInt(m_data, "world_pos", world_pos))
		{
			error_log("miss world_pos uid = %u",uid);
			return R_ERR_PARAM;
		}
		if (!Json::GetInt(m_data, "type", world_type))
		{
			error_log("miss occupy uid = %u",uid);
			return R_ERR_PARAM;
		}
		if (!Json::GetUInt(m_data, "occupy", occupy))
		{
			error_log("miss occupy uid = %u",uid);
			return R_ERR_PARAM;
		}
		if (!Json::GetUInt(m_data, "bNPC", bNPC))
		{
			error_log("miss bNPC uid = %u",uid);
			return R_ERR_PARAM;
		}

		NpcItem defendnpc;
		memset(&defendnpc,0x00,sizeof(defendnpc));
		defendnpc.worldpos = world_pos;
		ret = logicWorldNpc.GetNpc(defendnpc);
		if(ret != 0)
			return ret;
		if((bNPC && !IsWorldNPCUid(defendnpc.uid)) || ((!bNPC && IsWorldNPCUid(defendnpc.uid))))
		{
			LOGIC_ERROR_RETURN_MSG("had been occupied");
		}

		//删除失守玩家的数据
		if(!IsWorldNPCUid(defenduid))
		{
			// 删除原有分基地信息
			ret = logicBaseExtra.GetBaseExtra(defenduid,world_pos,defendBaseExtra);
			{
				if(0 != ret )
				{
					error_log("get_defend_user_sub_base_failed uid = %u, worldpos =%u",uid,world_pos);
					DB_ERROR_RETURN_MSG("get_defend_user_sub_base_failed");
				}
			}
			ret = logicBuilding.RemoveBuilding(defenduid,world_pos);
			if(ret != 0)
				return ret;

			ret = logicBaseExtra.RemoveSubBase(defenduid,world_pos);
			if(0 != ret )
				return ret;
		}

		ret = logicUserBasic.GetUserBasicLimit(uid,OpenPlatform::GetType(),userBasic);
		int npcId = 0;
		switch(world_type)
		{
		case JUN_CITY:
			npcId = 70001;
			break;
		case DU_CITY:
			npcId = 70003;
			break;
		case ZHOU_CITY:
			npcId = 70002;
			break;
		default:
			error_log("occupy city type error uid = %u, world_type =%u",uid,world_type);
			return R_ERR_LOGIC;
		}
		//处理占领玩家数据
		if(occupy)//占领新城，放弃已有
		{
			//先删除自己的同类型城市
			vector<DataBaseExtra> mybase;
			logicBaseExtra.GetBaseExtraOnType(uid,world_type,mybase);
			for(vector<DataBaseExtra>::iterator it=mybase.begin();it!=mybase.end();++it)
			{
				// world map
				WorldPoint temp;
				temp.uid = npcId;
				temp.map_flag = world_type;
				temp.protect_time = 0;
				temp.state = 0;
				temp.last_collect_ts = (*it).last_collect_ts;
				ret = logicWorld.ReplaceUser(temp,(*it).worldpos);
				if(ret != 0)
					return ret;

				//world npc
				NpcItem npc;
				memset(&npc,0x00,sizeof(npc));
				npc.uid = npcId;
				npc.SetType(world_type);
				npc.worldpos = (*it).worldpos;
				memcpy(npc.cityName,temp.cityname,sizeof(npc.cityName) - 1);
				memset(npc.name,0,sizeof(npc.name) - 1);
				ret = logicWorldNpc.ReplaceNpc(npc);
				if(ret != 0)
					return ret;

				//building
				ret = logicBuilding.RemoveBuilding(uid,(*it).worldpos);
				if(ret != 0)
					return ret;

				//base extra
				ret = logicBaseExtra.RemoveSubBase(uid,(*it).worldpos);
				if(0 != ret )
					return ret;
			}

			//再添加新的城市
			//building
			CLogicNpc logicNpc;
			Json::Value result;
			logicNpc.Load(npcId,uid,result);
			ret = logicBuilding.UpdateBuilding(uid, world_pos,result["baseop"],false);
			if (ret != 0)
				return ret;

			logicWorld.GetUserInfo(world_pos,worldpoint);
			// base extra
			DataBaseExtra dataBaseExtra;
			dataBaseExtra.worldpos = world_pos;
			dataBaseExtra.occupy_time = Time::GetGlobalTime();
			dataBaseExtra.uid = uid;
			dataBaseExtra.last_breath_time = Time::GetGlobalTime();
			dataBaseExtra.last_breath_uid = uid;
			dataBaseExtra.being_attack_flag = NO_ATTACK;
			dataBaseExtra.protected_time = Time::GetGlobalTime() + WORLD_OCCUPT_NEW_PROTECT_TIME;
			if(!IsWorldNPCUid(defenduid))
			{
				dataBaseExtra.last_collect_ts = defendBaseExtra.last_collect_ts;
			}else{
				dataBaseExtra.last_collect_ts = worldpoint.last_collect_ts;
			}
			dataBaseExtra.type = world_type;

			ret = logicBaseExtra.AddBaseExtra(uid,dataBaseExtra);
			if(ret != 0)
				return ret;

			// world map
			worldpoint.uid = uid;
			worldpoint.map_flag = world_type;
			worldpoint.protect_time = Time::GetGlobalTime() + WORLD_OCCUPT_NEW_PROTECT_TIME;
			worldpoint.state = 0;
			worldpoint.last_collect_ts = 0;
			ret = logicWorld.ReplaceUser(worldpoint,world_pos);
			if(ret != 0)
				return ret;

			//world npc
			NpcItem npc;
			memset(&npc,0x00,sizeof(npc));
			npc.uid = uid;
			npc.SetType(world_type);
			npc.worldpos = world_pos;
			memcpy(npc.cityName,worldpoint.cityname,sizeof(npc.cityName) - 1);
			memcpy(npc.name,userBasic.name.c_str(),sizeof(npc.name) - 1);
			ret = logicWorldNpc.ReplaceNpc(npc);
			if(ret != 0)
				return ret;
		}
		else//放弃占领
		{
			//将新城赋给npc
			logicWorld.GetUserInfo(world_pos,worldpoint);
			// world map
			worldpoint.uid = npcId;
			worldpoint.map_flag = world_type;
			worldpoint.protect_time = 0;
			worldpoint.state = 0;
			worldpoint.last_collect_ts = defendBaseExtra.last_collect_ts;
			ret = logicWorld.ReplaceUser(worldpoint,world_pos);
			if(ret != 0)
				return ret;

			//world npc
			NpcItem npc;
			memset(&npc,0x00,sizeof(npc));
			npc.uid = npcId;
			npc.SetType(world_type);
			npc.worldpos = world_pos;
			memcpy(npc.cityName,worldpoint.cityname,sizeof(npc.cityName) - 1);
			memset(npc.name,0,sizeof(npc.name) - 1);
			ret = logicWorldNpc.ReplaceNpc(npc);
			if(ret != 0)
				return ret;
		}

		//通知失守玩家
		updates.resize(1);
		updates[(unsigned)0]["s"] = "SUBBASELOSS";
		updates[(unsigned)0]["name"] = userBasic.name;
		updates[(unsigned)0]["uid"] = uid;
		updates[(unsigned)0]["ts"] = Time::GetGlobalTime();
		updates[(unsigned)0]["world_pos"] = world_pos;
		logicUpdates.AddUpdates(defenduid,updates);

		if(m_data["world_blocks"].isArray())
		{
			logicWorld.Move(m_data["world_blocks"],m_jsonResult["world_pos"]);
		}
		if(IsValidUid(defenduid))
		{
			WORLD_ATTACK_LOG("occupycity|%u|%u|%u|%d|%d|",uid,defenduid,world_pos,world_type,Time::GetGlobalTime() - defendBaseExtra.occupy_time);
		}else{
			WORLD_ATTACK_LOG("occupycity|%u|%u|%u|%d|%d|",uid,defenduid,world_pos,world_type,0);
		}
		return 0;
	}

	int MoveCity()
	{
		unsigned world_pos;
		CLogicUser logicUser;
		unsigned oldmainpos;
		int ret = logicUser.GetMainpos(m_uid,oldmainpos);
		if(ret != 0 || oldmainpos == 0)
		{
			error_log("get main pos failed uid=%d",m_uid);
			return ret;
		}
		if (!Json::GetUInt(m_data, "world_pos", world_pos)) {

			error_log("miss world_pos uid = %u",m_uid);
			return R_ERR_PARAM;
		}
		CLogicWorld logicWorld;
		ret = logicWorld.MoveCity(m_uid,oldmainpos,world_pos);
		if(ret != 0)
		{
			error_log("move city failed uid=%d",m_uid);
			return ret;
		}
		m_jsonResult["new_pos"] = world_pos;
		if(m_data["world_blocks"].isArray())
		{
			logicWorld.Move(m_data["world_blocks"],m_jsonResult["world_pos"]);
		}

		return 0;
	}
	int GetCollect()
	{
		CLogicUserData logicUserData;
		logicUserData.GetWorldposCollect(m_uid,m_jsonResult["collect"]);
		return 0;
	}
	int AddCollect()
	{
		unsigned world_pos;
		if (!Json::GetUInt(m_data, "world_pos", world_pos)) {

			error_log("miss world_pos uid = %u",m_uid);
			return R_ERR_PARAM;
		}
		CLogicUserData logicUserData;
		logicUserData.AddWorldposCollect(m_uid,world_pos);
		return 0;

	}
	int DelCollect()
	{
		if(!m_data.isMember("world_pos"))
		{
			return R_ERR_PARAM;
		}
		CLogicUserData logicUserData;
		unsigned world_pos = m_data["world_pos"].asUInt();
		logicUserData.DelWorldposCollect(m_uid,world_pos);
		return 0;

	}
	int GetWorldNpc()
	{
		CLogicWorldNpc logicWorldNpc;
		int ret = logicWorldNpc.GetAllNpc(m_jsonResult);
		return ret;
	}
	int QueryWorldNpcReward()
	{
		Json::Value value;
		DataBaseExtra baseExtra;
		CLogicBaseExtra logicBaseExtra;
		vector<DataBaseExtra> vBaseExtra;
		int ret = logicBaseExtra.GetBaseExtra(m_uid,vBaseExtra);
		if(ret != 0)
		{
			return ret;
		}
		for(int i = 0; i < vBaseExtra.size();i++)
		{
			if(vBaseExtra[i].last_collect_ts == 0)
			{
				value["worldpos"] = vBaseExtra[i].worldpos;
				value["occupy_time"] = vBaseExtra[i].occupy_time;
				value["type"] = vBaseExtra[i].type;
				m_jsonResult["wnpcdrwd"].append(value);
			}

		}
		return 0;
	}
private:

};

CGI_MAIN(CCgiWorld)






















