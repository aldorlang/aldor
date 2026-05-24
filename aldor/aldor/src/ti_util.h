#ifndef TI_UTIL_H
#define TI_UTIL_H

#include "cport.h"

/*****************************************************************************
 *
 * :: Function call state (functon/pattern/either)
 *
 ****************************************************************************/
enum patState {
	PS_None = 1, // No apply at all(!)
	PS_Function = 1,
	PS_Pattern = 2,
	PS_FnPat = 3
};

typedef Enum(patState) PatState;

Bool tiuAllowPatCall(PatState state);
Bool tiuAllowFnCall(PatState state);

#endif
