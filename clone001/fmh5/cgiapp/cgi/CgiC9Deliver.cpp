/*
 * CgiC9Deliver.cpp
 *
 *  Created on: 2015-8-26
 *      Author: Ralf
 */



#include "LogicInc.h"
#include "C9Platform.h"

class CCgiC9Deliver : public CCgiBase
{
public:
	CCgiC9Deliver() : CCgiBase("c9deliver")
	{
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_CHECK_PLATFORM);
	}

	CGI_DIRECT_PROCESS(deliver)

	int deliver()
	{
		const unsigned serverid = CCGIIn::GetCGIInt("serverid");
		const string serverid_s = CCGIIn::GetCGIStr("serverid");
		const string order_id = CCGIIn::GetCGIStr("order_id");
		const string openid = CCGIIn::GetCGIStr("uid");
		const unsigned cash = CCGIIn::GetCGIInt("coins");
		const string cash_s = CCGIIn::GetCGIStr("coins");
		const double moneys = CCGIIn::GetCGIDbl("moneys", 0, 100000000, 0, 0);
		unsigned money = (unsigned)(moneys*100);
		const string moneys_s = CCGIIn::GetCGIStr("moneys");
		const unsigned time = CCGIIn::GetCGIInt("time");
		const string time_s = CCGIIn::GetCGIStr("time");
		const string sign = CCGIIn::GetCGIStr("sign");

		Json::Value result;
		result["errno"] = 0;
		result["errmsg"] = "充值成功";
		result["data"]["order_id"] = order_id;
		result["data"]["uid"] = openid;
		result["data"]["role_id"] = 0;
		result["data"]["role_name"] = "";
		result["data"]["platfrom"] = 0;
		result["data"]["gkey"] = "sg17";
		result["data"]["skey"] = serverid;
		result["data"]["coins"] = cash;
		result["data"]["moneys"] = moneys;
		result["data"]["time"] = Time::GetGlobalTime();

		if(openid.empty() || order_id.empty() || sign.empty() ||
		cash == CCGIIn::CGI_INT_ERR || time == CCGIIn::CGI_INT_ERR || cash == 0)
		{
			result["errno"] = -1;
			result["errmsg"] = "参数不全";
			return PrintResult(result);
		}

		if (Math::Abs(Time::GetGlobalTime() - time) > 180)
		{
			result["errno"] = -4;
			result["errmsg"] = "请求超时";
			return PrintResult(result);
		}

		string osig  = serverid_s + order_id + openid + moneys_s + cash_s + time_s + OpenPlatform::GetPlatform()->GetAppKey();
		string sig = Crypt::Md5Encode(osig);
		if(sig != sign)
		{
			result["errno"] = -2;
			result["errmsg"] = "签名错误";
			return PrintResult(result);
		}

		int ret = 0;
		unsigned uid = 0;
		CDataUserMapping dbUserMapping;
		ret = dbUserMapping.GetUID(openid, serverid, uid);
		if(ret == R_ERR_NO_DATA)
		{
			result["errno"] = -3;
			result["errmsg"] = "用户不存在";
			return PrintResult(result);
		}
		if(ret || uid == 0)
		{
			result["errno"] = -5;
			result["errmsg"] = "系统错误";
			return PrintResult(result);
		}
		result["data"]["role_id"] = uid;

		//防重发, 如果该payment_id已经处理, 那么不再增加钻石
		DataPayHistory payHistory;
		CDataPayHistory dbPayHistory;
		ret = dbPayHistory.GetPayHistory(uid, OpenPlatform::GetType(), order_id, payHistory);
		if(ret == 0)
		{
			result["errno"] = 1;
			result["errmsg"] = "订单重复";
			return PrintResult(result);
		}

		CSG17Packet packet;
		packet.cmd = PROTOCOL_DELIVER;
		Common::Pay* msg = new Common::Pay;
		msg->set_uid(uid);
		msg->set_cash(cash);
		msg->set_ts(Time::GetGlobalTime());
		packet.m_msg = msg;

		ret = BattleSocket::Send(Config::GetZoneByUID(uid), &packet);
		if(ret)
		{
			result["errno"] = -7;
			result["errmsg"] = "系统错误";
			return PrintResult(result);
		}

		CSG17Packet reply;
		ret = BattleSocket::Receive(Config::GetZoneByUID(uid), &reply);
		if(ret)
		{
			result["errno"] = -7;
			result["errmsg"] = "系统错误";
			return PrintResult(result);
		}

		Common::ReplyPay* rmsg = (Common::ReplyPay*)reply.m_msg;
		ret = rmsg->ret();
		if(ret)
		{
			result["errno"] = -7;
			result["errmsg"] = "系统错误";
			return PrintResult(result);
		}

		DataPayHistory payhis;
		payhis.channel = OpenPlatform::GetType();
		payhis.channel_pay_id = order_id;
		payhis.count = cash;
		payhis.credit = money;
		payhis.status = PST_OK;
		payhis.type = 0;
		payhis.uid = uid;
		payhis.open_id = openid;
		CLogicQQPay logicPay;
		ret = logicPay.AddPayHistory(payhis);
		if (ret != 0)
		{
			error_log("[AddPayHistory fail][ret=%d,openid=%s,order_id=%s]",
					ret,openid.c_str(),order_id.c_str());
		}

		CGI_SEND_LOG("openid=%s&order_id=%s&serverid=%d&num=%u", openid.c_str(),order_id.c_str(),serverid,cash);
		return PrintResult(result);
	}

private:
	int PrintResult(Json::Value &result)
	{
		Json::FastWriter writer;
		string res = writer.write(result);
		CgiUtil::PrintText(res.c_str());
		unsigned ret = 0;
		Json::GetUInt(result,"errno",ret);
		return ret;
	}
};

CGI_MAIN(CCgiC9Deliver)

