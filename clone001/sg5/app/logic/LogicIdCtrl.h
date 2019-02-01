/*
 * LogicIdCtrl.h
 *
 *  Created on: 2011-7-14
 *      Author: dada
 */

#ifndef LOGICIDCTRL_H_
#define LOGICIDCTRL_H_

#include "LogicInc.h"

class CLogicIdCtrl
{

public:

	int GetNextId(int key, uint64_t &nextId, unsigned db=0);
	int GetCurrentId(int key, uint64_t &currId, unsigned db=0);

};

#endif /* LOGICIDCTRL_H_ */
