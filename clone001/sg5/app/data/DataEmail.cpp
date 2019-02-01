#include "DataEmail.h"


// add by aaron 20120726
int CDataEmail::GetInboxEmailList(unsigned int uid,vector<DataEmail> &vEmails)
{
	unsigned now = Time::GetGlobalTime() ;
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, post_flag,1);
	DBCREQ_SET_CONDITION(GE, post_ts,now - EMAI_MAX_EXISIT_TIME);
	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(email_id);
	DBCREQ_NEED(title);
	DBCREQ_NEED(text);
	DBCREQ_NEED(uid);
	DBCREQ_NEED(from_name);
	DBCREQ_NEED(opposite_uid);
	DBCREQ_NEED(to_name);
	DBCREQ_NEED(attachments);
	DBCREQ_NEED(post_ts);
	DBCREQ_NEED(read_ts);
	DBCREQ_NEED(attach_flag);
	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;

	DBCREQ_ARRAY_GET_BEGIN(vEmails);
	DBCREQ_ARRAY_GET_INT(vEmails, email_id);
	DBCREQ_ARRAY_GET_STR(vEmails, title);
	DBCREQ_ARRAY_GET_STR(vEmails, text);
	DBCREQ_ARRAY_GET_INT(vEmails, uid);
	DBCREQ_ARRAY_GET_STR(vEmails, from_name);
	DBCREQ_ARRAY_GET_INT(vEmails, opposite_uid);
	DBCREQ_ARRAY_GET_STR(vEmails, to_name);
	DBCREQ_ARRAY_GET_STR(vEmails, attachments);
	DBCREQ_ARRAY_GET_INT(vEmails, post_ts);
	DBCREQ_ARRAY_GET_INT(vEmails, read_ts);
	DBCREQ_ARRAY_GET_INT(vEmails, attach_flag);
	DBCREQ_ARRAY_GET_END();
	return 0;

}


