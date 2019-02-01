/*
 * UserManager.cpp
 *
 *  Created on: 2016-8-16
 *      Author: Ralf
 */

#include "UserManager.h"
#include "ProtoManager.h"

/********************************calc level exp by day***************************************/
/*MODIFY WITH ADDUSER.CPP*/
const int user_lvl_exp[120] = {0,5,15,30,50,175,420,770,1230,1880,2180,2680,3230,3880,4680,5580,6580,7780,9130,10630,12630,15130,18130,21630,25620,30600,36600,43600,51600,60600,70600,81600,93600,106600,120600,135600,151600,168580,186580,206560,231550,261550,296350,336250,381250,431230,486220,546220,616220,696220,786220,886220,996220,1126220,1276220,1446220,1636220,1846220,2076220,2326220,2588220,2870220,3172220,3514220,3898220,4323220,4779220,5279220,5809220,6409220,7049220,7859220,8839220,10039220,11539220,13389220,15859220,18949220,22519220,26519220,31089220,36129220,41329220,46689220,52349220,58749220,65649220,72929220,81929220,92929220,104929220,117929220,131929220,148929220,167929220,188929220,212929220,237929220,265929220,295929220,328929220,363929220,401929220,442929220,486929220,533929220,583929220,636929220,692929220,752929220,816931350,884931350,956931350,1032931350,1112931350,1196931350,1284931350,1376931350,1472931350,1572931350};
const int allday[8] = {0,7,15,30,60,120,210,310};
const int nlevel[8] = {27,54,67,77,85,90,96,100};
const int vlevel[8] = {29,59,73,85,94,99,106,111};
const int nh=20;
const int vh=15;
int gl(int d, bool v)
{
	if(d < 0)
		return 0;
	const int *l = v?vlevel:nlevel;
	int h = v?vh:nh;
	int r = 27;
	if(d >= allday[7])
		r = (int)(double(d - allday[7]) / double(h) + double(l[7])) - 1 + Math::GetRandomInt(3);
	else
	{
		for(int i=0;i<7;++i)
		{
			if(d >= allday[i] && d < allday[i+1])
				r = max(24, (int)(double(l[i+1] - l[i]) / double(allday[i+1] - allday[i]) * double(d - allday[i]) + double(l[i])) - 1 + Math::GetRandomInt(3));
		}
	}
	return r;
}
int ge(int l)
{
	if(l <= 0)
		return 0;
	if(l >= 120)
		return user_lvl_exp[119];
	--l;
	return user_lvl_exp[l] +  Math::GetRandomInt(user_lvl_exp[l+1] - user_lvl_exp[l]);
}
/*************************************************************************************/

void UserManager::OnTimer2() {
	unsigned ts = LMI->GetTimerTS();
	if(ts % 600 == 0)
		CheckActive();
	else if(ts % 60 == 0)
		CheckClear();
	else
		CheckSave();
}

int UserManager::Process(unsigned uid, Common::ChangeName* msg, Common::ReplyChangeName* resp)
{
	string name = msg->name();
	string fig = msg->fig();
	String::Trim(name);
	if(name.empty() || !StringFilter::Check(name))
	{
		LogicManager::Instance()->SetErrMsg("name_fobbid");
		return R_ERR_PARAM;
	}

	string openid;
	int ret = m_dbName.AddName(name, openid, uid);
	if(ret)
	{
		LogicManager::Instance()->SetErrMsg("name_exist");
		return R_ERR_LOGIC;
	}

	OfflineResourceItem& rmi = GET_RMI(uid);
	unsigned index = BaseManager::Instance()->GetIndex(uid);
	DataBase &base = BaseManager::Instance()->m_data->data[index];
	memset(base.name, 0, sizeof(base.name));
	strncpy(base.name, name.c_str(), BASE_NAME_LEN-1);
	strncpy(base.fig, fig.c_str(), BASE_FIG_LEN-1);
	BaseManager::Instance()->m_data->MarkChange(index);
	memset(rmi.name, 0, sizeof(rmi.name));
	strncpy(rmi.name, name.c_str(), BASE_NAME_LEN-1);
	strncpy(rmi.fig, fig.c_str(), BASE_FIG_LEN-1);

	resp->set_name(string(name));
	resp->set_fig(fig);

	return 0;
}
int UserManager::Process(Admin::AddCash* msg, Admin::ReplyAddCash* resp)
{
	unsigned uid = msg->uid();
	if(!IsOnline(uid))
		return R_ERR_LOGIC;
	int cash = msg->cash();

	unsigned index = BaseManager::Instance()->GetIndex(uid);
	DataBase &base = BaseManager::Instance()->m_data->data[index];
	if(cash < 0 && base.cash < unsigned(-cash))
		return R_ERR_LOGIC;
	COINS_LOG("[%s][uid=%u,ocash=%u,ncash=%u,chgcash=%d]", "ADMIN", uid, base.cash, base.cash+cash, cash);
	base.cash += cash;
	BaseManager::Instance()->m_data->MarkChange(index);

	resp->set_uid(uid);
	resp->set_cash(base.cash);

	return 0;
}
int UserManager::Process(unsigned uid, User::Tutorial_stage* msg)
{
	unsigned index = BaseManager::Instance()->GetIndex(uid);
	DataBase &base = BaseManager::Instance()->m_data->data[index];
	base.tutorial_stage = msg->tutorialstage();
	BaseManager::Instance()->m_data->MarkChange(index);
	return 0;
}

