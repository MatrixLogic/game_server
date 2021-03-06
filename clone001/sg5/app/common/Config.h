/*
 * Config.h
 *
 *  Created on: 2011-5-25
 *      Author: dada
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>
using std::string;

namespace Config
{
	bool Init(const string &configPath);
	bool GetValue(string &value, const string &name);
	string GetValue(const string &name);
	bool GetIntValue(int &value, const string &name);
	int GetIntValue(const string &name);
	bool GetUIntValue(unsigned &value, const string &name);
	bool UpdateDomain(string &host);
	void GetDomain(int &param);
	void GetDB(int &param);
	void SetDomain(int param);
	bool GetAppConfigFilePath(unsigned serverid, string &path);
}

#endif /* CONFIG_H_ */
