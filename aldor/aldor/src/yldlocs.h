#ifndef YLDLOCS_H
#define YLDLOCS_H
#include "axlobs.h"

typedef struct {
	AIntList reclocs;
	AIntList locs;
} *YldLocResult;

YldLocResult	ylProg		(Foam prog);
void		ylLocResultFree	(YldLocResult result);

#endif

