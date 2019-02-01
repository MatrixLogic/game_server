/*
 * LogicPromotionMapping.cpp
 *
 *  Created on: 2012-12-5
 *      Author: Administrator
 */

#include "LogicPromotionMapping.h"

CLogicPromotionMapping::CLogicPromotionMapping() {
	// TODO Auto-generated constructor stub

}

CLogicPromotionMapping::~CLogicPromotionMapping() {
	// TODO Auto-generated destructor stub
}
int CLogicPromotionMapping::GetPromotionMapping(DataPromotionMapping &data)
{
	CDataPromotionMapping dataPromotionMapping;
	int ret = dataPromotionMapping.GetPromotionMapping(data);
	if(ret != 0)
	{
		error_log("get exchangecode failed,uid = %u",data.uid);
		return ret;
	}
	return 0;

}
int CLogicPromotionMapping::AddPromotionMapping(const DataPromotionMapping &data)
{
	CDataPromotionMapping dataPromotionMapping;
	int ret = dataPromotionMapping.AddPromotionMapping(data);
	if(ret != 0)
	{
		error_log("Add exchangecode failed,uid = %u",data.uid);
		return ret;
	}
	return 0;

}
int CLogicPromotionMapping::SetPromotionMapping(const DataPromotionMapping &data)
{
	CDataPromotionMapping dataPromotionMapping;
	int ret = dataPromotionMapping.SetPromotionMapping(data);
	if(ret != 0)
	{
		error_log("Set exchangecode failed,uid = %u",data.uid);
		return ret;
	}
	return 0;
}
