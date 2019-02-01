#include "OpenPlatform.h"
#include "TestPlatform.h"
#include "TencentPlatform.h"
#include "C9Platform.h"

static map<int,bool> g_bInitPlatform;
static bool g_bMultiPlatform = true;
static PlatformType g_currPlatformType = PT_UNKNOW;
static IOpenPlatform *g_pCurrPlatform = NULL;
static map<int,map<string, string> > g_mapPlatformConfig;
static map<string, IOpenPlatform *> g_mapPlatform;

IOpenPlatform *InitPlatform(const string &configPath)
{
	//load config
	CMarkupSTL xmlConf;
	if(!xmlConf.Load(configPath.c_str()))
	{
		fatal_log("[load platform config fail][path=%s]", configPath.c_str());
		return NULL;
	}
	if(!xmlConf.FindElem("app"))
	{
		fatal_log("[parse platform config fail][path=%s,error=parse_config_fail]", configPath.c_str());
		return NULL;
	}
	xmlConf.IntoElem();

	map<string, string> config;
	while(xmlConf.FindElem("param"))
	{
		string name = xmlConf.GetAttrib("name");
		string value = xmlConf.GetAttrib("value");
		config[name] = value;
	}

	//init platform
	int nPlatform;
	if(!Convert::StringToInt(nPlatform, config["platform"]))
	{
		fatal_log("[parse platform config fail][path=%s, platform=%s,error=get_platform_type_fail]",
				configPath.c_str(), config["platform"].c_str());
		return NULL;
	}
	IOpenPlatform *pPlatform;
	switch(nPlatform)
	{
	case PT_TEST:
		pPlatform = new CTestPlatform();
		break;
	case PT_PENGYOU:
	case PT_QZONE:
	case PT_TXWEIBO:
	case PT_3366:
	case PT_qqgame:
	case PT_TX_C9:
		pPlatform = new CTencentPlatform();
		break;
    case PT_C9:
    case PT_7477:
    case PT_KW:
    case PT_7K:
    case PT_360UU:
    case PT_QD:
    case PT_V1:
    case PT_66YOU:
    case PT_51IGAME:
    case PT_HUANLEYUAN:
    case PT_51WAN:
    case PT_YUNQU:
    case PT_TIEXUE:
    case PT_FENGHUANG:
    case PT_YLT:
    case PT_CYXC:
        pPlatform = new C9Platform();
        break;
	default:
		fatal_log("[parse platform config fail][path=%s, error=unknow_platform, platform=%s]",
				configPath.c_str(), config["platform"].c_str());
		return NULL;
	}

	pPlatform->SetPlatformType((PlatformType)nPlatform);
	if(pPlatform->Initialize(config["appid"], config["appkey"], config) != 0)
	{
		fatal_log("[Init platform fail][error=init_platform_fail, platform=%d, appid=%s, appkey=%s]",
				nPlatform, config["appid"].c_str(), config["appkey"].c_str());
		delete pPlatform;
		return NULL;
	}

	return pPlatform;
}

//获取当前平台对象
IOpenPlatform *OpenPlatform::GetPlatform()
{
	return g_pCurrPlatform;
}

//获取当前平台类型
PlatformType OpenPlatform::GetType()
{
	return g_currPlatformType;
}
bool OpenPlatform::IsOurPlatform()
{
	return g_currPlatformType == PT_TEST
		|| g_currPlatformType == PT_FACEBOOK
		|| g_currPlatformType == PT_EN
		|| g_currPlatformType == PT_VN
		|| IsQQPlatform()
		|| IsLY_ALL_Platform();
}
bool OpenPlatform::IsLYPlatform()
{
	return g_currPlatformType == PT_7477
		|| g_currPlatformType == PT_KW
		|| g_currPlatformType == PT_7K
		|| g_currPlatformType == PT_360UU
		|| g_currPlatformType == PT_QD
		|| g_currPlatformType == PT_V1
		|| g_currPlatformType == PT_66YOU
		|| g_currPlatformType == PT_51IGAME
		|| g_currPlatformType == PT_HUANLEYUAN
		|| g_currPlatformType == PT_C9
		|| g_currPlatformType == PT_51WAN
		|| g_currPlatformType == PT_YUNQU
		|| g_currPlatformType == PT_XUNLEI
		|| g_currPlatformType == PT_TIEXUE
		|| g_currPlatformType == PT_FENGHUANG
		|| g_currPlatformType == PT_7k7k
		|| g_currPlatformType == PT_360
		|| g_currPlatformType == PT_YLT
		|| g_currPlatformType == PT_CYXC;
}
bool OpenPlatform::IsLY_ALL_Platform()
{
	return g_currPlatformType == PT_4399
		|| g_currPlatformType == PT_SOGOU
		|| g_currPlatformType == PT_BAIDU
		|| IsLYPlatform();
}
bool OpenPlatform::IsQQPlatform()
{
	return g_currPlatformType == PT_PENGYOU
		|| g_currPlatformType == PT_QZONE
		|| g_currPlatformType == PT_3366
		|| g_currPlatformType == PT_qqgame
		|| g_currPlatformType == PT_TX_C9;
}
bool OpenPlatform::IsFBPlatform()
{
	return g_currPlatformType == PT_FACEBOOK
		|| g_currPlatformType == PT_EN;
}
bool OpenPlatform::IsEN()
{
	return g_currPlatformType == PT_EN
		|| g_currPlatformType == PT_VN
		|| g_currPlatformType == PT_DO;
}

