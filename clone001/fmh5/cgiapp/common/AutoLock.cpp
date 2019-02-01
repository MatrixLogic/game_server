#include <stdio.h>
#include "AutoLock.h"

CAutoLock::CAutoLock(ILock *pLock, bool bLock, unsigned vnum, bool turn)
{
	m_turn = turn;
	m_bLock = bLock;
	m_num = vnum;
	m_pLock = pLock;

	if(m_pLock && m_bLock)
	{
		if(m_turn)
		{
			for(unsigned i=0;i<m_num;++i)
				m_pLock->Lock();
		}
		else
			m_pLock->Lock(m_num);
	}
}

CAutoLock::~CAutoLock()
{
	Unlock();
}

void CAutoLock::Lock()
{
	if(m_pLock && !m_bLock)
	{
		if(m_turn)
		{
			for(unsigned i=0;i<m_num;++i)
				m_pLock->Lock();
		}
		else
			m_pLock->Lock(m_num);
		m_bLock = true;
	}
}
void CAutoLock::Unlock()
{
	if(m_pLock && m_bLock)
	{
		m_pLock->Unlock(m_num);
		m_bLock = false;
	}
}
