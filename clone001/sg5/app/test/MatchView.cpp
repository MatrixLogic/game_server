#include "LogicInc.h"

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		cout << "usage: " << argv[0] << " path all|apply|regular|playoff" << endl;
		return 1;
	}
	Kernel::Init(APP_CONFIG_PATH);
	Config::SetDomain(0);
	int semgroup = 0;
	int semserver = 0;
	Config::GetDB(semserver);
	CShareMemory shm;
	if (!shm.CreateOrOpen(argv[1], sizeof(MatchData), SEM_ID(sem_match,semgroup,semserver)))
	{
		cout << "init fail" << endl;
		return 1;
	}
	CAutoLock lock(&shm, true);
	MatchData *pdata = (MatchData *)shm.GetAddress();
	if (!pdata)
	{
		cout << "null addr" << endl;
		return 1;
	}
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
				cout << (pdata->teams[i]).players[j].uid << "(" << (pdata->teams[i]).players[j].name << ")";
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
			cout << "-" << (i+1) << "-[aid:" << (pdata->rteams[i]).aid << "(" << (pdata->rteams[i]).aname << "),score:" << (pdata->rteams[i]).score << ",rivals:";
			for (unsigned j = 0; j < 9; j++)
			{
				if (j > 0)
				{
					cout << ",";
				}
				cout << "{player:" << (pdata->rteams[i]).rivals[j].player.uid << "(" << (pdata->rteams[i]).rivals[j].player.name
						<< "),score:" << (int)((pdata->rteams[i]).rivals[j].score)
						<< ",rplayers:" << (pdata->rteams[i]).rivals[j].rplayers[0].uid << "-" << (int)((pdata->rteams[i]).rivals[j].result[0])
						<< "(" << (pdata->rteams[i]).rivals[j].rplayers[0].name
						<< ")," << (pdata->rteams[i]).rivals[j].rplayers[1].uid << "-" << (int)((pdata->rteams[i]).rivals[j].result[1])
						<< "(" << (pdata->rteams[i]).rivals[j].rplayers[1].name
						<< ")," << (pdata->rteams[i]).rivals[j].rplayers[2].uid << "-" << (int)((pdata->rteams[i]).rivals[j].result[2])
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
			cout << "[aid:" << (pdata->top8[i]).aid << "(" << (pdata->top8[i]).aname << "),score:" << (int)((pdata->top8[i]).score) << ",plays:";
			for (unsigned j = 0; j < 9; j++)
			{
				if (j > 0) cout << ",";
				cout << (pdata->top8[i]).players[j].uid << "-" << (int)((pdata->top8[i]).result[j]) << "(" << (pdata->top8[i]).players[j].name << ")";
			}
			cout << "]" << endl;
		}
		cout << "top4:" << endl;
		for (unsigned i = 0; i < 4; i++)
		{
			cout << "[aid:" << (pdata->top4[i]).aid << "(" << (pdata->top4[i]).aname << "),score:" << (int)((pdata->top4[i]).score) << ",plays:";
			for (unsigned j = 0; j < 9; j++)
			{
				if (j > 0) cout << ",";
				cout << (pdata->top4[i]).players[j].uid << "-" << (int)((pdata->top4[i]).result[j]) << "(" << (pdata->top4[i]).players[j].name << ")";
			}
			cout << "]" << endl;
		}
		cout << "top2:" << endl;
		for (unsigned i = 0; i < 2; i++)
		{
			cout << "[aid:" << (pdata->top2[i]).aid << "(" << (pdata->top2[i]).aname << "),score:" << (int)((pdata->top2[i]).score) << ",plays:";
			for (unsigned j = 0; j < 9; j++)
			{
				if (j > 0) cout << ",";
				cout << (pdata->top2[i]).players[j].uid << "-" << (int)((pdata->top2[i]).result[j]) << "(" << (pdata->top2[i]).players[j].name << ")";
			}
			cout << "]" << endl;
		}
		cout << "champion:" << endl;
		cout << "[aid:" << (pdata->champion).aid << "(" << (pdata->champion).aname << "),score:" << (int)((pdata->champion).score) << ",plays:";
		for (unsigned j = 0; j < 9; j++)
		{
			if (j > 0) cout << ",";
			cout << (pdata->champion).players[j].uid << "-" << (int)((pdata->champion).result[j]) << "(" << (pdata->champion).players[j].name << ")";
		}
		cout << "]" << endl;
		cout << "----------------------------------------------------" << endl;
	}

	return 0;
}
