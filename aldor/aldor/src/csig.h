#ifndef _CSIG_H_
#define _CSIG_H_
#include "foam.h"

Foam csigNew(FoamList args, FoamList rets);

AInt csigArgc(Foam foam);
Foam csigArgN(Foam foam, Length n);

AInt csigRetc(Foam foam);
Foam csigRetN(Foam foam, Length n);


#endif
