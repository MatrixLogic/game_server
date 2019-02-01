/*
 * DataPromotionMapping.h
 *
 *  Created on: 2012-12-5
 *      Author: Administrator
 */

#ifndef DATAPROMOTIONMAPPING_H_
#define DATAPROMOTIONMAPPING_H_
#include "Kernel.h"
struct DataPromotionMapping{
	unsigned uid;
	unsigned pid;
	unsigned usetime;
	int useflag;
};
class CDataPromotionMapping:public CDataBaseDBC {
public:
	CDataPromotionMapping(int tableId = DB_PROMOTION_MAPPINT):CDataBaseDBC(tableId){}
	virtual ~CDataPromotionMapping();
	int AddPromotionMapping( const DataPromotionMapping &data );
	int SetPromotionMapping( const DataPromotionMapping &data );
	int GetPromotionMapping( DataPromotionMapping &data );
};

#endif /* DATAPROMOTIONMAPPING_H_ */
