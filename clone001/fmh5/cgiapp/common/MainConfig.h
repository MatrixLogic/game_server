/*
 * MainConfig.h
 *
 *  Created on: 2013-4-9
 *      Author: Administrator
 */

#ifndef MAIN_CONFIG_H_
#define MAIN_CONFIG_H_

#include <string>
using std::string;
#include <set>
using std::set;

namespace MainConfig
{
	bool Init(const string &path);
	bool GetValue(string &value, const string &name);
	bool GetDB(string &value, const string &name);
	bool GetIntDB(int &db, const string &name);
	//string GetValue(const string &name);
	bool GetIntValue(int &value, const string &name);
	int GetMergedDomain(int serverid);
	void GetDomains(set<int> &domains);
	void GetIncludeDomains(int serverid,set<int> &domains);
	string GetHost(int serverid);

	string GetAllServerPath(string path);
}
#endif /* MAIN_CONFIG_H_ */
