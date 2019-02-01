#ifndef __LOGIC_BROADCAST_H__
#define __LOGIC_BROADCAST_H__

#include "LogicInc.h"

class CLogicBroadcast
{
public:
	int AddBdMessage(const string &msg);
	int GetBdMessage(vector<BroadcastMessage> &msgs);

protected:
	CDataBroadcast* GetCDataBroadcast();
};

#endif