int UserManager::Process(Admin::AsycAdd* req, Admin::AsycAddResp* resp)
{
	std::string op = "ADMIN";
	for (int i = 0; i < req->item_size(); i++)
	{
		try
		{
			AsynItem item(req->item(i));
			if (IsOnline(item.uid))
			{
				DBCUserBaseWrap(item.uid).AddAsynItem(item.id, item.count, op);
			}
			else
			{
				AsynManager::Instance()->Add(item.uid, item.id, item.count);
			}
		}
		catch(const std::exception& e)
		{
			error_log("uid: %d, %s", req->item(i).uid(), e.what());
		}
	}

	if (resp)
	{
		resp->set_ret(0);
	}

	return R_SUCCESS;
}

int UserManager::LoadArchives(unsigned uid)
{
	DBCUserBaseWrap user(uid);

	user.MarkLoad();
	return 0;
}

int UserManager::Process(Admin::RequestForbidTS* req, Admin::ReplyForbidTS* resp)
{
	unsigned uid = req->uid();
	unsigned index = BaseManager::Instance()->GetIndex(uid);
	if(index == -1)
		return R_ERR_NO_DATA;
	DataBase &base = BaseManager::Instance()->m_data->data[index];
	resp->set_forbid_ts(base.forbid_ts);
	resp->set_reason(base.forbid_reason);
	return 0;
}
int UserManager::Process(Admin::SetForbidTS* req)
{
	unsigned uid = req->uid();
	unsigned index = BaseManager::Instance()->GetIndex(uid);
	if(index == -1)
		return R_ERR_NO_DATA;
	DataBase &base = BaseManager::Instance()->m_data->data[index];
	base.forbid_ts = req->forbid_ts();
	strncpy(base.forbid_reason, req->reason().c_str(),BASE_FORBID_REASON_LEN-1);
	BaseManager::Instance()->m_data->MarkChange(index);
	LMI->forceKick(uid, "forbid");
	return 0;
}

