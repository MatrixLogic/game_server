/*
 * LogicExchangeCode.h
 *
 *  Created on: 2012-12-5
 *      Author: Administrator
 */

#ifndef LOGICEXCHANGECODE_H_
#define LOGICEXCHANGECODE_H_
#include "LogicInc.h"
class CLogicExchangeCode {
public:
	CLogicExchangeCode();
	virtual ~CLogicExchangeCode();
	int GetExchangeCode(DataExchangeCode &data);
	int AddExchangeCode(const DataExchangeCode &data);
	int SetExchangeCode(const DataExchangeCode &data);
};
#endif /* LOGICEXCHANGECODE_H_ */
