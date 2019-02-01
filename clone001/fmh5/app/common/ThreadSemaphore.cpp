#include "Common.h"
#include <semaphore.h>
#include <fcntl.h>

CThreadSemaphore::CThreadSemaphore()
{
	if(sem_init(&m_sem, 0, 0) != 0)
	{
		LogError("sem_init Fail [%d]", errno);
	}
}

CThreadSemaphore::~CThreadSemaphore()
{
}

bool CThreadSemaphore::Wait()
{
	if(sem_wait(&m_sem) != 0)
	{
		LogError("sem_wait Fail [%d]", errno);
		return false;
	}
	return true;
}

bool CThreadSemaphore::Signal()
{
	if(sem_post(&m_sem) != 0)
	{
		LogError("sem_post Fail [%d]", errno);
		return false;
	}
	return true;
}
