#include "axlobs.h"
#include "foam.h"
#include "testlib.h"
#include "syme.h"
#include "sexpr.h"
#include "util.h"

local void testCall();
local void testDDecl();
local void testConstructors();
local void testHash();

void
foamTest()
{
	sxiInit();

	TEST(testCall);
	TEST(testDDecl);
	TEST(testConstructors);
	TEST(testHash);
}

local void
testCall()
{
	Foam foam;

	foam = foamNew(FOAM_OCall, 3, FOAM_Clos, foamNewNil(), foamNewNil());
	testIntEqual("foamOCallArgc", 0, foamOCallArgc(foam));

	foam = foamNew(FOAM_OCall, 4, FOAM_Clos, foamNewNil(), foamNewNil(), foamNewSInt(1));
	testIntEqual("foamOCallArgc", 1, foamOCallArgc(foam));

	foam = foamNewPCall(FOAM_Proto_C, FOAM_NOp, foamNewGlo(int0), NULL);
	testIntEqual("argc", 0, foamPCallArgc(foam));
	testIntEqual("protocol", FOAM_Proto_C, foam->foamPCall.protocol);
}

local void
testDDecl()
{
	Foam ddecl = foamNewDDecl(FOAM_DDecl_Local,
				  foamNewDecl(FOAM_SInt, strCopy("fred"), emptyFormatSlot),
				  NULL);
	testIntEqual("tag", FOAM_DDecl, foamTag(ddecl));
	testIntEqual("argc", 1, foamDDeclArgc(ddecl));
}

local void
testConstructors()
{
	Foam foam;
	foam = foamNewBCall0(FOAM_BVal_BoolNot);
	testIntEqual("argc", foamBCallSlotc, foamArgc(foam));
	testIntEqual("tag", FOAM_BVal_BoolNot, foam->foamBCall.op);

	Foam arg1 = foamNewLoc(int0);
	Foam arg2 = foamNewLoc(int0);
	foam = foamNewBCall1(FOAM_BVal_BoolNot, arg1);
	testIntEqual("argc", 1, foamBCallArgc(foam));
	testIntEqual("tag", FOAM_BVal_BoolNot, foam->foamBCall.op);
	testPointerEqual("arg1", arg1, foam->foamBCall.argv[0]);

	foam = foamNewBCall2(FOAM_BVal_BoolNot, arg1, arg2);
	testIntEqual("argc", 2, foamBCallArgc(foam));
	testIntEqual("tag", FOAM_BVal_BoolNot, foam->foamBCall.op);
	testPointerEqual("arg1", arg1, foam->foamBCall.argv[0]);
	testPointerEqual("arg1", arg2, foam->foamBCall.argv[1]);
}


local AInt
oldHashCombine(AInt h1, AInt h2)
{
	return ((( ((h2) & 16777215L) << 6) + h1) % 1073741789) ;

}

/*
 * Testing hash generation... there's a hashcode collision with the
 * '*' operation using the "old" mechanism (see above).
 *
 * This is mostly useful in figuring out why axiom and aldor sometimes
 * disagree on hashcodes.
 *
 * The axiom output comes from instrumenting hashcode.boot
 */

local void
testHash()
{
	AInt hULS = strHash("UnivariateLaurentSeries");
	AInt hUTS = strHash("UnivariateTaylorSeries");
	AInt hFRAC = strHash("Fraction");
	AInt hINT = strHash("Integer");
	AInt hMapping = strHash("->");
	AInt twist = 32236;

/*(HASH 484208045 134808007 (|Integer|)) */
	testIntEqual("INT", 484208045, hINT);
	testIntEqual("FRAC", 777777278, hFRAC);
	testIntEqual("ULS", 241975245, hULS);
	testIntEqual("UTS", 585167620, hUTS);
	testIntEqual("->", 51489085, hMapping);
/*(HASH 869829933 134808007 (|Fraction| (|Integer|))) */
	testIntEqual("FR_INT", 869829933, oldHashCombine(hINT, hFRAC));

/*(HASH 1027140807 134808007 (|Mapping| $ $)) */
	testIntEqual("X -> X", 1027140807, oldHashCombine(134808007,
							  oldHashCombine(twist,
									 oldHashCombine(134808007, hMapping))));

/*(HASH 476114119 134808007 (|Mapping| $ $ $)) */
	testIntEqual("(X, X) -> X", 476114119,
		     oldHashCombine(134808007,
				    oldHashCombine(twist, oldHashCombine(134808007,
									 oldHashCombine(134808007, hMapping)))));

/*(HASH 200862919 134808007 (|Mapping| $ (|Integer|) $)) */

	testIntEqual("T1", 200862919,
		     oldHashCombine(134808007,
				    oldHashCombine(twist, oldHashCombine(134808007,
									 oldHashCombine(hINT, hMapping)))));

/*(HASH 134808007 0 (|UnivariateLaurentSeries| (|Fraction| (|Integer|)) |z| (0 . 1))) */

	AInt hULS_FI = oldHashCombine(7, oldHashCombine(7,
							oldHashCombine(oldHashCombine(hINT, hFRAC),
								       hULS)));
	testIntEqual("HULS_FI", 134808007, hULS_FI);

/*
(HASH 350552519 134808007
 (|UnivariateTaylorSeries| (|Fraction| (|Integer|)) |z| (0 . 1)))
*/
	AInt hUTS_FI = oldHashCombine(7, oldHashCombine(7,
							oldHashCombine(oldHashCombine(hINT, hFRAC),
								       hUTS)));
	testIntEqual("HUTS_FI", 350552519, hUTS_FI);

/*(HASH 476114119 134808007
        (|Mapping| $ (|UnivariateTaylorSeries| (|Fraction| (|Integer|)) |z| (0 . 1)) $))
*/
	testIntEqual("(UTS_FI, X) -> X", 476114119,
		     oldHashCombine(hULS_FI,
				    oldHashCombine(twist, oldHashCombine(hULS_FI,
									 oldHashCombine(hUTS_FI, hMapping)))));
/* Clash: UTS_FI/ULS_FI */
	testIntEqual("(X, X) -> X", 476114119,
		     oldHashCombine(134808007,
				    oldHashCombine(twist, oldHashCombine(134808007,
									 oldHashCombine(134808007, hMapping)))));
	/*
	 * ++ New hash function
	 */
	int nULS_FI = hashCombinePair(7, hashCombinePair(7,
						 hashCombinePair(hashCombinePair(hINT, hFRAC),
							     hUTS)));
	int nUTS_FI = hashCombinePair(7, hashCombinePair(7,
						 hashCombinePair(hashCombinePair(hINT, hFRAC),
							     hULS)));
	int nMap_X_X_to_X = hashCombinePair(nULS_FI,
					   hashCombinePair(twist, hashCombinePair(nULS_FI,
										hashCombinePair(nULS_FI, hMapping))));
	int nMap_T_X_to_X = hashCombinePair(nULS_FI,
					   hashCombinePair(twist, hashCombinePair(nULS_FI,
										hashCombinePair(nUTS_FI, hMapping))));
	testTrue("%d %d\n", nMap_T_X_to_X != nMap_X_X_to_X);
}
