#include "LogicInc.h"

int main(int argc, char *argv[]) {
	if (argc != 6) {
		cout << "usage: InformMarketTask pfkey openid openkey contractid url" << endl;
		return 1;
	}

	if (!Kernel::Init(APP_CONFIG_PATH)) {
		cout << "Init kernel fail!" << endl;
		return -1;
	}
	Config::SetDomain(0);

	string pf = "qzone";
	string pfkey = argv[1];
	string openid = argv[2];
	string openkey = argv[3];
	string contractid = argv[4];
	string url = argv[5];
	string zoneid;
	string appid = "100688853";
	string appkey = "a8c1850bb4043bd69bfac68d9f5903be";

	url = "https://"+url+"/v3/pay/inform_task_completed?";
	//test config
	//string url = "https://119.147.19.43 openapi.tencentyun.com/v3/pay/inform_task_completed?";
	string qsig = "appid=" + appid;
	string qstr = "appid=" + Crypt::UrlEncodeForTX(appid);

	qsig += "&contractid=" + contractid;
	qstr += "&contractid=" + Crypt::UrlEncodeForTX(contractid);

	qsig += "&openid=" + openid + "&openkey=" + openkey;
	qstr += "&openid=" + Crypt::UrlEncodeForTX(openid) + "&openkey=" + Crypt::UrlEncodeForTX(openkey);

	qsig += "&pf=" + pf;
	qstr += "&pf=" + Crypt::UrlEncodeForTX(pf);

	qsig += "&pfkey=" + pfkey;
	qstr += "&pfkey=" + Crypt::UrlEncodeForTX(pfkey);

	string ts;
	String::Format(ts, "%u", Time::GetGlobalTime());
	qsig += "&ts=" + ts;
	qstr += "&ts=" + Crypt::UrlEncodeForTX(ts);

	qsig += "&zoneid=" + zoneid;
	qstr += "&zoneid=" + Crypt::UrlEncodeForTX(zoneid);

	string osig = "GET&" + Crypt::UrlEncodeForTX("/v3/pay/inform_task_completed") + "&" + Crypt::UrlEncodeForTX(qsig);
	string key = appkey + "&";
	string bsig = Crypt::HmacSha1(osig, key);
	string sig;
	Crypt::Base64Encode(sig, bsig);
	url += qstr + "&sig=" + Crypt::UrlEncodeForTX(sig);

	string response;
	if (!Network::HttpGetRequest(response, url) || response.empty())
	{
		cout<<"[http request fail]openid="<<openid<<endl;
		return -1;
	}
	Json::Value result;
	Json::Reader reader;
	int responsetRet = 0;
	if (   response.empty()
		|| !reader.parse(response, result)
		|| !Json::GetInt(result,"ret", responsetRet)
		|| responsetRet != 0)
	{
		cout<<"[response error]openid="<<openid<<"response="<<response<<endl;
		return -1;
	}
	cout<<"ok! response="<<response<<endl;

	return 0;
}

