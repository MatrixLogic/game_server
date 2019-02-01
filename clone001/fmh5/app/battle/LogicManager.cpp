#include <pthread.h>
#include <time.h>
#include <sys/select.h>
#include <stdarg.h>

#include "LogicManager.h"
#include "BattleServer.h"
#include "ProtoManager.h"

uint32_t LogicManager::ServerId = 0;
uint32_t LogicManager::SecOpenTime = 0;
uint32_t LogicManager::GlobalMilliTime = 0;
uint64_t LogicManager::StartMilliTime = 0;
bool LogicManager::IsClosed = false;
bool LogicManager::IsPreClosed = false;
bool LogicManager::NeedReloadConfig = false;
int LogicManager::m_signum = 0;

#define PER_FRAME_TIME 10

LogicManager::LogicManager():m_fd(0),channelId(-1),m_timer(0),m_last_hour_ts(0),lastLoopTime(0),pReplyProtocol(NULL),needDelReply(true),dispatcher(ProtoManager::DisCardMessage){}

void* TimerThread(void* arg){
	CTcpChannel* pChannel = (CTcpChannel*)arg;
	struct timeval timeOut;
	timeOut.tv_sec = 1;
	timeOut.tv_usec = 0;

	CFirePacket packet(PROTOCOL_EVENT_SECOND_TIMER);
	CStaticBuffer<PACKET_HEADER_SIZE> buf;
	packet.Encode(&buf);

	while(!LogicManager::IsClosed){
		timeOut.tv_sec = 1;
		timeOut.tv_usec = 0;

		if(select(0,NULL,NULL,NULL,&timeOut) == 0 && !LogicManager::IsClosed){
			pChannel->SendData(&buf);
		}
	}

	return NULL;
}
bool LogicManager::Initialize(){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	StartMilliTime = tv.tv_sec * 1000 + tv.tv_usec/1000;
	ServerId = Config::GetIntValue(CONFIG_SRVID);
	SecOpenTime = Config::GetIntValue(CONFIG_OPENTIME);
	m_last_hour_ts = time(NULL);
	m_timer = m_last_hour_ts % 60;
	unsigned base_buff = Config::GetIntValue(CONFIG_BASE);
	if(base_buff)
		DataSingletonBase::BASE_BUFF = base_buff;

	BattleServer* pServer = BattleServer::Instance();

	//定时器线程
	CTcpChannel* pTimeChannel = pServer->GetSelfClientChannel();
	pthread_t pid;
	if(pthread_create(&pid,NULL,TimerThread,(void*)pTimeChannel) != 0){
		error_log("start time thread failed,%m");
		return false;
	}
	info_log("time thread start,pid=%u",pid);

	if(!ConfigManager::Instance()->Inited())
	{
		error_log("ConfigManager Init error!");
		return false;
	}
	RegProto();
	RegDataManager();
	RegMemoryManager();
	RegBattleManager();
	RegActivityManager();

	int ret = 0;
	for(vector<DataSingletonBase*>::iterator it=m_dataManager.begin();it!=m_dataManager.end();++it)
	{
		try
		{
			ret = (*it)->Init();
			if(ret)
			{
				error_log("DataSingleton Init error!");
				return false;
			}
		}
		catch(const std::exception&)
		{
			error_log("DataSingleton Init error!");
			return false;
		}
	}
	for(vector<DataSingletonBase*>::iterator it=m_memoryManager.begin();it!=m_memoryManager.end();++it)
	{
		try
		{
			ret = (*it)->Init();
			if(ret)
			{
				error_log("MemorySingleton Init error!");
				return false;
			}
		}
		catch(const std::exception&)
		{
			error_log("MemorySingleton Init error!");
			return false;
		}
	}

	for(vector<DataSingletonBase*>::iterator it=m_dataManager.begin();it!=m_dataManager.end();++it)
	{
		try
		{
			ret = (*it)->OnInit();
			if(ret)
			{
				error_log("DataSingleton OnInit error!");
				return false;
			}
		}
		catch(const std::exception&)
		{
			error_log("DataSingleton OnInit error!");
			return false;
		}
	}
	for(vector<DataSingletonBase*>::iterator it=m_memoryManager.begin();it!=m_memoryManager.end();++it)
	{
		try
		{
			ret = (*it)->OnInit();
			if(ret)
			{
				error_log("MemorySingleton OnInit error!");
				return false;
			}
		}
		catch(const std::exception&)
		{
			error_log("MemorySingleton OnInit error!");
			return false;
		}
	}
	for(vector<BattleSingleton*>::iterator it=m_battleManager.begin();it!=m_battleManager.end();++it)
	{
		try
		{
			ret = (*it)->OnInit();
			if(ret)
			{
				error_log("BattleSingleton OnInit error!");
				return false;
			}
		}
		catch(const std::exception&)
		{
			error_log("BattleSingleton OnInit error!");
			return false;
		}
	}
	for(vector<ActivitySingletonBase*>::iterator it=m_activityManager.begin();it!=m_activityManager.end();++it)
	{
		try
		{
			(*it)->OnInit();
		}
		catch(const std::exception&) {}
	}

	return true;
}

