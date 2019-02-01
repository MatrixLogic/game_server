#ifndef __LOGIC_PAY_H__
#define __LOGIC_PAY_H__

#include "LogicInc.h"

//支付状态
enum PayStatusType
{
	PST_PENDING = 0, //未成交
	PST_OK = 1,	//成交
};

#define HERO_COIN_CASH 5000

//支付通道类型
enum PayChannelType
{
	PCT_UNKNOW				= -1,	//未知
	PCT_TEST				= 0,	//测试平台
	PCT_FACEBOOK_CREDITS	= 1,	//Facebook Credits
	PCT_QQ					= 2,	//QQ平台
	PCT_RENREN				= 3,	//人人
	PCT_ROCKPAY				= 4, 	//RockPay
	PCT_TAKE8				= 5,	//Take8
	PCT_KUAIWAN				= 6,	//快玩
	PCT_WEIYOUXI			= 7,
	PCT_VN					= 8,
	PCT_QQGAME				= 9,
};

class CLogicPay
{
public:
	int InitailizePay(unsigned uid);
	int AddUser(unsigned uid, unsigned cash = 0);
	int GetPay(unsigned uid, DataPay &pay);
	int ChangePayHeroCoins(unsigned uid,int coins2,const string &type, int count = 1);
	int AddHeroCoins(unsigned uid,int cash,const string &type);
	int ChangePay(unsigned uid, int cash, int coins, DataPay &pay, const string &type, int count = 1);
	int ChangePay(unsigned uid, int cash, int coins, const string &type, int count = 1);
	int SetUserPayRank(unsigned uid, int cash, const string &type);
	int SetUserPayTotal(unsigned uid, int cash, const string &type);
	int UserPayHundredDaysActivities(unsigned uid, int cash, const string &type);    //百日消费活动

	int AddAlliancePay(unsigned aid, const string &type, int coins = 0);
	int GetAlliancePay(unsigned aid, DataAlliancePay &pay);
	int ChangeAlliancePay(unsigned aid, int coins, const string &type, int count = 1);

	int AddPayHistory(DataPayHistory &payHistory);
	int GetPayHistory(unsigned uid, uint64_t payId, DataPayHistory &payHistory);
	int GetPayHistory(unsigned uid, PayChannelType channel, const string &channelPayId, DataPayHistory &payHistory);
	int SetPayHistoryStatus(unsigned uid, uint64_t payId, PayStatusType status);
	int SetPayHistoryStatus(unsigned uid, PayChannelType channel, const string &channelPayId, PayStatusType status);

};


#endif	//LOGICPAY_H_
