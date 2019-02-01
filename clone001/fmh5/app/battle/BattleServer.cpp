#include "BattleServer.h"


static void OnReloadConfig()
{
	LogicManager::NeedReloadConfig = true;
}
static void OnExit()
{
	LogicManager::IsPreClosed = true;
}
static void OnSigNum(int signum,siginfo_t *info,void *myact)
{
	LogicManager::m_signum = signum;
}

static bool ParseAddress(vector<CInternetAddress> &vecAddress, const string &sAddress)
{
	if(sAddress.empty())
	{
		return false;
	}
	vector<string> vecStrAddress;
	CBasic::StringSplitTrim(sAddress, ",", vecStrAddress);
	for(vector<string>::iterator itr = vecStrAddress.begin(); itr != vecStrAddress.end(); itr++)
	{
		CInternetAddress address;
		if(address.FromString(*itr))
		{
			vecAddress.push_back(address);
		}
	}
	return vecAddress.size() != 0;
}

bool BattleServer::Initialize()
{
	if(!Kernel::Init())
	{
		fatal_log("[Kernel::Init fail][server=Toywar]");
		return false;
	}
	vector<CInternetAddress> listenAddress;
//	string ip = "0.0.0.0";
//	ip.append(":").append(String::i2s(BASE_SERVER_PORT + atoi(Config::GetValue(CONFIG_SRVID).c_str()) * 10 + atoi(Config::GetValue("process_id").c_str()))))
	if(!ParseAddress(listenAddress,Config::GetValue("server_listen")))
	{
		fatal_log("[ParseAddress fail]");
		return false;
	}
	int maxConn = TCP_SERVER_MAX_CONNECTIONS;
	Config::GetIntValue(maxConn, "server_max_conn");
	if(!CTcpServer::Initialize(listenAddress, maxConn))
	{
		fatal_log("[TcpServer::Initialize fail]");
		return false;
	}

	if(!System::InitDaemon(OnExit, OnReloadConfig, OnSigNum))
	{
		fatal_log("[System::InitDaemon fail][error=%d]", errno);
		return false;
	}

	//CLog::startLogThread();

	return true;
}


void BattleServer::OnReceive(CFirePacket *pPacket){
	LogicManager::Instance()->process(pPacket);
	//debug_log("receive packet:%s",pPacket->ToString().c_str());
}

void BattleServer::OnChannelClose(int channelId){
	if(LogicManager::Instance()->getchannelId() == channelId)
		LogicManager::Instance()->ClearUser(false);
}