bool  LogicManager::sendMsgFD(unsigned fd, Message* msg, bool delmsg)
{
	CFirePacket packet(PROTOCOL_ACCESS_SEND, delmsg);
	packet.ChannelId = channelId;
	packet.fd = fd;
	packet.m_msg = msg;
	return BattleServer::Instance()->SendData(&packet);
}
bool LogicManager::sendMsg(unsigned uid,Message* msg, bool delmsg)
{
	if(channelId == -1)
	{
		if(delmsg)
			delete msg;
		return false;
	}
	unsigned fd = Getfd(uid);
	if(fd == -1)
	{
		if(delmsg)
			delete msg;
		return false;
	}
	return sendMsgFD(fd, msg, delmsg);
}
bool LogicManager::sendMsgGroup(set<unsigned>& uid, Message* msg, bool delmsg)
{
	if(channelId == -1)
	{
		if(delmsg)
			delete msg;
		return false;
	}

	set<unsigned> fds, del;
	for(set<unsigned>::iterator it=uid.begin();it!=uid.end();++it)
	{
		unsigned fd = Getfd(*it);
		if(fd == -1)
		{
			del.insert(*it);
			continue;
		}
		fds.insert(fd);
	}
	for(set<unsigned>::iterator it=del.begin();it!=del.end();++it)
		uid.erase(*it);
	if(fds.empty())
	{
		if(delmsg)
			delete msg;
		return false;
	}

	CFirePacket packet(PROTOCOL_ACCESS_GROUP_SEND, delmsg);
	packet.ChannelId = channelId;
	packet.m_msg = msg;
	packet.group = &fds;

	return BattleServer::Instance()->SendData(&packet);
}
bool LogicManager::broadcastMsg(Message* msg)
{
	if(channelId == -1)
	{
		delete msg;
		return false;
	}
	CFirePacket packet(PROTOCOL_ACCESS_BROAD_CAST);
	packet.ChannelId = channelId;
	packet.m_msg = msg;

	return BattleServer::Instance()->SendData(&packet);
}
void LogicManager::process(CFirePacket* packet)
{
	if(packet->cmd < PROTOCOL_ACCESS_LOFFLINE){
		channelId = packet->ChannelId;
		m_fd = packet->fd;
	}

	switch(packet->cmd){
	case PROTOCOL_ACCESS_TRANSFER:
		clientProcess(packet);
		break;
	case PROTOCOL_DELIVER:
		deliverProcess(packet);
		break;
	case PROTOCOL_ADMIN:
		adminProcess(packet);
		break;
	case PROTOCOL_ACCESS_HEARBEAT:
		heartProcess(packet);
		break;
	case PROTOCOL_ACCESS_COFFLINE:
		preOffline(packet);
		break;
	case PROTOCOL_BOT:
		botProcess(packet);
		break;
	case PROTOCOL_EVENT_SECOND_TIMER:
		timerProcess(packet);
		break;
	default:
		error_log("unknown cmd:%u",packet->cmd);
		break;
	}
}

