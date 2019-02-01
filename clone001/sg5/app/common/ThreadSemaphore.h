#ifndef __THREADSEMAPHORE_H__
#define __THREADSEMAPHORE_H__

#include "ISemaphore.h"
#include <semaphore.h>

class CThreadSemaphore : public ISemaphore
{
public:

	CThreadSemaphore();
	~CThreadSemaphore();

	bool Signal();
	bool Wait();

private:
	sem_t m_sem;
};

#endif //__THREADSEMAPHORE_H__
