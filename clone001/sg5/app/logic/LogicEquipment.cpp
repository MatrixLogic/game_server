#include "LogicEquipment.h"

int CLogicEquipment::InitializeEquipment(unsigned uid)
{
	const Json::Value *pInitJson = 0;
	int ret = CLogicArchive::GetInitArchive(pInitJson);
	if (ret != 0)
		return ret;
	if (!pInitJson->isMember("equipment"))
	{
		return 0;
	}
	const Json::Value &equipment = (*pInitJson)["equipment"];
	if (!equipment.isArray())
	{
		error_log("[init equipment error][uid=%u]",uid);
		DB_ERROR_RETURN_MSG("db_init_equipment_error");
	}

	Json::FastWriter writer;
	CDataEquipment equipmentDB;
	unsigned size = equipment.size();
	for (unsigned i = 0; i < size; i++)
	{
		unsigned id = 0;
		if (!Json::GetUInt(equipment[i], "ud", id))
		{
			error_log("[init equipment error][uid=%u,index=%u]",uid,i);
			DB_ERROR_RETURN_MSG("db_init_equipment_error");
		}
		ret = equipmentDB.AddEquipment(uid, id, writer.write(equipment[i]));
		if (ret != 0)
		{
			error_log("[AddEquipment fail][uid=%u,id=%u,ret=%d]",uid,id,ret);
			DB_ERROR_RETURN_MSG("db_add_equipment_fail");
		}
	}
	return 0;
}

