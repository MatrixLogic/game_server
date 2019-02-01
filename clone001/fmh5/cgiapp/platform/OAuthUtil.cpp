/*
 * OAuthUtil.cpp
 *
 *  Created on: 2011-9-28
 *      Author: dada
 */

//RFC 5849: http://tools.ietf.org/html/rfc5849

#include "OAuthUtil.h"

namespace OAuth
{

	static unsigned char g_encodeTable[] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	string Encode(const string &sData)
	{
		static char s_itox[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
		string sValue;
		const char* epos = sData.data() + sData.size();
		for(const char* bpos = sData.data(); bpos < epos; bpos++)
		{
			unsigned char ch = (unsigned char)(*bpos);
			if(g_encodeTable[ch] == 0)
			{
				sValue += "%";
				sValue += s_itox[ch >> 4];
				sValue += s_itox[ch & 0x0F];
			}
			else
			{
				sValue += ch;
			}
		}
		return sValue;
	}

	string Decode(const string &sData)
	{
		return CBasic::unescape(sData);
	}

	static bool ParamCompare(const pair<string, string> &left, const pair<string, string> &right)
	{
		int cmp = left.first.compare(right.first);
		if(cmp == 0)
		{
			cmp = left.second.compare(right.second);
		}
		return cmp < 0;
	}

	bool HttpRequest(string &response, HttpRequestMethod method, const string &url,
			const map<string, string> &params, const OAuthParam &oauthParam)
	{
		//get param
		string strSignMethod;
		if(oauthParam.SignMethod == PLAINTEXT)
		{
			strSignMethod = "PLAINTEXT";
		}
		else if(oauthParam.SignMethod == HMAC_SHA1)
		{
			strSignMethod = "HMAC-SHA1";
		}
		else if(oauthParam.SignMethod == RSA_SHA1)
		{
			strSignMethod = "RSA-SHA1";
		}
		else
		{
			response = "unknow_signature_method";
			return false;
		}
		static char s_nonceSet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
		string nonce;
		nonce.resize(32);
		for(int i = 0; i < 32; i++)
		{
			nonce[i] = s_nonceSet[Math::GetRandomInt(62)];
		}
		string ts = Convert::UIntToString(Time::GetGlobalTime());

		//sort param
		vector< pair<string, string> > vecParams;
		for(map<string, string>::const_iterator itr = params.begin(); itr != params.end(); itr++)
		{
			///TODO: '+' decode as whitespace
			vecParams.push_back(pair<string, string>(Encode(Decode(itr->first)), Encode(Decode(itr->second))));
		}
		vecParams.push_back(pair<string, string>("oauth_consumer_key", Encode(Decode(oauthParam.AppKey))));
		vecParams.push_back(pair<string, string>("oauth_signature_method", strSignMethod));
		vecParams.push_back(pair<string, string>("oauth_timestamp", ts));
		vecParams.push_back(pair<string, string>("oauth_nonce", nonce));
		vecParams.push_back(pair<string, string>("oauth_version", "1.0"));
		sort(vecParams.begin(), vecParams.end(), ParamCompare);

		//get base string
		string baseString;
		if(method == GET)
		{
			baseString = "GET";
		}
		else
		{
			baseString = "POST";
		}
		baseString.append("&");
		baseString.append(Encode(url));
		baseString.append("&");
		bool isFirst = true;
		for(vector< pair<string, string> >::iterator itr = vecParams.begin(); itr != vecParams.end(); itr++)
		{
			if(isFirst)
			{
				isFirst = false;
			}
			else
			{
				baseString.append("%26");
			}
			baseString.append(Encode(itr->first));
			baseString.append("%3D");
			baseString.append(Encode(itr->second));
		}

		//calc sign
		string key = Encode(oauthParam.AppSecret) + "&" + Encode(oauthParam.TokenSecret);
		string signature;
		if(oauthParam.SignMethod == PLAINTEXT)
		{
			signature = key;
		}
		else if(oauthParam.SignMethod == HMAC_SHA1)
		{
			string rawSignature = Crypt::HmacSha1(baseString, key);
			Crypt::Base64Encode(signature, rawSignature);
		}
		else if(oauthParam.SignMethod == RSA_SHA1)
		{
			string rawSignature;
			if(!Crypt::RsaSha1Signature(baseString, key, rawSignature))
			{
				response = "signature_fail";
				return false;
			}
			Crypt::Base64Encode(signature, rawSignature);
		}
		vecParams.push_back(pair<string, string>("oauth_signature", Encode(signature)));

		//gen params
		string strParams;
		isFirst = true;
		for(map<string, string>::const_iterator itr = params.begin(); itr != params.end(); itr++)
		{
			if(itr->first.find("oauth_") != 0)
			{
				if(isFirst)
				{
					isFirst = false;
				}
				else
				{
					strParams.append("&");
				}
				strParams.append(itr->first);
				strParams.append("=");
				strParams.append(itr->second);
			}
		}

		//gen oauth params
		string strOauthParams;
		isFirst = true;
		if(oauthParam.ParamTrans == AuthorizationHeader)
		{
			strOauthParams.append("OAuth realm=\"");
			strOauthParams.append(Encode(oauthParam.Realm));
			strOauthParams.append("\"");

		}
		for(vector< pair<string, string> >::iterator itr = vecParams.begin(); itr != vecParams.end(); itr++)
		{
			if(itr->first.find("oauth_") == 0)
			{
				if(oauthParam.ParamTrans == AuthorizationHeader)
				{
					strOauthParams.append(",");
					strOauthParams.append(itr->first);
					strOauthParams.append("=\"");
					strOauthParams.append(itr->second);
					strOauthParams.append("\"");
				}
				else
				{
					if(isFirst)
					{
						isFirst = false;
					}
					else
					{
						strOauthParams.append("&");
					}
					strOauthParams.append(itr->first);
					strOauthParams.append("=");
					strOauthParams.append(itr->second);
				}
			}
		}

		//gen request data
		string queryUrl = url;
		if(method == GET)
		{
			if(!strParams.empty())
			{
				queryUrl.append("?");
				queryUrl.append(strParams);
			}
			if(oauthParam.ParamTrans == RequestURIQuery)
			{
				if(strParams.empty())
				{
					queryUrl.append("?");
				}
				else
				{
					queryUrl.append("&");
				}
				queryUrl.append(strOauthParams);
			}
		}
		string data;
		if(method == POST)
		{
			data = strParams;
		}
		if(oauthParam.ParamTrans == FormEncodedBody)
		{
			if(!data.empty())
			{
				data.append("&");
			}
			data.append(strOauthParams);
		}

		//request
		if(oauthParam.ParamTrans == AuthorizationHeader)
		{
			map<string, string> headers;
			headers["Authorization"] = strOauthParams;
			if(method == GET)
			{

				return  Network::HttpGetRequest(response, queryUrl, headers);
				//info_log("[oauth req ][url=%s,Authorization=%s,response=%s]",queryUrl.c_str(),strOauthParams.c_str(),response.c_str());
				//return rettt;
			}
			else
			{
				return Network::HttpPostRequest(response, queryUrl, data, headers);
			}
		}
		else
		{
			if(method == GET)
			{
				return Network::HttpGetRequest(response, queryUrl);
			}
			else
			{
				return Network::HttpPostRequest(response, queryUrl, data);
			}
		}
	}

	bool CheckSignature(string signature, HttpRequestMethod method, const string &url,
			const map<string, string> &params, const OAuthParam &oauthParam)
	{
		//sort param
		vector< pair<string, string> > vecParams;
		for(map<string, string>::const_iterator itr = params.begin(); itr != params.end(); itr++)
		{
			///TODO: '+' decode as whitespace
			vecParams.push_back(pair<string, string>(Encode(Decode(itr->first)), Encode(Decode(itr->second))));
		}
		sort(vecParams.begin(), vecParams.end(), ParamCompare);

		//get base string
		string baseString;
		if(method == GET)
		{
			baseString = "GET";
		}
		else
		{
			baseString = "POST";
		}
		baseString.append("&");
		baseString.append(Encode(url));
		baseString.append("&");
		bool isFirst = true;
		for(vector< pair<string, string> >::iterator itr = vecParams.begin(); itr != vecParams.end(); itr++)
		{
			if(isFirst)
			{
				isFirst = false;
			}
			else
			{
				baseString.append("%26");
			}
			baseString.append(Encode(itr->first));
			baseString.append("%3D");
			baseString.append(Encode(itr->second));
		}

		//calc sign
		string key = Encode(oauthParam.AppSecret) + "&" + Encode(oauthParam.TokenSecret);
		string signatureCheck;
		string rawSignature;
		if(oauthParam.SignMethod == PLAINTEXT)
		{
			signatureCheck = key;
		}
		else if(oauthParam.SignMethod == HMAC_SHA1)
		{
			rawSignature = Crypt::HmacSha1(baseString, key);
			Crypt::Base64Encode(signatureCheck, rawSignature);
		}
		else if(oauthParam.SignMethod == RSA_SHA1)
		{
			if(!Crypt::RsaSha1Signature(baseString, key, rawSignature))
			{
				return false;
			}
			Crypt::Base64Encode(signatureCheck, rawSignature);
		}
		if(signature != signatureCheck)
		{
			error_log("[signature_error][basestring=%s,key=%s,sign=%s,realsign=%s]",
				baseString.c_str(), key.c_str(), signature.c_str(), signatureCheck.c_str());
			return false;
		}
		return true;
	}

	bool GetAuthHeader(map<string, string> &oauthHeader)
	{
		const char *pcAuth = getenv("HTTP_AUTHORIZATION");
		if(pcAuth == NULL)
		{
			return false;
		}
		string sAuth = pcAuth;
		if(!String::StartWith(sAuth, "OAuth"))
		{
			return false;
		}
		sAuth = sAuth.substr(5);
		vector<string> params;
		CBasic::StringSplitTrim(sAuth, ",", params);
		oauthHeader.clear();
		for(vector<string>::iterator itr = params.begin(); itr != params.end(); itr++)
		{
			String::Trim(*itr);
			string::size_type size = itr->size();
			if(size < 4)
			{
				continue;
			}
			if(itr->at(size - 1) != '\"')
			{
				return false;
			}
			string::size_type mid = itr->find('=');
			if(mid == string::npos)
			{
				return false;
			}
			if(size - mid < 3)
			{
				return false;
			}
			if(itr->at(mid + 1) != '\"')
			{
				return false;
			}
			string key = itr->substr(0, mid);
			string value = itr->substr(mid + 2, size - mid - 3);
			oauthHeader[key] = value;
		}
		return true;
	}

#define SESSION_TOKEN "token"
#define MAX_TOKEN_COUNT 5

	bool SaveToken(const string &token, const string &tokenSecret)
	{
		unsigned hash = CCRC::GetCrc32(token) & 0x7fffffff;
		string oldValue = Session::GetValue(hash, SESSION_TOKEN);
		Json::Value value;
		if(oldValue.empty())
		{
			value = Json::Value(Json::arrayValue);
		}
		else
		{
			Json::FromString(value, oldValue);
			if(!value.isArray())
			{
				value = Json::Value(Json::arrayValue);
			}
		}
		unsigned count = value.size();
		for(unsigned i = count; i > 0; i--)
		{
			value[i + 1] = value[i];
		}
		if(count >= MAX_TOKEN_COUNT)
		{
			value.resize(MAX_TOKEN_COUNT);
		}
		value[0u]["t"] = token;
		value[0u]["s"] = tokenSecret;

		string newValue = Json::ToString(value);
		int ret = Session::SetValue(hash, SESSION_TOKEN, newValue);
		if(ret != 0)
		{
			error_log("[save_token_fail][ret=%d,hash=%u,value=%s]", ret, hash, newValue.c_str());
			return false;
		}
		return true;
	}

	bool GetTokenSecret(const string &token, string &tokenSecret)
	{
		unsigned hash = CCRC::GetCrc32(token) & 0x7fffffff;
		string strValue;
		int ret = Session::GetValue(hash, SESSION_TOKEN, strValue);
		if(ret != 0)
		{
			if(ret != R_ERR_NO_DATA)
			{
				error_log("[get_token_value_fail][ret=%d,hash=%u,token=%s]", ret, hash, token.c_str());
			}
			return false;
		}
		Json::Value value;
		Json::FromString(value, strValue);
		if(!value.isArray())
		{
			error_log("[token_data_error][hash=%u,token=%s,value=%s]", hash, token.c_str(), strValue.c_str());
			return false;
		}
		unsigned count = value.size();
		for(unsigned i = 0; i < count; i++)
		{
			Json::Value &item = value[i];
			if(item.isObject())
			{
				string t;
				if(Json::GetString(item, "t", t))
				{
					if(t == token)
					{
						if(Json::GetString(item, "s", tokenSecret))
						{
							return true;
						}
					}
				}
			}
		}
		return false;
	}

}
