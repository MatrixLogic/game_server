#ifndef __MUTEX_H__
#define __MUTEX_H__

#include "Common.h"
#include <pthread.h>

class CMutex : public ILock
{
public:
	CMutex();
	~CMutex();

	bool Lock(int vnum = 1);
	bool Unlock(int vnum = 1);

private:
	pthread_mutex_t m_mutex;
};

#endif //__MUTEX_H__
