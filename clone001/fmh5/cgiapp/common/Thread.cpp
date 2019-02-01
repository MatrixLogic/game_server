/*
 * Thread.cpp
 *
 *  Created on: 2012-2-1
 *      Author: dada
 */

#include "Thread.h"
#include "Common.h"

static void *ThreadRoutine(void *pData)
{
	if(pData != NULL)
	{
		((CThread *)pData)->Run();
	}
	return NULL;
}

bool CThread::Start()
{
	if(m_status != TS_INIT)
	{
		return false;
	}
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	int ret = pthread_create(&m_thread, &attr, ThreadRoutine, this);
	if(ret != 0)
	{
		LogError("pthread_create Fail,error=%d", ret);
		return false;
	}
	m_status = TS_RUNNING;
	return true;
}

bool CThread::Stop()
{
	if(m_status == TS_RUNNING)
	{
		m_status = TS_STOPPING;
		return Wait();
	}
	return true;
}

bool CThread::Wait()
{
	if(m_status != TS_RUNNING && m_status != TS_STOPPING)
	{
		return false;
	}
	int ret = pthread_join(m_thread, NULL);
	if(ret != 0)
	{
		LogError("pthread_join Fail,tid=%llu,error=%d", (uint64_t)m_thread, ret);
		return false;
	}
	m_status = TS_STOPPED;
	return true;
}
