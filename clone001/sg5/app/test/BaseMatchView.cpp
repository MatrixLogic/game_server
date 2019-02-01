#include "LogicInc.h"

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		cout << "usage: " << argv[0] << " path all|apply|regular|playoff" << endl;
		return 1;
	}

	Config::SetDomain(0);
	int semgroup = 0;
	int semserver = 0;
	Config::GetDB(semserver);
	CShareMemory shm;
	if (!shm.CreateOrOpen(argv[1], sizeof(BaseMatchData), SEM_ID(sem_basematch,semgroup,semserver)))
	{
		cout << "init fail" << endl;
		return 1;
	}
	CAutoLock lock(&shm, true);
	BaseMatchData *pdata = (BaseMatchData *)shm.GetAddress();
	if (!pdata)
	{
		cout << "null addr" << endl;
		return 1;
	}
	cout << "instance_ndx:" << pdata->instance_ndx << endl;
	cout << "stage:" << (int)pdata->stage << endl;
	string action = argv[2];
	if (action == "all" || action == "apply")
	{
		cout << "numOfApply:" << pdata->numOfApply << endl;
		cout << "teams:" << endl;
		for (unsigned i = 0; i < pdata->numOfApply; i++)
		{
			cout << "-" << (i+1) << "-[aid:" << (pdata->teams[i]).aid << "(" << (pdata->teams[i]).aname << "),number:" << (pdata->teams[i]).number
					<< ",players:";
			for (unsigned j = 0; j < (pdata->teams[i]).number; j++)
			{
				if (j > 0)
				{
					cout << ",";
				}
				cout << (pdata->teams[i]).players[j].uid << "(" << (pdata->teams[i]).players[j].instanceid << ";" << (pdata->teams[i]).players[j].name << ")";
			}
			cout << "]" << endl;
		}
		cout << "----------------------------------------------------" << endl;
	}
	if (action == "all" || action == "regular")
	{
		cout << "numOfReg:" << pdata->numOfReg << endl;
		cout << "rteams:" << endl;
		for (unsigned i = 0; i < pdata->numOfReg; i++)
		{
			cout << "-" << (i+1) << "-[aid:" << (pdata->rteams[i]).aid << "(" << (pdata->rteams[i]).aname << "),damage:" << (pdata->rteams[i]).damage << ",rivals:";
			for (unsigned j = 0; j < 9; j++)
			{
				if (j > 0)
				{
					cout << ",";
				}
				cout << "{player:" << (pdata->rteams[i]).rivals[j].player.uid << "(" << (pdata->rteams[i]).rivals[j].player.name
						<< "),rplayers:" << (pdata->rteams[i]).rivals[j].rplayers[0].uid << "-" << (int)((pdata->rteams[i]).rivals[j].damages[0])
						<< "(" << (pdata->rteams[i]).rivals[j].rplayers[0].name
						<< ")," << (pdata->rteams[i]).rivals[j].rplayers[1].uid << "-" << (int)((pdata->rteams[i]).rivals[j].damages[1])
						<< "(" << (pdata->rteams[i]).rivals[j].rplayers[1].name
						<< ")," << (pdata->rteams[i]).rivals[j].rplayers[2].uid << "-" << (int)((pdata->rteams[i]).rivals[j].damages[2])
						<< "(" << (pdata->rteams[i]).rivals[j].rplayers[2].name
						<< ")}";
			}
			cout << "]" << endl;
		}
		cout << "----------------------------------------------------" << endl;
	}
	if (action == "all" || action == "playoff")
	{
		cout << "top8:" << endl;
		for (unsigned i = 0; i < 8; i++)
		{
			cout << "[aid:" << (pdata->top8[i]).aid << "(" << (pdata->top8[i]).aname << "),plays:";
			for (unsigned j = 0; j < 9; j++)
			{
				if (j > 0) cout << ",";
				cout << (pdata->top8[i]).players[j].uid << "-" << (int)((pdata->top8[i]).damages[j]) << "(" << (pdata->top8[i]).players[j].name << ")";
			}
			cout << "]" << endl;
		}
		cout << "top4:" << endl;
		for (unsigned i = 0; i < 4; i++)
		{
			cout << "[aid:" << (pdata->top4[i]).aid << "(" << (pdata->top4[i]).aname << "),plays:";
			for (unsigned j = 0; j < 9; j++)
			{
				if (j > 0) cout << ",";
				cout << (pdata->top4[i]).players[j].uid << "-" << (int)((pdata->top4[i]).damages[j]) << "(" << (pdata->top4[i]).players[j].name << ")";
			}
			cout << "]" << endl;
		}
		cout << "top2:" << endl;
		for (unsigned i = 0; i < 2; i++)
		{
			cout << "[aid:" << (pdata->top2[i]).aid << "(" << (pdata->top2[i]).aname << "),plays:";
			for (unsigned j = 0; j < 9; j++)
			{
				if (j > 0) cout << ",";
				cout << (pdata->top2[i]).players[j].uid << "-" << (int)((pdata->top2[i]).damages[j]) << "(" << (pdata->top2[i]).players[j].name << ")";
			}
			cout << "]" << endl;
		}
		cout << "champion:" << endl;
		cout << "[aid:" << (pdata->champion).aid << "(" << (pdata->champion).aname << "),plays:";
		for (unsigned j = 0; j < 9; j++)
		{
			if (j > 0) cout << ",";
			cout << (pdata->champion).players[j].uid << "-" << (int)((pdata->champion).damages[j]) << "(" << (pdata->champion).players[j].name << ")";
		}
		cout << "]" << endl;
		cout << "----------------------------------------------------" << endl;
	}

	return 0;
}
