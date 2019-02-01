#ifndef LOGIC_GAME_ACTIVITY_H
#define LOGIC_GAME_ACTIVITY_H

#include "ServerInc.h"

class LogicGameActivityManager : public CSingleton<LogicGameActivityManager>
{
private:
	friend class CSingleton<LogicGameActivityManager>;
	LogicGameActivityManager() {};

public:
	void FullMessage(unsigned uid, ProtoActivity::GameAcitivityAllCPP * msg);
};

#endif  //LOGIC_GAME_ACTIVITY_H
