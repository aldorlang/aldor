#include "tfcontext.h"

#include "store.h"
#include "tform.h"
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
	return ctxtAlloc(tf, NULL);
}

TFContext
ctxtCopy(TFContext tfc, TForm tf)
{
	return ctxtAlloc(tf, ctxtInfEnv(tfc));
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
uctxtEmpty(UTForm utf)
{
	return uctxtAlloc(utf, NULL);
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
	bug("Needs work");
	return false;
}

Bool
uctxtIsEmpty(UTFContext utfc)
{
	return uctxtInfEnv(utfc) == NULL;
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
