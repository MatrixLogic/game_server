#include "Mutex.h"

CMutex::CMutex()
{
	if(pthread_mutex_init(&m_mutex, NULL) != 0)
	{
		LogError("pthread_mutex_init Fail [%d]", errno);
	}
}

CMutex::~CMutex()
{
	if(pthread_mutex_destroy(&m_mutex) != 0)
	{
		LogError("pthread_mutex_destroy Fail [%d]", errno);
	}
}

bool CMutex::Lock()
{
	if(pthread_mutex_lock(&m_mutex) != 0)
	{
		LogError("pthread_mutex_lock Fail [%d]", errno);
		return false;
	}
	return true;
}

bool CMutex::Unlock()
{
	if(pthread_mutex_unlock(&m_mutex) != 0)
	{
		LogError("pthread_mutex_unlock Fail [%d]", errno);
		return false;
	}
	return true;
}
