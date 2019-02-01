#include "LogicInc.h"

int main(int argc, char *argv[]) {
	if (argc != 4) {
		cout << "usage: TryMarketTask openid uid set/get" << endl;
		return 1;
	}

	if (!Kernel::Init(APP_CONFIG_PATH)) {
		cout << "Init kernel fail!" << endl;
		return -1;
	}
	Config::SetDomain(0);
	int ret = 0;
	string openid = argv[1];
	string type = argv[3];
	string contractid = "100688853T2201304020001";
	string taskid = "10001";
	unsigned uid = atoi(argv[2]);
	int flag = 1;

	CDataMarketTask dbMarketTask;
	DataMarketTask task;
	if(type == "set")
	{
		task.openid = openid;
		task.contractid = contractid;
		task.uid = uid;
		int result = 0;

		ret = dbMarketTask.GetTask(task);
		if (R_ERR_NO_DATA == ret)
		{
			task.taskid = taskid;
			task.flag = flag;
			result = dbMarketTask.AddTask(task);
		}
		else if(0 == ret)
		{
			task.taskid = taskid;
			task.flag = flag;
			result = dbMarketTask.SetTask(task);
		}
		else
		{
			cout<<"get error, ret="<<ret<<endl;
			return ret;
		}

		if (0 != result)
		{
			cout<<"set error, ret="<<result<<endl;
			return result;
		}
		cout<<"set ok!"<<endl;
	}
	else if(type == "get")
	{
		task.openid = openid;
		task.contractid = contractid;
		task.uid = uid;

		ret = dbMarketTask.GetTask(task);
		if (0 != ret)
		{
			cout<<"get error, ret="<<ret<<endl;
			return ret;
		}
		cout<<task.taskid<<" "<<task.contractid<<" "<<task.flag<<endl;
		cout<<"get ok!"<<endl;
	}
	else if(type == "del")
	{
		task.openid = openid;
		task.contractid = contractid;
		ret = dbMarketTask.RemoveTask(task);
		if (0 != ret)
		{
			cout<<"del error, ret="<<ret<<endl;
			return ret;
		}
		cout<<"del ok!"<<endl;
	}

	return 0;
}

