/*
 * KernelUtil.h
 *
 *  Created on: 2011-5-26
 *      Author: dada
 */

#ifndef KERNELUTIL_H_
#define KERNELUTIL_H_

#include "Kernel.h"

namespace Kernel
{
	bool InitLog(const string &configPath);
	bool Init(const string &configPath = "");
}

#endif /* KERNELUTIL_H_ */
