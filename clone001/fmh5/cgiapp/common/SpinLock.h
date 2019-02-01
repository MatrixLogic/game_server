/*
 * SpinLock.h
 *
 *  Created on: 2012-2-22
 *      Author: dada
 */

#ifndef SPINLOCK_H_
#define SPINLOCK_H_

#include "Common.h"
#include <pthread.h>

class CSpinLock : public ILock
{
public:
	CSpinLock();
	~CSpinLock();

	bool Lock(int vnum = 1);
	bool Unlock(int vnum = 1);

private:
	pthread_spinlock_t m_spinlock;
};

#endif /* SPINLOCK_H_ */
