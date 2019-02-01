/*
 * LogicPromotionMapping.h
 *
 *  Created on: 2012-12-5
 *      Author: Administrator
 */

#ifndef LOGICPROMOTIONMAPPING_H_
#define LOGICPROMOTIONMAPPING_H_
#include "LogicInc.h"
class CLogicPromotionMapping {
public:
	CLogicPromotionMapping();
	virtual ~CLogicPromotionMapping();
	int GetPromotionMapping(DataPromotionMapping &data);
	int AddPromotionMapping(const DataPromotionMapping &data);
	int SetPromotionMapping(const DataPromotionMapping &data);

};

#endif /* LOGICPROMOTIONMAPPING_H_ */