void LogicManager::clientProcess(CFirePacket* packet){
	if(IsPreClosed)
		return;

	int ret = 0;
	packet->uid = Getuid(packet->fd);
	m_errmsg.clear();
	if(!IsValidUid(packet->uid) && packet->m_msg->GetTypeName() != "Common.Login")
	{
		info_log("kick not login, fd=%u, cmd=%s", packet->fd, packet->m_msg->GetTypeName().c_str());
		sendKickMsg(packet->fd, "not_login");
		return;
	}

	ret = dispatcher.onMessage(packet->m_msg);

	if(ret != 0){
		error_log("client process failed, uid=%u, ret=%d, msg=%s, cmd=%s", packet->uid, ret, m_errmsg.c_str(), packet->m_msg->GetTypeName().c_str());
		ErrorRet::ErrorRet* reply = new ErrorRet::ErrorRet;
		reply->set_error_ret(ret);
		reply->set_error_msg(m_errmsg);
		reply->set_request_msg(packet->m_msg->GetTypeName());
		pReplyProtocol = reply;
		needDelReply = true;
	}
	else if(IsValidUid(packet->uid))
	{
		unsigned index = BaseManager::Instance()->GetIndex(packet->uid);
		DataBase &base = BaseManager::Instance()->m_data->data[index];
		base.last_active_time = Time::GetGlobalTime();
	}

	if(pReplyProtocol != NULL){
		CFirePacket rspPacket(PROTOCOL_ACCESS_ANSWER, needDelReply);
		rspPacket.fd = packet->fd;
		rspPacket.time = packet->time;
		rspPacket.microTime = packet->microTime;
		rspPacket.ChannelId = packet->ChannelId;
		rspPacket.m_msg = pReplyProtocol;

		if(!BattleServer::Instance()->SendData(&rspPacket))
			error_log("send rsp failed:[%u,%u,%u]",rspPacket.fd,rspPacket.time,rspPacket.microTime);

		pReplyProtocol = NULL;
	}
	needDelReply = true;
}
void LogicManager::deliverProcess(CFirePacket* packet){
	if(IsPreClosed)
		return;

	int ret = 0;
	Common::Pay* msg = (Common::Pay*)packet->m_msg;
	unsigned uid = msg->uid();
	string name;
	if(!UMI->IsOnline(uid))
		ret =  R_ERR_LOGIC;
	else
	{
		if(RMI->GetIndex(uid) == -1)
			ret =  R_ERR_LOGIC;
		else
		{
			unsigned ts = msg->ts();
			if(ts - 3 > Time::GetGlobalTime() || ts + 3 < Time::GetGlobalTime())
				ret =  R_ERR_PARAM;
			else
				name = string(GET_RMI(uid).name);
		}
	}
	if(ret != 0)
		error_log("deliver process failed, ret=%d",ret);

	Common::ReplyPay* reply = new Common::ReplyPay;
	reply->set_ret(ret);
	reply->set_name(name);
	CFirePacket rspPacket(PROTOCOL_DELIVER);
	rspPacket.fd = packet->fd;
	rspPacket.time = packet->time;
	rspPacket.microTime = packet->microTime;
	rspPacket.ChannelId = packet->ChannelId;
	rspPacket.m_msg = reply;

	if(!BattleServer::Instance()->SendData(&rspPacket))
		error_log("send rsp failed:[%u,%u,%u]",rspPacket.fd,rspPacket.time,rspPacket.microTime);

	if(ret == 0)
	{
		unsigned cash = msg->cash();
		unsigned index = BaseManager::Instance()->GetIndex(uid);
		DataBase &base = BaseManager::Instance()->m_data->data[index];
		COINS_LOG("[%s][uid=%u,ocash=%u,ncash=%u,chgcash=%d]", "DELIVER", uid, base.cash, base.cash+cash, cash);
		base.cash += cash;
		base.acccharge += cash;

		//@add oulong 20161009 首充
//		if (base.acccharge >= 100 && base.first_recharge == 0)
//		{
//			//base.first_recharge = 1;
//		}

		BaseManager::Instance()->m_data->MarkChange(index);

		LogicUserManager::Instance()->NotifyRecharge(uid, cash);

		DoDataManagerSave(uid);
	}
}
void LogicManager::adminProcess(CFirePacket* packet){
	if(IsPreClosed)
		return;

	int ret = 0;
	m_errmsg.clear();

	ret = dispatcher.onMessage(packet->m_msg);

	if(ret != 0){
		error_log("admin process failed, ret=%d, msg=%s",ret, m_errmsg.c_str());
		ErrorRet::ErrorRet* reply = new ErrorRet::ErrorRet;
		reply->set_error_ret(ret);
		reply->set_error_msg(m_errmsg);
		reply->set_request_msg(packet->m_msg->GetTypeName());
		pReplyProtocol = reply;
	}

	if(pReplyProtocol != NULL){
		CFirePacket rspPacket(PROTOCOL_ADMIN);
		rspPacket.fd = packet->fd;
		rspPacket.time = packet->time;
		rspPacket.microTime = packet->microTime;
		rspPacket.ChannelId = packet->ChannelId;
		rspPacket.m_msg = pReplyProtocol;

		if(!BattleServer::Instance()->SendData(&rspPacket))
			error_log("send rsp failed:[%u,%u,%u]",rspPacket.fd,rspPacket.time,rspPacket.microTime);

		pReplyProtocol = NULL;
	}
}
void LogicManager::botProcess(CFirePacket* packet){
	if(IsPreClosed)
		return;

	int ret = dispatcher.onMessage(packet->m_msg);

	/*if(packet->m_msg->GetTypeName() == "Bot.RequestLogin")
	{
		Bot::ReplyLogin* reply = new Bot::ReplyLogin;
		reply->set_ret(ret);
		pReplyProtocol = reply;
	}*/

	if(pReplyProtocol != NULL){
		CFirePacket rspPacket(PROTOCOL_BOT);
		rspPacket.fd = packet->fd;
		rspPacket.time = packet->time;
		rspPacket.microTime = packet->microTime;
		rspPacket.ChannelId = packet->ChannelId;
		rspPacket.m_msg = pReplyProtocol;

		if(!BattleServer::Instance()->SendData(&rspPacket))
			error_log("send rsp failed:[%u,%u,%u]",rspPacket.fd,rspPacket.time,rspPacket.microTime);

		pReplyProtocol = NULL;
	}

	m_errmsg.clear();
}

