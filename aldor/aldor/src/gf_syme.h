#ifndef _GF_SYME_H
#define _GF_SYME_H

extern void gen0InitConstTable(int max);
extern void genSetConstNum(Syme syme, int defineIdx, UShort index, Bool unique);
extern Bool genHasConstNum(Syme syme);
extern UShort genGetConstNum(Syme syme);
extern SymeList genGetSymeInlined(Syme syme);
extern void genKillOldSymeConstNums(int generation);
extern void gen0KillSymeConstNums();

#endif
