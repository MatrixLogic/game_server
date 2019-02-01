/*
 * UserManager.h
 *
 *  Created on: 2016-8-16
 *      Author: Ralf
 */

#ifndef USERMANAGER_H_
#define USERMANAGER_H_

#include "ServerInc.h"

class UserManager : public BattleSingleton, public CSingleton<UserManager>
{
private:
	friend class CSingleton<UserManager>;
	UserManager(){};
	virtual ~UserManager(){}

	map<unsigned, Common::Login> m_infomap;
	set<unsigned> m_save;

	CDataName m_dbName;
public:
	virtual void CallDestroy() {Destroy();}
	virtual void OnTimer2();

	int ProcessLogin(Common::Login* msg);
	int Process(unsigned myuid, User::RequestOtherUser* msg, User::OtherUser* reply);
	int Process(unsigned uid, Common::ChangeName* msg, Common::ReplyChangeName* resp);
	int Process(Admin::AddCash* msg, Admin::ReplyAddCash* resp);
	int Process(unsigned uid, User::Tutorial_stage* msg);
	int Process(Admin::AsycAdd* req, Admin::AsycAddResp* resp);
	int Process(Admin::RequestForbidTS* req, Admin::ReplyForbidTS* resp);
	int Process(Admin::SetForbidTS* req);

	int Process(unsigned uid, Common::ShutDown* msg);

	int CheckUser(unsigned uid);
	int AddUser(unsigned uid);
	int LoadUser(unsigned uid);

	int OnNewUser(unsigned uid, Common::Login* msg);
	int OnUserLogin(unsigned uid, Common::Login* msg);
	int UserOffLine(unsigned uid);

	void CheckActive();
	void CheckSave();
	void CheckClear();

	void GetOnlineUsers(std::vector<unsigned>& users);
	const map<unsigned, Common::Login>& GetUser(){return m_infomap;}
	bool IsOnline(unsigned uid) {return m_infomap.count(uid);}
	bool GetUserInfo(unsigned uid, Common::Login& info)
	{
		if(m_infomap.count(uid))
		{
			info = m_infomap[uid];
			return true;
		}
		return false;
	}

	std::string GetOpenId(unsigned uid);

private:
	int LoadArchives(unsigned uid);
};


#endif /* USERMANAGER_H_ */