void LogicManager::heartProcess(CFirePacket* packet){
	//todo 此处是原来的心跳，应该无用
}

bool LogicManager::sendKickMsg(unsigned fd, string reason)
{
	if(channelId == -1 || fd == -1)
		return false;

	Common::Kick* m = new Common::Kick;
	m->set_reason(reason);
	sendMsgFD(fd, m);

	CFirePacket packet(PROTOCOL_ACCESS_LOFFLINE);
	packet.fd = fd;
	packet.ChannelId = channelId;
	return BattleServer::Instance()->SendData(&packet);
}
void LogicManager::forceKick(unsigned uid, string reason){
	sendKickMsg(Getfd(uid), reason);
	offline(uid);
	Eraseuid(uid);
}

void LogicManager::offline(unsigned uid)
{
	if(IsValidUid(uid)){
		//todo 此处是下线的逻辑处理
		UserManager::Instance()->UserOffLine(uid);
		//队列离线处理
		LogicQueueManager::Instance()->Offline(uid);
		//删除定时任务管理器中该用户的任务
		LogicRoutineManager::Instance()->Offline(uid);
	}
}

void LogicManager::timerProcess(CFirePacket* packet)
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	GlobalMilliTime = tv.tv_sec * 1000 + tv.tv_usec/1000 - StartMilliTime;

	//抛弃挤压消息，避免消息阻塞引起的雪崩
	if(GlobalMilliTime - lastLoopTime < PER_FRAME_TIME){
//		info_log("timer process time out time=%u",GlobalMilliTime - lastLoopTime);
		return;
	}

	if(IsPreClosed){
		static int c = 0;
		++c;
		if(c == 1)
			ClearUser(true);
		else if(c == 4){
			for(vector<ActivitySingletonBase*>::iterator it=m_activityManager.begin();it!=m_activityManager.end();++it)
			{
				try
				{
					if((*it)->IsOn())
						(*it)->OnExit();
				}
				catch(const std::exception&) {}
			}
			for(vector<BattleSingleton*>::iterator it=m_battleManager.begin();it!=m_battleManager.end();++it)
			{
				try
				{
					(*it)->OnExit();
				}
				catch(const std::exception&) {}
			}
			for(vector<DataSingletonBase*>::iterator it=m_memoryManager.begin();it!=m_memoryManager.end();++it)
			{
				try
				{
					(*it)->OnExit();
				}
				catch(const std::exception&) {}
			}
			for(vector<DataSingletonBase*>::iterator it=m_dataManager.begin();it!=m_dataManager.end();++it)
			{
				try
				{
					(*it)->OnExit();
				}
				catch(const std::exception&) {}
			}

			for(vector<DataSingletonBase*>::iterator it=m_memoryManager.begin();it!=m_memoryManager.end();++it)
			{
				try
				{
					(*it)->Exit();
				}
				catch(const std::exception&) {}
			}
			for(vector<DataSingletonBase*>::iterator it=m_dataManager.begin();it!=m_dataManager.end();++it)
			{
				try
				{
					(*it)->Exit();
				}
				catch(const std::exception&) {}
			}

			IsClosed = true;
			BattleServer::Instance()->Close();
		}
	}
	else
	{
		if(NeedReloadConfig)
		{
			NeedReloadConfig = false;
			if(ConfigManager::Reload())
			{
				info_log("ConfigManager Reload sucess!");
				OnReload();
			}
			else
				info_log("ConfigManager Reload fail!");
		}
		if(m_signum)
			CheckSig();

		++m_timer;
		unsigned now = Time::GetGlobalTime();
		if(CTime::IsDiffHour(m_last_hour_ts,now))
		{
			m_last_hour_ts = now;
			try
			{
				CheckHour();
			}
			catch(const std::exception& e)
			{
				error_log("err_msg: %s", e.what());
			}
		}
		if(m_timer % 60 == 0)
		{
			try
			{
				CheckMin();
			}
			catch(const std::exception& e)
			{
				error_log("err_msg: %s", e.what());
			}
		}
		for(list<pair<unsigned, unsigned> >::iterator it=m_leaveList.begin(); it!=m_leaveList.end();){
			if(OFFLINE_DELAY + it->first <= now){
				info_log("kick offline, uid=%u", it->second);
				offline(it->second);
				it = m_leaveList.erase(it);
			}
			else
				break;
		}

		for(vector<BattleSingleton*>::iterator it=m_battleManager.begin();it!=m_battleManager.end();++it)
		{
			try
			{
				(*it)->OnTimer1();
			}
			catch(const std::exception&) {}
		}
		for(vector<DataSingletonBase*>::iterator it=m_memoryManager.begin();it!=m_memoryManager.end();++it)
		{
			try
			{
				(*it)->OnTimer1();
			}
			catch(const std::exception&) {}
		}
		for(vector<DataSingletonBase*>::iterator it=m_dataManager.begin();it!=m_dataManager.end();++it)
		{
			try
			{
				(*it)->OnTimer1();
			}
			catch(const std::exception&) {}
		}

		for(vector<DataSingletonBase*>::iterator it=m_memoryManager.begin();it!=m_memoryManager.end();++it)
		{
			try
			{
				(*it)->Timer1();
			}
			catch(const std::exception&) {}
		}
		for(vector<DataSingletonBase*>::iterator it=m_dataManager.begin();it!=m_dataManager.end();++it)
		{
			try
			{
				(*it)->Timer1();
			}
			catch(const std::exception&) {}
		}

		if(m_timer % 3 == 0)
		{
			this->onTimer2();
		}
	}

	gettimeofday(&tv,NULL);
	lastLoopTime = tv.tv_sec * 1000 + tv.tv_usec/1000 - StartMilliTime;
	if(lastLoopTime - GlobalMilliTime > 40)
	{
		info_log("timer run time = %u",lastLoopTime - GlobalMilliTime);
	}
}

