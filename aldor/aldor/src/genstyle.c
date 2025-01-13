#include "genstyle.h"

#if GENERATOR_COROUTINES
GenType
gfGenTypeDefault(void)
{
	return GENTYPE_Coroutine;
}

GenType
gfGenTypeAlt(void)
{
	return GENTYPE_Function;
}

#else
GenType
gfGenTypeDefault(void)
{
	return GENTYPE_Function;
}

GenType
gfGenTypeAlt(void)
{
	return GENTYPE_Coroutine;
}

#endif

local GenType
gfGenTypeAbSyn(AbSyn absyn)
{
	if (abFlag_IsNewIter(absyn))
		return gfGenTypeAlt();
	else 
		return gfGenTypeDefault();
}

GenType
gfGenTypeGenerator(AbSyn absyn)
{
	return gfGenTypeAbSyn(absyn);
}

GenType
gfGenTypeFor(AbSyn absyn)
{
	return gfGenTypeAbSyn(absyn);
}


