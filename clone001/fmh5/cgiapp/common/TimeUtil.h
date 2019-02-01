/*
 * TimeUtil.h
 *
 *  Created on: 2011-5-27
 *      Author: dada
 */

#ifndef TIMEUTIL_H_
#define TIMEUTIL_H_

#define TIME_COUNT_RESTART(fmt, args...) debug_log("[TIME_RESTART=%u,uid=%u]"fmt, Time::TimeCountMS(),uid, ##args)
#define TIME_COUNT_LOG(fmt, args...) debug_log("[TIME_COUNT=%u,uid=%u]"fmt, Time::TimeCountMS(),uid, ##args)

namespace Time
{
	bool Init();
	bool UpdateGlobalTime();
	unsigned GetGlobalTime();
	bool IsToday(unsigned ts);
	bool IsThisWeek(unsigned ts);
	unsigned TimeCountMS();
	void Sleep(int seconds, int mseconds);
};

#endif /* TIMEUTIL_H_ */