void LogicManager::preOffline(CFirePacket* packet)
{
	if(IsPreClosed)
		return;

	if(Getuid(packet->fd) == -1)
		return;

	info_log("kick pre offline, uid=%u, fd=%u", Getuid(packet->fd), packet->fd);
	m_leaveList.push_back(make_pair(Time::GetGlobalTime(), Getuid(packet->fd)));
	Erasefd(packet->fd);
}

int LogicManager::onTimer2()
{
	for(vector<BattleSingleton*>::iterator it=m_battleManager.begin();it!=m_battleManager.end();++it)
	{
		try
		{
			(*it)->OnTimer2();
		}
		catch(const std::exception&) {}
	}
	for(vector<DataSingletonBase*>::iterator it=m_memoryManager.begin();it!=m_memoryManager.end();++it)
	{
		try
		{
			(*it)->OnTimer2();
		}
		catch(const std::exception&) {}
	}
	for(vector<DataSingletonBase*>::iterator it=m_dataManager.begin();it!=m_dataManager.end();++it)
	{
		try
		{
			(*it)->OnTimer2();
		}
		catch(const std::exception&) {}
	}

	for(vector<DataSingletonBase*>::iterator it=m_memoryManager.begin();it!=m_memoryManager.end();++it)
	{
		try
		{
			(*it)->Timer2();
		}
		catch(const std::exception&) {}
	}
	for(vector<DataSingletonBase*>::iterator it=m_dataManager.begin();it!=m_dataManager.end();++it)
	{
		try
		{
			(*it)->Timer2();
		}
		catch(const std::exception&) {}
	}

	return 0;
}

LogicManager::~LogicManager()
{
	for(vector<ActivitySingletonBase*>::iterator it=m_activityManager.begin();it!=m_activityManager.end();++it)
	{
		try
		{
			(*it)->CallDestroy();
		}
		catch(const std::exception&) {}
	}
	for(vector<BattleSingleton*>::iterator it=m_battleManager.begin();it!=m_battleManager.end();++it)
	{
		try
		{
			(*it)->CallDestroy();
		}
		catch(const std::exception&) {}
	}
	for(vector<DataSingletonBase*>::iterator it=m_memoryManager.begin();it!=m_memoryManager.end();++it)
	{
		try
		{
			(*it)->CallDestroy();
		}
		catch(const std::exception&) {}
	}
	for(vector<DataSingletonBase*>::iterator it=m_dataManager.begin();it!=m_dataManager.end();++it)
	{
		try
		{
			(*it)->CallDestroy();
		}
		catch(const std::exception&) {}
	}
	ConfigManager::Instance()->Destroy();
}

