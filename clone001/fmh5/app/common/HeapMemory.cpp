/*
 * HeapMemory.cpp
 *
 *  Created on: 2012-2-3
 *      Author: dada
 */

#include "HeapMemory.h"

CHeapMemory::CHeapMemory() :
	m_pAddress(NULL),
	m_size(0)
{
}

CHeapMemory::CHeapMemory(int size)
{
	if(size <= 0)
	{
		m_pAddress = NULL;
		m_size = 0;
		return ;
	}
	m_pAddress = new char[size];
	if(m_pAddress == NULL)
	{
		m_size = 0;
	}
	else
	{
		memset(m_pAddress, 0, size);
		m_size = size;
	}
}

CHeapMemory::~CHeapMemory()
{
	if(m_pAddress != NULL)
	{
		delete (char *)m_pAddress;
	}
}

bool CHeapMemory::Create(int size)
{
	if(m_pAddress != NULL || size <= 0)
	{
		return false;
	}
	m_pAddress = new char[size];
	if(m_pAddress == NULL)
	{
		m_size = 0;
		return false;
	}
	memset(m_pAddress, 0, size);
	m_size = size;
	return true;
}

bool CHeapMemory::Close()
{
	if(m_pAddress != NULL)
	{
		delete (char *)m_pAddress;
		m_pAddress = NULL;
		m_size = 0;
	}
	return true;
}

