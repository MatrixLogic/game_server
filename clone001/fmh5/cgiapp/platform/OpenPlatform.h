/*
 * OpenPlatform
 *
 *  Created on: 2011-5-16
 *      Author: dada
 */

#ifndef __OPENPLATFORM_H__
#define __OPENPLATFORM_H__

#include "Kernel.h"
#include "IOpenPlatform.h"

namespace OpenPlatform
{
	//获取当前平台对象
	IOpenPlatform *GetPlatform();

	//获取当前平台类型
	PlatformType GetType();

	bool IsOurPlatform();
	bool IsLYPlatform();
	bool IsLY_ALL_Platform();
	bool IsQQPlatform();
	bool IsFBPlatform();
	bool IsEN();

	//更新当前平台类型
	bool Reset(const string &host_suffix = "");
}

#define IsTencentPlatform(pt) ((pt) == PT_PENGYOU || (pt) == PT_QZONE || (pt) == PT_TXWEIBO)

#endif	//__OPENPLATFORM_H__
