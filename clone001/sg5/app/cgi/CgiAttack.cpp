#include "LogicInc.h"

class CCgiAttack : public CCgiBase
{
public:

	CCgiAttack() : CCgiBase("attack")
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
	CGI_SET_ACTION_MAP("gethistory", GetAttackHistory)
	CGI_SET_ACTION_MAP("getlog", GetAttackLog)
	CGI_SET_ACTION_MAP("getdetail", GetAttackDetail)
	CGI_SET_ACTION_MAP("getvedio", GetVedio)
	CGI_ACTION_MAP_END

	int GetAttackHistory()
	{
		vector<DataAttack> attacks;
		CLogicAttack logicAttack;
		unsigned world_pos;
		if(!Json::GetUInt(m_data,"world_pos",world_pos))
		{
			return R_ERR_PARAM;
		}
		// add by aaron 20121112
		CLogicUser logicUser;
		unsigned mainpos;
		int result = logicUser.GetMainpos(m_uid,mainpos);
		if(result != 0)
		{
			return result;
		}
		bool isMainPos = (mainpos == world_pos)?true:false;
		// add by aaron 20121112
		if(isMainPos)
		{
			result = logicAttack.GetAttackHistory(m_uid,attacks);

		}else{
			result = logicAttack.GetAttackHistory(m_uid,world_pos, attacks,isMainPos);
		}
		if(result != 0)
		{
			return result;
		}
		Json::Value &attacksData = m_jsonResult["attacks"];
		attacksData.resize(attacks.size());
		for (unsigned i = 0; i < attacksData.size(); i++)
		{
			attacksData[i]["attack_id"] = Convert::UInt64ToString(attacks[i].attack_id);
			attacksData[i]["a_uid"] = attacks[i].attack_uid;
			attacksData[i]["d_uid"] = attacks[i].defence_uid;
			attacksData[i]["world_pos"] = attacks[i].worldpos;

			attacksData[i]["a_pt"] = attacks[i].attack_platform;
			attacksData[i]["a_name"] = attacks[i].attack_user_name;
			attacksData[i]["d_pt"] = attacks[i].defence_platform;
			attacksData[i]["d_name"] = attacks[i].defence_user_name;
			attacksData[i]["st"] = attacks[i].start_time;
			attacksData[i]["et"] = attacks[i].end_time;
			attacksData[i]["r1"] = attacks[i].r1_loot;
			attacksData[i]["r2"] = attacks[i].r2_loot;
			attacksData[i]["r3"] = attacks[i].r3_loot;
			attacksData[i]["r4"] = attacks[i].r4_loot;
			attacksData[i]["r5"] = attacks[i].r5_loot;
			attacksData[i]["hasvedio"] = (attacks[i].flag == AF_VEDIO ? 1:0);
		}

		CGI_SEND_LOG("action=gethistory&uid=%u&attacks=%u", m_uid, attacksData.size());
		return R_SUCCESS;
	}

	int GetAttackLog()
	{
		uint64_t attackId;
		if(!Json::GetUInt64(m_data, "attackid", attackId))
		{
			PARAM_ERROR_RETURN_MSG("check_attackid_fail");
		}
		CLogicAttack logicAttack;
		DataAttack attack;
		int result = logicAttack.GetAttack(attackId, attack);
		if(result != 0)
			return result;

		m_jsonResult["attack_id"] = Convert::UInt64ToString(attackId);
		m_jsonResult["log"] = attack.log;

		CGI_SEND_LOG("action=getlog&uid=%u&attack_id=%llu", m_uid, attackId);
		return R_SUCCESS;
	}

	int GetAttackDetail()
	{
		uint64_t attackId;
		if(!Json::GetUInt64(m_data, "attackid", attackId))
		{
			PARAM_ERROR_RETURN_MSG("check_attackid_fail");
		}
		CLogicAttack logicAttack;
		DataAttack attack;
		int result = logicAttack.GetAttack(attackId,  attack);
		if(result != 0)
			return result;

		m_jsonResult["attack_id"] = Convert::UInt64ToString(attack.attack_id);
		m_jsonResult["a_uid"] = attack.attack_uid;
		m_jsonResult["d_uid"] = attack.defence_uid;
		m_jsonResult["a_pt"] = attack.attack_platform;
		m_jsonResult["a_name"] = attack.attack_user_name;
		m_jsonResult["d_pt"] = attack.defence_platform;
		m_jsonResult["d_name"] = attack.defence_user_name;
		m_jsonResult["st"] = attack.start_time;
		m_jsonResult["et"] = attack.end_time;
		m_jsonResult["r1"] = attack.r1_loot;
		m_jsonResult["r2"] = attack.r2_loot;
		m_jsonResult["r3"] = attack.r3_loot;
		m_jsonResult["r4"] = attack.r4_loot;
		m_jsonResult["r5"] = attack.r5_loot;
		m_jsonResult["hasvedio"] = (attack.flag == AF_VEDIO ? 1:0);
		Json::Reader reader;
		if (attack.log.empty())
			attack.log = "[]";
		reader.parse(attack.log, m_jsonResult["log"]);

		CGI_SEND_LOG("action=getdetail&uid=%u&attack_id=%llu", m_uid, attackId);
		return R_SUCCESS;
	}

	int GetVedio()
	{
		uint64_t attackId;
		if(!Json::GetUInt64(m_data, "attackid", attackId))
		{
			PARAM_ERROR_RETURN_MSG("check_attackid_fail");
		}

		string vedioStr;
		CLogicAttack logicAttack;
		int ret = logicAttack.GetVedio(attackId, vedioStr);
		if (ret != 0)
			return ret;
		m_jsonResult["vedio"] = vedioStr;

		CGI_SEND_LOG("action=getvedio&uid=%u&attack_id=%llu", m_uid, attackId);
		return R_SUCCESS;
	}
};

CGI_MAIN(CCgiAttack)