void LogicManager::RegProto()
{
	//登录
	dispatcher.registerMessageCallback<Common::Login>(ProtoManager::ProcessLogin);
	//读取别人数据
	dispatcher.registerMessageCallback<User::RequestOtherUser>(ProtoManager::Process<User::RequestOtherUser, User::OtherUser, UserManager>);
	//新手
	dispatcher.registerMessageCallback<User::Tutorial_stage>(ProtoManager::ProcessNoReply<User::Tutorial_stage, UserManager>);
	//起名
	dispatcher.registerMessageCallback<Common::ChangeName>(ProtoManager::Process<Common::ChangeName, Common::ReplyChangeName, UserManager>);
	/***********GM********************/
	//加钱
	dispatcher.registerMessageCallback<Admin::AddCash>(ProtoManager::ProcessNoUID<Admin::AddCash, Admin::ReplyAddCash, UserManager>);
	//批量加资源或道具
	dispatcher.registerMessageCallback<Admin::AsycAdd>(ProtoManager::ProcessNoUID<Admin::AsycAdd, Admin::AsycAddResp, UserManager>);
	//查询封号
	dispatcher.registerMessageCallback<Admin::RequestForbidTS>(ProtoManager::ProcessNoUID<Admin::RequestForbidTS, Admin::ReplyForbidTS, UserManager>);
	//封号
	dispatcher.registerMessageCallback<Admin::SetForbidTS>(ProtoManager::ProcessNoReplyNoUID<Admin::SetForbidTS, UserManager>);

	//钻石消耗
	dispatcher.registerMessageCallback<User::CostCashReq>(ProtoManager::Process<User::CostCashReq, User::CostCashResp, LogicUserManager>);
	dispatcher.registerMessageCallback<Common::ShutDown>(ProtoManager::ProcessNoReply<Common::ShutDown, UserManager>);
	//购买材料
	dispatcher.registerMessageCallback<User::BuyMaterialReq>(ProtoManager::Process<User::BuyMaterialReq, User::BuyMaterialResp, LogicPropsManager>);

	//内政
	//建造
	dispatcher.registerMessageCallback<ProtoBuilding::BuildReq>(ProtoManager::Process<ProtoBuilding::BuildReq, ProtoBuilding::BuildResp, LogicBuildManager>);
	//移动
	dispatcher.registerMessageCallback<ProtoBuilding::MoveReq>(ProtoManager::Process<ProtoBuilding::MoveReq, ProtoBuilding::MoveResp, LogicBuildManager>);
	//翻转
	dispatcher.registerMessageCallback<ProtoBuilding::FlipReq>(ProtoManager::Process<ProtoBuilding::FlipReq, ProtoBuilding::FlipResp, LogicBuildManager>);
	//仓库升级
	dispatcher.registerMessageCallback<ProtoBuilding::BuildingUpReq>(ProtoManager::Process<ProtoBuilding::BuildingUpReq, ProtoBuilding::BuildingUpResp, LogicBuildManager>);

	//通用加速
	dispatcher.registerMessageCallback<User::SpeedUpReq>(ProtoManager::Process<User::SpeedUpReq, User::SpeedUpResp, LogicQueueManager>);

	//---------------地块生产线
	//种植
	dispatcher.registerMessageCallback<ProtoProduce::PlantCropReq>(ProtoManager::Process<ProtoProduce::PlantCropReq, ProtoProduce::PlantCropResp, LogicProductLineManager>);
	//收割
	dispatcher.registerMessageCallback<ProtoProduce::ReapCropReq>(ProtoManager::Process<ProtoProduce::ReapCropReq, ProtoProduce::ReapCropResp, LogicProductLineManager>);

	//---------------设备生产
	//扩展队列
	dispatcher.registerMessageCallback<ProtoProduce::ExpandQueueReq>(ProtoManager::Process<ProtoProduce::ExpandQueueReq, ProtoProduce::ExpandQueueResp, LogicProductLineManager>);
	//放入生产队列
	dispatcher.registerMessageCallback<ProtoProduce::JoinQueueReq>(ProtoManager::Process<ProtoProduce::JoinQueueReq, ProtoProduce::JoinQueueResp, LogicProductLineManager>);
	//取回仓库
	dispatcher.registerMessageCallback<ProtoProduce::FetchProductReq>(ProtoManager::Process<ProtoProduce::FetchProductReq, ProtoProduce::FetchProductResp, LogicProductLineManager>);

	//----------------动物生产
	//领养动物
	dispatcher.registerMessageCallback<ProtoProduce::AdoptAnimalReq>(ProtoManager::Process<ProtoProduce::AdoptAnimalReq, ProtoProduce::AdoptAnimalResp, LogicProductLineManager>);
	//喂养动物
	dispatcher.registerMessageCallback<ProtoProduce::FeedAnimalReq>(ProtoManager::Process<ProtoProduce::FeedAnimalReq, ProtoProduce::FeedAnimalResp, LogicProductLineManager>);
	//获取产品
	dispatcher.registerMessageCallback<ProtoProduce::ObtainProductReq>(ProtoManager::Process<ProtoProduce::ObtainProductReq, ProtoProduce::ObtainProductResp, LogicProductLineManager>);

	//GM
	dispatcher.registerMessageCallback<ProtoGM::GMCmdReq>(ProtoManager::ProcessNoReply<ProtoGM::GMCmdReq, LogicGM>);

	//通知系统
	dispatcher.registerMessageCallback<ProtoNotify::GetNotifyReq>(ProtoManager::Process<ProtoNotify::GetNotifyReq, ProtoNotify::GetNotifyResp, LogicNotifyManager>);
}

