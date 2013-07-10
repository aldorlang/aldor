/******************************************************************************
 *
 * :: Timers
 *
 *****************************************************************************/

#ifndef _TIMER_H_
#define _TIMER_H_
 
#include "foam_c.h"
#include "axlgen.h"

typedef struct {
	FiWord time;
	FiWord start;
	FiWord live;
} *TmTimer, _TmTimer;

extern TmTimer tmAlloc();
extern void tmFree(TmTimer tm);
extern FiSInt tmRead(TmTimer tm);
extern void tmStart(TmTimer tm);
extern void tmStop(TmTimer tm);

#endif /*_TIMER_H_*/

