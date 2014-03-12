/******************************************************************************
 *
 * :: Timers
 *
 *****************************************************************************/

#ifndef TIMER_H_
#define TIMER_H_
 
#include "axlgen.h"
#include "foam_c.h"

typedef struct tmTimer {
	FiWord time;
	FiWord start;
	FiWord live;
} *TmTimer;

extern TmTimer	tmAlloc	(void);
extern void	tmFree	(TmTimer tm);
extern FiSInt	tmRead	(TmTimer tm);
extern void	tmStart	(TmTimer tm);
extern void	tmStop	(TmTimer tm);

#endif /* TIMER_H_ */
