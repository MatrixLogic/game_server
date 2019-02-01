/*
 * KernelUtil.cpp
 *
 *  Created on: 2011-5-26
 *      Author: dada
 */

#include "KernelUtil.h"

bool Kernel::InitLog(const string &configPath)
{
	int level ;
	string path;
	string logName;
	int rewind;
	int size;
	int record_size;

	if (!configPath.empty())
	{
		path = Config::GetPath(CONFIG_LOG_PATH);
		if( !Config::GetIntValue(level, CONFIG_LOG_LEVEL)	||
			path.empty()								    ||
			!Config::GetIntValue(rewind, CONFIG_LOG_REWIND)	||
			!Config::GetIntValue(size, CONFIG_LOG_SIZE)	    ||
			!Config::GetIntValue(record_size, CONFIG_LOG_RECORD_SIZE) ||
			!Config::GetValue(logName, CONFIG_LOG_NAME))
		{
			return false;
		}
		int domain = 0;
		Config::GetDomain(domain);
		CLog::SetDomain(domain);
		CLog::GetInstance()->initalize(level, path.c_str(), logName.c_str(), rewind, size, record_size);
		return true;
	}

	set<int> domains;
	MainConfig::GetDomains(domains);

	for(set<int>::iterator it=domains.begin();it!=domains.end();++it)
	{
		Config::SetDomain(*it);
		path = Config::GetPath(CONFIG_LOG_PATH);
		if( !Config::GetIntValue(level, CONFIG_LOG_LEVEL)	||
			path.empty()								    ||
			!Config::GetIntValue(rewind, CONFIG_LOG_REWIND)	||
			!Config::GetIntValue(size, CONFIG_LOG_SIZE)	    ||
			!Config::GetIntValue(record_size, CONFIG_LOG_RECORD_SIZE) ||
			!Config::GetValue(logName, CONFIG_LOG_NAME))
		{
			return false;
		}

		CLog::SetDomain(*it);
		CLog::GetInstance()->initalize(level, path.c_str(), logName.c_str(), rewind, size, record_size);
	}
	return true;
}

bool Kernel::Init(const string &configPath)
{
	Time::Init();
	Math::InitRandom();
	if (!MainConfig::Init(APP_DOMAIN_CONFIG_PATH))
	{
		return false;
	}
	if(!Config::Init(configPath))
	{
		return false;
	}
	if(!StringFilter::Init(MainConfig::GetAllServerPath(CONFIG_STRING_FILTER_DATA)))
	{
		return false;
	}
	if(!Kernel::InitLog(configPath))
	{
		cout << "Kernel::InitLog faile" << endl;
		return false;
	}
	return true;
}
