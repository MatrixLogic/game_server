/*
 * Proto.cpp
 *
 *  Created on: 2015-12-4
 *      Author: Ralf
 */

#include "Kernel.h"

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		cout<<"need para [p2c p2s p2j j2p]"<<endl;
		return 1;
	}

	string a1 = argv[1];

	if(a1 == "p2c")
	{
		if(argc < 3)
		{
			cout<<"need p2c [pkg.msg]"<<endl;
			return 1;
		}

		string name = argv[2];
		Proto2CPP p2c(name);
		cout<<p2c.m_cpp<<endl;
	}
	else if(a1 == "p2s")
	{
		if(argc < 4)
		{
			cout<<"need p2s [pkg.msg] [path]"<<endl;
			return 1;
		}

		string name = argv[2];
		string path = argv[3];
		ProtoBase pb(name);
		if(pb.Parse(path))
		{
			cout<<"parse fail!"<<endl;
			return 1;
		}
		cout<<pb.DebugString()<<endl;
	}
	else if(a1 == "p2j")
	{
		if(argc < 4)
		{
			cout<<"need p2j [pkg.msg] [path]"<<endl;
			return 1;
		}

		string name = argv[2];
		string path = argv[3];
		Proto2Json p2j(name);
		if(p2j.Parse(path))
		{
			cout<<"parse fail!"<<endl;
			return 1;
		}
		if(p2j.P2J())
		{
			cout<<"p2j fail!"<<endl;
			return 1;
		}
		cout<<p2j.SerializeJson()<<endl;
	}
	else if(a1 == "j2p")
	{
		if(argc < 5)
		{
			cout<<"need j2p [pkg.msg] [msg path] [json path]"<<endl;
			return 1;
		}

		string name = argv[2];
		string path = argv[3];
		string jpath = argv[4];
		Json2Proto j2p(name);
		if(j2p.ParseJson(jpath))
		{
			cout<<"parse json fail!"<<endl;
			return 1;
		}
		if(j2p.J2P())
		{
			cout<<"j2p fail!"<<endl;
			return 1;
		}
		if(j2p.Serialize(path))
		{
			cout<<"serialize fail!"<<endl;
			return 1;
		}
		cout<<j2p.DebugString()<<endl;
	}

	return 0;
}
//
///*******auto created test struct*********/
//struct Datatm {
//	int tm1;
//	unsigned int tm2;
//	long tm3;
//	unsigned long tm4;
//	float tm5;
//	double tm6;
//	string tm7;
//	bool tm8;
//	Datatm() {
//		tm1 = 0;
//		tm2 = 0;
//		tm3 = 0;
//		tm4 = 0;
//		tm5 = 0.0f;
//		tm6 = 0.0f;
//		tm8 = false;
//	}
//	void Serialize(test::tm *p) {
//		p->set_tm1(tm1);
//		p->set_tm2(tm2);
//		p->set_tm3(tm3);
//		p->set_tm4(tm4);
//		p->set_tm5(tm5);
//		p->set_tm6(tm6);
//		p->set_tm7(tm7);
//		p->set_tm8(tm8);
//	}
//	void Parse(const test::tm &p) {
//		tm1 = p.tm1();
//		tm2 = p.tm2();
//		tm3 = p.tm3();
//		tm4 = p.tm4();
//		tm5 = p.tm5();
//		tm6 = p.tm6();
//		tm7 = p.tm7();
//		tm8 = p.tm8();
//	}
//};
//struct Datatest {
//	vector<int> test1;
//	vector<unsigned int> test2;
//	vector<long> test3;
//	vector<unsigned long> test4;
//	vector<float> test5;
//	vector<double> test6;
//	vector<string> test7;
//	vector<bool> test8;
//	Datatm test9;
//	map<unsigned int, Datatm> test10;
//	Datatest() {
//	}
//	void Serialize(test::test *p) {
//		for (vector<int>::iterator it = test1.begin(); it != test1.end(); ++it)
//			p->add_test1(*it);
//		for (vector<unsigned int>::iterator it = test2.begin();
//				it != test2.end(); ++it)
//			p->add_test2(*it);
//		for (vector<long>::iterator it = test3.begin(); it != test3.end(); ++it)
//			p->add_test3(*it);
//		for (vector<unsigned long>::iterator it = test4.begin();
//				it != test4.end(); ++it)
//			p->add_test4(*it);
//		for (vector<float>::iterator it = test5.begin(); it != test5.end();
//				++it)
//			p->add_test5(*it);
//		for (vector<double>::iterator it = test6.begin(); it != test6.end();
//				++it)
//			p->add_test6(*it);
//		for (vector<string>::iterator it = test7.begin(); it != test7.end();
//				++it)
//			p->add_test7(*it);
//		for (vector<bool>::iterator it = test8.begin(); it != test8.end(); ++it)
//			p->add_test8(*it);
//		test9.Serialize(p->mutable_test9());
//		for (map<unsigned int, Datatm>::iterator it = test10.begin();
//				it != test10.end(); ++it)
//			it->second.Serialize(p->add_test10());
//	}
//	void Parse(const test::test &p) {
//		for (int i = 0; i < p.test1_size(); ++i)
//			test1.push_back(p.test1(i));
//		for (int i = 0; i < p.test2_size(); ++i)
//			test2.push_back(p.test2(i));
//		for (int i = 0; i < p.test3_size(); ++i)
//			test3.push_back(p.test3(i));
//		for (int i = 0; i < p.test4_size(); ++i)
//			test4.push_back(p.test4(i));
//		for (int i = 0; i < p.test5_size(); ++i)
//			test5.push_back(p.test5(i));
//		for (int i = 0; i < p.test6_size(); ++i)
//			test6.push_back(p.test6(i));
//		for (int i = 0; i < p.test7_size(); ++i)
//			test7.push_back(p.test7(i));
//		for (int i = 0; i < p.test8_size(); ++i)
//			test8.push_back(p.test8(i));
//		test9.Parse(p.test9());
//		for (int i = 0; i < p.test10_size(); ++i)
//			test10[p.test10(i).tm1()].Parse(p.test10(i));
//	}
//};
//
///*********test test struct*************/
//void testtest()
//{
//	Datatest t, t1;
//	t.test1.push_back(1);
//	t.test2.push_back(2);
//	t.test3.push_back(3);
//	t.test4.push_back(4);
//	t.test5.push_back(5.0f);
//	t.test6.push_back(6.0f);
//	t.test7.push_back("7");
//	t.test8.push_back(true);
//	t.test9.tm1 = 1;
//	t.test9.tm2 = 2;
//	t.test9.tm3 = 3;
//	t.test9.tm4 = 4;
//	t.test9.tm5 = 5.0f;
//	t.test9.tm6 = 6.0f;
//	t.test9.tm7 = "7";
//	t.test9.tm8 = true;
//	t.test10[t.test9.tm1] = t.test9;
//
//	{
//		cout<<"Serialize"<<endl;
//		google::protobuf::Message* msg = new test::test();
//		t.Serialize((test::test *)msg);
//		cout<<msg->DebugString()<<endl;
//
//		fstream output("test.dat", ios::out | ios::trunc | ios::binary);
//		if(!msg->SerializeToOstream(&output))
//		{
//			error_log("serialize fail");
//			return;
//		}
//
//		((test::test *)msg)->Clear();
//		delete msg;
//	}
//
//
//	{
//		cout<<"Parse"<<endl;
//		google::protobuf::Message* msg = new test::test();
//		fstream input("test.dat", ios::in | ios::binary);
//		if(!input)
//		{
//			error_log("create new fail");
//			return;
//		}
//		else if(!msg->ParseFromIstream(&input))
//		{
//			error_log("parse fail");
//			return;
//		}
//
//		cout<<msg->DebugString()<<endl;
//		t1.Parse(*(test::test *)msg);
//
//		((test::test *)msg)->Clear();
//		delete msg;
//	}
//}