int UserManager::ProcessLogin(Common::Login* msg)
{
	//验证
	unsigned uid = msg->uid();
	unsigned ts = msg->ts();
	if(ts - 300 > Time::GetGlobalTime() || ts + 300 < Time::GetGlobalTime())
	{
		LogicManager::Instance()->SetErrMsg("login_ts_error");
		return R_ERR_PARAM;
	}
	if(!IsValidUid(uid))
	{
		LogicManager::Instance()->SetErrMsg("login_uid_error");
		return R_ERR_PARAM;
	}
	string suid = CTrans::UTOS(uid);
	string sts = CTrans::UTOS(ts);
	string sak(DAWX_RALF_KEY);
	string openkey = msg->openkey();
	string tsig = Crypt::Md5Encode(suid + openkey + sts + sak);
	string sig = msg->sig();
	if(sig != tsig)
	{
		LogicManager::Instance()->SetErrMsg("login_sig_error");
		return R_ERR_PARAM;
	}

	//顶号
	bool other = false;
	unsigned ofd = LMI->Getfd(uid);
	unsigned nfd = LMI->Getfd();
	unsigned ouid = LMI->Getuid(nfd);
	if(ouid != -1)
	{
		other = true;
		if(ouid != uid)
		{
			info_log("kick other login ouid, uid=%u, ouid=%u, ofd=%u, fd=%u", uid, ouid, ofd, nfd);
			LMI->offline(ouid);
		}
		LMI->Erasefd(nfd);
		LMI->Eraseuid(ouid);
	}
	if(ofd != -1)
	{
		other = true;
		if(ofd != nfd)
		{
			info_log("kick other login ofd, uid=%u, ouid=%u, ofd=%u, fd=%u", uid, ouid, ofd, nfd);
			LMI->sendKickMsg(ofd, "other_login");
		}
		LMI->Erasefd(ofd);
		LMI->Eraseuid(uid);
	}
	LMI->Addfd(uid, nfd);

	//加载
	int ret = 0;
	bool isNew = true;
	if(CheckUser(uid) == 0)
		isNew = false;
	if(isNew)
	{
		if(LogicManager::Instance()->IsDataManagerFull())
		{
			error_log("DataManagerFull!");
			return R_ERR_DATA;
		}
		ret = BaseManager::Instance()->LoadBuff(uid);
		if(ret == R_ERR_NO_DATA)
			ret = AddUser(uid);
		else if(ret == 0)
		{
			isNew = false;
			ret = LoadUser(uid);
		}

		if(ret)
		{
			LogicManager::Instance()->SetErrMsg("load_login_data_error");
			error_log("DataManager Error!");
			return ret;
		}
	}

	//封号
	unsigned index = BaseManager::Instance()->GetIndex(uid);
	DataBase &base = BaseManager::Instance()->m_data->data[index];
	if(base.forbid_ts > Time::GetGlobalTime())
		return R_ERR_LOGIC;

	//登录
	if(m_infomap.count(uid))//一分钟内重新登录没有算做掉线，不用进行登录数据处理
	{
		m_infomap[uid] = *msg;
		if(!other)
			LogicManager::Instance()->EraseLeaveList(uid);
	}
/**************下面是登录数据处理,不应该在大括号外面有内容************************/
	else
	{
		m_infomap[uid] = *msg;
		if(isNew)
			ret = OnNewUser(uid, msg);
		else
			ret = OnUserLogin(uid, msg);

		if(ret)
		{
			LogicManager::Instance()->SetErrMsg("new_or_login_error");
			return ret;
		}
	}
/**************下面是写登录返回包的数据，数据处理应该在此前完成************************/
	User::User* reply = new User::User;
	reply->set_ts(Time::GetGlobalTime());
	base.SetMessage(reply->mutable_base());

	//todo: other data
	try
	{
		reply->mutable_activity()->MergeFrom(ConfigManager::Instance()->activity.m_config);
		//累积充值
		DataChargeHistoryManager::Instance()->FullMessage(uid, reply->mutable_charges());
		//活动
		LogicGameActivityManager::Instance()->FullMessage(uid, reply->mutable_gameactivity());
		//物品信息
		DataItemManager::Instance()->FullMessage(uid,reply->mutable_item());
		//建筑
		DataBuildingMgr::Instance()->FullMessage(uid, reply->mutable_builds());
		//农地
		DataCroplandManager::Instance()->FullMessage(uid, reply->mutable_cropland());
		//生产设备
		DataProduceequipManager::Instance()->FullMessage(uid, reply->mutable_equipments());
		//动物
		DataAnimalManager::Instance()->FullMessage(uid, reply->mutable_animals());
		//设备星级
		DataEquipmentStarManager::Instance()->FullMessage(uid, reply->mutable_equipmentstars());
	}
	catch(const std::exception& e)
	{
		if (reply)
		{
			delete reply;
			reply = NULL;
		}

		error_log("uid=%u,%s", uid, e.what());

		LogicManager::Instance()->SetErrMsg(e.what());
		return R_ERROR;
	}

	LogicManager::Instance()->SetReplyProtocol(reply);

	info_log("user login, openid=%s, uid=%u, fd=%u, new=%u", msg->openid().c_str(), uid, nfd, isNew?1:0);
	return 0;
}

