#ifndef _GENJAVA_H
#define _GENJAVA_H

#include "javacode.h"

JavaCode genJavaUnit(Foam foam, String name);

void gjGenSetMain(Bool flg);
extern Bool genJavaDebug;

#endif
