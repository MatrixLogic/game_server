/*
 * Event.h
 *
 *  Created on: 2012-2-23
 *      Author: dada
 */

#ifndef EVENT_H_
#define EVENT_H_

#include "Common.h"
#include <pthread.h>

class CEvent
{
public:
	CEvent();
	~CEvent();
	bool Wait();
	bool Trigger();
private:
	pthread_mutex_t m_mutex;
	pthread_cond_t m_cond;
	int m_wait;
};

#endif /* EVENT_H_ */
