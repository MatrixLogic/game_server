#include "LogicVNPay.h"

CLogicVNPay::T_ItemMap CLogicVNPay::g_ItemInfo;

int CLogicVNPay::GetItemInfo(
		const string &itemid,
		VNItemInfo &item)
{
	static bool bInit = false;
	if (!bInit)
	{
		string itemFile;
		itemFile = Config::GetValue(CONFIG_QQ_ITEM_INFO);
		if (itemFile.empty())
		{
			fatal_log("[empty item info][path=%s]", itemFile.c_str());
			ERROR_RETURN_MSG(R_ERROR, "load_item_fail");
		}
		CMarkupSTL xmlConf;
		if(!xmlConf.Load(itemFile.c_str()))
		{
			fatal_log("[load item info fail][path=%s]", itemFile.c_str());
			ERROR_RETURN_MSG(R_ERROR, "load_item_fail");
		}
		if(!xmlConf.FindElem("items"))
		{
			fatal_log("[parse item info fail][path=%s]", itemFile.c_str());
			ERROR_RETURN_MSG(R_ERROR, "load_item_fail");
		}
		xmlConf.IntoElem();
		while(xmlConf.FindElem("item"))
		{
			VNItemInfo itemTmp;
			itemTmp.itemid = xmlConf.GetAttrib("id");
			itemTmp.name = xmlConf.GetAttrib("name");
			itemTmp.description = xmlConf.GetAttrib("description");
			itemTmp.url = xmlConf.GetAttrib("url");
			itemTmp.price = CTrans::STOI(xmlConf.GetAttrib("price"));
			itemTmp.type = CTrans::STOI(xmlConf.GetAttrib("type"));
			itemTmp.cash = CTrans::STOI(xmlConf.GetAttrib("cash"));
			g_ItemInfo[itemTmp.itemid] = itemTmp;
		}
		bInit = true;
	}

	if (g_ItemInfo.empty())
	{
		error_log("[no item info][itemid=%s]",itemid.c_str());
		ERROR_RETURN_MSG(R_ERR_NO_DATA, "no_item_info");
	}

	T_ItemMap::const_iterator it;
	it = g_ItemInfo.find(itemid);
	if (it == g_ItemInfo.end())
	{
		error_log("[no item info][itemid=%s]",itemid.c_str());
		ERROR_RETURN_MSG(R_ERR_NO_DATA, "no_item_info");
	}

	item.itemid = (it->second).itemid;
	item.name = (it->second).name;
	item.description = (it->second).description;
	item.url = (it->second).url;
	item.price = (it->second).price;
	item.type = (it->second).type;
	item.cash = (it->second).cash;

	return 0;
}

int CLogicVNPay::deliver(
		const string &openid,
		unsigned serverid,
		unsigned count,
		const string &billno)
{
	int ret = 0;

	CLogicPay logicPay;

	unsigned uid = 0;
	CDataUserMapping dbUserMapping;
	ret = dbUserMapping.GetUID(openid, serverid, uid);
	if( ret!=0 || uid == 0)
		return R_ERR_DB;

	if (count != 0)
	{
		ret = logicPay.ChangePay(uid, count, 0,"VNTOPUP", 1);
		if (ret != 0) {
			error_log("qqdeliever failed");
			ERROR_RETURN_MSG(1, "系统繁忙 ");
		}
	}

	/* daily accharge  add by aaron */
	int accCharge = count;
	CDataUser userDb;
	DataUser user;
	userDb.GetUserLimit(uid,user);
	userDb.AddAccCharge(uid, accCharge);

	/*********************
	if(user.accCharge < 20000 && user.accCharge + accCharge >= 20000
	&& Time::GetGlobalTime()-user.register_time<86400*60)
	{
		DataEmail email;
		CLogicEmail logicEmail;
		vector<uint64_t> toUidList;
		toUidList.push_back(uid);
		email.attach_flag = 0;
		email.attachments = "";
		email.uid = ADMIN_UID;
		email.post_ts = Time::GetGlobalTime();
		email.title = "至尊会员";
		email.text = "尊敬的玩家您好，恭喜您成为我们的高级VIP用户。请联系VN：2592360979领取VIP礼包并有专业人士为您一对一服务。";
		logicEmail.AddEmail(email,toUidList,OpenPlatform::GetType());
	}
	********************/
	CLogicUser logicUser;
	Json::Value userFlag;

	ret = logicUser.GetUserFlag(uid, userFlag);
	if (ret != 0)
	{
		error_log("[add daily charge fail][uid=%u,coins=%u]",uid,accCharge);
	}
	else
	{
		if (!userFlag.isMember("dchg")
				|| CTime::GetDayInterval(userFlag["dchg"][0u].asUInt(), Time::GetGlobalTime()) != 0)
		{

			userFlag["dchg"][0u] = Time::GetGlobalTime();
			userFlag["dchg"][1u] = 0;
		}
		userFlag["dchg"][1u] = userFlag["dchg"][1u].asInt() + accCharge;

		ret = logicUser.SetUserFlag(uid, userFlag);
		if (ret != 0)
		{
			error_log("[add daily charge fail][uid=%u,coins=%u]",uid,accCharge);
		}
	}
	/*  daily accharge  add by aaron */

	DataPayHistory payhis;
	payhis.channel = PCT_VN;
	payhis.channel_pay_id = billno;
	payhis.count = 1;
	payhis.credit = count;
	payhis.status = PST_OK;
	payhis.type = 0;
	payhis.uid = uid;
	payhis.open_id = openid;
	ret = logicPay.AddPayHistory(payhis);
	if (ret != 0)
	{
		error_log("[AddPayHistory fail][ret=%d,openid=%s,billno=%s]",
				ret,openid.c_str(),billno.c_str());
	}
	// add by aaron

	return 0;
}

