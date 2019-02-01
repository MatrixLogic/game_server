/*
 * Event.cpp
 *
 *  Created on: 2012-2-23
 *      Author: dada
 */

#include "Event.h"

//#define EVENT_BROADCAST

CEvent::CEvent()
{
	if(pthread_mutex_init(&m_mutex, NULL) != 0)
	{
		LogError("pthread_mutex_init Fail [%d]", errno);
	}
	if(pthread_cond_init(&m_cond, NULL) != 0)
	{
		LogError("pthread_cond_init Fail [%d]", errno);
	}
	m_wait = 0;
}

CEvent::~CEvent()
{
	if(pthread_cond_destroy(&m_cond) != 0)
	{
		LogError("pthread_cond_destroy Fail [%d]", errno);
	}
	if(pthread_mutex_destroy(&m_mutex) != 0)
	{
		LogError("pthread_mutex_destroy Fail [%d]", errno);
	}
}

bool CEvent::Wait()
{
#ifdef EVENT_BROADCAST
	m_wait = 1;
#endif
	if(pthread_mutex_lock(&m_mutex) != 0)
	{
		LogError("pthread_mutex_lock Fail [%d]", errno);
		return false;
	}
#ifndef EVENT_BROADCAST
	m_wait++;
#endif
	if(pthread_cond_wait(&m_cond, &m_mutex) != 0)
	{
		LogError("pthread_cond_wait Fail [%d]", errno);
		return false;
	}
#ifndef EVENT_BROADCAST
	m_wait--;
#endif
	if(pthread_mutex_unlock(&m_mutex) != 0)
	{
		LogError("pthread_mutex_unlock Fail [%d]", errno);
		return false;
	}
	return true;
}

bool CEvent::Trigger()
{
#ifdef EVENT_BROADCAST
	if(!Atomic::CompareAndSwap(&m_wait, 1, 0))
#else
	if(m_wait <= 0)
#endif
	{
		return true;
	}
	if(pthread_mutex_lock(&m_mutex) != 0)
	{
		LogError("pthread_mutex_lock Fail [%d]", errno);
		return false;
	}
#ifdef EVENT_BROADCAST
	if(pthread_cond_broadcast(&m_cond) != 0)
#else
	if(pthread_cond_signal(&m_cond) != 0)
#endif
	{
		LogError("pthread_cond_signal Fail [%d]", errno);
		return false;
	}
	if(pthread_mutex_unlock(&m_mutex) != 0)
	{
		LogError("pthread_mutex_unlock Fail [%d]", errno);
		return false;
	}
	return true;
}

