/*
 * TimeUtil.cpp
 *
 *  Created on: 2011-5-27
 *      Author: dada
 */

#include "TimeUtil.h"
#include "Common.h"

extern long timezone;

namespace Time
{
	unsigned g_nGlobalTime = 0;
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
	return ((g_nGlobalTime - timezone) / 86400) == ((ts - timezone) / 86400);
}

int Time::ThroughDay(unsigned beg, unsigned end)
{
	return ((end - timezone) / 86400 - (beg - timezone) / 86400);
}
