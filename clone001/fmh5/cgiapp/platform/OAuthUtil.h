/*
 * OAuthUtil.h
 *
 *  Created on: 2011-9-28
 *      Author: dada
 */

#ifndef OAUTHUTIL_H_
#define OAUTHUTIL_H_

#include "Kernel.h"

namespace OAuth
{
	enum HttpRequestMethod
	{
		GET,
		POST
	};

	enum SignatureMethod
	{
		PLAINTEXT,
		HMAC_SHA1,
		RSA_SHA1
	};

	enum ParamTransmission
	{
		AuthorizationHeader,
		FormEncodedBody,
		RequestURIQuery
	};

	struct OAuthParam
	{
		string Realm;
		string AppKey;
		string AppSecret;
		string Token;
		string TokenSecret;
		SignatureMethod SignMethod;
		ParamTransmission ParamTrans;
	};

	string Encode(const std::string &sData);
	string Decode(const std::string &sData);

	bool HttpRequest(string &response, HttpRequestMethod method, const string &url,
			const map<string, string> &params, const OAuthParam &oauthParam);

	bool CheckSignature(string signature, HttpRequestMethod method, const string &url,
			const map<string, string> &params, const OAuthParam &oauthParam);

	bool GetSignature(string &signature, HttpRequestMethod method, const string &url,
			const vector< pair<string, string> > &params, const OAuthParam &oauthParam);

	bool GetAuthHeader(map<string, string> &oauthHeader);

	bool SaveToken(const string &token, const string &tokenSecret);
	bool GetTokenSecret(const string &token, string &tokenSecret);

}

#endif /* OAUTHUTIL_H_ */
