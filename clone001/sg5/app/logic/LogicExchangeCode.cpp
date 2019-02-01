/*
 * LogicExchangeCode.cpp
 *
 *  Created on: 2012-12-5
 *      Author: Administrator
 */

#include "LogicExchangeCode.h"

CLogicExchangeCode::CLogicExchangeCode() {
	// TODO Auto-generated constructor stub

}

CLogicExchangeCode::~CLogicExchangeCode() {
	// TODO Auto-generated destructor stub
}

int CLogicExchangeCode::GetExchangeCode(DataExchangeCode &data)
{
	CDataExchangeCode dataExchangeCode;
	int ret = dataExchangeCode.GetExchageCode(data);
	if(ret != 0)
	{
		error_log("Get exchangecode failed,code = %s",data.code.c_str());
		return ret;
	}
	return 0;
}
int CLogicExchangeCode::AddExchangeCode(const DataExchangeCode &data)
{
	CDataExchangeCode dataExchangeCode;
	int ret = dataExchangeCode.AddExchageCode(data);
	if(ret != 0)
	{
		error_log("Add exchangecode failed,code = %s",data.code.c_str());
		return ret;
	}
	return 0;
}
int CLogicExchangeCode::SetExchangeCode(const DataExchangeCode &data)
{
	CDataExchangeCode dataExchangeCode;
	int ret = dataExchangeCode.SetExchageCode(data);
	if(ret != 0)
	{
		error_log("Set exchangecode failed,code = %s",data.code.c_str());
		return ret;
	}
	return 0;
}
