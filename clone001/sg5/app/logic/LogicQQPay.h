#ifndef __LOGIC_QQ_PAY_H__
#define __LOGIC_QQ_PAY_H__

#include "LogicInc.h"

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

class CLogicQQPay
{
public:
	int qz_buy_goods(
			const string &openid,
			const string &openkey,
			const string &userip,
			const string &itemid,
			string &appid,
			string &url_params);

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
	int PayRewarld(unsigned uid, int cash, int &coins);

	int InformTaskCompleted(
			const string &pf,
			const string &pfkey,
			const string &openid,
			const string &openkey,
			string &zoneid,
			string &appid,
			string &contractid);
	int CheckTask(const string &appid, const string &openid, const string &contractid);
	int ReqQQPanel(const string &pf,
			const string &pfkey,
			const string &openid,
			const string &openkey,
			string &userip,
			string &appid);
private:
	static int GetItemInfo(
			const string &itemid,
			QQItemInfo &item);

	typedef map<string, QQItemInfo> T_ItemMap;
	static T_ItemMap g_ItemInfo;
};

#endif
