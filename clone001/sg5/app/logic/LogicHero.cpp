#include "LogicHero.h"

int CLogicHero::InitializeHero(unsigned uid)
{
	const Json::Value *pInitJson = 0;
	int ret = CLogicArchive::GetInitArchive(pInitJson);
	if (ret != 0)
		return ret;
	if (!pInitJson->isMember("hero"))
	{
		return 0;
	}
	const Json::Value &hero = (*pInitJson)["hero"];
	if (!hero.isArray())
	{
		error_log("[init hero error][uid=%u]",uid);
		DB_ERROR_RETURN_MSG("db_init_hero_error");
	}

	Json::FastWriter writer;
	CDataHero heroDB;
	unsigned size = hero.size();
	for (unsigned i = 0; i < size; i++)
	{
		unsigned id = 0;
		if (!Json::GetUInt(hero[i], "ud", id))
		{
			error_log("[init hero error][uid=%u,index=%u]",uid,i);
			DB_ERROR_RETURN_MSG("db_init_hero_error");
		}
		ret = heroDB.AddHero(uid, id, writer.write(hero[i]));
		if (ret != 0)
		{
			error_log("[AddHero fail][uid=%u,id=%u,ret=%d]",uid,id,ret);
			DB_ERROR_RETURN_MSG("db_add_hero_fail");
		}
	}
	return 0;
}

