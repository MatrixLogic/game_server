#include "LogicInc.h"

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		cout << "usage: " << argv[0] << " path datafile" << endl;
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
	FILE *fp = fopen(argv[2], "r");
	if (!fp)
	{
		cout << "open datafile fail" << endl;
		return 1;
	}
	memset(pdata, 0, sizeof(*pdata));
	int index = 0;
	int lineno = 0;
	char buf[4096];
	memset(buf, 0, sizeof(buf));
	while (fgets(buf, sizeof(buf), fp) && index < LADDER_ITEM_NUMBER)
	{
		lineno++;
		string line(buf);
		if (!line.empty() && line[line.length() - 1] == '\n')
			line = line.substr(0, line.length() - 1);
		if (!line.empty() && line[line.length() - 1] == '\r')
			line = line.substr(0, line.length() - 1);
		if (line.empty())
			continue;
		vector<string> items;
		CBasic::StringSplit(line, " ", items);
		if (items.size() != 4)
		{
			cout << "format error,line:" << lineno << endl;
			continue;
		}
		pdata->ladder[index].uid = atoi(items[0].c_str());
		pdata->ladder[index].level = atoi(items[1].c_str());
		snprintf(pdata->ladder[index].name, sizeof(pdata->ladder[index].name), "%s", items[2].c_str());
		snprintf(pdata->ladder[index].pic, sizeof(pdata->ladder[index].pic), "%s", items[3].c_str());
		cout << pdata->ladder[index].uid << " " << (int)(pdata->ladder[index].level) << " "
				<< pdata->ladder[index].name << " " << pdata->ladder[index].pic << endl;
		index++;
		memset(buf, 0, sizeof(buf));
	}
	shm.SetInitDone();
	fclose(fp);
	cout << "init finish,line:" << lineno << ",index:" << index << endl;
	return 0;
}
