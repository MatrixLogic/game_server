/*
 * CgiUtil.h
 *
 *  Created on: 2011-5-27
 *      Author: dada
 */

#ifndef CGIUTIL_H_
#define CGIUTIL_H_

#include "Kernel.h"
#include "fcgi_stdio.h"
#include "cookie.h"
#include "cgiin.h"
#include "staticdata.h"

namespace CgiUtil
{
	void GetRequestMethod(string &method);
	void GetContentType(string &contentType);
	void GetRequestParams(map<string, string> &params);
	void GetPostData(string &data);
	bool PrintText(const string &text);
	bool PrintFormat(const char *format, ...);
	bool PrintJson(const Json::Value &json);
	bool CompressPrint(const string &text);
	bool HttpRedirect(const string &url);
	bool ScriptRedirect(const string &url);
	bool ScriptTopRedirect(const string &url);
}

#endif /* CGIUTIL_H_ */