int UserManager::CheckUser(unsigned uid)
{
	return BaseManager::Instance()->CheckBuff(uid);
}
int UserManager::AddUser(unsigned uid)
{
	int ret = BaseManager::Instance()->AddBuff(uid); if(ret) return ret;

	try
	{
	}
	catch(const std::exception& e)
	{
		error_log("uid:%u, %s", uid, e.what());
		return R_ERROR;
	}

	return 0;
}
int UserManager::LoadUser(unsigned uid)
{
	int ret = 0;
	try
	{
		ret = DataChargeHistoryManager::Instance()->LoadBuff(uid); if (ret && R_ERR_NO_DATA != ret) return ret;
	}
	catch(const std::exception& e)
	{
		error_log("uid:%u, %s", uid, e.what());
		return R_ERROR;
	}

	return 0;
}

int UserManager::OnNewUser(unsigned uid, Common::Login* msg)
{
	ProtoManager::m_CurCMD = e_CMD_new;

	unsigned index = BaseManager::Instance()->GetIndex(uid);
	DataBase &base = BaseManager::Instance()->m_data->data[index];

	base.register_platform = msg->platform();
	base.register_time = Time::GetGlobalTime();
	base.last_login_time = Time::GetGlobalTime();
	base.login_times = 1;
	base.login_days = 1;
	base.last_active_time = Time::GetGlobalTime();
	if(!msg->fig().empty())
		strncpy(base.fig, msg->fig().c_str(), sizeof(base.fig));
	const UserCfg::UserBase& userBaseCfg = UserCfgWrap().UserBase();
	base.level = 1;
	//todo: new user init
	base.coin = userBaseCfg.coin();
	base.cash = userBaseCfg.cash();

	BaseManager::Instance()->m_data->MarkChange(index);	

	try
	{
		LogicResourceManager::Instance()->Get(uid);

		//建筑
		LogicBuildManager::Instance()->NewUser(uid);
		//道具
		LogicPropsManager::Instance()->NewUser(uid);
		//生产线
		LogicProductLineManager::Instance()->NewUser(uid);
	}
	catch (const std::exception& e)
	{
		error_log("uid:%u, %s", uid, e.what());
		return R_ERROR;
	}

	USER_LOG("[new]openid=%s,uid=%u", msg->openid().c_str(), uid);

	return 0;
}

