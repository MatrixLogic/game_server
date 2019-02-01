/*
 * HeapMemory.h
 *
 *  Created on: 2012-2-3
 *      Author: dada
 */

#ifndef HEAPMEMORY_H_
#define HEAPMEMORY_H_

#include "Common.h"

class CHeapMemory : public IMemory
{
public:

	CHeapMemory();
	CHeapMemory(int size);
	~CHeapMemory();

	bool Create(int size);
	bool Close();

	void *GetAddress() const { return m_pAddress; }
	int GetSize() const { return m_size; }

private:
	void *m_pAddress;
	int m_size;
};

#endif /* HEAPMEMORY_H_ */
