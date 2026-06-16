#include "tfcontext.h"

#include "format.h"
#include "infenv.h"
#include "store.h"
#include "sefo.h"
#include "tform.h"
#include "tfsat.h"
#include "util.h"
/*
 * TFContext
 * Used for type inference.  Basically a type, plus a unifier list.
 */

/*
 * :: TFContext
 */

CREATE_LIST(TFContext);

local TFContext
ctxtAlloc(TForm tf, InferEnv inf)
{
	TFContext tfc = (TFContext) stoAlloc(OB_TFContext, sizeof(*tfc));
	tfc->tform = tf;
	tfc->infEnv = inf;
	return tfc;
}


TFContext
ctxtEmpty(TForm tf)
{
	return ctxtAlloc(tf, infEnvEmpty());
}

TFContext
ctxtCopy(TFContext tfc, TForm tf)
{
	return ctxtAlloc(tf, ctxtInfEnv(tfc));
}

TFContext
ctxtCopyUTFContext(UTFContext utfc, TForm tf)
{
	return ctxtAlloc(tf, uctxtInfEnv(utfc));
}


TFContext
ctxtFollowOnly(TFContext tfc)
{
	TForm tf = tfFollowOnly(ctxtTForm(tfc));
	if (tf == ctxtTForm(tfc)) {
		return tfc;
	}
	return ctxtCopy(tfc, tf);
}

int
ctxtOStreamWrite(OStream os, TFContext tfc)
{
	if (infEnvIsEmpty(uctxtInfEnv(tfc)))
		return tformOStreamWrite(os, false, ctxtTForm(tfc));
	else {
		return ostreamPrintf(os, "[C: %pTForm (%pInferEnv)]",
				     ctxtTForm(tfc), uctxtInfEnv(tfc));
	}
}

/*
 * :: UTFContext
 */
CREATE_LIST(UTFContext);

local UTFContext
uctxtAlloc(UTForm utf, InferEnv inf)
{
	UTFContext tfc = (UTFContext) stoAlloc(OB_TFContext, sizeof(*tfc));
	tfc->utform = utf;
	tfc->infEnv = inf;

	return tfc;
}

UTFContext
uctxtNew(InferEnv env, UTForm utf)
{
	return uctxtAlloc(utf, env);
}

UTFContext
uctxtNewConst(InferEnv env, TForm tf)
{
  return uctxtAlloc(utformNewConstant(tf), env);
}

UTFContext
uctxtEmpty(UTForm utf)
{
	return uctxtAlloc(utf, infEnvEmpty());
}

UTFContext
uctxtCopy(UTFContext utfc, UTForm utf)
{
	return uctxtAlloc(utf, uctxtInfEnv(utfc));
}

UTFContext
uctxtFollowOnly(UTFContext uctxt)
{
	UTForm utform = utformFollowOnly(uctxtUTForm(uctxt));
	if (uctxtUTForm(uctxt) == utform) {
		return uctxt;
	}
	return uctxtCopy(uctxt, utform);
}

Bool
uctxtEqual(UTFContext utfc1, UTFContext utfc2)
{
	if (utfc1 == utfc2)
		return true;
	return infEnvEqual(uctxtInfEnv(utfc1), uctxtInfEnv(utfc2))
		&& utformEqual(uctxtUTForm(utfc1), uctxtUTForm(utfc2));
}

Bool
uctxtIsEmpty(UTFContext utfc)
{
	return infEnvIsEmpty(uctxtInfEnv(utfc));
}

Bool
uctxtUTFIsConstant(UTFContext utfc)
{
	return utfIsConstant(uctxtUTForm(utfc));
}

TForm
uctxtUTFConstOrFail(UTFContext utfc)
{
	return utformConstOrFail(uctxtUTForm(utfc));
}

TForm
uctxtInferredType(UTFContext utfc)
{
	if (utfc == NULL) {
		return NULL;
	}
	InferEnv infEnv = uctxtInfEnv(utfc);
	TForm tf = uctxtUTFConstOrFail(utfc);

	if (infEnvIsEmpty(infEnv)) {
		return tf;
	}
	return tformFollowVars(infEnv, tf);
}

UTFContext
uctxtEmbedResult(UTFContext utfc, AbEmbed e)
{
	TForm otf = utformConstOrFail(uctxtUTForm(utfc));
	TForm ntf = tfsEmbedResult(utformConstOrFail(uctxtUTForm(utfc)), e);
	if (otf == ntf) {
		return utfc;
	}
	else {
		return uctxtCopy(utfc, utformNewConstant(ntf));
	}
}


int
uctxtOStreamWrite(OStream os, UTFContext utfc)
{
	if (infEnvIsEmpty(uctxtInfEnv(utfc)))
		return utfOStreamWrite(os, false, uctxtUTForm(utfc));
	else {
		return ostreamPrintf(os, "[C: %pUTForm (%pInferEnv)]",
				     uctxtUTForm(utfc), uctxtInfEnv(utfc));
	}
}