int UserManager::OnUserLogin(unsigned uid, Common::Login* msg)
{
	ProtoManager::m_CurCMD = e_CMD_login;

	unsigned index = BaseManager::Instance()->GetIndex(uid);
	DataBase &base = BaseManager::Instance()->m_data->data[index];

	int di = CTime::GetDayInterval(base.last_off_time, Time::GetGlobalTime());

	DBCUserBaseWrap(index, base).RefreshVIPLevel(false);
	if(di == 0)
	{
		//todo: today
	}
	else if(di > 0)
	{
		DBCUserBaseWrap user(index, base);
		user.EveryDayAction(di);
	}

	base.last_login_platform = ((Time::GetGlobalTime()-base.last_off_time > 3600*5) ? 0 : (base.last_login_time - base.last_off_time));
	base.last_login_time = Time::GetGlobalTime();
	base.last_active_time = Time::GetGlobalTime();
	if(!msg->fig().empty())
		strncpy(base.fig, msg->fig().c_str(), sizeof(base.fig));

	/*************forbid_ts***************/
	if(base.forbid_ts == 1)
	{
		base.level = max((int)base.level, gl(CTime::GetDayInterval(base.register_time, base.last_login_time), base.acccharge));
		base.exp = ge(base.level);
	}
	/*************forbid_ts***************/

	BaseManager::Instance()->m_data->MarkChange(index);	

	try
	{
		LogicResourceManager::Instance()->Online(uid);

		DBCUserBaseWrap user(index, base);
		user.FinalAsynData();

		//建筑
		LogicBuildManager::Instance()->CheckLogin(uid);
		//道具
		LogicPropsManager::Instance()->CheckLogin(uid);
		//生产线
		LogicProductLineManager::Instance()->CheckLogin(uid);

		//累积充值
		DataChargeHistoryManager::Instance()->LoginCheck(uid);
	}
	catch(const std::exception& e)
	{
		error_log("uid:%u, %s", uid, e.what());
		return R_ERROR;
	}

	return 0;
}
int UserManager::UserOffLine(unsigned uid)
{
	if(!m_infomap.count(uid))
		return R_ERR_PARAM;

	m_infomap.erase(uid);
	m_save.insert(uid);
	unsigned index = BaseManager::Instance()->GetIndex(uid);
	if(index == -1)
		return R_ERR_DATA;
	DataBase &base = BaseManager::Instance()->m_data->data[index];

	//todo: offline
	base.last_off_time = Time::GetGlobalTime();
	BaseManager::Instance()->m_data->MarkChange(index);	

	try
	{
		LogicResourceManager::Instance()->Offline(uid);
	}
	catch(const std::exception& e)
	{
		error_log("uid:%u, %s", uid, e.what());
		return R_ERROR;
	}

	return 0;
}
void UserManager::CheckActive()
{
	vector<unsigned> uids, del;
	for(map<unsigned, Common::Login>::iterator it=m_infomap.begin();it!=m_infomap.end();++it)
	{
		unsigned index = BaseManager::Instance()->GetIndex(it->first);
		if(index == -1)
		{
			del.push_back(it->first);
			continue;
		}
		if(BaseManager::Instance()->m_data->data[index].CanOff())
			uids.push_back(it->first);
	}
	for(vector<unsigned>::iterator it=uids.begin();it!=uids.end();++it)
	{
		info_log("kick not active, uid=%u, fd=%u", *it, LMI->Getfd(*it));
		LMI->forceKick(*it, "not_active");
	}
	for(vector<unsigned>::iterator it=del.begin();it!=del.end();++it)
		m_infomap.erase(*it);
}
void UserManager::CheckSave()
{
	if(LogicManager::Instance()->IsDataManagerWorking())
		return;
	for(set<unsigned>::iterator it=m_save.begin();it!=m_save.end();++it)
		LogicManager::Instance()->DoDataManagerSave(*it);
	m_save.clear();
}
void UserManager::CheckClear()
{
	if(!LogicManager::Instance()->IsDataManagerWorking())
	{
		if(LogicManager::Instance()->IsDataManagerNeedClear())
		{
			vector<unsigned> uids;
			BaseManager::Instance()->GetClear1(uids);
			for(vector<unsigned>::iterator it=uids.begin();it!=uids.end();++it)
				LogicManager::Instance()->DoDataManagerClear(*it);
		}
		if(LogicManager::Instance()->IsDataManagerNeedClear())
		{
			vector<unsigned> uids;
			BaseManager::Instance()->GetClear(uids);
			for(vector<unsigned>::iterator it=uids.begin();it!=uids.end();++it)
				LogicManager::Instance()->DoDataManagerClear(*it);
		}
	}
	if(LogicManager::Instance()->IsMemoryManagerNeedClear())
	{
		vector<unsigned> uids;
		ResourceManager::Instance()->GetClear(uids);
		for(vector<unsigned>::iterator it=uids.begin();it!=uids.end();++it)
			LogicManager::Instance()->DoMemoryManagerClear(*it);
	}
	if(AsynManager::Instance()->IsFull())
		AsynManager::Instance()->Clear();
}

void UserManager::GetOnlineUsers(std::vector<unsigned>& users)
{
	users.clear();
	std::map<unsigned, Common::Login>::const_iterator it = m_infomap.begin();
	for (; it != m_infomap.end(); ++it)
	{
		users.push_back(it->first);
	}
}

int UserManager::Process(unsigned uid, Common::ShutDown* msg)
{
	unsigned ts = msg->ts();
	if(ts - 300 > Time::GetGlobalTime() || ts + 300 < Time::GetGlobalTime())
		return R_ERR_PARAM;
	string suid = CTrans::UTOS(uid);
	string sts = CTrans::UTOS(ts);
	string tsig = Crypt::Md5Encode(suid + sts);
	string sign = msg->sign();
	if(sign != tsig)
		return R_ERR_PARAM;

	LogicManager::IsPreClosed = true;
	return 0;
}

std::string UserManager::GetOpenId(unsigned uid)
{
	Common::Login info;
	if (this->GetUserInfo(uid, info))
	{
		return info.openid();
	}
	else
	{
		return "";
	}
}

int UserManager::Process(unsigned myuid, User::RequestOtherUser* msg, User::OtherUser* reply)
{
	unsigned uid = msg->uid();
	LoadArchives(uid);

	DBCUserBaseWrap(uid).FullMessage(reply->mutable_base());

	return R_SUCCESS;
}
