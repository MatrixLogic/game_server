#include "LogicInc.h"
#include <iostream>

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		cout << "usage: " << argv[1] << " path" << endl;
		return 1;
	}
	CDataDragonball dragonball;
	if (dragonball.Init(argv[1]) != 0)
	{
		cout << "init fail" << endl;
		return 1;
	}
	DragonballData balls;
	if (dragonball.ViewAllDragonball(balls) != 0)
	{
		cout << "view balls fail" << endl;
		return 1;
	}
	for (int i = 0; i < 7; i++)
	{
		cout << (i+1) << ": " << balls.ball[i].holderUid << "," << balls.ball[i].allianceId << "," << balls.ball[i].name << "," <<balls.ball[i].aname << "," << balls.ball[i].ts << endl;
	}

	cout << "alliance:" << endl;
	cout << balls.aBall.allianceId << "," << balls.aBall.count << "," << balls.aBall.ts << endl;

	return 0;
}
