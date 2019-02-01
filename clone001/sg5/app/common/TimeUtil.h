/*
 * TimeUtil.h
 *
 *  Created on: 2011-5-27
 *      Author: dada
 */

#ifndef TIMEUTIL_H_
#define TIMEUTIL_H_

namespace Time
{
	bool Init();
	bool UpdateGlobalTime();
	unsigned GetGlobalTime();
	bool IsToday(unsigned ts);
};

#endif /* TIMEUTIL_H_ */