void LogicManager::RegMemoryManager()
{
	//下面的放在最前，顺序不要变
	m_memoryManager.push_back(ResourceManager::Instance());
	m_memoryManager.push_back(AsynManager::Instance());
	m_memoryManager.push_back(NotifyManager::Instance());
	//上面的放在最前，顺序不要变
}

void LogicManager::RegDataManager()
{
	m_dataManager.push_back(BaseManager::Instance());
	m_dataManager.push_back(DataGameActivityManager::Instance());
	m_dataManager.push_back(DataChargeHistoryManager::Instance());
	m_dataManager.push_back(DataBuildingMgr::Instance());
	m_dataManager.push_back(DataCroplandManager::Instance());
	m_dataManager.push_back(DataItemManager::Instance());
	m_dataManager.push_back(DataProduceequipManager::Instance());
	m_dataManager.push_back(DataAnimalManager::Instance());
	m_dataManager.push_back(DataEquipmentStarManager::Instance());
}

void LogicManager::RegBattleManager()
{
	m_battleManager.push_back(LogicUserManager::Instance());
	m_battleManager.push_back(LogicNotifyManager::Instance());
	m_battleManager.push_back(LogicBuildManager::Instance());
	m_battleManager.push_back(LogicProductLineManager::Instance());
	m_battleManager.push_back(LogicPropsManager::Instance());
	m_battleManager.push_back(LogicQueueManager::Instance());
	m_battleManager.push_back(LogicRoutineManager::Instance());

	//下面的放在最后，顺序不要变
	m_battleManager.push_back(UserManager::Instance());
	//上面的放在最后，顺序不要变
}

void LogicManager::RegActivityManager()
{
}

bool LogicManager::IsDataManagerWorking()
{
	for(vector<DataSingletonBase*>::iterator it=m_dataManager.begin();it!=m_dataManager.end();++it)
	{
		if((*it)->IsWorking())
			return true;
	}
	return false;
}
bool LogicManager::IsDataManagerFull()
{
	for(vector<DataSingletonBase*>::iterator it=m_dataManager.begin();it!=m_dataManager.end();++it)
	{
		if((*it)->IsFull())
			return true;
	}
	return false;
}
bool LogicManager::IsDataManagerNeedClear()
{
	for(vector<DataSingletonBase*>::iterator it=m_dataManager.begin();it!=m_dataManager.end();++it)
	{
		if((*it)->IsNeedClear())
			return true;
	}
	return false;
}
bool LogicManager::IsMemoryManagerNeedClear()
{
	for(vector<DataSingletonBase*>::iterator it=m_memoryManager.begin();it!=m_memoryManager.end();++it)
	{
		if((*it)->IsNeedClear())
			return true;
	}
	return false;
}
void LogicManager::DoDataManagerSave(unsigned uid)
{
	for(vector<DataSingletonBase*>::iterator it=m_dataManager.begin();it!=m_dataManager.end();++it)
	{
		try
		{
			(*it)->DoSave(uid);
		}
		catch(const std::exception&) {}
	}

}
void LogicManager::DoDataManagerClear(unsigned uid)
{
	for(vector<DataSingletonBase*>::iterator it=m_dataManager.begin();it!=m_dataManager.end();++it)
	{
		try
		{
			(*it)->DoClear(uid);
		}
		catch(const std::exception&) {}
	}

}
void LogicManager::DoMemoryManagerClear(unsigned uid)
{
	for(vector<DataSingletonBase*>::iterator it=m_memoryManager.begin();it!=m_memoryManager.end();++it)
	{
		try
		{
			(*it)->DoClear(uid);
		}
		catch(const std::exception&) {}
	}
}

