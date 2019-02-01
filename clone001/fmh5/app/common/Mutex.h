#ifndef __MUTEX_H__
#define __MUTEX_H__

#include "Common.h"
#include <pthread.h>

class CMutex : public ILock
{
public:
	CMutex();
	~CMutex();

	bool Lock();
	bool Unlock();

private:
	pthread_mutex_t m_mutex;
};

#endif //__MUTEX_H__
