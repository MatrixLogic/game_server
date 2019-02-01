#ifndef _DATA_EMAIL_H_
#define _DATA_EMAIL_H_
#define EMAI_MAX_EXISIT_TIME (3600 * 24 * 30)
#include "Kernel.h"

struct DataEmail
{
	uint64_t email_id;
	unsigned uid;
	unsigned opposite_uid;
	string   title;
	string   text;
	string   from_name;
	string   to_name;
	unsigned post_ts;
	unsigned read_ts;
	string   attachments;
	int      attach_flag;     // 0 无附件 1附件未读 2 已读
	int      post_flag;       // 1 收件 2发件
};

class CDataEmail : public CDataBaseDBC
{
public:
	CDataEmail(int table = DB_EMAIL) : CDataBaseDBC(table) {}
	int GetInboxEmailList(unsigned int uid,vector<DataEmail> &vdata);
	int GetOutboxEmailList(unsigned int uid, vector<DataEmail> &vdata);
	int AddEmail(const DataEmail &data);
	int DeleteEmails(unsigned uid, const vector<uint64_t> &vEmail_Id);
	int GetAttachments(unsigned uid, const vector<uint64_t> &vEmail_Id,vector<string> &vattchments);
	int SetAttach_Flag(unsigned uid, const vector<uint64_t> &vEmail_Id);
	int ReadEmail(unsigned uid, const uint64_t email_Id,DataEmail &data);
	int SetReadTime(unsigned uid,const uint64_t email_Id);
};

#endif
