/*
 * MessageQueue.cpp
 *
 *  Created on: 2012-2-8
 *      Author: dada
 */

#include "MessageQueue.h"

struct MessageQueueHead
{
	uint32_t Flag;
	uint32_t MaxSize;
	uint32_t Head;
	uint32_t Tail;
	uint32_t Count;
	uint32_t MaxCount;
	uint32_t Reserve[2];
};

#define MESSAGE_QUEUE_HEAD_SIZE ((uint32_t)sizeof(MessageQueueHead))

CMessageQueue::CMessageQueue():
	m_pMemory(NULL),
	m_pEnqueueLock(NULL),
	m_pDequeueLock(NULL)
{
}

CMessageQueue::CMessageQueue(IMemory *pMemory, ILock *pEnqueueLock, ILock *pDequeueLock)
{
	Attach(pMemory, pEnqueueLock, pDequeueLock);
}

bool CMessageQueue::Attach(IMemory *pMemory, ILock *pEnqueueLock, ILock *pDequeueLock)
{
	if(pMemory == NULL)
	{
		m_pMemory = NULL;
		m_pEnqueueLock = NULL;
		m_pDequeueLock = NULL;
		return false;
	}
	if(pMemory->GetSize() < (int)MESSAGE_QUEUE_HEAD_SIZE)
	{
		m_pMemory = NULL;
		m_pEnqueueLock = NULL;
		m_pDequeueLock = NULL;
		return false;
	}
	m_pMemory = pMemory;
	m_pEnqueueLock = pEnqueueLock;
	m_pDequeueLock = pDequeueLock;
	MessageQueueHead *pHead = (MessageQueueHead *)m_pMemory->GetAddress();
	if(pHead != NULL && pHead->Flag == 0)
	{
		CAutoLock(m_pEnqueueLock, true);
		if(m_pDequeueLock != NULL && m_pDequeueLock != m_pEnqueueLock)
		{
			m_pDequeueLock->Lock();
		}
		if(Atomic::CompareAndSwap<uint32_t>(&pHead->Flag, 0, 1))
		{
			pHead->MaxSize = (m_pMemory->GetSize() - MESSAGE_QUEUE_HEAD_SIZE) >> 2 << 2;
			pHead->Head = 0;
			pHead->Tail = 0;
			pHead->Count = 0;
			pHead->MaxCount = UINT32_MAX;
		}
		if(m_pDequeueLock != NULL && m_pDequeueLock != m_pEnqueueLock)
		{
			m_pDequeueLock->Unlock();
		}
	}
	return true;
}

bool CMessageQueue::Detach()
{
	m_pMemory = NULL;
	m_pEnqueueLock = NULL;
	m_pDequeueLock = NULL;
	return true;
}

bool CMessageQueue::SetMaxCount(uint32_t count)
{
	MessageQueueHead *pHead = (MessageQueueHead *)m_pMemory->GetAddress();
	if(pHead != NULL)
	{
		pHead->MaxCount = count;
	}
	return true;
}

bool CMessageQueue::Enqueue(const IBuffer *pBuffer)
{
	if(pBuffer == NULL)
	{
		return false;
	}
	const byte *pData = pBuffer->GetConstBuffer();
	uint32_t dataSize = pBuffer->GetSize();
	if(pData == NULL || dataSize == 0)
	{
		return false;
	}
	MessageQueueHead *pHead = (MessageQueueHead *)m_pMemory->GetAddress();
	if(pHead == NULL)
	{
		return false;
	}
	CAutoLock(m_pEnqueueLock, true);
	if(pHead->Count >= pHead->MaxCount)
	{
		return false;
	}
	uint32_t maxSize = pHead->MaxSize;
	uint32_t head = pHead->Head;
	uint32_t tail = pHead->Tail;
	uint32_t leftSize;
	if(head <= tail)
	{
		leftSize = maxSize - (tail - head) - sizeof(uint32_t);
	}
	else
	{
		leftSize = head - tail - sizeof(uint32_t);
	}
	if(dataSize + sizeof(uint32_t) > leftSize)
	{
		return false;
	}

	byte *pQueueTail = (byte *)pHead + MESSAGE_QUEUE_HEAD_SIZE + tail;
	*((uint32_t *)pQueueTail) = dataSize;
	tail += sizeof(uint32_t);
	if(tail >= maxSize)
	{
		tail = 0;
		pQueueTail = (byte *)pHead + MESSAGE_QUEUE_HEAD_SIZE;
	}
	else
	{
		pQueueTail += sizeof(uint32_t);
	}
	if(head <= tail)
	{
		if(tail + dataSize <= maxSize)
		{
			memcpy(pQueueTail, pData, dataSize);
			tail = (tail + dataSize + sizeof(uint32_t) - 1) >> 2 << 2;
			if(tail >= maxSize)
			{
				tail = 0;
			}
			pHead->Tail = tail;
			Atomic::Add<uint32_t>(&pHead->Count, 1);
			return true;
		}
		else
		{
			uint32_t copySize = maxSize - tail;
			memcpy(pQueueTail, pData, copySize);
			tail = 0;
			pQueueTail = (byte *)pHead + MESSAGE_QUEUE_HEAD_SIZE;
			pData += copySize;
			dataSize -= copySize;
		}
	}
	memcpy(pQueueTail, pData, dataSize);
	tail = (tail + dataSize + sizeof(uint32_t) - 1) >> 2 << 2;
	if(tail >= maxSize)
	{
		tail = 0;
	}
	pHead->Tail = tail;
	Atomic::Add<uint32_t>(&pHead->Count, 1);
	return true;
}

