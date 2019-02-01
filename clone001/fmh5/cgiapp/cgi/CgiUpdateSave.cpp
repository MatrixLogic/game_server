
#include "LogicInc.h"

class CCgiUpdateSave : public CCgiBase
{
public:

	CCgiUpdateSave() : CCgiBase("updatesave")
	{
		SetFeature(CF_PRINT_JSON);
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_CHECK_PLATFORM);
	}

	CGI_DIRECT_PROCESS(UpdateSave)

	int UpdateSave()
	{
		int need = CCGIIn::GetCGIInt("need");
		if(need)
		{
			string openid = CCGIIn::GetCGIStr("openid");
			string openkey = CCGIIn::GetCGIStr("openkey");
			string pf = CCGIIn::GetCGIStr("pf");
			OpenPlatform::GetPlatform()->Is_Login(openid,openkey,pf);
			CGI_SEND_LOG("operator=%s", openid.c_str());
		}

		return R_SUCCESS;
	}

};

CGI_MAIN(CCgiUpdateSave)
