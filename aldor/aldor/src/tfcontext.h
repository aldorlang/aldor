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

struct inferEnv {
	TFormList varTypes; /* Slightly random field - will be a persistent table */
};

extern TFContext ctxtEmpty(TForm);
extern TFContext ctxtCopy(TFContext, TForm);
extern TFContext ctxtFollowOnly(TFContext);
#define ctxtTForm(tfc) ((tfc)->tform)
#define ctxtInfEnv(tfc) ((tfc)->infEnv)

extern UTFContext uctxtEmpty(UTForm);
extern UTFContext uctxtCopy(UTFContext, UTForm);
extern UTFContext uctxtFollowOnly(UTFContext);
extern Bool       uctxtEqual(UTFContext, UTFContext);
extern Bool       uctxtIsEmpty(UTFContext);
extern Bool       uctxtUTFIsConstant(UTFContext);
extern TForm      uctxtUTFConstOrFail(UTFContext);

#define uctxtUTForm(tfc) ((tfc)->utform)
#define uctxtInfEnv(tfc) ((tfc)->infEnv)

#endif
