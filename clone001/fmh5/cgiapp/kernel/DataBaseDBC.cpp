/*
 * DataBaseDBC.cpp
 *
 *  Created on: 2011-1-4
 *      Author: LARRY
 */

#include "DataBaseDBC.h"

static char g_sDbcConfigPath[250];
static bool g_bDbcInit = false;

CDataBaseDBC::CDataBaseDBC( const int table ) : m_iTableId( table )
{
}

CDataBaseDBC::~CDataBaseDBC()
{
}

int CDataBaseDBC::Init()
{
	return 0;
}

int CDataBaseDBC::SetTableId( int tableid )
{
	m_iTableId = tableid;
	return 0 ;
}

DBC::Server* CDataBaseDBC::LoadHandle( unsigned long long uId )
{
	if(!g_bDbcInit)
	{
		strcpy(g_sDbcConfigPath, MainConfig::GetAllServerPath(CONFIG_DBC_SERVER).c_str());
		g_bDbcInit = true;
	}
	m_dbcsrv = NULL;
	int iRet=CDBCRouteTable::GetInstance(g_sDbcConfigPath)->get_route( m_iTableId, uId, &m_dbcsrv, false, CDBCKeepAlive::_ka.GetMaxHandle(), NULL );
	if(iRet!=0)
	{
		error_log( "dbc get_route failed:%s", CDBCRouteTable::GetInstance(g_sDbcConfigPath)->get_msg().c_str());
	}
	return m_dbcsrv;
}

DBC::Server* CDataBaseDBC::GetHandle( unsigned long long uId )
{
	return m_dbcsrv;
}

void CDataBaseDBC::PrintfError(  )
{
	printf( "<ResultCode>%d</ResultCode>\n"
		"<ErrorMessage>%s</ErrorMessage>\n"
		"<ErrorFrom>%s</ErrorFrom>\n",
		m_dbcret.ResultCode(),
		m_dbcret.ErrorMessage(),
		m_dbcret.ErrorFrom() );
}
