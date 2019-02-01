#include "LogicInc.h"
#include <iostream>

#define ARCHIVE "{\"barrackQ\":{\"i0\":{\"o\":1,\"r\":0},\"i1\":{\"o\":1,\"r\":0},\"upts\":1339484213},\"baseop\":[{\"X\":-85,\"Y\":-85,\"id\":1,\"l\":6,\"status\":0,\"t\":1,\"upts\":1339484213},{\"X\":-105,\"Y\":-5,\"id\":8,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":105,\"Y\":55,\"id\":10,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":-105,\"Y\":55,\"id\":9,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":-105,\"Y\":-25,\"id\":11,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":-45,\"Y\":-205,\"id\":12,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":-65,\"Y\":-145,\"id\":13,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":65,\"Y\":-105,\"id\":14,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":85,\"Y\":-105,\"id\":15,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":105,\"Y\":-105,\"id\":16,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":105,\"Y\":-65,\"id\":17,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":105,\"Y\":-45,\"id\":18,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":105,\"Y\":-25,\"id\":19,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":105,\"Y\":-5,\"id\":20,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":25,\"Y\":-205,\"id\":22,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":105,\"Y\":15,\"id\":21,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":65,\"Y\":-125,\"id\":23,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":105,\"Y\":-85,\"id\":24,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":65,\"Y\":-145,\"id\":25,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":-105,\"Y\":35,\"id\":26,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":-25,\"Y\":-205,\"id\":27,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":-65,\"Y\":-205,\"id\":29,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":-105,\"Y\":15,\"id\":28,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":-65,\"Y\":-185,\"id\":31,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":-65,\"Y\":-165,\"id\":30,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":105,\"Y\":35,\"id\":32,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":45,\"Y\":-205,\"id\":33,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":-105,\"Y\":-85,\"id\":34,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":-65,\"Y\":-125,\"id\":35,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":-105,\"Y\":-45,\"id\":37,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":-105,\"Y\":-105,\"id\":36,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":-65,\"Y\":-105,\"id\":38,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":-105,\"Y\":-65,\"id\":39,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":65,\"Y\":-185,\"id\":40,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":-85,\"Y\":-105,\"id\":41,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":65,\"Y\":-205,\"id\":42,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":65,\"Y\":-165,\"id\":43,\"status\":0,\"t\":4,\"upts\":1339484213},{\"X\":-45,\"Y\":-185,\"id\":44,\"l\":6,\"status\":0,\"t\":11,\"upts\":1339484213}],\"buildQ\":{\"i0\":{\"o\":1,\"r\":0},\"i1\":{\"o\":1,\"r\":0},\"i2\":{\"o\":0,\"r\":0},\"i3\":{\"o\":0,\"r\":0},\"upts\":1339484213},\"equipment\":[],\"hero\":[{\"bud\":0,\"ctcd\":0,\"def\":1,\"eq\":[],\"es\":0,\"exp\":0,\"heq\":{},\"hp\":1000,\"id\":\"H3019\",\"l\":10,\"mp\":320,\"p\":[35,15,5,5],\"plock\":\"111111000000\",\"pt\":3.80,\"scd\":0,\"sid\":\"\",\"skill\":[],\"status\":0,\"tcd\":0,\"ud\":1,\"upts\":1339484218,\"zhen\":6}],\"lasttime\":1339484370,\"resources\":[{\"c\":3000,\"m\":8010},{\"c\":1000,\"m\":5510},{\"c\":1000,\"m\":5510},{\"c\":1000,\"m\":5510},{\"c\":2000,\"m\":5510}],\"skillQ\":{\"i0\":{\"o\":1,\"r\":0},\"i1\":{\"o\":1,\"r\":0},\"upts\":1339484213},\"soldierlevel\":{\"upts\":1339484213},\"stats\":{\"bng\":[\"20120612\",0],\"bpc3\":[0,0,0],\"chh\":[\"0,0\",\"0,0\",\"0,0\"],\"dg\":[\"20120612\",1,1],\"dr\":[0,0],\"drr\":[4332,4412],\"ds\":[\"\",0,0,0,0,0,0,0],\"hc\":0,\"music\":0,\"nb\":1,\"onts\":0,\"opLv\":1,\"promo\":{},\"scnt\":0,\"sound\":0,\"upts\":1339484213,\"vdrt\":0,\"vnr\":0,\"week\":2214,\"wood\":0},\"trainQ\":{\"i0\":{\"o\":1,\"r\":0},\"i1\":{\"o\":1,\"r\":0},\"upts\":1339484213},\"userid\":"

int main(int argc, char *argv[])
{
	Kernel::Init(APP_CONFIG_PATH);
	Config::SetDomain(0);

	if (argc != 3)
	{
		cout << "usage: InitFightArena id domain" << endl;
		return 1;
	}
	unsigned arenaid = CTrans::STOI(argv[1]);
	DataFightArena data;
	CLogicFightArena logicArena;
	int ret = logicArena.GetFightArena(arenaid, data);
	if (ret != 0)
	{
		cout << "get error,ret:" << ret << endl;
		return 1;
	}
	data.host = 0;
	data.level = 1;
	data.winCount = 0;
	data.challenger = 0;
	data.challengeTs = 0;
	data.breathTs = 0;
	data.preHost = 0;
	data.preChallenger = 0;
	data.prizePool = 0;
	string sname = "挑战我吧";
	strcpy(data.name, sname.c_str());
	string pic = string(argv[2]) + "/images/arena.png";
	strcpy(data.pic, pic.c_str());
	string arc = ARCHIVE;
	arc += string(argv[1]) + "}";
	strcpy(data.archive, arc.c_str());
	CDataFightArena *arenaDb = CLogicFightArena::GetCDataFightArena();
	if (!arenaDb)
	{
		cout << "init fail." << endl;
		return 1;
	}
	ret = arenaDb->SetFightArena(arenaid, data);
	if (ret != 0)
	{
		cout << "set fail,ret:" << ret << endl;
		return 1;
	}

	cout << "ok." << endl;
	return 0;
}