int CLogicVNPay::v3_buy_goods(
		unsigned int uid,
		const string &openid,
		const string &openkey,
		const string &userip,
		const string &itemid,
		string &url_params)
{
	int ret = 0;
	CLogicUserBasic logicUserBasic;
	//unsigned uid = 0;
	//ret = logicUserBasic.GetUid(uid, OpenPlatform::GetType(), openid, true);
	if (ret != 0)
		return ret;
	VNItemInfo itemInfo;
	ret = GetItemInfo(itemid, itemInfo);
	if (ret != 0)
		return ret;

	string appkey = OpenPlatform::GetPlatform()->GetAppKey();

	string url = "https://" + OpenPlatform::GetPlatform()->GetConfig("v3domain") + "/pay/buy_goods?";

	string qsig = "itemid=" + itemid;
	string qstr = "itemid=" + Crypt::UrlEncodeForTX(itemid);
	qsig += "&openid=" + openid + "&openkey=" + openkey;
	qstr += "&openid=" + Crypt::UrlEncodeForTX(openid) + "&openkey=" + Crypt::UrlEncodeForTX(openkey);
	string price = CTrans::UTOS((unsigned)(itemInfo.price));
	qsig += "&price=" + price;
	qstr += "&price=" + Crypt::UrlEncodeForTX(price);
	qsig += "&ts=";
	qstr += "&ts=";
	string ts = CTrans::UTOS((unsigned)time(NULL));
	string uids = CTrans::UTOS(uid);
	qsig += ts + "&uid=" + uids + "&userip=" + userip;
	qstr += ts + "&uid=" + Crypt::UrlEncodeForTX(uids) + "&userip=" + Crypt::UrlEncodeForTX(userip);

	string osig = "GET&" + Crypt::UrlEncodeForTX("/pay/buy_goods") + "&" + Crypt::UrlEncodeForTX(qsig);
	string key = appkey + "&";
	string bsig = Crypt::HmacSha1(osig, key);
	string sig;
	Crypt::Base64Encode(sig, bsig);
	url += qstr + "&sig=" + Crypt::UrlEncodeForTX(sig);

	//info_log("[qqtopup][url=%s]",url.c_str());
	//info_log("[qqtopup][osig=%s,key=%s]",osig.c_str(),key.c_str());
	string response;

	error_log("[buy_goods]%s",url.c_str());
	if (!Network::HttpGetRequest(response, url) || response.empty())
	{
		error_log("[http request fail][openid=%s]",openid.c_str());
		ERROR_RETURN_MSG(R_ERR_NO_DATA, "http_request_fail");
	}
	error_log("[buy_goods_response]%s",response.c_str());

	Json::Value result;
	Json::Reader reader;
	if (response.empty() || !reader.parse(response, result)
			|| !result.isMember("url_params") || !result["url_params"].isString())
	{
		error_log("[response error][openid=%s,response=%s]",openid.c_str(),response.c_str());
		ERROR_RETURN_MSG(R_ERR_NO_DATA, "response_error");
	}

	url_params = result["url_params"].asString();

	return 0;
}


