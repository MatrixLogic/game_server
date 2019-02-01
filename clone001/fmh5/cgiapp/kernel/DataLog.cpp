#include "DataLog.h"
#include "Kernel.h"
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include "basic.h"

int CDataLog::fdall = -1;
int CDataLog::m_domain = 0;
map<int,DataLogCtrl> CDataLog::datalog_map;

bool CDataLog::init( struct tm **p )
{
	static char buf[100];
	time_t now;
	time( &now );
	*p = localtime(&now);
	int write_times = 0;
	DataLogCtrl datalog;
	datalog.fd = -1;
	datalog.write_times = 0;
	datalog.last_day = 0;

	map<int,DataLogCtrl>::iterator itr = datalog_map.find(m_domain);
	if (itr != datalog_map.end())
		datalog = itr->second;

	if( datalog.write_times > 1000 || datalog.last_day != (*p)->tm_mday )
	{
		close(datalog.fd);
		close(fdall);
		datalog.fd = -1;
		fdall = -1;
		datalog.write_times = 0;
		datalog_map[m_domain] = datalog;
	}

	if( datalog.fd > -1 )
	{
		return true;
	}

	string logDir = Config::GetPath(CONFIG_DATA_LOG_PATH);
	if(logDir.empty())
	{
		return false;
	}
	string logDir2 = MainConfig::GetAllServerPath(CONFIG_ALL_DATA_LOG_PATH);

	sprintf(buf, "%s/data_%04d%02d%02d.log", logDir.c_str(),1900+(*p)->tm_year, (*p)->tm_mon + 1, (*p)->tm_mday);
	datalog.fd = open(buf, O_CREAT | O_WRONLY | O_APPEND, 0644);
	if(datalog.fd < 0)
	{
		return false;
	}
	if (!logDir2.empty())
	{
		sprintf(buf, "%s/data_%04d%02d%02d.log", logDir2.c_str(),1900+(*p)->tm_year, (*p)->tm_mon + 1, (*p)->tm_mday);
		fdall = open(buf, O_CREAT | O_WRONLY | O_APPEND);
	}
	datalog.last_day = (*p)->tm_mday;
	datalog.write_times++;
	datalog_map[m_domain] = datalog;
	return true;
}

void CDataLog::DataLog(const char *action, const char *ip, const char *type, const char* format, ...)
{
	struct tm *p;
	if( !init( &p ) )
	{
		return;
	}
	static char buf[500];

	/*std::string realIp;
	if (ip != NULL)
	{
		std::string ipstr(ip);
		if (!ipstr.empty())
		{
			ParseIP( ipstr, realIp );
		}
	}*/

	snprintf( buf, sizeof(buf), "%04d%02d%02d %02d:%02d:%02d|%s|%s|%s|",
			1900+p->tm_year, p->tm_mon + 1, p->tm_mday, p->tm_hour,
			p->tm_min, p->tm_sec, action, ip, type );
	int len = strlen(buf);
	va_list args;
	va_start(args, format);
	vsnprintf(buf+len, sizeof(buf)-len-4, format, args);
	va_end(args);
	strcat(buf, "\r\n");

	DataLogCtrl datalog;
	map<int,DataLogCtrl>::iterator itr = datalog_map.find(m_domain);
	if (itr == datalog_map.end())
	{
		return;
	}
	datalog = itr->second;
	write(datalog.fd,buf,strlen(buf));

	if (fdall > -1)
	{	//gosling的特殊需求，所有分服的日志再统一在另一文件中再记一次
		write(fdall, buf, strlen(buf));
	}
}

std::string& CDataLog::ParseIP( const std::string pyIp, std::string& realIp )
{
	realIp.clear();
	int ip1,ip2,ip3,ip4;
	char buf[20];
	std::vector<std::string> ipvec;
	CBasic::StringSplit(pyIp, std::string(", "), ipvec);
	for( unsigned i = 0; i < ipvec.size(); i++ )
	{
		if( ipvec[i].length() == 40 )
		{
			sscanf(ipvec[i].c_str(), "%02x%02x%02x%02x", &ip1,&ip2,&ip3,&ip4);
			sprintf( buf, "%d.%d.%d.%d", ip1,ip2,ip3,ip4 );
			realIp.append(buf);
		}
		else
		{
			realIp.append(ipvec[i]);
		}
		if( ipvec.size() - 1 != i )
		{
			realIp.append(",");
		}
	}
	return realIp;
}

void CDataLog::SetDomain(int domain)
{
	m_domain = domain;
}
