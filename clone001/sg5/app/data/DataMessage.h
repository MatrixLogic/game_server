#ifndef __DATA_MESSAGE_H__
#define __DATA_MESSAGE_H__

#include "Kernel.h"

struct DataMessage
{
	uint64_t message_id;
	unsigned from_uid;
	unsigned to_uid;
	int      from_platform;
	string   from_user_name;
	string   from_photo;
	int      to_platform;
	string   to_user_name;
	string   to_photo;
	unsigned time;
	int      type;
	int      status;
	string   content;
	uint64_t reply_id;
};

class CDataMessage : public CDataBaseDBC
{
public:
	CDataMessage(int table = DB_MESSAGE) : CDataBaseDBC(table) {}

	int AddMessage(const DataMessage &data);
	int GetMessage(uint64_t message_id, DataMessage &data);
	int SetMessage(const DataMessage &message);
	int RemoveMessage(uint64_t message_id);
	int SetMessageStatus(uint64_t message_id, int status);
	int SetMessageContent(uint64_t message_id, const string &content);
};

#endif