int CDataEmail::GetOutboxEmailList(unsigned int uid, vector<DataEmail> &vEmails)
{
	unsigned now = Time::GetGlobalTime();
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, post_flag,2);
	DBCREQ_SET_CONDITION(GE, post_ts,now - EMAI_MAX_EXISIT_TIME);
	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(email_id);
	DBCREQ_NEED(title);
	DBCREQ_NEED(text);
	DBCREQ_NEED(uid);
	DBCREQ_NEED(from_name);
	DBCREQ_NEED(opposite_uid);
	DBCREQ_NEED(to_name);
	DBCREQ_NEED(attachments);
	DBCREQ_NEED(post_ts);
	DBCREQ_NEED(read_ts);
	DBCREQ_NEED(attach_flag);
	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;

	DBCREQ_ARRAY_GET_BEGIN(vEmails);
	DBCREQ_ARRAY_GET_INT(vEmails, email_id);
	DBCREQ_ARRAY_GET_STR(vEmails, title);
	DBCREQ_ARRAY_GET_STR(vEmails, text);
	DBCREQ_ARRAY_GET_INT(vEmails, uid);
	DBCREQ_ARRAY_GET_STR(vEmails, from_name);
	DBCREQ_ARRAY_GET_INT(vEmails, opposite_uid);
	DBCREQ_ARRAY_GET_STR(vEmails, to_name);
	DBCREQ_ARRAY_GET_STR(vEmails, attachments);
	DBCREQ_ARRAY_GET_INT(vEmails, post_ts);
	DBCREQ_ARRAY_GET_INT(vEmails, read_ts);
	DBCREQ_ARRAY_GET_INT(vEmails, attach_flag);
	DBCREQ_ARRAY_GET_END();
	return 0;
}
// add by aaron 20120726
int CDataEmail::AddEmail( const DataEmail &data)
{
	DBCREQ_DECLARE(DBC::InsertRequest, data.uid);
	DBCREQ_SET_KEY(data.uid);
	DBCREQ_SET_STR(data, title);
	DBCREQ_SET_STR(data, text);
	DBCREQ_SET_STR(data, from_name);
	DBCREQ_SET_INT(data, opposite_uid);
	DBCREQ_SET_STR(data, to_name);
	DBCREQ_SET_INT(data, post_ts);
	DBCREQ_SET_INT(data, read_ts);
	DBCREQ_SET_STR(data, attachments);
	DBCREQ_SET_INT(data, attach_flag);
	DBCREQ_SET_INT(data, post_flag);
	DBCREQ_EXEC;
	return 0;
}
int CDataEmail::DeleteEmails(unsigned uid, const vector<uint64_t> &vEmail_Id)
{

	vector<uint64_t>::const_iterator begin_it = vEmail_Id.begin();
	vector<uint64_t>::const_iterator end_it = vEmail_Id.end();
	for(vector<uint64_t>::const_iterator it = begin_it; it != end_it; it++)
	{
		DBCREQ_DECLARE(DBC::DeleteRequest, *it);
		DBCREQ_SET_KEY(uid);
		DBCREQ_SET_CONDITION(EQ,email_id, *it);
		DBCREQ_EXEC;
	}
	return 0;
}
int CDataEmail::GetAttachments(unsigned uid, const vector<uint64_t> &vEmail_Id,vector<string> &vattchments)
{
	vector<uint64_t>::const_iterator begin_it = vEmail_Id.begin();
	vector<uint64_t>::const_iterator end_it = vEmail_Id.end();
	//DataEmail dataemail;
	string attachments;
	//vattchments.reserve(vEmail_Id.size());
	//vattchments.clear();
	for(vector<uint64_t>::const_iterator it = begin_it; it != end_it; it++)
	{
		attachments.clear();
		DBCREQ_DECLARE(DBC::GetRequest, *it);
		DBCREQ_SET_KEY(uid);
		DBCREQ_SET_CONDITION(EQ,email_id,*it);
		DBCREQ_SET_CONDITION(EQ,attach_flag,1);
		DBCREQ_NEED_BEGIN();
		DBCREQ_NEED(attachments);
		DBCREQ_EXEC;
		DBCREQ_IFNULLROW;
		DBCREQ_IFFETCHROW;
		DBCREQ_GET_BEGIN();
		DBCREQ_GET_STR_S(attachments);
		vattchments.push_back(attachments);



	}

	return 0;
}
int CDataEmail::SetAttach_Flag(unsigned uid, const vector<uint64_t> &vEmail_Id)
{
	vector<uint64_t>::const_iterator begin_it = vEmail_Id.begin();
	vector<uint64_t>::const_iterator end_it = vEmail_Id.end();

	int attach_flag=2;
	for(vector<uint64_t>::const_iterator it = begin_it; it != end_it; it++)
	{
		DBCREQ_DECLARE(DBC::UpdateRequest, *it);
		DBCREQ_SET_KEY(uid);
		DBCREQ_SET_CONDITION(EQ,email_id,*it);
		DBCREQ_SET_INT_S(attach_flag);
		DBCREQ_EXEC;

	}
	return 0;
}
int CDataEmail::ReadEmail(unsigned uid, const uint64_t email_Id,DataEmail &data)
{
	DBCREQ_DECLARE(DBC::GetRequest, email_Id);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, email_id,email_Id);
	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(email_id);
	DBCREQ_NEED(title);
	DBCREQ_NEED(text);
	DBCREQ_NEED(uid);
	DBCREQ_NEED(from_name);
	DBCREQ_NEED(opposite_uid);
	DBCREQ_NEED(to_name);
	DBCREQ_NEED(attachments);
	DBCREQ_NEED(attach_flag);
	DBCREQ_NEED(post_ts);
	DBCREQ_NEED(read_ts);
	DBCREQ_NEED(post_flag);

	DBCREQ_EXEC;

	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(data, email_id);
	DBCREQ_GET_STR(data, title);
	DBCREQ_GET_STR(data, text);
	DBCREQ_GET_INT(data, uid);
	DBCREQ_GET_STR(data, from_name);
	DBCREQ_GET_INT(data, opposite_uid);
	DBCREQ_GET_STR(data, to_name);
	DBCREQ_GET_STR(data, attachments);
	DBCREQ_GET_INT(data, attach_flag);
	DBCREQ_GET_INT(data, post_ts);
	DBCREQ_GET_INT(data, read_ts);
	DBCREQ_GET_INT(data, post_flag);

	return 0;
}

int CDataEmail::SetReadTime(unsigned uid,const uint64_t email_Id)
{
	unsigned int read_ts = Time::GetGlobalTime();;
	DBCREQ_DECLARE(DBC::UpdateRequest, email_Id);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ,email_id,email_Id);
	DBCREQ_SET_INT_S(read_ts);
	DBCREQ_EXEC;
	return 0;
}
