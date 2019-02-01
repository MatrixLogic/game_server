/*
 * LogicEmail.h
 *
 *  Created on: 2012-7-26
 *      Author: Administrator
 */

#ifndef LOGICEMAIL_H_
#define LOGICEMAIL_H_
#include "LogicInc.h"
class CLogicEmail {
public:
	CLogicEmail();
	virtual ~CLogicEmail();
	int GetInboxEmailList(unsigned uid, vector<DataEmail> &vEmail);
	int GetOutboxEmailList(unsigned uid, vector<DataEmail> &vEmail);
	int GetInboxEmailListJson(unsigned uid, vector<DataEmail> &vEmail,Json::Value &value);
	int GetOutboxEmailListJson(unsigned uid, vector<DataEmail> &vEmail,Json::Value &value);
	int AddEmail(DataEmail &data,vector<uint64_t> &vto_uid,Json::Value &attachments,PlatformType platform);
	int AddEmail(DataEmail &data,vector<uint64_t> &vto_uid,PlatformType platform);
	int GetAttachments(unsigned uid,const vector<uint64_t> &vEmail_Id,vector<string> &attchments);
	int SetAttachFlag(unsigned uid,const vector<uint64_t> &vEmail_Id);
	int DeleteEmails(unsigned uid,const vector<uint64_t> &vEmail_Id);
	int ReadEmail(unsigned uid,const uint64_t email_Id,DataEmail &data);
};

#endif /* LOGICEMAIL_H_ */
