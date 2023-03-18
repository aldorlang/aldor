#ifndef _RDLN_H
#define _RDLN_H

#include "cport.h"

void rdlnInit(void);
void rdlnCheck(void);
void rdlnFini(void);

void rdlnSetPrompt(String fmt, ...);

#endif
