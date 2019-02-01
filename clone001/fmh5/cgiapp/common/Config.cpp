/*
 * Config.cpp
 *
 *  Created on: 2011-5-25
 *      Author: dada
 */

#include "Config.h"
#include "Common.h"
#include <sys/stat.h>
#include <unistd.h>
#include "../kernel/Kernel.h"

//static map<string, string> g_config;
static map<int, map<string, string> > g_config;
static int g_domain = 0;
static int g_db = 0;

bool Config::Init(const string &configPath)
{
	if (!configPath.empty())
	{
		cout << "configPath=" << configPath << endl;
		CMarkupSTL xmlConf;
		if(!xmlConf.Load(configPath.c_str()))
		{
			return false;
		}
		if(!xmlConf.FindElem("configs") )
		{
			return false;
		}
		xmlConf.IntoElem();
		map<string, string> temp_cofig;
		while(xmlConf.FindElem("config"))
		{
			string name = xmlConf.GetAttrib("name");
			string value = xmlConf.GetAttrib("value");
			temp_cofig[name] = value;

		}
		g_config[g_domain] = temp_cofig;
		return true;
	}

	string serverPath;
	if(!MainConfig::GetValue(serverPath, "server_path"))
		serverPath = DEFAULT_APP_PATH;
	if (serverPath[serverPath.length() - 1] != '/')
		serverPath.append("/");
	set<int> domains;
	MainConfig::GetDomains(domains);
	string appConfigPath(DEFAULT_APP_CONFIG_PATH);
	for(set<int>::iterator it=domains.begin();it!=domains.end();++it)
	{
		string configPath;
		string serverid;
		String::Format(serverid, "s%d/", *it);
		configPath=serverPath + serverid + appConfigPath;
		cout << configPath << endl;
		struct stat buf;
		int ret = stat(configPath.c_str(), &buf);
		cout <<"ret=" << ret << endl;
		if (0 != ret)
		{
			continue;
		}
		CMarkupSTL xmlConf;
		if(!xmlConf.Load(configPath.c_str()))
		{
			return false;
		}
		if(!xmlConf.FindElem("configs") )
		{
			return false;
		}
		xmlConf.IntoElem();
		map<string, string> temp_cofig;
		while(xmlConf.FindElem("config"))
		{
			string name = xmlConf.GetAttrib("name");
			string value = xmlConf.GetAttrib("value");
			temp_cofig[name] = value;
		}
		g_config[*it] = temp_cofig;
	}
	return true;
}

bool Config::GetValue(string &value, const string &name)
{
	map<int, map<string, string> >::iterator temp_itr = g_config.find(g_domain);
	if (temp_itr == g_config.end())
	{
		cout << "GetValue fail, g_domain=" << g_domain << endl;
		return false;
	}
	map<string, string> temp_config = temp_itr->second;
	map<string, string>::iterator itr = temp_config.find(name);
	if(itr == temp_config.end())
	{
		return false;
	}
	value = itr->second;
	return true;
}

string Config::GetValue(const string &name)
{
	string value;
	Config::GetValue(value, name);
	return value;
}

bool Config::GetIntValue(int &value, const string &name)
{
	string sValue;
	if(!Config::GetValue(sValue, name))
	{
		return false;
	}
	return Convert::StringToInt(value, sValue);
}

int Config::GetIntValue(const string &name)
{
	int value;
	if(!Config::GetIntValue(value, name))
	{
		return 0;
	}
	return value;
}

bool Config::GetUIntValue(unsigned &value, const string &name)
{
	string sValue;
	if(!Config::GetValue(sValue, name))
	{
		return false;
	}
	return Convert::StringToUInt(value, sValue);
}

void Config::SelectDomain(int serverid, int db)
{
	Config::SetDomain(serverid);
	CLog::SetDomain(serverid);
	CDataLog::SetDomain(serverid);
	CBusinessLogHelper::SetDomain(serverid);
	g_db = db;
}

bool Config::UpdateDomain(string &host)
{
	char *pHost = getenv("SERVER_NAME");
	if(pHost == NULL)
	{
		return false;
	}

	host = pHost;
	String::ToLower(host);
	int serverid = 0;
	int db = 0;

	if(!MainConfig::GetIntValue(serverid, host))
		return false;
	if(!MainConfig::GetIntDB(db, host))
		return false;

	SelectDomain(serverid, db);

	return true;
}

void Config::GetDomain(int &param)
{
	param = g_domain;
	return;
}

void Config::SetDomain(int param)
{
	g_domain = param;
	return;
}

void Config::GetDB(int &param)
{
	if(!g_domain || !g_db)
		GetIntValue(param, CONFIG_SERVER_ID);
	else
		param = g_db;
	return;
}

bool Config::GetAppConfigFilePath(unsigned serverid, string &path)
{
	string serverPath;
	if (!MainConfig::GetValue(serverPath, "server_path"))
	{
		return false;
	}
	if (serverPath.empty())
	{
		return false;
	}
	if (path[path.length()-1] == '/')
	{
		path = serverPath.append("s").append(CTrans::UTOS(serverid)).append("/conf/app_config.xml");
	}
	else
	{
		path = serverPath.append("/s").append(CTrans::UTOS(serverid)).append("/conf/app_config.xml");
	}
	return true;
}

string Config::GetPath(string path, int server)
{
	if(server == 0)
		server = g_domain;
	string serverPath;
	if(server == 0)
		serverPath = "../";
	else
	{
		if(!MainConfig::GetValue(serverPath, "server_path"))
			serverPath = DEFAULT_APP_PATH;
		if(serverPath[serverPath.length() - 1] != '/')
			serverPath.append("/");
		string serverid;
		String::Format(serverid, "s%d/", server);
		serverPath += serverid;
	}

	return serverPath + path;
}

unsigned Config::GetUIDStart(unsigned serverid)
{
	return UID_MIN + UID_ZONE * (serverid - 1);
}
unsigned Config::GetUIDStart()
{
	if(g_db)
		return UID_MIN + UID_ZONE * (g_db - 1);
	else
		return UID_MIN;
}
unsigned Config::GetZoneByUID(unsigned uid)
{
	return (uid - UID_MIN) / UID_ZONE + 1;
}
unsigned Config::GetAIDStart(unsigned serverid)
{
	return ALLIANCE_ID_START + AID_ZONE * (serverid - 1);
}
unsigned Config::GetAIDStart()
{
	if(g_db)
		return ALLIANCE_ID_START + AID_ZONE * (g_db - 1);
	else
		return ALLIANCE_ID_START;
}
unsigned Config::GetZoneByAID(unsigned aid)
{
	return (aid - ALLIANCE_ID_START) / AID_ZONE + 1;
}
