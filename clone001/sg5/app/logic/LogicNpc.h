#ifndef LOGICNPC_H_
#define LOGICNPC_H_

#include "LogicInc.h"

class CLogicNpc
{
public:
	int Load(unsigned npcId, unsigned uidBy, Json::Value &result);
	int Save(unsigned npcId, DataUser &userBy, Json::Value &data, Json::Value &result);

	//将一个用户的数据导出成json格式的数据，并存储到指定的文件中
	int ExportNpcData(unsigned uid, unsigned npcId, const string &dir);

protected:
	static int GetNpcArchive(unsigned npcId, Json::Value &data);
};

#endif /* LOGICNPC_H_ */
