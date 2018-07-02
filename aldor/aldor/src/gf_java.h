#ifndef GF_JAVA_H
#define GF_JAVA_H

extern Foam gfjGetImport	(Syme syme);
extern Foam gfjSymeImport	(Syme syme);
extern void gfjIssueFns		(void);
extern void gfjInit		(void);
extern void gfjFini		(void);
extern void gfjVarImport	(Syme syme, Stab stab);

extern void gfjExportToJava     (AbSyn what, AbSyn dest);

extern void gfjClassFormatNumber(AbSyn);

#endif

