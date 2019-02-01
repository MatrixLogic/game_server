#ifndef __AUTOLOCK_H__
#define __AUTOLOCK_H__

#include "ILock.h"

class CAutoLock 
{
public:
	CAutoLock(ILock *pLock, bool bLock = false);
	~CAutoLock();
private:
	ILock *m_pLock;
};

#endif //__AUTOLOCK_H__
