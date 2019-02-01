#include "LogicInc.h"

class CCgiPostReport : public CCgiBase
{
public:
	CCgiPostReport() : CCgiBase("postreport")
	{
		SetFeature(CF_PRINT_JSON);
		SetFeature(CF_GET_REMOTE_IP);
	}

	CGI_DIRECT_PROCESS(PostReport)

	int PostReport()
	{
		string uid = CCGIIn::GetCGIStr("uid");
		string error = CCGIIn::GetCGIStr("error");
		string cgi = CCGIIn::GetCGIStr("cgi");
		string action = CCGIIn::GetCGIStr("action");
		string msg = CCGIIn::GetCGIStr("msg");
		if(error == "1")
			error_log("uid=%s,error=%s,cgi=%s,action=%s,msg=%s,ip=%s",uid.c_str(),error.c_str(),cgi.c_str(),action.c_str(),msg.c_str(),m_ipstr.c_str());
		else if(error == "2")
			error_log("uid=%s,error=%s,msg=%s,ip=%s",uid.c_str(),error.c_str(),msg.c_str(),m_ipstr.c_str());
		else
			debug_log("uid=%s,error=%s,msg=%s,ip=%s",uid.c_str(),error.c_str(),msg.c_str(),m_ipstr.c_str());
		return R_SUCCESS;
	}
};

CGI_MAIN(CCgiPostReport)
