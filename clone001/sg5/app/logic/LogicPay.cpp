
#include "LogicPay.h"

int CLogicPay::InitailizePay(unsigned uid)
{
	DataPay pay;
	pay.uid = uid;
	pay.cash = NEW_USER_CASH;
	pay.coins = NEW_USER_COINS;
	CDataPay dbPay;
	int ret = dbPay.AddPay(pay);
	if(ret != 0)
	{
		fatal_log("[AddPay fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("add_pay_fail");
	}
	return 0;
}

int CLogicPay::AddUser(unsigned uid, unsigned cash)
{
	DataPay pay;
	pay.uid = uid;
	pay.cash = cash;
	pay.coins = 0;
	CDataPay dbPay;
	int ret = dbPay.AddPay(pay);
	if(ret != 0)
	{
		fatal_log("[AddPay fail][ret=%d,uid=%u,cash=%u]", ret, uid, cash);
		DB_ERROR_RETURN_MSG("add_pay_fail");
	}
	return 0;
}

int CLogicPay::GetPay(unsigned uid, DataPay &pay)
{
	CDataPay dbPay;
	int ret = dbPay.GetPay(uid, pay);
	if(ret != 0)
	{
		if(ret != R_ERR_NO_DATA || !IsAllianceId(uid))
		{
			error_log("[GetPay fail][ret=%d,uid=%u]", ret, uid);
		}
		if(ret == R_ERR_NO_DATA)
		{
			ERROR_RETURN_MSG(R_ERR_NO_DATA, "no_pay_data");
		}
		DB_ERROR_RETURN_MSG("db_get_pay_fail");
	}
	return 0;
}

//int CLogicPay::GetCash(unsigned uid, unsigned &cash)
//{
//	DataPay pay;
//	int ret = GetPay(uid, pay);
//	if (ret != 0)
//		return ret;
//	cash = pay.cash;
//	return 0;
//}

//用于购买隐藏武将碎片的积分
int CLogicPay::ChangePayHeroCoins(unsigned uid,int coins2,const string &type, int count)
{
	Json::Value userFlag;
	CLogicUser logicUser;
	userFlag.clear();
	int ret = logicUser.GetUserFlag(uid,userFlag);
	if(ret)
		return ret;
	if(!userFlag.isMember("heroCoins"))
	{
		ERROR_RETURN_MSG(R_ERR_DATA_LIMIT, "ChangePayNewCoins_limit_error");
	}

	int hero_coins_old = userFlag["heroCoins"][1u].asInt();

	int hero_coins = hero_coins_old + coins2;
	if(hero_coins < 0)
	{
		ERROR_RETURN_MSG(R_ERR_DATA_LIMIT, "ChangePayNewCoins_limit_error");
	}

	userFlag["heroCoins"][1u] = hero_coins;

	ret = logicUser.SetUserFlag(uid, userFlag);
	if (ret != 0)
	{
		error_log("[set user flag fail][uid=%u,oherocoins=%d,herocoins=%d]",uid,hero_coins_old,hero_coins);
		return ret;
	}

	COINS_LOG("[change hero_coins pay log][uid=%u,oherocoins=%d,herocoins=%d,type=%s,count=%d]",
			uid,hero_coins_old,coins2,type.c_str(),count);

	return 0;
}

int CLogicPay::AddHeroCoins(unsigned uid,int cash,const string &type)
{
	unsigned now = Time::GetGlobalTime();
	Json::Value userFlag;
	CLogicUser logicUser;
	int cash_total=0;
	int ocash_total = 0;
	int old_herocoins = 0;
	userFlag.clear();
	int ret = logicUser.GetUserFlag(uid,userFlag);
	if(ret)
		return ret;

	if(!userFlag.isMember("heroCoins"))
	{
		ocash_total =0;
		old_herocoins = 0;
		userFlag["heroCoins"][0u]= now;
		userFlag["heroCoins"][1u]= 0;          //herocoins
		userFlag["heroCoins"][2u]= cash;
	}
	else
	{
		unsigned last_ts = userFlag["heroCoins"][0u].asInt();
		old_herocoins = userFlag["heroCoins"][1u].asInt();
		if(CTime::GetDayInterval(last_ts,now) == 0)
		{
			ocash_total = userFlag["heroCoins"][2u].asInt();
			cash_total = userFlag["heroCoins"][2u].asInt() + cash;
			userFlag["heroCoins"][2u] = cash_total;
			userFlag["heroCoins"][0u] = now;
		}
		else
		{
			ocash_total =0;
			userFlag["heroCoins"][2u] = cash;
			userFlag["heroCoins"][0u] = now;
		}
	}

	unsigned oldcount = ocash_total / HERO_COIN_CASH;
	unsigned newcount = userFlag["heroCoins"][2u].asInt()  / HERO_COIN_CASH;
	unsigned count = (newcount - oldcount) * 10;
	userFlag["heroCoins"][0u] = now;
	userFlag["heroCoins"][1u] = count + userFlag["heroCoins"][1u].asInt();

	ret = logicUser.SetUserFlag(uid, userFlag);
	if (ret != 0)
	{
		error_log("[set user flag fail][uid=%u,cash=%u]",uid,-cash);
		return ret;
	}
	else
		debug_log("[addHeroCoins][uid=%u,herocoins=%d,cash=%d,ts=%u]",uid,
				userFlag["heroCoins"][1u].asInt(),userFlag["heroCoins"][2u].asInt(),userFlag["heroCoins"][0u].asInt());

	COINS_LOG("[change hero_coins pay log][uid=%u,oherocoins=%d,herocoins=%d,type=%s]",
			uid,old_herocoins,count,type.c_str());

	return 0;
}

int CLogicPay::ChangePay(unsigned uid, int cash, int coins, DataPay &pay, const string &type, int count)
{
	int ret = GetPay(uid, pay);
	if (ret != 0)
		return ret;
	unsigned org_cash = pay.cash;
	unsigned org_coins = pay.coins;
	if ((cash < 0 && (unsigned)(-cash) > pay.cash)
			|| (coins < 0 && (unsigned)(-coins) > pay.coins))
	{
		error_log("[pay overspend][uid=%u,cash=%d,coins=%d,cashbal=%u,coinsbal=%u]",
				uid, cash, coins, pay.cash, pay.coins);
		ERROR_RETURN_MSG(R_ERR_DATA_LIMIT, "pay_limit_error");
	}
	pay.cash += cash;
	pay.coins += coins;
	CDataPay dbPay;
	ret = dbPay.SetPay(uid, pay);
	if (ret != 0)
	{
		error_log("[SetPay fail][ret=%d,uid=%u,cash=%d,coins=%d,cashbal=%u,coinsbal=%u]",
				ret, uid, cash, coins, pay.cash, pay.coins);
		DB_ERROR_RETURN_MSG("set_pay_fail");
	}

	//累积消费
	ret = SetUserPayTotal(uid, cash, type);
	if (0 != ret)
	{
		error_log("[set user pay total fail][uid=%d,ret=%d,cash=%d]", uid, ret, -cash);
	}
	//百日庆典消费钻石
	ret = UserPayHundredDaysActivities( uid, cash, type);
	if (0 != ret)
	{
		error_log("[set user hundred days activity rank fail][uid=%d,ret=%d,cash=%d]", uid, ret, -cash);
	}
	//消费积分
	ret = SetUserPayRank(uid, cash+coins, type);
	if (0 != ret)
	{
		error_log("[set user pay rank fail][uid=%d,ret=%d,cash=%d]", uid, ret, -cash);
	}

	if( cash > 0)
	{
		ret = AddHeroCoins(uid,cash,type);
		if(0 != ret)
		{
			error_log("[add herocoins  fail][uid=%d,ret=%d,cash=%d]", uid, ret, cash);
		}
	}

	COINS_LOG("[change pay log][uid=%u,ocash=%u,ocoins=%u,ncash=%u,nconis=%u,cash=%d,coins=%d,type=%s,count=%d]",
			uid,org_cash,org_coins,pay.cash,pay.coins,cash,coins,type.c_str(),count);
	return 0;
}

int CLogicPay::ChangePay(unsigned uid, int cash, int coins, const string &type, int count)
{
	DataPay pay;
	return ChangePay(uid, cash, coins, pay, type, count);
}

int CLogicPay::AddPayHistory(DataPayHistory &payHistory)
{
	payHistory.time = Time::GetGlobalTime();
	uint64_t tempid = 0;
	CLogicIdCtrl logicIdCtrl;
	int ret = logicIdCtrl.GetNextId(KEY_PAY_ID_CTRL, tempid);
	if(ret != 0)
	{
		error_log("[GetNextId fail][ret=%d,uid=%u,channel=%d]",
				ret, payHistory.uid, payHistory.channel);
		DB_ERROR_RETURN_MSG("get_pay_id_fail");
	}
	payHistory.pay_id = tempid;
	CDataPayHistory dbPayHistory;
	ret = dbPayHistory.AddPayHistory(payHistory);
	if(ret != 0)
	{
		error_log("[AddPayHistory fail][ret=%d,uid=%u,payid=%llu,channel=%d]",
				ret, payHistory.uid, payHistory.pay_id, payHistory.channel);
		DB_ERROR_RETURN_MSG("add_pay_history_fail");
	}
	return 0;
}

int CLogicPay::GetPayHistory(unsigned uid, uint64_t payId, DataPayHistory &payHistory)
{
	CDataPayHistory dbPayHistory;
	int ret = dbPayHistory.GetPayHistory(uid, payId, payHistory);
	if(ret != 0 && ret != R_ERR_NO_DATA)
	{
		error_log("[GetPayHistory fail][ret=%d,uid=%u,payid=%llu]", ret, uid, payId);
		DB_ERROR_RETURN_MSG("get_pay_history_fail");
	}
	return ret;
}

int CLogicPay::GetPayHistory(unsigned uid, PayChannelType channel, const string &channelPayId, DataPayHistory &payHistory)
{
	CDataPayHistory dbPayHistory;
	int ret = dbPayHistory.GetPayHistory(uid, channel, channelPayId, payHistory);
	if(ret != 0 && ret != R_ERR_NO_DATA)
	{
		error_log("[GetPayHistory fail][ret=%d,uid=%u,channel=%d,channel_pay_id=%s]",
				ret, uid, channel, channelPayId.c_str());
		DB_ERROR_RETURN_MSG("get_pay_history_fail");
	}
	return ret;
}

int CLogicPay::SetPayHistoryStatus(unsigned uid, uint64_t payId, PayStatusType status)
{
	CDataPayHistory dbPayHistory;
	int ret = dbPayHistory.SetPayHistoryStatus(uid, payId, status);
	if(ret != 0)
	{
		error_log("[SetPayHistoryStatus fail][ret=%d,uid=%u,payId=%llu,status=%d]",
				ret, uid, payId, status);
		DB_ERROR_RETURN_MSG("set_pay_history_fail");
	}
	return ret;
}

int CLogicPay::SetPayHistoryStatus(unsigned uid, PayChannelType channel, const string &channelPayId, PayStatusType status)
{
	CDataPayHistory dbPayHistory;
	int ret = dbPayHistory.SetPayHistoryStatus(uid, channel, channelPayId, status);
	if(ret != 0)
	{
		error_log("[SetPayHistoryStatus fail][ret=%d,uid=%u,channel=%d,channelPayId=%s,,status=%d]",
				ret, uid, channel, channelPayId.c_str(), status);
		DB_ERROR_RETURN_MSG("set_pay_history_fail");
	}
	return ret;
}

int CLogicPay::AddAlliancePay(unsigned aid, const string &type, int coins)
{
	CDataAlliancePay dbAlliancePay;
	DataAlliancePay alliancePay;
	alliancePay.alliance_id = aid;
	if (coins < 0)
	{
		fatal_log("[AddAlliancePay fail][coins=%d,aid=%u]", coins, aid);
		DB_ERROR_RETURN_MSG("add_pay_fail");
	}
	alliancePay.coins = coins;
	int ret = dbAlliancePay.AddPay(alliancePay);
	if(ret != 0)
	{
		fatal_log("[AddAlliancePay fail][ret=%d,aid=%u]", ret, aid);
		DB_ERROR_RETURN_MSG("add_pay_fail");
	}

	COINS_LOG("[change pay log][uid=%u,ocash=0,ocoins=0,ncash=0,nconis=%u,cash=0,coins=%u,type=%s,count=1]",
				aid,alliancePay.coins,coins,type.c_str());

	return 0;
}
int CLogicPay::GetAlliancePay(unsigned aid, DataAlliancePay &pay)
{
	CDataAlliancePay dbAlliancePay;
	int ret = dbAlliancePay.GetPay(aid, pay);
	if(ret != 0)
	{
		if(ret != R_ERR_NO_DATA || !IsAllianceId(aid))
		{
			error_log("[GetAlliancePay fail][ret=%d,uid=%u]", ret, aid);
		}
		if(ret == R_ERR_NO_DATA)
		{
			ERROR_RETURN_MSG(R_ERR_NO_DATA, "no_pay_data");
		}
		DB_ERROR_RETURN_MSG("db_get_pay_fail");
	}
	return 0;
}

int CLogicPay::ChangeAlliancePay(unsigned aid, int coins, const string &type, int count)
{
	DataAlliancePay alliancePay;
	int ret = GetAlliancePay(aid, alliancePay);
	if (ret != 0)
			return ret;
	unsigned org_coins = alliancePay.coins;
	if ((coins < 0 && (unsigned)(-coins) > alliancePay.coins))
	{
		error_log("[pay overspend][aid=%u,coins=%d,coinsbal=%u]", aid, coins, alliancePay.coins);
		ERROR_RETURN_MSG(R_ERR_DATA_LIMIT, "pay_limit_error");
	}

	alliancePay.coins += coins;
	CDataAlliancePay dbAlliancePay;
	ret = dbAlliancePay.SetPay(aid, alliancePay);
	if (ret != 0)
	{
		error_log("[ChangeAlliancePay fail][ret=%d,aid=%u,coins=%d,coinsbal=%u]",
				ret, aid, coins, alliancePay.coins);
		DB_ERROR_RETURN_MSG("set_pay_fail");
	}

	COINS_LOG("[change pay log][uid=%u,ocash=0,ocoins=%u,ncash=0,nconis=%u,cash=0,coins=%d,type=%s,count=%d]",
			aid,org_coins,alliancePay.coins,coins,type.c_str(),count);
	return 0;
}

int CLogicPay::SetUserPayRank(unsigned uid, int cash, const string &type)
{
	int ret = 0;
	unsigned now = Time::GetGlobalTime();
	if(    now >= Config::GetIntValue(CONFIG_PAYRANK_BEGIN_TS)
		&& now <= Config::GetIntValue(CONFIG_PAYRANK_END_TS)
		&& cash < 0
		&& type != "MATCHGUESSAPPLY")
	{
		Json::Value userFlag;
		CLogicUser logicUser;
		int lastpay_ts = 0;
		int payTotal = 0;
		userFlag.clear();
		ret = logicUser.GetUserFlag(uid,userFlag);
		if(ret)
			return ret;
		if(!userFlag.isMember("pointpay"))
		{
			userFlag["pointpay"][0u]= -cash;
			userFlag["pointpay"][1u]= now;
		}
		else
		{
			lastpay_ts = userFlag["pointpay"][1u].asInt();
			if(    lastpay_ts >= Config::GetIntValue(CONFIG_PAYRANK_BEGIN_TS)
				&& lastpay_ts <= Config::GetIntValue(CONFIG_PAYRANK_END_TS))
			{
				payTotal = userFlag["pointpay"][0u].asUInt() - cash;
				userFlag["pointpay"][0u] = payTotal;
				userFlag["pointpay"][1u]= now;
			}
			else
			{
				userFlag["pointpay"][0u] = payTotal = - cash;
				userFlag["pointpay"][1u] = now;
			}
		}
		ret = logicUser.SetUserFlag(uid, userFlag);
		if (ret != 0)
		{
			error_log("[set user flag fail][uid=%u,cash=%u]", uid, -cash);
			return ret;
		}
		CLogicUserBasic logicUserBasic;
		DataUserBasic userBasic;
		ret = logicUserBasic.GetUserBasicLimit(uid,OpenPlatform::GetType(),userBasic);
		if (0 != ret)
		{
			error_log("[get usr basic fail][uid=%u,ret=%d]", uid, ret);
		}
		CLogicPayRank logicPayRank;
		PayItem payItem;
		payItem.uid = uid;
		payItem.payTotal = payTotal;
		memcpy(payItem.name,userBasic.name.c_str(),sizeof(payItem.name) - 1);
		ret = logicPayRank.UpdatePayRank(uid,payItem);
		if (0 != ret)
		{
			error_log("[UpdatePayRank fail][uid=%u,ret=%d]", uid, ret);
		}
	}
	return 0;
}

int CLogicPay::SetUserPayTotal(unsigned uid, int cash, const string &type)
{
	int ret = 0;
	unsigned now = Time::GetGlobalTime();
	if(    now >= Config::GetIntValue(CONFIG_PAY_TOTAL_BEGIN_TS)
		&& now <= Config::GetIntValue(CONFIG_PAY_TOTAL_END_TS)
		&& cash < 0
		&& type != "MATCHGUESSAPPLY")
	{
		Json::Value userFlag;
		CLogicUser logicUser;
		int lastpay_ts = 0;
		int payTotal = 0;
		userFlag.clear();
		ret = logicUser.GetUserFlag(uid,userFlag);
		if(ret)
			return ret;
		if(!userFlag.isMember("payTotal"))
		{
			userFlag["payTotal"][0u]= -cash;
			userFlag["payTotal"][1u]= now;
		}
		else
		{
			lastpay_ts = userFlag["payTotal"][1u].asInt();
			if(    lastpay_ts >= Config::GetIntValue(CONFIG_PAY_TOTAL_BEGIN_TS)
				&& lastpay_ts <= Config::GetIntValue(CONFIG_PAY_TOTAL_END_TS))
			{
				payTotal = userFlag["payTotal"][0u].asUInt() - cash;
				userFlag["payTotal"][0u] = payTotal;
				userFlag["payTotal"][1u]= now;
			}
			else
			{
				userFlag["payTotal"][0u] = payTotal = - cash;
				userFlag["payTotal"][1u] = now;
			}
		}
		ret = logicUser.SetUserFlag(uid, userFlag);
		if (ret != 0)
		{
			error_log("[set user flag fail][uid=%u,coins=%u]",uid,-cash);
			return ret;
		}
	}
	return 0;
}

//百日庆典活动
int CLogicPay::UserPayHundredDaysActivities(unsigned uid, int cash, const string &type)
{
	int ret = 0;
	unsigned now = Time::GetGlobalTime();
	if(    now >= Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS)
		&& now <= Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_END_TS)
		&& cash < 0
		&& type != "MATCHGUESSAPPLY")
	{
		Json::Value userFlag;
		CLogicUser logicUser;
		unsigned lastpay_ts = 0;
		int payTotal = 0;
		userFlag.clear();
		ret = logicUser.GetUserFlag(uid,userFlag);
		if(ret)
			return ret;
		if(!userFlag.isMember("hundredDaysPay"))
		{
			userFlag["hundredDaysPay"][0u]= -cash;
			userFlag["hundredDaysPay"][1u]= now;
			payTotal = -cash;
		}
		else
		{
			lastpay_ts = userFlag["hundredDaysPay"][1u].asUInt();
			if(    lastpay_ts >= Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS)  //一天的累计消费
				&& lastpay_ts <= Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_END_TS)
				&& CTime::GetDayInterval(lastpay_ts,now) == 0)
			{
				payTotal = userFlag["hundredDaysPay"][0u].asUInt() - cash;
				userFlag["hundredDaysPay"][0u] = payTotal;
				userFlag["hundredDaysPay"][1u]= now;
			}
			else if(lastpay_ts >= Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS)  //不是同一天的第一次
				 && lastpay_ts <= Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_END_TS)
				 && CTime::GetDayInterval(lastpay_ts,now) != 0)
			{
				userFlag["hundredDaysPay"][0u] = -cash;
				payTotal = -cash;
				userFlag["hundredDaysPay"][1u] = now;
			}
			else if(lastpay_ts <= Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS)
				&&  Time::GetGlobalTime() >= Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS)
				&&  Time::GetGlobalTime() <= Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_END_TS))
			{
				userFlag["hundredDaysPay"][0u] = -cash;
				payTotal = -cash;
				userFlag["hundredDaysPay"][1u] = now;

			}
			else if(lastpay_ts >= Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_END_TS)
				&&  Time::GetGlobalTime() >= Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS)
				&&  Time::GetGlobalTime() <= Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_END_TS))
			{
				userFlag["hundredDaysPay"][0u] = -cash;
				payTotal = -cash;
				userFlag["hundredDaysPay"][1u] = now;
			}
		}


		ret = logicUser.SetUserFlag(uid, userFlag);
		if (ret != 0)
		{
			error_log("[set user flag fail][uid=%u,cash=%u]",uid,-cash);
			return ret;
		}

		DataHDaysAcvitityPay points;
		CLogicUserBasic logicUserBasic;
		DataUserBasic dataUserBasic;
		int ret = 0;
		CLogicHundredDaysActivityRank hdaysactivittyRank;
		logicUserBasic.GetUserBasicLimit(uid,OpenPlatform::GetType(),dataUserBasic);
		points.uid = uid;
		points.pay = payTotal;
		memcpy(points.name,dataUserBasic.name.c_str(),sizeof(points.name) - 1);
		ret = hdaysactivittyRank.UpdateHundredDaysActivityRank(uid,points);
		if(ret != 0)
		{
			return ret;
		}
	}

	return 0;
}