void LogicManager::Addfd(unsigned uid, unsigned fd)
{
	m_fdmap[fd] = uid;
	m_uidmap[uid] = fd;
}
void LogicManager::Erasefd(unsigned fd)
{
	if(m_fdmap.count(fd))
	{
		m_uidmap.erase(m_fdmap[fd]);
		m_fdmap.erase(fd);
	}
}
void LogicManager::Eraseuid(unsigned uid)
{
	if(m_uidmap.count(uid))
	{
		m_fdmap.erase(m_uidmap[uid]);
		m_uidmap.erase(uid);
	}
}
unsigned LogicManager::Getfd(unsigned uid)
{
	if(m_uidmap.count(uid))
		return m_uidmap[uid];
	return -1;
}
unsigned LogicManager::Getuid(unsigned fd)
{
	if(m_fdmap.count(fd))
		return m_fdmap[fd];
	return -1;
}

void LogicManager::EraseLeaveList(unsigned uid)
{
	for(list<pair<unsigned, unsigned> >::iterator it = m_leaveList.begin();it != m_leaveList.end();++it)
	{
		if(it->second == uid)
		{
			m_leaveList.erase(it);
			return;
		}
	}
}
void LogicManager::ClearUser(bool send)
{
	vector<unsigned> uids;
	UMI->GetOnlineUsers(uids);

	info_log("kick close %u", uids.size());
	for(vector<unsigned>::iterator it=uids.begin();it!=uids.end();++it)
	{
		if(send)
			sendKickMsg(Getfd(*it), "server_close");
		offline(*it);
	}

	m_fdmap.clear();
	m_uidmap.clear();
	m_leaveList.clear();
	channelId = -1;
	m_fd = -1;
}
unsigned LogicManager::GetOpenDays()
{
	return CTime::GetDayInterval(SecOpenTime, Time::GetGlobalTime());
}
void LogicManager::CheckSave(){
	debug_log("---------------------------");
	debug_log("CheckSave!");
	debug_log("---------------------------");
	for(vector<DataSingletonBase*>::iterator it=m_dataManager.begin();it!=m_dataManager.end();++it)
		(*it)->OnCheck();
}
void LogicManager::DataLog(){
	debug_log("---------------------------");
	for(vector<DataSingletonBase*>::iterator it=m_dataManager.begin();it!=m_dataManager.end();++it)
		(*it)->DebugLog();
	debug_log("---------------------------");
}
void LogicManager::TryClear()
{
	debug_log("---------------------------");
	debug_log("%s",IsDataManagerWorking()?"DataManager Working":"DataManager not Working");
	debug_log("%s",IsDataManagerNeedClear()?"DataManager Need Clear":"DataManager not Need Clear");
	vector<unsigned> uids;
	BaseManager::Instance()->TryClear(uids);
	debug_log("clear num: %u",uids.size());
	debug_log("---------------------------");
	debug_log("%s",IsMemoryManagerNeedClear()?"MemoryManager Need Clear":"MemoryManager not Need Clear");
	vector<unsigned> uids1;
	ResourceManager::Instance()->TryClear(uids1);
	debug_log("clear num: %u",uids1.size());
	debug_log("---------------------------");
}

void LogicManager::CheckMin()
{
	info_log("Online:%u",m_fdmap.size());

	User::ServerTime * msg = new User::ServerTime;
	msg->set_ts(now);
	broadcastMsg(msg);

	for(vector<ActivitySingletonBase*>::iterator it=m_activityManager.begin();it!=m_activityManager.end();++it)
	{
		try
		{
			if((*it)->IsOn())
				(*it)->OnMin();
		}
		catch(const std::exception&) {}
	}
}

void LogicManager::CheckHour()
{
	m_timer = 0;

	if(LogicCommonUtil::GetHourByTime(Time::GetGlobalTime()) == 0)
		CheckDay();

	for(vector<ActivitySingletonBase*>::iterator it=m_activityManager.begin();it!=m_activityManager.end();++it)
	{
		try
		{
			if((*it)->IsOn())
				(*it)->OnHour();
		}
		catch(const std::exception&) {}
	}
}

void LogicManager::CheckDay()
{
	for(vector<ActivitySingletonBase*>::iterator it=m_activityManager.begin();it!=m_activityManager.end();++it)
	{
		try
		{
			(*it)->CheckDay();
		}
		catch(const std::exception&) {}
	}

	LogicUserManager::Instance()->EveryDayAction();
}

void LogicManager::OnReload()
{
	for(vector<ActivitySingletonBase*>::iterator it=m_activityManager.begin();it!=m_activityManager.end();++it)
	{
		try
		{
			(*it)->OnReload();
		}
		catch(const std::exception&) {}
	}
}

void LogicManager::CheckSig(){
	switch(m_signum)
	{
	case e_Sig_Save:
		CheckSave();
		break;
	case e_Sig_data_log:
		DataLog();
		break;
	case e_Sig_try_clear:
		TryClear();
		break;
	case e_Sig_print_info:
		LogicResourceManager::Instance()->Print();
		break;
	default:
		break;
	}
	m_signum = 0;
}
