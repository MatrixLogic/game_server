/*
 * SystemUtil.h
 *
 *  Created on: 2012-1-31
 *      Author: dada
 */

#ifndef SYSTEMUTIL_H_
#define SYSTEMUTIL_H_

#include <stdio.h>

typedef void (*SignalCallback)();

namespace System
{
	bool InitDaemon(SignalCallback sigusr1 = NULL, SignalCallback sigusr2 = NULL);
	bool IgnoreSignal(int signum);
}

#endif /* SYSTEMUTIL_H_ */