//更新当前平台类型
bool OpenPlatform::Reset(const string &host_suffix)
{
	int domain = 0;
	Config::GetDomain(domain);
	map<int,bool>::iterator bInitItr = g_bInitPlatform.find(domain);
	if(bInitItr == g_bInitPlatform.end() || !bInitItr->second)
	{
		//init
		g_bInitPlatform[domain] = true;
		string mappingPath = MainConfig::GetAllServerPath(CONFIG_PLATFORM_MAPPING);
		if(mappingPath.empty())
		{
			//single platform init
			g_bMultiPlatform = false;
			info_log("[get platform mapping fail][mode=single_platform]");
			string configPath;
			if(!Config::GetValue(configPath, CONFIG_PLATFORM))
			{
				fatal_log("[get config fail][config_name=%s]", CONFIG_PLATFORM);
				return false;
			}
			g_pCurrPlatform = InitPlatform(configPath);
			if(g_pCurrPlatform == NULL)
			{
				return false;
			}
			g_currPlatformType = g_pCurrPlatform->GetPlatformType();
			return true;
		}

		//multi platform init
		info_log("[get platform mapping ok][mode=multi_platform]");
		CMarkupSTL xmlConf;
		if(!xmlConf.Load(mappingPath.c_str()))
		{
			fatal_log("[load platform mapping fail][path=%s]", mappingPath.c_str());
			return false;
		}
		if(!xmlConf.FindElem("mapping"))
		{
			fatal_log("[parse platform mapping fail][path=%s,error=parse_xml_fail]", mappingPath.c_str());
			return false;
		}
		xmlConf.IntoElem();
		map<string, string> config;
		map<int,map<string,string> >::iterator mapPlatformItr = g_mapPlatformConfig.find(domain);
		if (mapPlatformItr == g_mapPlatformConfig.end())
		{
			map<string,string> tempPlatformCofing;
			while(xmlConf.FindElem("platform"))
			{
				string name = xmlConf.GetAttrib("host");
				string value = xmlConf.GetAttrib("config");
				if(name.empty() || value.empty())
				{
					fatal_log("[invalid platform mapping][path=%s,host=%s,config=%s]",
							mappingPath.c_str(), name.c_str(), value.c_str());
				}
				else
				{
					String::ToLower(name);
					tempPlatformCofing[name] = value;
				}
			}
			g_mapPlatformConfig[domain] = tempPlatformCofing;
		}
	}

	//single platform
	if(!g_bMultiPlatform)
	{
		return g_pCurrPlatform != NULL;
	}

	//multi platform
	g_pCurrPlatform = NULL;
	g_currPlatformType = PT_UNKNOW;
	char *pHost = getenv("SERVER_NAME");
	if(pHost == NULL)
	{
		error_log("[server_name_empty][server_name=<null>]");
		return false;
	}
	string host = pHost;
	String::ToLower(host);
	if (!host_suffix.empty())
		host += "." + host_suffix;
	map<string, IOpenPlatform *>::iterator itrMapPlatform;
	itrMapPlatform = g_mapPlatform.find(host);
	if(itrMapPlatform != g_mapPlatform.end())
	{
		//platform has inited
		g_pCurrPlatform = itrMapPlatform->second;
		if(g_pCurrPlatform == NULL)
		{
			error_log("[invalid_platform_type][host=%s,platform=NULL]", host.c_str());
			return false;
		}
		g_currPlatformType = g_pCurrPlatform->GetPlatformType();
		return true;
	}

	//init platform
	map<string, string>::iterator itrMapConfig;
	itrMapConfig = g_mapPlatformConfig[domain].find(host);
	if(itrMapConfig == g_mapPlatformConfig[domain].end())
	{
		for(itrMapConfig=g_mapPlatformConfig[domain].begin();itrMapConfig!=g_mapPlatformConfig[domain].end();++itrMapConfig)
		{
			if(host.find(itrMapConfig->first) != string::npos)
				break;
		}
		if(itrMapConfig == g_mapPlatformConfig[domain].end())
		{
			string all = "default";
			itrMapConfig = g_mapPlatformConfig[domain].find(all);
			if(itrMapConfig == g_mapPlatformConfig[domain].end())
			{
				error_log("[unknow_host][host=%s]", host.c_str());
				return false;
			}
		}
	}
	g_pCurrPlatform = InitPlatform(itrMapConfig->second);
	if(g_pCurrPlatform == NULL)
	{
		return false;
	}
	g_currPlatformType = g_pCurrPlatform->GetPlatformType();
	g_mapPlatform[host] = g_pCurrPlatform;
	return true;
}
