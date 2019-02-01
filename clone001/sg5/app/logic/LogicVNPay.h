#ifndef __LOGIC_VN_PAY_H__
#define __LOGIC_VN_PAY_H__

#include "LogicInc.h"

struct VNItemInfo
{
	string itemid;
	string name;
	string description;
	string url;
	int price;
	int type;
	int cash;
};

class CLogicVNPay
{
public:
	int deliver(
			const string &openid,
			unsigned serverid,
			unsigned count,
			const string &billno);

	int v3_buy_goods(
			unsigned int uid,
			const string &openid,
			const string &openkey,
			const string &userip,
			const string &itemid,
			string &url_params);

private:
	static int GetItemInfo(
			const string &itemid,
			VNItemInfo &item);

	typedef map<string, VNItemInfo> T_ItemMap;
	static T_ItemMap g_ItemInfo;
};

#endif
