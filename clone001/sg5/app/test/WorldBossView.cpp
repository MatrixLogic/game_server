#include "LogicInc.h"

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		cout << "usage: " << argv[0] << " path" << endl;
		return 1;
	}

	Config::SetDomain(0);
	int semgroup = 100;
	int semserver = 0;
	Config::GetDB(semserver);
	CShareMemory shm;
	if (!shm.CreateOrOpen(argv[1], sizeof(WorldBossData), SEM_ID(sem_worldboss,semgroup,semserver)))
	{
		cout << "init fail" << endl;
		return 1;
	}
	CAutoLock lock(&shm, true);
	WorldBossData *pdata = (WorldBossData *)shm.GetAddress();
	if (!pdata)
	{
		cout << "null addr" << endl;
		return 1;
	}
	cout<<"fullBlood="<<pdata->fullBlood<<endl;
	cout<<"blood="<<pdata->blood<<endl;
	cout<<"ts="<<pdata->ts<<endl;
	cout<<"standts="<<pdata->standts<<endl;
	cout<<"challNum="<<pdata->challNum<<endl;
	cout<<"showNum="<<pdata->showNum<<endl;

	for (int i = 0; i < WORLDBOSS_CHALLENGERS_MAX; i++)
	{
		cout<<(i+1)<<":\t"<<pdata->challengers[i].uid<<" "<<pdata->challengers[i].name<<" "<<pdata->challengers[i].damage<<endl;
	}
	return 0;
}
