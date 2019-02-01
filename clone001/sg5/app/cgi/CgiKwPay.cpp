#include "LogicInc.h"

//参数
//openid	用户ID
//payid	订单号（小于64个字符）
//cash	充值金币数
//credit	花费现金数
//ts	快玩服务器UNIX时间戳，与本地时间相差大于60秒将拒绝
//key	MD5(ts + "|" + openid + "|" + payid +"|" + cash + "|" + credit + "|" + appkey)
//
//返回
//JSON格式
//成功	{"error":0,"key":"XXX","ts":100000}
//失败	{"error":"param_error","key":"XXX","ts":100000}
//error	0表示成功，错误时返回错误字符串
//ts	玩具军团服务器UNIX时间戳，建议判断是否在正常范围内
//key	MD5(ts + "|" + payid + "|" + appkey)，校验KEY，确保是服务器返回

class CCgiKwPay : public CCgiBase
{
public:

	CCgiKwPay() : CCgiBase("kwpay")
	{
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_CHECK_PLATFORM);
		SetFeature(CF_PRINT_JSON);
	}

	CGI_DIRECT_PROCESS(KwPay)

	int KwPay()
	{
		//check platform
		if(OpenPlatform::GetType() != PT_KUAIWAN)
		{
			error_log("[platform_error][cgi=KwPay,platform=%d]", OpenPlatform::GetType());
			SetError(R_ERR_REFUSE, "server_error");
			return R_ERR_REFUSE;
		}

		//check param
		string openid = CCGIIn::GetCGIStr("openid");
		string payid = CCGIIn::GetCGIStr("payid");
		string sCash = CCGIIn::GetCGIStr("cash");
		string sCredit = CCGIIn::GetCGIStr("credit");
		string sTs = CCGIIn::GetCGIStr("ts");
		string key = CCGIIn::GetCGIStr("key");
		if(openid.empty() || payid.empty() || key.empty())
		{
			error_log("[param_error][cgi=KwPay,openid=%s,payid=%s,cash=%s,credit=%s,ts=%s,key=%s]",
					openid.c_str(), payid.c_str(), sCash.c_str(), sTs.c_str(), sCredit.c_str(), key.c_str());
			return R_ERR_PARAM;
		}
		unsigned cash;
		if(!Convert::StringToUInt(cash, sCash))
		{
			error_log("[param_error][cgi=KwPay,openid=%s,payid=%s,cash=%s,credit=%s,ts=%s,key=%s]",
					openid.c_str(), payid.c_str(), sCash.c_str(), sTs.c_str(), sCredit.c_str(), key.c_str());
			return R_ERR_PARAM;
		}
		if(cash <= 0)
		{
			error_log("[param_error][cgi=KwPay,openid=%s,payid=%s,cash=%s,credit=%s,ts=%s,key=%s]",
					openid.c_str(), payid.c_str(), sCash.c_str(), sTs.c_str(), sCredit.c_str(), key.c_str());
			return R_ERR_PARAM;
		}
		unsigned credit;
		if(!Convert::StringToUInt(credit, sCredit))
		{
			error_log("[param_error][cgi=KwPay,openid=%s,payid=%s,cash=%s,credit=%s,ts=%s,key=%s]",
					openid.c_str(), payid.c_str(), sCash.c_str(), sTs.c_str(), sCredit.c_str(), key.c_str());
			return R_ERR_PARAM;
		}
		unsigned ts;
		if(!Convert::StringToUInt(ts, sTs))
		{
			error_log("[param_error][cgi=KwPay,openid=%s,payid=%s,cash=%s,credit=%s,ts=%s,key=%s]",
					openid.c_str(), payid.c_str(), sCash.c_str(), sTs.c_str(), sCredit.c_str(), key.c_str());
			return R_ERR_PARAM;
		}
		int timeDelta = Time::GetGlobalTime() - ts;
		if(timeDelta > 60 || timeDelta < -60)
		{
			error_log("[check_time_error][cgi=KwPay,openid=%s,payid=%s,cash=%s,credit=%s,ts=%u,real_ts=%u,delta=%d]",
					openid.c_str(), payid.c_str(), sCash.c_str(), sCredit.c_str(), ts, Time::GetGlobalTime(), timeDelta);
		}

		//check key
		string check;
		String::Format(check, "%s|%s|%s|%s|%s|%s", sTs.c_str(), openid.c_str(), payid.c_str(),
				sCash.c_str(), sCredit.c_str(), OpenPlatform::GetPlatform()->GetAppKey().c_str());
		string checkKey = Crypt::Md5Encode(check);
		if(!String::EqualNoCase(key, checkKey))
		{
			error_log("[key_error][cgi=KwPay,openid=%s,payid=%s,cash=%s,credit=%s,ts=%s,key=%s,realkey=%s]",
					openid.c_str(), payid.c_str(), sCash.c_str(), sCredit.c_str(), sTs.c_str(), key.c_str(), checkKey.c_str());
			return R_ERR_PARAM;
		}

		//get user
		int ret;
		CLogicUserBasic logicUser;
		unsigned uid;
		ret = logicUser.GetUid(uid, OpenPlatform::GetType(), openid);
		if(ret == R_ERR_NO_DATA)
		{
			error_log("[pay_no_user][channel=kuaiwan,platform=%d,openid=%s,payid=%s,cash=%u,credit=%u]",
					OpenPlatform::GetType(), openid.c_str(), payid.c_str(), cash, credit);
			SetError(R_ERR_NO_DATA, "no_user");
			return R_ERR_NO_DATA;
		}
		if(ret != 0)
		{
			error_log("[GetUid fail][channel=kuaiwan,ret=%d,platform=%d,openid=%s,payid=%s,cash=%u,credit=%u]",
					ret, OpenPlatform::GetType(), openid.c_str(), payid.c_str(), cash, credit);
			return ret;
		}

		//set pay
		DataPayHistory payHistory;
		CLogicPay logicPay;
		ret = logicPay.GetPayHistory(uid, PCT_KUAIWAN, payid, payHistory);
		if(ret == 0)
		{
			error_log("[reduplicate_pay_id][channel=kuaiwan,platform=%d,openid=%s,payid=%s,cash=%u,credit=%u,exist_pay_id=%llu]",
					OpenPlatform::GetType(), openid.c_str(), payid.c_str(), cash, credit, payHistory.pay_id);
			SetError(R_ERR_DATA, "reduplicate_pay_id");
			return R_ERR_DATA;
		}
		unsigned cashNow;
		ret = logicPay.ChangePay(uid, cash, 0, "KWTOPUP");
		if(ret != 0)
		{
			error_log("[ChangeCash fail][channel=kuaiwan,ret=%d,platform=%d,openid=%s,payid=%s,cash=%u,credit=%u]",
					ret, OpenPlatform::GetType(), openid.c_str(), payid.c_str(), cash, credit);
			return ret;
		}
		payHistory.uid = uid;
		payHistory.type = 0;
		payHistory.channel = PCT_KUAIWAN;
		payHistory.channel_pay_id = payid;
		payHistory.credit = credit * 100;	//以分为单位记录
		payHistory.count = cash;
		payHistory.status = PST_OK;
		payHistory.open_id = openid;
		ret = logicPay.AddPayHistory(payHistory);
		if(ret != 0)
		{
			error_log("[AddPayHistory fail][channel=kuaiwan,ret=%d,platform=%d,openid=%s,payid=%s,cash=%u,credit=%u]",
					ret, OpenPlatform::GetType(), openid.c_str(), payid.c_str(), cash, credit);
		}

		CDataUser userDb;
		userDb.AddAccCharge(uid, cash);

		//set result
		m_jsonResult["ts"] = Time::GetGlobalTime();
		String::Format(check, "%u|%s|%s", m_jsonResult["ts"].asUInt(), payid.c_str(), OpenPlatform::GetPlatform()->GetAppKey().c_str());
		m_jsonResult["key"] = Crypt::Md5Encode(check);

		CGI_SEND_LOG("platform=%d&openid=%s&payid=%s&cash=%u&credit=%u&ts=%u&dbpayid=%llu&key=%s",
				OpenPlatform::GetType(), openid.c_str(), payid.c_str(), cash, credit,
				m_jsonResult["ts"].asUInt(), payHistory.pay_id, m_jsonResult["key"].asString().c_str());
		return R_SUCCESS;
	}

};

CGI_MAIN(CCgiKwPay)