bool CMessageQueue::Dequeue(IBuffer *pBuffer)
{
	if(pBuffer == NULL)
	{
		return false;
	}
	MessageQueueHead *pHead = (MessageQueueHead *)m_pMemory->GetAddress();
	if(pHead == NULL)
	{
		return false;
	}
	CAutoLock(m_pDequeueLock, true);
	if(pHead->Count == 0)
	{
		return false;
	}
	uint32_t maxSize = pHead->MaxSize;
	uint32_t head = pHead->Head;
	byte *pQueueHead = (byte *)pHead + MESSAGE_QUEUE_HEAD_SIZE + head;
	uint32_t dataSize = *((uint32_t *)pQueueHead);
	head += sizeof(uint32_t);
	if(head >= maxSize)
	{
		head = 0;
		pQueueHead = (byte *)pHead + MESSAGE_QUEUE_HEAD_SIZE;
	}
	else
	{
		pQueueHead += sizeof(uint32_t);
	}
	if(head > pHead->Tail)
	{
		if(head + dataSize <= maxSize)
		{
			pBuffer->Append(pQueueHead, dataSize);
			head = (head + dataSize + sizeof(uint32_t) - 1) >> 2 << 2;
			if(head >= maxSize)
			{
				head = 0;
			}
			pHead->Head = head;
			Atomic::Sub<uint32_t>(&pHead->Count, 1);
			return true;
		}
		else
		{
			uint32_t copySize = maxSize - head;
			pBuffer->Append(pQueueHead, copySize);
			head = 0;
			pQueueHead = (byte *)pHead + MESSAGE_QUEUE_HEAD_SIZE;
			dataSize -= copySize;
		}
	}
	pBuffer->Append(pQueueHead, dataSize);
	head = (head + dataSize + sizeof(uint32_t) - 1) >> 2 << 2;
	if(head >= maxSize)
	{
		head = 0;
	}
	pHead->Head = head;
	Atomic::Sub<uint32_t>(&pHead->Count, 1);
	return true;
}

bool CMessageQueue::Peek(IBuffer *pBuffer) const
{
	if(pBuffer == NULL)
	{
		return false;
	}
	MessageQueueHead *pHead = (MessageQueueHead *)m_pMemory->GetAddress();
	if(pHead == NULL)
	{
		return false;
	}
	CAutoLock(m_pDequeueLock, true);
	if(pHead->Count == 0)
	{
		return false;
	}
	uint32_t maxSize = pHead->MaxSize;
	uint32_t head = pHead->Head;
	byte *pQueueHead = (byte *)pHead + MESSAGE_QUEUE_HEAD_SIZE + head;
	uint32_t dataSize = *((uint32_t *)pQueueHead);
	head += sizeof(uint32_t);
	if(head >= maxSize)
	{
		head = 0;
		pQueueHead = (byte *)pHead + MESSAGE_QUEUE_HEAD_SIZE;
	}
	else
	{
		pQueueHead += sizeof(uint32_t);
	}
	if(head > pHead->Tail)
	{
		if(head + dataSize <= maxSize)
		{
			pBuffer->Append(pQueueHead, dataSize);
			head = (head + dataSize + sizeof(uint32_t) - 1) >> 2 << 2;
			if(head >= maxSize)
			{
				head = 0;
			}
			return true;
		}
		else
		{
			uint32_t copySize = maxSize - head;
			pBuffer->Append(pQueueHead, copySize);
			head = 0;
			pQueueHead = (byte *)pHead + MESSAGE_QUEUE_HEAD_SIZE;
			dataSize -= copySize;
		}
	}
	pBuffer->Append(pQueueHead, dataSize);
	head = (head + dataSize + sizeof(uint32_t) - 1) >> 2 << 2;
	if(head >= maxSize)
	{
		head = 0;
	}
	return true;
}

uint32_t CMessageQueue::GetMaxCount() const
{
	MessageQueueHead *pHead = (MessageQueueHead *)m_pMemory->GetAddress();
	if(pHead == NULL)
	{
		return 0;
	}
	return pHead->MaxCount;
}

uint32_t CMessageQueue::GetCount() const
{
	MessageQueueHead *pHead = (MessageQueueHead *)m_pMemory->GetAddress();
	if(pHead == NULL)
	{
		return 0;
	}
	return pHead->Count;
}

bool CMessageQueue::IsFull() const
{
	MessageQueueHead *pHead = (MessageQueueHead *)m_pMemory->GetAddress();
	if(pHead == NULL)
	{
		return true;
	}
	return pHead->Count >= pHead->MaxCount || pHead->Tail + 1 == pHead->Head;
}

bool CMessageQueue::IsEmpty() const
{
	MessageQueueHead *pHead = (MessageQueueHead *)m_pMemory->GetAddress();
	if(pHead == NULL)
	{
		return true;
	}
	return pHead->Tail == pHead->Head;
}

uint32_t CMessageQueue::GetHeadSize()
{
	return MESSAGE_QUEUE_HEAD_SIZE;
}
