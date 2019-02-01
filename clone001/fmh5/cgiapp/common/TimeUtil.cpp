/*
 * TimeUtil.cpp
 *
 *  Created on: 2011-5-27
 *      Author: dada
 */

#include "TimeUtil.h"
#include "Common.h"

//extern long timezone;

namespace Time
{
	unsigned g_nGlobalTime = 0;
	unsigned g_nTimeCount = 0;
}

bool Time::Init()
{
	tsc_init();
	UpdateGlobalTime();
	return true;
}

bool Time::UpdateGlobalTime()
{
	g_nGlobalTime = time(NULL);
	return true;
}

unsigned Time::GetGlobalTime()
{
	return g_nGlobalTime;
}

bool Time::IsToday(unsigned ts)
{
	//return ((g_nGlobalTime - timezone) / 86400) == ((ts - timezone) / 86400);
	return !CTime::IsDiffDay(ts, g_nGlobalTime);
}

bool Time::IsThisWeek(unsigned ts)
{
	//return ((g_nGlobalTime - timezone - 86400 * 3) / 86400 / 7) == ((ts - timezone - 86400 * 3) / 86400 / 7);
	return !CTime::IsDiffWeek(ts, g_nGlobalTime);
}

unsigned Time::TimeCountMS()
{
	unsigned now;
	mtime(now);
	unsigned delta = now - g_nTimeCount;
	g_nTimeCount = now;
	return delta;
}

void Time::Sleep(int seconds, int mseconds)
{
	struct timeval temp;

	temp.tv_sec = seconds;
	temp.tv_usec = mseconds;

	select(0, NULL, NULL, NULL, &temp);

	return ;
}
