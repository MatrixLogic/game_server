//! @file thread_mutex.h
//! @brief 线程锁


#ifndef _THREAD_MUTEX_H_
#define _THREAD_MUTEX_H_

#include <pthread.h>

//! @class Thread_Mutex
//! @brief 线程锁
class ThreadMutex
{
public:
	ThreadMutex();
	~ThreadMutex();

	//! 加锁
	//! @return 0:成功, <0:失败
	int acquire();

	//! 解锁
	//! @return 0:成功, <0:失败
	int release();

private:
	//! 线程锁
	pthread_mutex_t m_thread_mutex;

};


//! @class Thread_Mutex_Guard
//! @brief 线程锁工具类
//!
//! 此类在构造函数加锁, 析构函数解锁
class ThreadMutexGuard
{
public:
	//! 构造函数
	//! @param mutex 用到的线程锁
	ThreadMutexGuard(ThreadMutex* mutex);

	//! 析构函数
	~ThreadMutexGuard();

private:
	ThreadMutex* m_mutex;
};


#endif // _THREAD_MUTEX_H_
