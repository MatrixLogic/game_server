#ifndef __LOGIC_HERO_H__
#define __LOGIC_HERO_H__

#include "LogicInc.h"

class CLogicHero
{
public:
	int InitializeHero(unsigned uid);

	int GetHero(unsigned uid, Json::Value &data);
	int GetHeroUsed(unsigned uid, Json::Value &data);
	int GetHeroAccredited(unsigned uid,unsigned world_pos,Json::Value &data);
	int UpdateHero(unsigned uid, const Json::Value &data);
	int ImportHero(unsigned uid, const Json::Value &data);
};

#endif
