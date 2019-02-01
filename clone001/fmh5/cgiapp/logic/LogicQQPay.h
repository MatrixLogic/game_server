#ifndef __LOGIC_QQ_PAY_H__
#define __LOGIC_QQ_PAY_H__

#include "LogicInc.h"

#define MARKET_QUEST_STEP_1 0x01
#define MARKET_QUEST_STEP_2 0x02
#define MARKET_QUEST_STEP_3 0x04
#define MARKET_QUEST_STEP_4 0x08

struct QQItemInfo
{
	string itemid;
	string name;
	string description;
	string url;
	int price;
	int type;
	int cash;
};

enum PayFlag
{
	PAY_FLAG_CHARGE 	= 1,
	PAY_FLAG_ADMIN 		= 2,
	PAY_FLAG_COST_ALL 	= 4,
	PAY_FLAG_NO_REPLY	= 8,
};
enum PayStatusType
{
	PST_PENDING = 0, //未成交
	PST_OK = 1,	//成交
	PST_BACK = 2,	//chargeback
};

class CLogicQQPay
{
public:

	int deliver(
			const string &appid,
			const string &openid,
			const string &payitem,
			int amt,
			const string &billno);

	int v3_buy_goods(
			const string &pf,
			const string &pfkey,
			const string &openid,
			const string &openkey,
			const string &userip,
			const string &itemid,
			string &appid,
			string &url_params);
	int v3_pay_get_token(const string &pfkey,
			const string &pf,
			const string &openkey,
			const string &openid,
			const string &tokentype,
			const string &discountid,
			string &appid,
			string &token,
			string &zoneid);
	int AddPayHistory(DataPayHistory &payHistory);

private:
	static int GetItemInfo(
			const string &itemid,
			QQItemInfo &item);

	typedef map<string, QQItemInfo> T_ItemMap;
	static T_ItemMap g_ItemInfo;
};

#endif
