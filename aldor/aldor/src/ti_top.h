#ifndef _TI_TOP_H_
#define _TI_TOP_H_
#include "axlobs.h"

typedef AbSynList SymeCContext;

typedef struct _tiTopLevel {
	void    (*terrorTypeConstFailed)(TConst);
	void    (*tiBottomUp)		(Stab, AbSyn, TForm);
	void    (*tiTopDown)		(Stab, AbSyn, TForm);
	Bool    (*tiCanSefo)		(Sefo);
	TForm   (*tiGetTopLevelTForm)	(AbLogic, AbSyn);

	Bool	(*tiUnaryToRaw)		(Stab, AbSyn, TForm);
	Bool	(*tiRawToUnary)		(Stab, AbSyn, TForm);
	void	(*tiSefo)		(Stab, Sefo);
	void	(*tiTfSefo)		(Stab, TForm);
	void    (*typeInferTForms)	(Stab);

	Bool	(*tqShouldImport)	(TQual);
	TForm	(*typeInferTForm)	(Stab, TForm);
} *TiTopLevel;

extern void tiTopLevelInit(TiTopLevel fns);
extern void tiTopLevelFini();

extern TiTopLevel tiTopFns();

#endif
