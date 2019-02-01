#include <stdio.h>
#include "AutoLock.h"

CAutoLock::CAutoLock(ILock *pLock, bool bLock)
{
	m_pLock = pLock;
	if(pLock != NULL && bLock)
	{
		pLock->Lock();
	}
}

CAutoLock::~CAutoLock()
{
	if(m_pLock != NULL)
	{
		m_pLock->Unlock();
	}
}
