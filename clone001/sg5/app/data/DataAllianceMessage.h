/*
 * DataAllianceMessage.h
 *
 *  Created on: 2013-4-18
 *      Author: Administrator
 */

#ifndef DATAALLIANCEMESSAGE_H_
#define DATAALLIANCEMESSAGE_H_

#include "Kernel.h"
#include "DataUserMessage.h"

class CDataAllianceMessage : public CDataBaseDBC
{
public:
	CDataAllianceMessage(int table = DB_ALLIANCE_MSG) : CDataBaseDBC(table) {}
public:
	int AddAllianceMessage(const DataUserMessage &data);
	int SetAllianceMessage(const DataUserMessage &data);
	int SetAllianceMessageStatus(unsigned uid, uint64_t message_id, int status);
	int GetAllianceMessage(unsigned uid, uint64_t message_id, DataUserMessage &data);
	int GetAllianceMessage(unsigned uid, int type, int status,unsigned startTime, vector<DataUserMessage> &datas);
	int GetAllianceMessageForTime(unsigned uid, int type, unsigned startTime, vector<DataUserMessage> &datas);
	int GetAllianceMessageForTime(unsigned uid, int type, int toward, unsigned startTime, vector<DataUserMessage> &datas);
};

#endif /* DATAALLIANCEMESSAGE_H_ */