int CLogicEquipment::GetEquipment(unsigned uid, Json::Value &data, bool sync)
{
	CDataEquipment equipDB;
	vector<DataEquipment> datas;
	int ret = equipDB.GetEquipment(uid, datas);
	if (ret != 0 && ret != R_ERR_NO_DATA)
	{
		error_log("[db_get_equipment_fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("db_get_equipment_fail");
	}
	unsigned maxid = 0;
	Json::Reader reader;
	data.resize(datas.size());
	for (unsigned i = 0; i < datas.size(); i++)
	{
		if (datas[i].id > maxid)
			maxid = datas[i].id;
		if (!reader.parse(datas[i].data, data[i]))
		{
			error_log("[parse fail][uid=%u,id=%u,i=%u]",uid,datas[i].id,i);
		}
	}
	maxid++;
	if (!sync)
		return 0;
	CLogicInventory logicInventory;
	if (datas.size() >= 2000)
	{
		logicInventory.RemoveAllItem(uid);
		return 0;
	}
	Json::FastWriter writer;
	map<string, unsigned> items;
	ret = logicInventory.GetItems(uid, items);
	if (ret == 0 && items.size() > 0)
	{
		logicInventory.RemoveAllItem(uid);
		map<string, unsigned>::const_iterator it = items.begin();
		for (; it != items.end(); it++)
		{
			int eqid = CTrans::STOI(it->first);
			if (eqid <= 0)
			{
				error_log("[equipment id error][uid=%u,seqid=%s]",uid,(it->first).c_str());
				continue;
			}
			for (unsigned count = 0; count < it->second; count++)
			{
				Json::Value json;
				json["ud"] = maxid;
				json["id"] = eqid;
				ret = equipDB.AddEquipment(uid, maxid, writer.write(json));
				if (ret != 0)
				{
					error_log("[AddEquipment fail][uid=%u,ud=%u,id=%d,ret=%d]",uid,maxid,eqid,ret);
					continue;
				}
				maxid++;
				data.append(json);
			}
		}
	}

	return 0;
}

int CLogicEquipment::UpdateEquipment(unsigned uid, const Json::Value &data)
{
	if (!data.isArray())
	{
		error_log("[equipment type error][uid=%u,type=%d]",uid,data.type());
		DATA_ERROR_RETURN_MSG("data_equipment_error");
	}
	int ret = 0;
	Json::FastWriter writer;
	Json::Reader reader;
	CDataEquipment equipDB;
	vector<DataEquipment> oldeq;
	ret = equipDB.GetEquipment(uid, oldeq);
	for (unsigned i = 0; i < data.size(); i++)
	{
		unsigned id = 0;
		int eqid = 0;
		if (!Json::GetInt(data[i], "id", eqid))
		{
			string seqid;
			if (Json::GetString(data[i], "id", seqid))
			{
				eqid = CTrans::STOI(seqid);
			}
		}
		if (!Json::GetUInt(data[i], "ud", id) || eqid <= 0)
		{
			error_log("[equipment data error][uid=%u,index=%u,id=%u]",uid,i,id);
			DATA_ERROR_RETURN_MSG("data_equipment_error");
		}

		bool needLog = false;
		bool bDel = false;
		int oldcount = 0, newcount = 0;

		Json::GetInt(data[i], "count", newcount);
		string code;
		if (Json::GetString(data[i], "code", code))
		{
			needLog = true;
		}

		vector<DataEquipment>::iterator it;
		for(it=oldeq.begin();it!=oldeq.end();++it)
		{
			if((*it).id == id)
			{
				Json::Value old;
				int oldeqid = 0;
				if(reader.parse((*it).data,old) && Json::GetInt(old,"id",oldeqid))
				{
					if(oldeqid != eqid)
					{
						error_log("[equip type error][uid=%u,id=%u,oldeqid=%d,eqid=%d]",uid,id,oldeqid,eqid);
						DATA_ERROR_RETURN_MSG("equip_type_error");
					}

					Json::GetInt(old,"count",oldcount);
					if(oldcount > newcount)
					{
						needLog = true;
						bDel = true;
					}
					else if(oldcount < newcount)
					{
						needLog = true;
					}
				}
			}
		}
		if(it == oldeq.end())
			needLog = true;

		string equipData = writer.write(data[i]);

		if (data[i].isMember("status")
				&& data[i]["status"].isIntegral()
				&& data[i]["status"].asInt() == 9)
		{
			bDel = true;
			ret = equipDB.RemoveEquipment(uid, id);
			if (ret != 0)
			{
				error_log("[RemoveEquipment fail][uid=%u,id=%u,ret=%d]",uid,id,ret);
				DB_ERROR_RETURN_MSG("db_rm_equipment_fail");
			}
		}
		else
		{
			ret = equipDB.ReplaceEquipment(uid, id, writer.write(data[i]));
			if (ret != 0)
			{
				error_log("[ReplaceEquipment fail][uid=%u,id=%u,ret=%d]",uid,id,ret);
				DB_ERROR_RETURN_MSG("db_set_equipment_fail");
			}
		}

		if (needLog)
		{
			EQUIPMENT_LOG("uid=%u,id=%u,eqid=%d,act=%s,chg=%d,count=%d,code=%s,data=%s",uid,id,eqid,
					(bDel?"del":"unk"),newcount-oldcount,newcount,code.c_str(),equipData.c_str());
		}
	}
	return 0;
}

int CLogicEquipment::GetEquipmentUsed(unsigned uid, Json::Value &data)
{
	CDataEquipment equipDB;
	vector<DataEquipment> datas;
	int ret = equipDB.GetEquipment(uid, datas);
	if (ret != 0 && ret != R_ERR_NO_DATA)
	{
		error_log("[GetEquipment fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("db_get_equipment_fail");
	}
	Json::Reader reader;
	data.resize(0);
	for (unsigned i = 0; i < datas.size(); i++)
	{
		Json::Value json;
		if (!reader.parse(datas[i].data, json))
		{
			error_log("[parse fail][uid=%u,id=%u,i=%u]",uid,datas[i].id,i);
			continue;
		}
		int status = 0;
		if (Json::GetInt(json, "status", status) && status != 0 && status != 9)
		{
			data.append(json);
		}
	}
	return 0;
}
