#include "LogicInc.h"
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
	if(argc !=2)
	{
		cout << "need para" << endl;
		return -1;
	}

	if (!Kernel::Init(APP_CONFIG_PATH)) {
		cout << "init kernel fail" << endl;
		return -1;
	}
	Config::SetDomain(0);

	int ret = 0;
	typedef pair<unsigned,unsigned> point;
	map<point, DataBaseExtra> posbase;
	CDataBaseExtra dbBaseExtra;
	CLogicBuilding logicBuilding;
	CLogicWorld logicWorld;
	ifstream fin(argv[1]);
	if (!fin.good()) {
		cout << "open file fail" << endl;
		return -1;
	}
	while (!fin.eof() && fin.good()) {
		DataBaseExtra temp;
		fin >> temp.uid >> temp.worldpos;
		posbase[point(temp.uid,temp.worldpos)] = temp;
	}

	CDataWorldNpc *pdata;
	CLogicWorldNpc logicWorldNpc;
	NpcItem NpcItemList[1000];
	int iCount;
	pdata = logicWorldNpc.GetDataWorldNpc();
	if (pdata==NULL)
	{
		cout<<"failed"<<endl;
		return -1;
	}
	ret = pdata->GetAllNpc(NpcItemList,iCount);
	if(0 != ret)
	{
		cout<<"GetAllNpc failed "<<endl;
		return 0;
	}
	map<point, NpcItem> npcmap;
	for(int i=0;i<iCount;++i)
		npcmap[point(NpcItemList[i].uid,NpcItemList[i].worldpos)] = NpcItemList[i];

	for (map<point, DataBaseExtra>::iterator it = posbase.begin();it != posbase.end(); ++it)
	{
		if (!npcmap.count(it->first))
		{
			cout << "try del base uid=" << it->second.uid << endl;
			ret = dbBaseExtra.RemoveSubBase(it->second.uid, it->second.worldpos);
			if (ret != 0)
				cout << "del base error! uid=" << it->second.uid << " ret="<< ret << endl;
			else
				cout << "del base ok! uid=" << it->second.uid << endl;

			ret = logicBuilding.RemoveBuilding(it->second.uid,it->second.worldpos);
			if(ret != 0)
			{
				cout << "del base building error! uid=" << it->second.uid << " ret="<< ret << endl;
			}
		}
	}

	for (map<point, NpcItem>::iterator iter = npcmap.begin();iter != npcmap.end(); ++iter)
	{
		if (!IsWorldNPCUid(iter->second.uid) && !posbase.count(iter->first))
		{
			cout << "try add base uid=" << iter->second.uid << endl;
			DataBaseExtra dataBaseExtra;
			dataBaseExtra.worldpos = iter->second.worldpos;
			dataBaseExtra.occupy_time = Time::GetGlobalTime();
			dataBaseExtra.uid = iter->second.uid;
			dataBaseExtra.last_breath_time = Time::GetGlobalTime();
			dataBaseExtra.last_breath_uid = iter->second.uid;
			dataBaseExtra.being_attack_flag = NO_ATTACK;
			dataBaseExtra.protected_time = 0;
			dataBaseExtra.last_collect_ts = Time::GetGlobalTime();
			dataBaseExtra.type = iter->second.type;

			ret = dbBaseExtra.AddBaseExtra(iter->second.uid, dataBaseExtra);
			if (ret != 0)
				cout << "add base error! uid=" << iter->second.uid << " ret="<< ret << endl;
			else
				cout << "add base ok! uid=" << iter->second.uid << endl;

			int npcId = 0;
			switch(iter->second.type)
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
				cout<<"add city type error uid="<<iter->second.uid<<endl;
				continue;
			}
			CLogicNpc logicNpc;
			Json::Value result;
			logicNpc.Load(npcId,iter->second.uid,result);
			ret = logicBuilding.UpdateBuilding(iter->second.uid, iter->second.worldpos,result["baseop"],false);
			if (ret != 0)
				cout<<"add city building error uid="<<iter->second.uid<<endl;
		}

		WorldPoint worldpoint;
		ret = logicWorld.GetUserInfo(iter->second.worldpos,worldpoint);
		if(ret == 0 && worldpoint.uid != iter->second.uid)
		{
			worldpoint.uid = iter->second.uid;
			worldpoint.state = 0;
			ret = logicWorld.ReplaceUser(worldpoint,iter->second.worldpos);
			if(ret != 0)
			{
				return ret;
			}
		}
	}

	cout << "OK!" << endl;

	return 0;
}
