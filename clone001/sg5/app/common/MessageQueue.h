/*
 * MessageQueue.h
 *
 *  Created on: 2012-2-8
 *      Author: dada
 */

#ifndef MESSAGEQUEUE_H_
#define MESSAGEQUEUE_H_

#include "Common.h"

class CMessageQueue : public IMessageQueue
{

public:

	CMessageQueue();
	CMessageQueue(IMemory *pMemory, ILock *pEnqueueLock = NULL, ILock *pDequeueLock = NULL);

	virtual bool Attach(IMemory *pMemory, ILock *pEnqueueLock = NULL, ILock *pDequeueLock = NULL);
	virtual bool Detach();
	virtual bool SetMaxCount(uint32_t count);

	virtual bool Enqueue(const IBuffer *pBuffer);
	virtual bool Dequeue(IBuffer *pBuffer);
	virtual bool Peek(IBuffer *pBuffer) const;

	virtual uint32_t GetMaxCount() const;
	virtual uint32_t GetCount() const;
	virtual bool IsFull() const;
	virtual bool IsEmpty() const;

	static uint32_t GetHeadSize();

private:

	IMemory *m_pMemory;
	ILock *m_pEnqueueLock;
	ILock *m_pDequeueLock;

};

#endif /* MESSAGEQUEUE_H_ */
