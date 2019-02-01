/*
 *BatchAddResourceEquip.cpp
 *
 *  Created on: 2016-10-10
 *      Author: City
 */

#include "LogicInc.h"
#include <fstream>

int main(int argc, char* argv[])
{
	if (!Kernel::Init())
	{
		cout << "init error";
		return 1;
	}

	if (argc != 3)
	{
		cout << "param error. usage: ./programe file zone";
		return 1;
	}

	string filepath = argv[1];
	ifstream inputfile(filepath.c_str(), ios::in);

	if (!inputfile.is_open())
	{
		cout<<"file open error."<<filepath<<endl;
		return 1;
	}

	string itemstr;
	vector<string> vctItems;
	int line = 0;

	Admin::AsycAdd * addmsg = new Admin::AsycAdd;

	int localzone = atoi(argv[2]);

	while(!inputfile.eof())
	{
		++line;
		//uid,id,count
		getline(inputfile, itemstr);

		if (itemstr.empty())
		{
			continue;
		}

		String::Split(itemstr, ',', vctItems);

		if (vctItems.size() != 3)
		{
			cout<<"wrong data format in line:"<<line<<",data:"<<itemstr<<endl;
			continue;
		}

		unsigned uid = CTrans::STOI(vctItems[0].c_str());
		unsigned id	= CTrans::STOI(vctItems[1].c_str());
		unsigned count = CTrans::STOI(vctItems[2].c_str());

		if (0 == uid || 0 == count)
		{
			cout<<"wrong data value in line:"<<line<<",data:"<<itemstr<<endl;
			continue;
		}

		Admin::AsycItem*  singleitem = addmsg->add_item();
		singleitem->set_uid(uid);
		singleitem->set_id(id);
		singleitem->set_count(count);
	}

	//发送协议
	CSG17Packet packet;
	packet.cmd = PROTOCOL_ADMIN;
	packet.m_msg = addmsg;

	int ret = BattleSocket::Send(localzone, &packet);

	if(ret)
	{
		cout<<"send system error"<<endl;
		return ret;
	}

	CSG17Packet reply;
	ret = BattleSocket::Receive(localzone, &reply);

	if(ret)
	{
		cout<<"recv system error"<<endl;
		return ret;
	}

	Admin::AsycAddResp* rmsg = (Admin::AsycAddResp*) reply.m_msg;
	ret = rmsg->ret();

	if(ret)
	{
		cout<<"reply error."<<endl;
		return ret;
	}

	cout<<"batch success"<<endl;

	return 0;
}


