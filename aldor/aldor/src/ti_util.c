#include "ti_util.h"

Bool
tiuAllowPatCall(PatState state)
{
	return state & PS_Pattern;
}

Bool
tiuAllowFnCall(PatState state)
{
	return state & PS_Function;
}


