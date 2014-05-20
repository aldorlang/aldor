#include "store.h"
#include "errorset.h"
#include "format.h"

ErrorSet
errorSetNew(void)
{
	ErrorSet errors = (ErrorSet) stoAlloc(OB_Other, sizeof(*errors));

	errors->list = listNil(String);
	errors->alloc = listNil(String);

	return errors;
}

void
errorSetFree(ErrorSet errors)
{
	listFreeDeeply(String)(errors->alloc, strFree);
}

Bool
errorSetHasErrors(ErrorSet errors)
{
	return errors->list != listNil(String);
}

StringList
errorSetErrors(ErrorSet errors)
{
	return errors->list;
}

Bool
errorSetCheck(ErrorSet errors, Bool test, String message)
{
	if (test)
		return true;

	errorSetAdd(errors, message);
	return false;
}


void
errorSetAdd(ErrorSet errors, String message)
{
	errors->list = listCons(String)(message, errors->list);
}

Bool
errorSetPrintf(ErrorSet errors, Bool test, String format, ...)
{
	String message;
	va_list argp;
	if (test)
		return true;
	va_start(argp, format);
	message = vaStrPrintf(format, argp);
	va_end(argp);

	errorSetAdd(errors, message);
	errors->alloc = listCons(String)(message, errors->alloc);

	return false;
}






