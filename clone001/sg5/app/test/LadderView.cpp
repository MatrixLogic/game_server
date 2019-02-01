#include "LogicInc.h"

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		cout << "usage: " << argv[0] << " path" << endl;
		return 1;
	}

	Config::SetDomain(0);
	int semgroup = 0;
	int semserver = 0;
	Config::GetDB(semserver);
	CShareMemory shm;
	if (!shm.CreateOrOpen(argv[1], sizeof(LadderData), SEM_ID(sem_ladder,semgroup,semserver)))
	{
		cout << "init fail" << endl;
		return 1;
	}
	CAutoLock lock(&shm, true);
	LadderData *pdata = (LadderData *)shm.GetAddress();
	if (!pdata)
	{
		cout << "null addr" << endl;
		return 1;
	}
	for (int i = 0; i < LADDER_ITEM_NUMBER; i++)
	{
		cout << (i+1) << ":\t" << pdata->ladder[i].uid << " "
				<< (int)(pdata->ladder[i].level) << " " << pdata->ladder[i].name
				<< " " << pdata->ladder[i].pic << endl;
	}
	return 0;
}
