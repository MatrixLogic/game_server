/*
 * Thread.h
 *
 *  Created on: 2012-2-1
 *      Author: dada
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <pthread.h>
#include <inttypes.h>

namespace Atomic
{
	//temp = *pValue
	//*pValue += delta
	//return temp
	template<typename Type>
	Type Add(Type *pValue, Type delta)
	{
		return __sync_fetch_and_add(pValue, delta);
	}

	//temp = *pValue
	//*pValue -= delta
	//return temp
	template<typename Type>
	Type Sub(Type *pValue, Type delta)
	{
		return __sync_fetch_and_sub(pValue, delta);
	}

	//if *pValue == oldValue:
	//	*pValue = newValue
	//	return true
	//else:
	//	return false
	template<typename Type>
	bool CompareAndSwap(Type *pValue, Type oldValue, Type newValue)
	{
		return __sync_bool_compare_and_swap(pValue, oldValue, newValue);
	}

	//temp = *pValue
	//*pValue = newValue
	//return temp
	template<typename Type>
	Type TestAndSet(Type *pValue, Type newValue)
	{
		return __sync_lock_test_and_set(pValue, newValue);
	}
}

enum ThreadStatus
{
	TS_INIT = 0,
	TS_BLOCKED = 1,
	TS_RUNNING = 2,
	TS_STOPPING = 3,
	TS_STOPPED = 4
};

class CThread
{

public:

	CThread() : m_thread(-1), m_status(TS_INIT) {}
	virtual ~CThread() {}

	virtual void Run() = 0;

	bool Start();
	bool Stop();
	bool Wait();

	uint64_t GetThreadId() { return m_thread; }
	ThreadStatus GetStatus() { return m_status; }

private:

	pthread_t m_thread;
	ThreadStatus m_status;

};

#endif /* THREAD_H_ */
