#ifndef _TFCONTEXT_H_
#define _TFCONTEXT_H_

#include "utform.h"

struct tfContext {
	TForm  tform;
	InferEnv infEnv;
};

struct utfContext {
	UTForm  utform;
	InferEnv infEnv;
};

extern TFContext ctxtEmpty(TForm);
extern TFContext ctxtCopy(TFContext, TForm);
extern TFContext ctxtCopyUTFContext(UTFContext, TForm);
extern TFContext ctxtFollowOnly(TFContext);
extern int	 ctxtOStreamWrite(OStream os, TFContext tfc);

#define ctxtTForm(tfc) ((tfc)->tform)
#define ctxtInfEnv(tfc) ((tfc)->infEnv)

extern UTFContext uctxtEmpty(UTForm);
extern UTFContext uctxtNew(InferEnv, UTForm);
extern UTFContext uctxtNewConst(InferEnv, TForm);
extern UTFContext uctxtCopy(UTFContext, UTForm);
extern UTFContext uctxtFollowOnly(UTFContext);
extern Bool       uctxtEqual(UTFContext, UTFContext);
extern Bool       uctxtIsEmpty(UTFContext);
extern Bool       uctxtUTFIsConstant(UTFContext);
extern TForm      uctxtUTFConstOrFail(UTFContext);

extern int	  uctxtOStreamWrite(OStream, UTFContext);

extern TForm	  uctxtInferredType(UTFContext context);

#define uctxtUTForm(tfc) ((tfc)->utform)
#define uctxtInfEnv(tfc) ((tfc)->infEnv)

#endif