int CLogicHero::GetHero(unsigned uid, Json::Value &data)
{
	CDataHero heroDB;
	vector<DataHero> datas;
	int ret = heroDB.GetHero(uid, datas);
	if (ret != 0 && ret != R_ERR_NO_DATA)
	{
		error_log("[GetHero fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("db_get_hero_fail");
	}
	if (ret == R_ERR_NO_DATA)
	{
		data.resize(0);
		return 0;
	}
	Json::Reader reader;
	data.resize(datas.size());
	for (unsigned i = 0; i < datas.size(); i++)
	{
		if (!reader.parse(datas[i].data, data[i]))
		{
			error_log("[parse fail][uid=%u,i=%u]",uid,i);
		}
	}
	return 0;
}

int CLogicHero::GetHeroUsed(unsigned uid, Json::Value &data)
{
	CDataHero heroDB;
	vector<DataHero> datas;
	int ret = heroDB.GetHero(uid, datas);
	if (ret != 0 && ret != R_ERR_NO_DATA)
	{
		error_log("[GetHero fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("db_get_hero_fail");
	}
	if (ret == R_ERR_NO_DATA)
	{
		data.resize(0);
		return 0;
	}
	Json::Reader reader;
	//data.resize(datas.size());
	for (unsigned i = 0; i < datas.size(); i++)
	{
		Json::Value json;
		if (!reader.parse(datas[i].data, json))
		{
			error_log("[parse fail][uid=%u,i=%u]",uid,i);
			continue;
		}

		int bud = 0;
		int def = 0;
		Json::GetInt(json,"bud",bud);
		Json::GetInt(json,"def",def);

		if(bud != 0 || def != 0 ){
			data.append(json);
		}

	}
	return 0;
}

int CLogicHero::UpdateHero(unsigned uid, const Json::Value &data)
{
	if (!data.isArray())
	{
		error_log("[hero type error][uid=%u,type=%d]",uid,data.type());
		DATA_ERROR_RETURN_MSG("data_hero_error");
	}
	int ret = 0;
	Json::FastWriter writer;
	Json::Reader reader;
	CDataHero heroDB;
	map<unsigned, string> oldHeros;
	ret = heroDB.GetHero(uid, oldHeros);
	if (ret != 0 && ret != R_ERR_NO_DATA)
	{
		error_log("[GetHero fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("get_hero_fail");
	}
	map<unsigned, string>::const_iterator oldItr;
	for (unsigned i = 0; i < data.size(); i++)
	{
		unsigned id = 0;
		string heroId;
		if (!Json::GetUInt(data[i], "ud", id) || !Json::GetString(data[i], "id", heroId))
		{
			error_log("[hero data error][uid=%u,index=%u,id=%u]",uid,i,id);
			DATA_ERROR_RETURN_MSG("data_hero_error");
		}
		bool needLog = false;
		string code;
		if (Json::GetString(data[i], "code", code))
		{
			needLog = true;
		}
		bool bAdd = false;
		oldItr = oldHeros.find(id);
		if (oldItr != oldHeros.end())
		{
			Json::Value oldHero;
			string oldHeroId;
			if (reader.parse(oldItr->second, oldHero) && Json::GetString(oldHero, "id", oldHeroId))
			{
				int l = 0, oldl = 0;
				Json::GetInt(data[i], "l", l);
				Json::GetInt(oldHero, "l", oldl);
				if (l < oldl)
				{
					error_log("[hero level error][uid=%u,id=%u,l=%d,oldl=%d]",uid,id,l,oldl);
					DATA_ERROR_RETURN_MSG("hero_level_error");
				}
				if (heroId != oldHeroId)
				{
					error_log("[hero id error][uid=%u,id=%u,heroid=%s,oldhid=%s]",uid,id,heroId.c_str(),oldHeroId.c_str());
					DATA_ERROR_RETURN_MSG("hero_type_error");
				}
			}

		}
		else
		{
			bAdd = true;
			needLog = true;
			int l = 0;
			Json::GetInt(data[i], "l", l);
			if (l != 1)
			{
				error_log("[add hero level error][uid=%u,id=%u,l=%d]",uid,id,l);
				DATA_ERROR_RETURN_MSG("add_hero_level_error");
			}
		}

		string heroData = writer.write(data[i]);
		if (data[i].isMember("status")
				&& data[i]["status"].isIntegral()
				&& data[i]["status"].asInt() == 9)
		{
			bAdd = false;
			needLog = true;
			ret = heroDB.RemoveHero(uid, id);
			if (ret != 0)
			{
				error_log("[RemoveHero fail][uid=%u,id=%u,ret=%d]",uid,id,ret);
				DB_ERROR_RETURN_MSG("db_rm_hero_fail");
			}
		}
		else
		{
			ret = heroDB.ReplaceHero(uid, id, writer.write(data[i]));
			if (ret != 0)
			{
				error_log("[ReplaceHero fail][uid=%u,id=%u,ret=%d]",uid,id,ret);
				DB_ERROR_RETURN_MSG("db_set_hero_fail");
			}
		}
		if (needLog)
		{
			HERO_LOG("uid=%u,id=%u,heroid=%s,act=%s,code=%s,data=%s",uid,id,heroId.c_str(),
					(bAdd?"add":"del"),code.c_str(),heroData.c_str());
		}
	}
	return 0;
}


int CLogicHero::ImportHero(unsigned uid, const Json::Value &data)
{
	if (!data.isArray())
	{
		error_log("[hero type error][uid=%u,type=%d]",uid,data.type());
		DATA_ERROR_RETURN_MSG("data_hero_error");
	}
	int ret = 0;
	Json::FastWriter writer;
	Json::Reader reader;
	CDataHero heroDB;
	map<unsigned, string> oldHeros;
	ret = heroDB.GetHero(uid, oldHeros);
	if (ret != 0 && ret != R_ERR_NO_DATA)
	{
		error_log("[GetHero fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("get_hero_fail");
	}
	map<unsigned, string>::const_iterator oldItr;
	for (unsigned i = 0; i < data.size(); i++)
	{
		unsigned id = 0;
		string heroId;
		if (!Json::GetUInt(data[i], "ud", id) || !Json::GetString(data[i], "id", heroId))
		{
			error_log("[hero data error][uid=%u,index=%u,id=%u]",uid,i,id);
			DATA_ERROR_RETURN_MSG("data_hero_error");
		}
		bool needLog = false;
		string code;
		if (Json::GetString(data[i], "code", code))
		{
			needLog = true;
		}
		bool bAdd = false;
		oldItr = oldHeros.find(id);
		if (oldItr != oldHeros.end())
		{
			Json::Value oldHero;
			string oldHeroId;
			if (reader.parse(oldItr->second, oldHero) && Json::GetString(oldHero, "id", oldHeroId))
			{
				int l = 0, oldl = 0;
				Json::GetInt(data[i], "l", l);
				Json::GetInt(oldHero, "l", oldl);
//				if (l < oldl)
//				{
//					error_log("[hero level error][uid=%u,id=%u,l=%d,oldl=%d]",uid,id,l,oldl);
//					DATA_ERROR_RETURN_MSG("hero_level_error");
//				}
				if (heroId != oldHeroId)
				{
					error_log("[hero id error][uid=%u,id=%u,heroid=%s,oldhid=%s]",uid,id,heroId.c_str(),oldHeroId.c_str());
					DATA_ERROR_RETURN_MSG("hero_type_error");
				}
			}

		}
		else
		{
			bAdd = true;
			needLog = true;
//			int l = 0;
//			Json::GetInt(data[i], "l", l);
//			if (l != 1)
//			{
//				error_log("[add hero level error][uid=%u,id=%u,l=%d]",uid,id,l);
//				DATA_ERROR_RETURN_MSG("add_hero_level_error");
//			}
		}

		string heroData = writer.write(data[i]);
		if (data[i].isMember("status")
				&& data[i]["status"].isIntegral()
				&& data[i]["status"].asInt() == 9)
		{
			bAdd = false;
			needLog = true;
			ret = heroDB.RemoveHero(uid, id);
			if (ret != 0)
			{
				error_log("[RemoveHero fail][uid=%u,id=%u,ret=%d]",uid,id,ret);
				DB_ERROR_RETURN_MSG("db_rm_hero_fail");
			}
		}
		else
		{
			ret = heroDB.ReplaceHero(uid, id, writer.write(data[i]));
			if (ret != 0)
			{
				error_log("[ReplaceHero fail][uid=%u,id=%u,ret=%d]",uid,id,ret);
				DB_ERROR_RETURN_MSG("db_set_hero_fail");
			}
		}
		if (needLog)
		{
			HERO_LOG("uid=%u,id=%u,heroid=%s,act=%s,code=%s,data=%s",uid,id,heroId.c_str(),
					(bAdd?"add":"del"),code.c_str(),heroData.c_str());
		}
	}
	return 0;
}

int CLogicHero::GetHeroAccredited(unsigned uid, unsigned world_pos,Json::Value &data)
{
	CDataHero heroDB;
	vector<DataHero> datas;
	unsigned worldpos = 0;
	CLogicUser logicUser;
	unsigned mainPos;
	int ret = heroDB.GetHero(uid, datas);
	if (!(ret == 0 || ret == R_ERR_NO_DATA))
	{
		error_log("[GetHero fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("db_get_hero_fail");
	}
	if (ret == R_ERR_NO_DATA)
	{
		data.resize(0);
		return 0;
	}
	Json::Reader reader;
	for (unsigned i = 0; i < datas.size(); i++)
	{
		Json::Value json;
		if (!reader.parse(datas[i].data, json))
		{
			error_log("[parse fail][uid=%u,i=%u]",uid,i);
			continue;
		}

		int bud = 0;
		int def = 0;
		Json::GetUInt(json,"world_pos",worldpos);
		if( world_pos == worldpos ){
			data.append(json["id"]);
		}

	}
	return 0;
}
