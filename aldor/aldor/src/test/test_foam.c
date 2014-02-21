#include "axlobs.h"
#include "foam.h"
#include "testlib.h"
#include "syme.h"
#include "sexpr.h"
#include "util.h"

local void testCall();
local void testDDecl();
local void testConstructors();
local void testTests();
local void testHash();
local void testIter();
local void testSIntReduce();
local void testFoamBuffer();

void
foamTest()
{
	sxiInit();

	TEST(testCall);
	TEST(testDDecl);
	TEST(testConstructors);
	TEST(testTests);
	TEST(testHash);
	TEST(testIter);
	TEST(testSIntReduce);
	TEST(testFoamBuffer);
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

local void
testTests()
{
	Foam foam = foamNewSet(foamNewLoc(1), foamNewLoc(1));
	testFalse("", foamIsMultiAssign(foam));

	foam = foamNewSet(foamNew(FOAM_Values, 2, foamNewLoc(1), foamNewLoc(2)),
			  foamNewLoc(3));
	testTrue("", foamIsMultiAssign(foam));
}


local void
testIter()
{
	Foam seq;
	int i;
	seq = foamNewSeq(NULL);
	i = -1;
	i = foamSeqNextReachable(seq, i);
	testIntEqual("1", -1, i);

	seq = foamNewSeq(foamNewNOp(),
			 foamNewGoto(10),
			 foamNewLabel(10),
			 NULL);
	i = -1;
	i = foamSeqNextReachable(seq, i);
	testIntEqual("", 0, i);
	i = foamSeqNextReachable(seq, i);
	testIntEqual("", 1, i);
	i = foamSeqNextReachable(seq, i);
	testIntEqual("", 2, i);
	i = foamSeqNextReachable(seq, i);
	testIntEqual("", -1, i);

	seq = foamNewSeq(foamNewNOp(),
			 foamNewGoto(10),
			 foamNewNOp(),
			 foamNewLabel(10),
			 NULL);
	i = -1;
	i = foamSeqNextReachable(seq, i);
	testIntEqual("", 0, i);
	i = foamSeqNextReachable(seq, i);
	testIntEqual("", 1, i);
	i = foamSeqNextReachable(seq, i);
	testIntEqual("", 3, i);
	i = foamSeqNextReachable(seq, i);
	testIntEqual("", -1, i);
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
	testIntEqual("FR_INT", 850477418, hashCombinePair(hINT, hFRAC));

/*(HASH 1027140807 134808007 (|Mapping| $ $)) */
	testIntEqual("X -> X", 1021768245, hashCombinePair(134808007,
							  hashCombinePair(twist,
									  hashCombinePair(134808007, hMapping))));

/*(HASH 476114119 134808007 (|Mapping| $ $ $)) */
	testIntEqual("(X, X) -> X", 898414238,
		     hashCombinePair(134808007,
				 hashCombinePair(twist, hashCombinePair(134808007,
									hashCombinePair(134808007, hMapping)))));

/*(HASH 200862919 134808007 (|Mapping| $ (|Integer|) $)) */

	testIntEqual("T1", 972614544,
		     hashCombinePair(134808007,
				     hashCombinePair(twist, hashCombinePair(134808007,
									 hashCombinePair(hINT, hMapping)))));

/*(HASH 134808007 0 (|UnivariateLaurentSeries| (|Fraction| (|Integer|)) |z| (0 . 1))) */

	AInt hULS_FI = hashCombinePair(7, hashCombinePair(7,
							  hashCombinePair(hashCombinePair(hINT, hFRAC),
									  hULS)));
	testIntEqual("HULS_FI", 794083080, hULS_FI);

/*
(HASH 350552519 134808007
 (|UnivariateTaylorSeries| (|Fraction| (|Integer|)) |z| (0 . 1)))
*/
	AInt hUTS_FI = hashCombinePair(7, hashCombinePair(7,
							  hashCombinePair(hashCombinePair(hINT, hFRAC),
									  hUTS)));
	testIntEqual("HUTS_FI", 659312886, hUTS_FI);

/*(HASH 476114119 134808007
        (|Mapping| $ (|UnivariateTaylorSeries| (|Fraction| (|Integer|)) |z| (0 . 1)) $))
*/
	testIntEqual("(UTS_FI, X) -> X", 937065739,
		     hashCombinePair(hULS_FI,
				     hashCombinePair(twist, hashCombinePair(hULS_FI,
									    hashCombinePair(hUTS_FI, hMapping)))));
/* Clash: UTS_FI/ULS_FI */
	testIntEqual("(X, X) -> X", 898414238,
		     hashCombinePair(134808007,
				    hashCombinePair(twist, hashCombinePair(134808007,
									 hashCombinePair(134808007, hMapping)))));
}


local void
testSIntReduce()
{
	Foam foam, reduced;
	if (sizeof(AInt) < 8) {
		return;
	}
	foam = foamNewSInt(1L<<40);
	reduced = foamSIntReduce(foam);
	testFalse("t0", foam == reduced);

	foam = foamNewSInt(-(1L<<40));
	reduced = foamSIntReduce(foam);
	testFalse("t0", foam == reduced);
	testFalse("t0", foamEqual(foamSIntReduce(foamNewSInt(1L<<40)), reduced));
	testTrue("t1", foamTag(reduced) == FOAM_BCall && reduced->foamBCall.op == FOAM_BVal_SIntNegate);

	/* Really need a working foam interpreter to test this properly */
	/* .. probably easier to do as library tests */
}

local Buffer tFoamToBuffer(Foam foam);
local void
testFoamBuffer()
{
	Foam foam1, foam2;
	Buffer buf1, buf2;

	foam1 = foamNewSInt(23);
	foam2 = foamNewSInt(24);

	testTrue("t1", foamVerifyBuffer(tFoamToBuffer(foam1), foam1));
	testFalse("t2", foamVerifyBuffer(tFoamToBuffer(foam2), foam1));
}

local Buffer
tFoamToBuffer(Foam foam)
{
	Buffer buf = bufNew();
	foamToBuffer(buf, foam);

	return buf;
}
