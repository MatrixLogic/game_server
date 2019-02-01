/*
 * DataBaseDBC.h
 *
 *  Created on: 2011-1-4
 *      Author: LARRY
 */

#ifndef DATABASEDBC_H_
#define DATABASEDBC_H_

#include "Kernel.h"
#include "dbcapi.h"
#include "dbc_route_table.h"
#include "dbc_keepalive.h"

class CDataBaseDBC
{
public:
	CDataBaseDBC( const int table );
	virtual ~CDataBaseDBC();

public:
	virtual int Init();
	virtual int SetTableId(int table_id);
	virtual DBC::Server* LoadHandle( unsigned uId );
	virtual DBC::Server* GetHandle( unsigned uId );
	virtual void PrintfError();

public:
	DBC::Result m_dbcret;
	std::string m_table;
	DBC::Server* m_dbcsrv;
	int m_iTableId;
};

#define DBCREQ_DECLARE( type, id )		if ( NULL == LoadHandle( id ) ) { return -100; }; type req( GetHandle( id ) );
#define DBCREQ_EXEC						if ( 0 != req.Execute( m_dbcret ) ){  error_log( "[dbc exec failed][code=%d, msg=%s, from=%s]", m_dbcret.ResultCode(), m_dbcret.ErrorMessage(), m_dbcret.ErrorFrom() ); return -1; }
#define DBCREQ_IFNULLROW				if ( 0 == m_dbcret.TotalRows() ) { return R_ERR_NO_DATA; }
#define DBCREQ_IFFETCHROW				if ( 0 != m_dbcret.FetchRow() ) { return -1; }

#define DBCREQ_SET_KEY(key)				req.SetKey(key)
#define DBCREQ_SET_CONDITION(cond, field, data)	req.cond(#field, data)

#define DBCREQ_SET_INT(data, field)		req.Set(#field, data.field)
#define DBCREQ_SET_STR(data, field)		req.Set(#field, data.field.c_str())
#define DBCREQ_SET_INT_V(field)			req.Set(#field, field);DBCREQ_EXEC
#define DBCREQ_SET_STR_V(field)			req.Set(#field, field.c_str());DBCREQ_EXEC
#define DBCREQ_ADD_INT_V(field)			req.Add(#field, field);DBCREQ_EXEC
#define DBCREQ_SET_INT_S(field)			req.Set(#field, field)
#define DBCREQ_SET_STR_S(field)			req.Set(#field, field.c_str())
#define DBCREQ_GET_INSERT_KEY(data)		data = m_dbcret.InsertID()

#define DBCREQ_NEED_BEGIN()				int reqItemIndex = 0
#define DBCREQ_NEED(field)				req.Need(#field, ++reqItemIndex)
#define DBCREQ_GET_BEGIN()				reqItemIndex = 0
#define DBCREQ_GET_INT(data, field)		data.field = m_dbcret.IntValue(++reqItemIndex)
#define DBCREQ_GET_STR(data, field)		data.field = m_dbcret.StringValue(++reqItemIndex)
#define DBCREQ_GET_INT_S(field)			field = m_dbcret.IntValue(++reqItemIndex)
#define DBCREQ_GET_STR_S(field)			field = m_dbcret.StringValue(++reqItemIndex)

#define DBCREQ_GET_INT_V(field)	\
		req.Need(#field, 1);	\
		DBCREQ_EXEC;	\
		DBCREQ_IFNULLROW;	\
		DBCREQ_IFFETCHROW;	\
		field = m_dbcret.IntValue(1)	\

#define DBCREQ_GET_STR_V(field)	\
		req.Need(#field, 1);	\
		DBCREQ_EXEC;	\
		DBCREQ_IFNULLROW;	\
		DBCREQ_IFFETCHROW;	\
		field = m_dbcret.StringValue(1)	\

#define DBCREQ_ARRAY_GET_BEGIN(array)	\
	array.resize(m_dbcret.TotalRows());	\
	for(size_t i = 0; i < array.size(); i++)	\
	{	\
		DBCREQ_IFFETCHROW;	\
		reqItemIndex = 0	\

#define DBCREQ_ARRAY_GET_INT(array, field)		array[i].field = m_dbcret.IntValue(++reqItemIndex)
#define DBCREQ_ARRAY_GET_STR(array, field)		array[i].field = m_dbcret.StringValue(++reqItemIndex)
#define DBCREQ_ARRAY_SET(array, field, value)	array[i].field = value
#define DBCREQ_ARRAY_GET_END()					}
#define DBCREQ_ARRAY_GET_INT_S(array, field)	array[i] = m_dbcret.IntValue(++reqItemIndex)
#define DBCREQ_ARRAY_GET_STR_S(array, field)	array[i] = m_dbcret.StringValue(++reqItemIndex)

#define DECLARE_DBC_DATA_CLASS(ClassName, TableId)	\
class ClassName: public CDataBaseDBC \
{	\
public:	\
	ClassName(int table = TableId) : CDataBaseDBC(table) {}	\
	virtual ~ClassName(){}	\

#define DECLARE_DBC_DATA_CLASS_END	\
};	\


#endif /* DATABASEDBC_H_ */
