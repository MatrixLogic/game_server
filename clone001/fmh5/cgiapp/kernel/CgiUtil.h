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

enum ProtoType
{
	e_ProtoType_int 	= 0,
	e_ProtoType_uint 	= 1,
	e_ProtoType_long 	= 2,
	e_ProtoType_ulong 	= 3,
	e_ProtoType_float 	= 4,
	e_ProtoType_double 	= 5,
	e_ProtoType_bool 	= 6,
	e_ProtoType_string 	= 7,
	e_ProtoType_msg 	= 8,

	e_ProtoType_max
};
class ProtoElement
{
public:
	ProtoType type;
	bool isRepeat;
	string name, tname, fname;
	map<string, string> *StructNames;

	ProtoElement(bool i, const string &n, const string& f, map<string, string> *s)
		:isRepeat(i)
		,name(n)
		,fname(f)
		,StructNames(s)
	{
		type = e_ProtoType_max;
	}
	string PrintMember();
	string PrintConstruct();
	string PrintSerialize();
	string PrintParse();
};

class ProtoBase
{
public:
	string SpaceName, MsgName;
	Message* m_msg;

	ProtoBase(const string& typeName);
	ProtoBase() {m_msg = NULL;}
	~ProtoBase();
	int Parse(const string& path);
	int Serialize(const string& path);
	string DebugString();
};

class Proto2CPP : public ProtoBase
{
public:
	map<string, string> StructNames;
	static const char* TYPE_NAME[e_ProtoType_max];
	string m_cpp;

	string _proto(const Descriptor *des);
	Proto2CPP(const string& typeName);
	Proto2CPP(){};
};

class Proto2Json : public ProtoBase
{
public:
	Json::Value m_json;
	Proto2Json(const string& typeName);
	Proto2Json() {}
	int P2J();
	void _p2j(const Message* msg, Json::Value &json);
	string SerializeJson();
};
class Json2Proto : public ProtoBase
{
public:
	Json::Value m_json;
	Json2Proto(const string& typeName);
	Json2Proto() {}
	int ParseJson(const string& jpath);
	int J2P();
	int _j2p(Message* msg, Json::Value &json);
};
/**********************************************************************/

namespace CgiUtil
{
	void GetRequestMethod(string &method);
	void GetContentType(string &contentType);
	void GetRequestParams(map<string, string> &params);
	void GetPostData(string &data);
	bool PrintText(const string &text);
	bool PrintText(const string &text,const vector<string>& headers);
	bool PrintFormat(const char *format, ...);
	bool PrintJson(const Json::Value &json);
	bool CompressPrint(const string &text);
	bool HttpRedirect(const string &url);
	bool ScriptRedirect(const string &url);
	bool ScriptTopRedirect(const string &url);
}

#endif /* CGIUTIL_H_ */
