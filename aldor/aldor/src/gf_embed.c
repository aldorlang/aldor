#include "gf_embed.h"
#include "gf_prog.h"
#include "gf_util.h"
#include "gf_rtime.h"
#include "tform.h"
#include "util.h"

Foam
gen0Embed(Foam val, AbSyn ab, TForm tf, AbEmbed embed)
{
	/* Deal with delta-equality of cross/multis */
	tf = tfDefineeMaybeType(tf);

	if (embed & AB_Embed_Identity) {
		embed = embed & ~AB_Embed_Identity;
	}
	switch (embed) {
	case 0:
		return val;
	case AB_Embed_CrossToTuple:
		return gen0CrossToTuple(val, tf);
	case AB_Embed_CrossToMulti:
		return gen0CrossToMulti(val, tf);
	case AB_Embed_CrossToUnary:
		return gen0CrossToUnary(val, tf);
	case AB_Embed_MultiToTuple:
		return gen0MultiToTuple(val);
	case AB_Embed_MultiToCross:
		return gen0MultiToCross(val, tf);
	case AB_Embed_MultiToUnary:
		return gen0MultiToUnary(val);
	case AB_Embed_UnaryToTuple:
		return gen0UnaryToTuple(val);
	case AB_Embed_UnaryToCross:
		return gen0UnaryToCross(val, tf);
	case AB_Embed_UnaryToMulti:
		return gen0UnaryToMulti(val);
	case AB_Embed_UnaryToRaw:
		return gen0UnaryToRaw(val, ab);
	case AB_Embed_RawToUnary:
		return gen0RawToUnary(val, ab);
	default:
		bugBadCase(embed);
		NotReached(return val);
	}
}

Foam
gen0CrossToMulti(Foam val, TForm tf)
{
	Foam	values;
	Foam	t;
	int	i, size;
	AInt    cfmt, ftype;

	size = tfCrossArgc(tf);
	ftype = gen0Type(tf, &cfmt);
	cfmt  = gen0CrossFormatNumber(tf);
	t   = gen0TempLocal0(FOAM_Rec, cfmt);
	gen0SetTemp(t, foamNewCast(FOAM_Rec, val));
	values = foamNewEmpty(FOAM_Values, size);
	for (i = 0; i < size ; i++)
		values->foamValues.argv[i] = foamNewRElt(cfmt, foamCopy(t), i);

	foamFree(t);
	return values;
}

Foam
gen0CrossToUnary(Foam val, TForm tf)
{
	AInt    cfmt = gen0CrossFormatNumber(tf);

	return foamNewRElt(cfmt, val, int0);
}

Foam
gen0CrossToTuple(Foam val, TForm tf)
{
	Foam vars[2], tupl, elts, relt;
	AInt cfmt, ftype;
	Foam t;
	int  i;

	ftype = gen0Type(tf, &cfmt);
	cfmt  = gen0CrossFormatNumber(tf);
	t     = gen0TempLocal0(FOAM_Rec, cfmt);

	gen0SetTemp(t, foamNewCast(FOAM_Rec, val));
	gen0MakeEmptyTuple(foamNewSInt(tfCrossArgc(tf)), vars, NULL);
	tupl = vars[0];
	elts = vars[1];

	for (i=0; i < tfCrossArgc(tf); i++) {
		relt = foamNewRElt(cfmt, foamCopy(t), i);
		gen0AddStmt(gen0ASet(elts, i, FOAM_Word, relt), NULL);
	}

	return tupl;
}

Foam
gen0MultiToTuple(Foam val)
{
	Length	i, argc = foamArgc(val);
	Foam	vars[2], tupl, elts, elt;

	assert(foamTag(val) == FOAM_Values);
	gen0MakeEmptyTuple(foamNewSInt(argc), vars, NULL);
	tupl = vars[0];
	elts = vars[1];

	for (i = 0; i < argc; i += 1) {
		elt = val->foamValues.argv[i];
		gen0AddStmt(gen0ASet(elts, (AInt) i, FOAM_Word, elt), NULL);
	}

	return tupl;
}

