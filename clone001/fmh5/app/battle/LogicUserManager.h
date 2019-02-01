/*
 * LogicUserMangager.h
 *
 *  Created on: 2016-9-12
 *      Author: dawx62fac
 */

#ifndef LOGICUSERMANGAGER_H_
#define LOGICUSERMANGAGER_H_

#include "ServerInc.h"

class DBCUserBaseWrap
{
public:
	DBCUserBaseWrap(unsigned uid);
	DBCUserBaseWrap(unsigned index, DataBase& data);

	void Save()
	{
		container_->m_data->MarkChange(index_);
	}

	void MarkLoad()
	{
		container_->m_data->SetPlusTS(index_);
	}

	DataBase& Obj()
	{
		return data_;
	}

	const DataBase& Obj() const
	{
		return data_;
	}

	unsigned Id() const { return data_.uid; }

	/////////////////////////////////////////////////////////////////////////
	void CostCash(int cash, const std::string& reason);
	void AddCash(int cash, const std::string& reason);
	unsigned GetCash();
	//累计充值
	void AccRecharge(int count, bool isPush = true);
	//
	void FinalAsynData();
	void AddAsynItem(unsigned id, unsigned count, const std::string& op = "asyn_data");

	//首充
	bool IsGetFirstRechargeReward() const;
	void SetFirstRechargeRewardStatus();

	//再次充值
	bool IsGetSecondRechargeReward() const;
	void SetSecondRechargeRewardStatus();

	//零点逻辑，解决登录跨天导致的bug
	void EveryDayAction(int di = 1);

	void 	RefreshVIPLevel(bool isPush = true);



	//获取用户注册小时数
	unsigned GetRegisterHours() const;

	bool AddExp(int exp);

	//通用属性操作
	void BaseProcess(const ::CommonGiftConfig::BaseItem& base, DataCommon::UserBaseChangeCPP* obj,
			const std::string& reason, double factor = 1.0);

	//检查条件
	int CheckBaseBeforeCost(unsigned uid, const string & reason, const CommonGiftConfig::BaseItem & base);

	void FullMessage(User::Base* obj) const;
private:
	int _Index(unsigned uid);

	int AsynType2ResourceType(ASYN_TYPE type);

	void OnUserUpgradeReward(unsigned old_level);
private:
	BaseManager*	container_;
	unsigned 		index_;
	DataBase& 		data_;
};


class LogicUserManager : public BattleSingleton, public CSingleton<LogicUserManager>
{
	class _RechargeRecord
	{
	public:
		_RechargeRecord(unsigned nUid, unsigned nCash)
			: uid(nUid), cash(nCash)
		{

		}

		unsigned uid;
		unsigned cash;
	};
private:
	friend class CSingleton<LogicUserManager>;
	LogicUserManager();

public:
	virtual void CallDestroy() { Destroy();}

	void OnTimer1();

	//消费钻石
	void CostCash(unsigned uid, int cash, string reason, unsigned & newcash);

	//每日零点推送给所有在线用户奖励
	int EveryDayAction();

	void NotifyRecharge(unsigned uid, unsigned cash);

	//消耗钻石
	int Process(unsigned uid, User::CostCashReq* req, User::CostCashResp* resp);

	//通用处理，包含通用消耗和通用奖励
	int CommonProcess(unsigned uid, const CommonGiftConfig::CommonModifyItem& modifyitem, const std::string& reason,
			DataCommon::CommonItemsCPP* obj, double multiple = 1.0);
private:
	//通用处理底层
	void CommonUnderlaying(DBCUserBaseWrap& user, const CommonGiftConfig::CommonModifyItem& cfg, const std::string& reason,
			DataCommon::CommonItemsCPP* obj, double multiple = 1.0);

	void CheckPropsBeforeCost(unsigned uid, const string & reason, const CommonGiftConfig::CommonModifyItem& cfg);
private:
	BaseManager* baseData_;
	unsigned  start_time;

	std::deque<_RechargeRecord> recharge_records_;
};

#endif /* LOGICUSERMANGAGER_H_ */
