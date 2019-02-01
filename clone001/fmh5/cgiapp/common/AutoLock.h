#ifndef __AUTOLOCK_H__
#define __AUTOLOCK_H__

#include "ILock.h"

class CAutoLock 
{
public:
	CAutoLock(ILock *pLock, bool bLock = false, unsigned vnum = 1, bool turn = true);
	~CAutoLock();

	void Lock();
	void Unlock();
private:
	ILock *m_pLock;
	unsigned m_num;
	bool m_bLock;
	bool m_turn;
};

#endif //__AUTOLOCK_H__
