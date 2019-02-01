#include "LogicRenrenPay.h"

int CLogicRenrenPay::PrePay(const string &openid, const string &openkey, int type, string &orderId, string &token)
{
	unsigned int call_id = 0;
	mtime(call_id);
	unsigned int ts = 0;
	unixtime(ts);
	string api_key = OpenPlatform::GetPlatform()->GetConfig("appid");
	string appkey = OpenPlatform::GetPlatform()->GetConfig("appkey");

	unsigned uid = 0;
	CLogicUserBasic logicUserBasic;
	int ret = logicUserBasic.GetUid(uid, OpenPlatform::GetType(), openid);
	if (ret != 0)
		return ret;

	string desc;
	int credit = 0;
	switch(type)
	{
	case 301:credit = 5;desc = "5";break;
	case 302:credit = 10;desc = "10";break;
	case 303:credit = 50;desc = "50";break;
	case 304:credit = 100;desc = "100";break;
	case 305:credit = 200;desc = "200";break;
	case 306:credit = 500;desc = "500";break;
	default:
		PARAM_ERROR_RETURN_MSG("unknown_type");
	}

	DataPayHistory payHis;
	payHis.channel = PCT_RENREN;
	payHis.channel_pay_id = "";
	payHis.count = 1;
	payHis.credit = credit * 100;	//以分为单位
	payHis.open_id = openid;
	payHis.status = PST_PENDING;
	payHis.type = type;
	payHis.uid = uid;
	payHis.pay_id = 0;
	payHis.time = 0;
	CLogicPay logicPay;
	ret = logicPay.AddPayHistory(payHis);
	if (ret != 0)
		return ret;

	orderId = Convert::UInt64ToString(payHis.pay_id);

	string ssig;
	String::Format(ssig, "amount=%dapi_key=%scall_id=%udesc=%sformat=JSONmethod=pay.regOrderorder_id=%ssession_key=%sv=1.0%s",
			credit, api_key.c_str(), call_id, desc.c_str(), orderId.c_str(), openkey.c_str(), appkey.c_str());
	string sig = Crypt::Md5Encode(ssig);
	string qstr;
	String::Format(qstr, "amount=%d&api_key=%s&call_id=%u&desc=%s&format=JSON&method=pay.regOrder&order_id=%s&session_key=%s&v=1.0&sig=%s",
			credit, api_key.c_str(), call_id, desc.c_str(), orderId.c_str(), openkey.c_str(), sig.c_str());

	string apiUrl = OpenPlatform::GetPlatform()->GetConfig("apiurl");

	info_log("[renrenpay][uid=%d,qstr=%s]", uid,qstr.c_str());

	string response;
	if(!Network::HttpPostRequest(response, apiUrl, qstr))
	{
		error_log("[http request fail][uid=%u,response=%s]",uid, response.c_str());
		PT_ERROR_RETURN_MSG("request_error");
	}
	Json::Reader reader;
	Json::Value value;
	if(!reader.parse(response, value))
	{
		error_log("[json parse fail][uid=%u,response=%s]",uid, response.c_str());
		PT_ERROR_RETURN_MSG("response_error");
	}
	token = value["token"].asString();
	if (token.empty())
	{
		error_log("[token is empty][uid=%u,response=%s]", uid, response.c_str());
		PT_ERROR_RETURN_MSG("token_error");
	}

	return 0;
}

int CLogicRenrenPay::DeliverGood(const string &openid, const string &sig, const string orderId, int &amount)
{
	unsigned uid = 0;
	CLogicUserBasic logicUserBasic;
	int ret = logicUserBasic.GetUid(uid, OpenPlatform::GetType(), openid);
	if (ret != 0)
		return ret;
	string check_osig = "dawx1234" + openid;
	string osig = Crypt::Md5Encode(check_osig);
	if (osig != sig)
	{
		error_log("[check skey fail][uid=%d,sig=%s,osig=%s,openid=%s]",
				uid,sig.c_str(),osig.c_str(),openid.c_str());
		PARAM_ERROR_RETURN_MSG("sig_error");
	}
	DataPayHistory payHis;
	CLogicPay logicPay;
	uint64_t payId = 0;
	if (!Convert::StringToUInt64(payId, orderId))
	{
		error_log("[order_id error][uid=%d,orderId=%s,openid=%s]",
				uid,orderId.c_str(),openid.c_str());
		PARAM_ERROR_RETURN_MSG("order_id_error");
	}
	ret = logicPay.GetPayHistory(uid, payId, payHis);
	if (ret != 0)
		return ret;
	int credit = 0;
	int cash = 0;
	switch(payHis.type)
	{
	case 301:credit = 5;cash = 50;break;
	case 302:credit = 10;cash = 110;break;
	case 303:credit = 50;cash = 575;break;
	case 304:credit = 100;cash = 1200;break;
	case 305:credit = 200;cash = 2450;break;
	case 306:credit = 500;cash = 6250;break;
	default:
		PARAM_ERROR_RETURN_MSG("unknown_type");
	}
	if (payHis.status != PST_OK)
	{
		payHis.status = PST_OK;
		ret = logicPay.SetPayHistoryStatus(uid, payId, PST_OK);
		if (ret != 0)
			return ret;
		ret = logicPay.ChangePay(uid, cash, 0, "RENRENTOPUP");
		if (ret != 0)
			return ret;
	}
	amount = credit;

	CDataUser userDb;
	userDb.AddAccCharge(uid, cash);
	return 0;
}
