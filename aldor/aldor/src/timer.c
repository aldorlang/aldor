/******************************************************************************
 *
 * :: Timers 
 *
 *****************************************************************************/

/*
 * [Not really a part of foam, but easiest to put here]
 */

#include "store.h"
#include "timer.h"

#define tmCpuTime osCpuTime

TmTimer
tmAlloc()
{
	TmTimer tm = (TmTimer) stoAlloc(OB_Other, sizeof(*tm));
	tm->time  = 0;
	tm->start = 0;
	tm->live  = 0;
	return tm;
}

void
tmFree(TmTimer tm)
{
	stoFree(tm);
}

FiSInt
tmRead(TmTimer tm)
{
  return tm->time + (tm->live ? tmCpuTime() - tm->start: 0);
}

void
tmStart(TmTimer tm)
{
	tm->live = 1;
	tm->start = tmCpuTime();
}

void
tmStop(TmTimer tm)
{
	tm->live = 0;
	tm->time += tmCpuTime() - tm->start;
}
