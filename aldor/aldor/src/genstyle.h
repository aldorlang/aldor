#ifndef _GF_GENSTYLE_H_
#define _GF_GENSTYLE_H_

#include "absyn.h"
#include "cport.h"

enum genType {
	GENTYPE_Function,
	GENTYPE_Coroutine,
	GENTYPE_Unknown
};

typedef Enum(genType) GenType;

#if 0
}; /* for editor indentation */
#endif

extern GenType gfGenTypeDefault(void);
extern GenType gfGenTypeAlt(void);

extern GenType gfGenTypeFor(AbSyn absyn);
extern GenType gfGenTypeGenerator(AbSyn absyn);
extern FoamTag gfGenFoamType(GenType genType);

#endif

