/*
 * SpinLock.cpp
 *
 *  Created on: 2012-2-22
 *      Author: dada
 */

#include "SpinLock.h"

CSpinLock::CSpinLock()
{
	if(pthread_spin_init(&m_spinlock, PTHREAD_PROCESS_PRIVATE) != 0)
	{
		LogError("pthread_spin_init Fail [%d]", errno);
	}
}

CSpinLock::~CSpinLock()
{
	if(pthread_spin_destroy(&m_spinlock) != 0)
	{
		LogError("pthread_spin_destroy Fail [%d]", errno);
	}
}

bool CSpinLock::Lock()
{
	if(pthread_spin_lock(&m_spinlock) != 0)
	{
		LogError("pthread_spin_lock Fail [%d]", errno);
		return false;
	}
	return true;
}

bool CSpinLock::Unlock()
{
	if(pthread_spin_unlock(&m_spinlock) != 0)
	{
		LogError("pthread_spin_unlock Fail [%d]", errno);
		return false;
	}
	return true;
}
