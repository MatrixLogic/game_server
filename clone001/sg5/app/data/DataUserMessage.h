#ifndef __DATA_USER_MESSAGE_H__
#define __DATA_USER_MESSAGE_H__

#include "Kernel.h"

struct DataUserMessage
{
	unsigned uid;
	uint64_t message_id;
	unsigned opposite_uid;
	int      type;
	int      toward;	//主动发出或是被动接收
	int      status;
	unsigned time;
};

class CDataUserMessage : public CDataBaseDBC
{
public:
	CDataUserMessage(int table = DB_USER_MESSAGE) : CDataBaseDBC(table) {}

	int AddUserMessage(const DataUserMessage &data);
	int SetUserMessage(const DataUserMessage &data);
	int SetUserMessageStatus(unsigned uid, uint64_t message_id, int status);
	int GetUserMessage(unsigned uid, uint64_t message_id, DataUserMessage &data);
	int GetUserMessage(unsigned uid, int type, int status,
			unsigned startTime, vector<DataUserMessage> &datas);
	int GetUserMessageForTime(unsigned uid, int type, unsigned startTime, vector<DataUserMessage> &datas);
	int GetUserMessageForTime(unsigned uid, int type, int toward, unsigned startTime, vector<DataUserMessage> &datas);
};

#endif