Foam
gen0MultiToCross(Foam val, TForm tf)
{
	TForm ctf = tfCrossFrMulti(tf);
	Length	i, argc = foamArgc(val);
	AInt    cfmt;
	Foam	t;
	Foam    elt;

	assert(foamTag(val) == FOAM_Values);

	cfmt = gen0CrossFormatNumber(ctf);
	t = gen0TempLocal0(FOAM_Rec, cfmt);

	gen0SetTemp(t, foamNewRNew(cfmt));

	for (i = 0; i < argc; i += 1) {
		elt = val->foamValues.argv[i];
		gen0AddStmt(foamNewSet(foamNewRElt(cfmt, foamCopy(t), i),
				       elt), NULL);
	}

	return foamNewCast(FOAM_Word, t);
}

Foam
gen0MultiToUnary(Foam val)
{
	assert (foamTag(val) == FOAM_Values);
	assert (foamArgc(val) == 1);
	return val->foamValues.argv[0];
}

Foam
gen0UnaryToTuple(Foam val)
{
	Foam	vars[2], tupl, elts;
	FoamTag type = gen0FoamType(val);
	if (type != FOAM_Word)
		val = foamNewCast(FOAM_Word, val);

	gen0MakeEmptyTuple(foamNewSInt(1), vars, NULL);
	tupl = vars[0];
	elts = vars[1];

	gen0AddStmt(gen0ASet(elts, (AInt) 0, FOAM_Word, val), NULL);
	return tupl;
}

Foam
gen0UnaryToMulti(Foam val)
{
	Foam	values;

	values = foamNewEmpty(FOAM_Values, 1);
	values->foamValues.argv[0] = val;
	return values;
}

Foam
gen0UnaryToCross(Foam val, TForm tf)
{
	TForm   ctf;
	AInt    cfmt, ftype;
	Foam	t;

	ctf   = tfCross(1, tf);
	ftype = gen0Type(tf, &cfmt);
	cfmt  = gen0CrossFormatNumber(ctf);
	t = gen0TempLocal0(FOAM_Rec, cfmt);

	gen0AddStmt(foamNewSet(foamCopy(t), foamNewRNew(cfmt)), NULL);
	gen0AddStmt(foamNewSet(foamNewRElt(cfmt, foamCopy(t), int0), val), NULL);

	return foamNewCast(FOAM_Rec, t);
}


Foam
gen0UnaryToRaw(Foam val, AbSyn ab)
{
	AbSyn	imp = abImplicit(ab);
/* BDS: */
/* BDS:  This is the bug for pack0.sh */
/* BDS: */
/* Orig	FoamTag	raw = gen0Type(gen0AbType(imp), NULL); */
	FoamTag	raw = gen0Type(gen0AbType(imp), NULL);
	Syme	syme = abSyme(abApplyOp(imp));
	Foam	foam, *argloc;

/*	printf("BDS: Inside gen0UnaryToRaw\n");  */

	foam = gen0ApplySyme(raw, syme, abSymeImpl(abApplyOp(imp)), 1, &argloc);
	/* BDS This foamPrint may cause a crash because argloc isn't initialized */
/*	foamPrint(stdout,foam); */


/* BDS: */
/* BDS:  This is the bug for pack0.sh */
/* BDS: */
/* 	argloc[0] = genFoamCast(val, ab, FOAM_Word); */
/*	argloc[0] = genFoamCast(val, ab, raw);  */
/*	argloc[0] = genFoamCast(val, ab, raw);  */
	/*
	 *  In its original form, this code was casting the value to a word
         *  without considering its type.  However, no cast should be
         *  performed because it is raw's job to perform the conversion
         *  to the raw data type.
         */
	argloc[0] = val;

/*
	printf("BDS: About to finish in gen0UnaryToRaw\n");
	foamPrint(stdout,foam);
*/

	return foam;
}

Foam
gen0RawToUnary(Foam val, AbSyn ab)
{
	AbSyn	imp = abImplicit(ab);
	FoamTag	raw = gen0Type(gen0AbType(abApplyArg(imp, int0)), NULL);
	Syme	syme = abSyme(abApplyOp(imp));
	Foam	foam, *argloc;

/*	printf("BDS: Inside gen0RawToUnary\n"); */

	foam = gen0ApplySyme(FOAM_Word, syme, abSymeImpl(abApplyOp(imp)),
							 1, &argloc);
	argloc[0] = gen0FoamCast(val, ab, raw);

/*	printf("BDS: Done in gen0RawToUnary\n"); */

	return foam;
}

