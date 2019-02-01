#ifndef __ILOCK_H__
#define __ILOCK_H__

#include "IBase.h"

class ILock : IBase
{
public:
	virtual bool Lock(int vnum = 1) = 0;
	virtual bool Unlock(int vnum = 1) = 0;
};

#endif //__ILOCK_H__
