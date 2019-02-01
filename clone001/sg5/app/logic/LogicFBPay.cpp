#include "LogicFBPay.h"

CLogicFBPay::T_ItemMap CLogicFBPay::g_ItemInfo;

int CLogicFBPay::GetItemInfo(
		unsigned item_id,
		const string &locale,
		FBItemInfo &item)
{
	static bool bInit = false;
	if (!bInit)
	{
		string itemFile;
		itemFile = Config::GetValue("pay_item_info");
		if (itemFile.empty())
			itemFile = "/data/release/sanguo/conf/pay_item_info.xml";
		CMarkupSTL xmlConf;
		if(!xmlConf.Load(itemFile.c_str()))
		{
			fatal_log("[load item info fail][path=%s]", itemFile.c_str());
			DB_ERROR_RETURN_MSG("load_item_fail");
		}
		if(!xmlConf.FindElem("items"))
		{
			fatal_log("[parse item info fail][path=%s]", itemFile.c_str());
			DB_ERROR_RETURN_MSG("load_item_fail");
		}
		xmlConf.IntoElem();
		while(xmlConf.FindElem("item"))
		{
			FBItemInfo itemTmp;
			itemTmp.item_id = CTrans::STOI(xmlConf.GetAttrib("id"));
			itemTmp.title = xmlConf.GetAttrib("title");
			itemTmp.description = xmlConf.GetAttrib("description");
			itemTmp.image_url = xmlConf.GetAttrib("image_url");
			itemTmp.product_url = xmlConf.GetAttrib("product_url");
			itemTmp.price = CTrans::STOI(xmlConf.GetAttrib("price"));
			itemTmp.cash = CTrans::STOI(xmlConf.GetAttrib("cash"));
			xmlConf.IntoElem();
			while(xmlConf.FindElem("locale"))
			{
				string name = xmlConf.GetAttrib("name");
				if (!name.empty())
				{
					itemTmp.titleMap[name] = xmlConf.GetAttrib("title");
					itemTmp.descMap[name] = xmlConf.GetAttrib("description");
				}
			}
			xmlConf.OutOfElem();
			g_ItemInfo[itemTmp.item_id] = itemTmp;
		}
		bInit = true;
	}

	if (g_ItemInfo.empty())
	{
		error_log("[no item info][item_id=%u]",item_id);
		DB_ERROR_RETURN_MSG("no_item_info");
	}

	T_ItemMap::const_iterator it;
	it = g_ItemInfo.find(item_id);
	if (it == g_ItemInfo.end())
	{
		error_log("[no item info][item_id=%u]",item_id);
		DB_ERROR_RETURN_MSG("no_item_info");
	}

	item.item_id = (it->second).item_id;
	item.title = (it->second).title;
	item.description = (it->second).description;
	item.image_url = (it->second).image_url;
	item.product_url = (it->second).product_url;
	item.price = (it->second).price;
	item.cash = (it->second).cash;
	if (!locale.empty())
	{
		map<string, string>::const_iterator it2;
		it2 = (it->second).titleMap.find(locale);
		if (it2 != (it->second).titleMap.end())
		{
			item.title = it2->second;
		}
		it2 = (it->second).descMap.find(locale);
		if (it2 != (it->second).descMap.end())
		{
			item.description = it2->second;
		}
	}

	return 0;
}

int CLogicFBPay::payments_get_items(
		const string &openid,
		const string &orderId,
		unsigned itemId,
		const string &locale,
		FBItemInfo &itemInfo)
{
	int ret = 0;
	CLogicUserBasic logicUserBasic;
	unsigned uid = 0;
	ret = logicUserBasic.GetUid(uid, PT_FACEBOOK, openid, false);
	if (ret != 0)
		return ret;

	ret = GetItemInfo(itemId, locale, itemInfo);
	if (ret != 0)
		return ret;

	CLogicPay logicPay;
	DataPayHistory payHis;
	payHis.uid = uid;
	payHis.type = itemId;
	payHis.channel = PCT_FACEBOOK_CREDITS;
	payHis.channel_pay_id = orderId;
	payHis.credit = itemInfo.price * 10;
	payHis.count = 1;
	payHis.status = PST_PENDING;
	payHis.open_id = openid;
	ret = logicPay.AddPayHistory(payHis);
	if (ret != 0)
		return ret;

	return 0;
}

int CLogicFBPay::payments_status_update(
		const string &openid,
		const string &orderId,
		const string &status,
		string &next_state)
{
	if (status != "placed" && status != "settled")
	{
		PARAM_ERROR_RETURN_MSG("status_error");
	}

	int ret = 0;
	CLogicUserBasic logicUserBasic;
	unsigned uid = 0;
	ret = logicUserBasic.GetUid(uid, PT_FACEBOOK, openid, false);
	if (ret != 0)
		return ret;

	CLogicPay logicPay;
	DataPayHistory payHis;

	if (status == "placed")
	{
		ret = logicPay.GetPayHistory(uid, PCT_FACEBOOK_CREDITS, orderId, payHis);
		if (ret != 0)
			return ret;
		next_state = "settled";
	}
	else
	{
		ret = logicPay.GetPayHistory(uid, PCT_FACEBOOK_CREDITS, orderId, payHis);
		if (ret != 0)
			return ret;

		if (payHis.status == PST_PENDING)
		{
			FBItemInfo itemInfo;
			ret = GetItemInfo(payHis.type, "", itemInfo);
			if (ret != 0)
				return ret;
			int addCash = itemInfo.cash;
			unsigned balance = 0;
			ret = logicPay.ChangePay(uid, addCash, 0, "FBTOPUP");
			if (ret != 0)
				return ret;
			ret = logicPay.SetPayHistoryStatus(uid, PCT_FACEBOOK_CREDITS, orderId, PST_OK);
			if (ret != 0)
			{
				error_log("[SetPayHistoryStatus fail][uid=%d,ret=%d,orderId=%s]",
						uid,ret,orderId.c_str());
			}
		}
		else
		{
			info_log("[duplicate settled][uid=%u,orderId=%s,status=%s]",uid,orderId.c_str(),status.c_str());
		}
		next_state = "";
	}

	return 0;
}
