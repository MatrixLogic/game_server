#include "LogicQQPay.h"

#define BLUE1_VIP_MAX 12
#define BLUE2_VIP_MAX 60

CLogicQQPay::T_ItemMap CLogicQQPay::g_ItemInfo;

int CLogicQQPay::GetItemInfo(
		const string &itemid,
		QQItemInfo &item)
{
	static bool bInit = false;
	if (!bInit)
	{
		string itemFile;
		itemFile = MainConfig::GetAllServerPath(CONFIG_QQ_ITEM_INFO);
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
			QQItemInfo itemTmp;
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

int CLogicQQPay::deliver(
		const string &appid,
		const string &openid,
		const string &payitem,
		int amt,
		const string &billno)
{
	int ret = 0;

	string o_appid = OpenPlatform::GetPlatform()->GetAppId();
	if (appid != o_appid)
	{
		ERROR_RETURN_MSG(4, "请求参数错误：（appid）");
	}
	vector<string> items;
	CBasic::StringSplit(payitem, "*", items);
	if (items.size() != 3)
	{
		ERROR_RETURN_MSG(4, "请求参数错误：（payitem）");
	}

	unsigned uid = 0;
	vector<string> uidanitem;
	CBasic::StringSplit(items[0], "_", uidanitem);
	if (uidanitem.size() == 2)
	{
		Convert::StringToUInt(uid, uidanitem[0]);
		items[0] = uidanitem[1];
	}

	//防重发, 如果该payment_id已经处理, 那么不再增加钻石
	DataPayHistory payHistory;
	CDataPayHistory dbPayHistory;
	ret = dbPayHistory.GetPayHistory(uid, OpenPlatform::GetType(), billno, payHistory);
	if(ret == 0)
		return 0;

	QQItemInfo itemInfo;
	ret = GetItemInfo(items[0], itemInfo);
	if (ret != 0)
	{
		ERROR_RETURN_MSG(4, "请求参数错误：（payitem） ");
	}
	int count = CTrans::STOI(items[2]);

	CSG17Packet packet;
	packet.cmd = PROTOCOL_DELIVER;
	Common::Pay* msg = new Common::Pay;
	msg->set_uid(uid);
	msg->set_cash(itemInfo.cash * count);
	msg->set_ts(Time::GetGlobalTime());
	packet.m_msg = msg;

	ret = BattleSocket::Send(Config::GetZoneByUID(uid), &packet);
	if(ret)
	{
		ERROR_RETURN_MSG(5, "system error");
	}

	CSG17Packet reply;
	ret = BattleSocket::Receive(Config::GetZoneByUID(uid), &reply);
	if(ret)
	{
		ERROR_RETURN_MSG(5, "system error");
	}

	Common::ReplyPay* rmsg = (Common::ReplyPay*)reply.m_msg;
	ret = rmsg->ret();
	if(ret)
	{
		ERROR_RETURN_MSG(5, "system error");
	}

	DataPayHistory payhis;
	payhis.channel = PT_QZONE;
	payhis.channel_pay_id = billno;
	payhis.count = itemInfo.cash * count;
	payhis.credit = amt;
	payhis.status = PST_OK;
	payhis.type = itemInfo.type;
	payhis.uid = uid;
	payhis.open_id = openid;
	ret = AddPayHistory(payhis);
	if (ret != 0)
	{
		error_log("[AddPayHistory fail][ret=%d,openid=%s,billno=%s,payitem=%s,amt=%d]",
				ret,openid.c_str(),billno.c_str(),payitem.c_str(),amt);
	}

	return 0;
}

int CLogicQQPay::v3_buy_goods(
		const string &pf,
		const string &pfkey,
		const string &openid,
		const string &openkey,
		const string &userip,
		const string &itemid,
		string &appid,
		string &url_params)
{
	int ret = 0;
	unsigned uid = 0;
	CDataUserMapping dbUserMapping;
	ret = dbUserMapping.GetMapping(openid, PT_TEST, uid);
	if (ret != 0)
		return ret;
	QQItemInfo itemInfo;
	ret = GetItemInfo(itemid, itemInfo);
	if (ret != 0)
		return ret;
	appid = OpenPlatform::GetPlatform()->GetAppId();
	string appkey = OpenPlatform::GetPlatform()->GetAppKey();

	string url = "https://" + OpenPlatform::GetPlatform()->GetConfig("v3domain") + "/v3/pay/buy_goods?";
	string qsig = "appid=" + appid + "&appmode=2&format=json&goodsmeta=";
	string qstr = "appid=" + Crypt::UrlEncodeForTX(appid) + "&appmode=2&format=json&goodsmeta=";
	string tmp = itemInfo.name + "*" + itemInfo.description;
	string goodsmeta;
	Crypt::Base64Encode(goodsmeta, tmp);
	qsig += goodsmeta;
	qstr += Crypt::UrlEncodeForTX(goodsmeta);
	qsig += "&goodsurl=" + itemInfo.url;
	qstr += "&goodsurl=" + Crypt::UrlEncodeForTX(itemInfo.url);
	qsig += "&openid=" + openid + "&openkey=" + openkey;
	qstr += "&openid=" + Crypt::UrlEncodeForTX(openid) + "&openkey=" + Crypt::UrlEncodeForTX(openkey);
	string payitem;
	String::Format(payitem, "%u", uid);
	payitem += "_" + itemid + "*" + CTrans::ITOS(itemInfo.price) + "*1";

	qsig += "&payitem=" + payitem + "&pf=" + pf + "&pfkey=" + pfkey;
	qstr += "&payitem=" + Crypt::UrlEncodeForTX(payitem) + "&pf=" + Crypt::UrlEncodeForTX(pf) + "&pfkey=" + Crypt::UrlEncodeForTX(pfkey);
	if(pf == "3366"){
		qsig += "&platform=30006";
		qstr += "&platform=30006";
	}
	qsig += "&ts=";
	qstr += "&ts=";
	string ts = CTrans::UTOS((unsigned)time(NULL));
	qsig += ts + "&userip=" + userip;
	qstr += ts + "&userip=" + Crypt::UrlEncodeForTX(userip);

	string zoneid;
	if(!Config::GetValue(zoneid, CONFIG_ZONE_ID))
		zoneid = "0";
	qsig += "&zoneid=" + zoneid;
	qstr += "&zoneid=" + Crypt::UrlEncodeForTX(zoneid);

	string osig = "GET&" + Crypt::UrlEncodeForTX("/v3/pay/buy_goods") + "&" + Crypt::UrlEncodeForTX(qsig);
	string key = appkey + "&";
	string bsig = Crypt::HmacSha1(osig, key);
	string sig;
	Crypt::Base64Encode(sig, bsig);
	url += qstr + "&sig=" + Crypt::UrlEncodeForTX(sig);

	//info_log("[qqtopup][url=%s]",url.c_str());
	//info_log("[qqtopup][osig=%s,key=%s]",osig.c_str(),key.c_str());
	string response;
	if (!Network::HttpGetRequest(response, url) || response.empty())
	{
		error_log("[http request fail][openid=%s]",openid.c_str());
		ERROR_RETURN_MSG(R_ERR_NO_DATA, "http_request_fail");
	}
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


int CLogicQQPay::v3_pay_get_token(const string &pfkey,
		const string &pf,
		const string &openkey,
		const string &openid,
		const string &tokentype,
		const string &discountid,
		string &appid,
		string &token,
		string &zoneid)
{
	int ret = 0;
	string ts = CTrans::UTOS((unsigned)time(NULL));
	if(!Config::GetValue(zoneid, CONFIG_ZONE_ID))
		zoneid = "0";

	appid = OpenPlatform::GetPlatform()->GetAppId();
	string appkey = OpenPlatform::GetPlatform()->GetAppKey();

	string url = "https://" + OpenPlatform::GetPlatform()->GetConfig("v3domain") + "/v3/pay/get_token?";
	//string url = "https://119.147.19.43/v3/pay/get_token?";
	string qsig = "appid=" + appid + "&discountid=" + discountid + "&openid=" + openid + "&openkey=" + openkey +"&pf="+ pf+ "&pfkey=" + pfkey + "&tokentype=" + tokentype +"&ts=" + ts + "&version=v3&zoneid=" + zoneid;
	string qstr = "appid=" + appid + "&discountid=" + discountid + "&openid=" + openid + "&openkey=" + openkey +"&pf="+ pf+ "&pfkey=" + pfkey + "&tokentype=" + tokentype +"&ts=" + ts + "&version=v3&zoneid=" + zoneid;

	string osig = "GET&" + Crypt::UrlEncodeForTX("/v3/pay/get_token") + "&" + Crypt::UrlEncodeForTX(qsig);
	string key = appkey + "&";
	string bsig = Crypt::HmacSha1(osig, key);
	string sig;
	Crypt::Base64Encode(sig, bsig);

	url += qstr + "&sig=" + Crypt::UrlEncodeForTX(sig);

	//info_log("[qqtopup][url=%s]",url.c_str());
	//info_log("[qqtopup][osig=%s,key=%s]",osig.c_str(),key.c_str());
	string response;
	if (!Network::HttpGetRequest(response, url) || response.empty())
	{
		error_log("[http request fail][pfkey=%s]",openid.c_str());
		ERROR_RETURN_MSG(R_ERR_NO_DATA, "http_request_fail");
	}
	Json::Value result;
	Json::Reader reader;
	int responsetRet = 0;
	if (response.empty()
		|| !reader.parse(response, result)
		|| !Json::GetInt(result,"ret", responsetRet)
		|| responsetRet != 0)
	{
		error_log("[response error][openid=%s,response=%s]",openid.c_str(),response.c_str());
		ERROR_RETURN_MSG(R_ERR_NO_DATA, "response_error");
	}

	token = result["token"].asString();

	return 0;
}

int CLogicQQPay::AddPayHistory(DataPayHistory &payHistory)
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
