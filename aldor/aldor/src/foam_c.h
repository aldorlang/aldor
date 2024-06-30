/*
 * foam_c.h: Interface for C implementation of abstract machine.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 */

/* Fi is an acronym for Foam Implementation */
#ifndef FOAM_C_H
#define FOAM_C_H

/*
 *  2006/Dec/20
 *
 *  Including cconfig.h became necessary in order to be able to test
 *  CC_SF_is_double and typedef FiSFlo correctly
 */
#include "cconfig.h"

typedef char			*Ptr;
typedef long			*LPtr;

typedef Ptr			FiNil;
typedef unsigned long int	FiWord;
typedef long int		FiArb;
typedef Ptr			FiPtr;
typedef long			FiBool; /* Fortran-77 compatability */
typedef unsigned char		FiByte;
typedef short			FiHInt;
typedef long			FiSInt;
typedef unsigned char		FiChar;
typedef Ptr			FiArr;
typedef Ptr			FiRec;
typedef Ptr			FiRRec;
typedef Ptr			FiTR;
typedef Ptr			FiBInt;
#ifdef CC_SF_is_double
  typedef double		FiSFlo;
#else
  typedef float			FiSFlo;
#endif
typedef double			FiDFlo;
typedef Ptr			(*FiFun)();

/* When adding FiInt8 etc, please implement fiArrNew_Int8 too! */

typedef unsigned short		FiBIntS; /* For fiBIntFrPlacev */

/* If you change this then change StoFiProg as well */
typedef struct _FiProg {
	FiFun	fun;
	FiFun	fcall;
	FiWord	progInfo;
	FiWord	data;
} *FiProg;

typedef struct _FiEnv {
	Ptr	level;
	struct _FiEnv	*next;
	FiWord	info;
} *FiEnv;

/* If you change this then change StoFiClos as well */
typedef struct _FiClos {
	FiEnv	env;
	FiProg	prog;
} *FiClos;

#ifdef CC_SF_is_double
typedef struct _FiComplexSF {
	double	real;
	double	imag;
} FiComplexSF;
#else
typedef struct _FiComplexSF {
	float	real;
	float	imag;
} FiComplexSF;
#endif

typedef struct _FiComplexDF {
	double	real;
	double	imag;
} FiComplexDF;


typedef struct _FiList {
	FiWord		data;
	struct _FiList	*next;
} FiList;


enum fiDbgTag
{
	FI_DBG_START,

		FI_DBG_Init = FI_DBG_START,
		FI_DBG_Enter,
		FI_DBG_Inside,
		FI_DBG_Return,
		FI_DBG_Exit,
		FI_DBG_Assign,
		FI_DBG_Throw,
		FI_DBG_Catch,
		FI_DBG_Step,
		FI_DBG_Call,

	FI_DBG_LIMIT
};

typedef enum fiDbgTag	FiDbgTag;

#define		fiCProg(cl) 		((cl)->prog)
#define		fiRawCProg(cl)		((cl)->prog->fun)
#define		fiCFun(t, fn)		(*((t (*)())(fn)->prog->fun))
#define		fiProtoCFun(t, at, fn)	(*((t (*) at)(fn)->prog->fun))
#define		fiCFcall(cl)		((cl)->prog->fcall)
#define		fiCProgInfo(cl)		((cl)->prog->progInfo)
#define		fiCData(cl)		((cl)->prog->data)

#define		fiCCall0(t,fn)		(fiCFun(t,fn)((fn)->env))
#define		fiCCall1(t,fn,a)	(fiCFun(t,fn)((fn)->env,a))
#define		fiCCall2(t,fn,a,b)	(fiCFun(t,fn)((fn)->env,a,b))
#define		fiCCall3(t,fn,a,b,c)	(fiCFun(t,fn)((fn)->env,a,b,c))
#define		fiCCall4(t,fn,a,b,c,d)	(fiCFun(t,fn)((fn)->env,a,b,c,d))
#define		fiCCall5(t,fn,a,b,c,d,e) (fiCFun(t,fn)((fn)->env,a,b,c,d,e))

#define		fiEnvLevel(e)		((FiEnv) (e)->level)
#define		fiEnvNext(e)		((FiEnv) (e)->next)
#define		fiEnvPush(a,e)		fiEnvPushFun((FiPtr) (a), e)
#define		fiEnvInfo(e)		(((FiEnv) (e))->info)
extern FiEnv	fiEnvPushFun		(FiPtr, FiEnv);

#define		fiEnvEnsure(e)		fiEnvEnsureFun(e)
extern void	fiEnvEnsureFun		(FiEnv);

#define		fiEnv0(e)		((FiEnv)(e))
#define		fiEnv1(e)		(fiEnv0(e)->next)
#define		fiEnv2(e)		(fiEnv1(e)->next)
#define		fiEnv3(e)		(fiEnv2(e)->next)
#define		fiEnv4(e)		(fiEnv3(e)->next)
#define		fiEnv5(e)		(fiEnv4(e)->next)

#define		fiEnvLevel0(e)		(fiEnv0(e)->level)
#define		fiEnvLevel1(e)		(fiEnv1(e)->level)
#define		fiEnvLevel2(e)		(fiEnv2(e)->level)
#define		fiEnvLevel3(e)		(fiEnv3(e)->level)
#define		fiEnvLevel4(e)		(fiEnv4(e)->level)
#define		fiEnvLevel5(e)		(fiEnv5(e)->level)

#define		fiNew(T)		((T *) fiAlloc(sizeof(T)))
#define		fi0New(T,O)		((T *) fi0Alloc(sizeof(T), O))
#define		fi0RecNew(T,O)		((T *) fi0RecAlloc(sizeof(T), O))

#define		fiClosMake(e,p)		fiClosMakeFun(e, (FiProg) (p))
extern FiClos	fiClosMakeFun		(FiEnv, FiProg);

#define fiProgHashCode(prog)	((prog)->data)

/*****************************************************************************
 *
 * :: Sundry machine-specific optimisations
 *
 ****************************************************************************/

#include "optcfg.h"
#include "foamopt.h"

/*****************************************************************************
 *
 * :: Hack for experimental store census taking
 *
 ****************************************************************************/

#define CENSUS_Unknown		( 0) /* Must be the same as OB_Other */
#define CENSUS_Bogus		( 1) /* Must be the same as OB_Bogus */
#define CENSUS_BInt		( 2) /* Must be the same as OB_BInt */
#define CENSUS_DFlo		( 3)
#define CENSUS_Clos		( 4)
#define CENSUS_Rec		( 5)
#define CENSUS_RRec		( 6)
#define CENSUS_RRFmt		( 7)
#define CENSUS_CharArray	( 8)
#define CENSUS_BoolArray	( 9)
#define CENSUS_ByteArray	(10)
#define CENSUS_HIntArray	(11)
#define CENSUS_SIntArray	(12)
#define CENSUS_SFloArray	(13)
#define CENSUS_DFloArray	(14)
#define CENSUS_WordArray	(15)
#define CENSUS_PtrArray		(16) /* Includes NilArray, ArbArray etc */
#define CENSUS_TrailingArray	(17)
#define CENSUS_EnvInfo		(18)
#define CENSUS_EnvLevel		(19)
#define CENSUS_Fluid		(20)
#define CENSUS_GlobalInfo	(21)
#define CENSUS_SaveState	(22)
/*      CENSUS_Limit		(31) [OB_Max] */

/*****************************************************************************
 *
 * :: Helpers for TrailingArrays
 *
 ****************************************************************************/

#define fiNARY	5
#define fiNARYNew(type, itype, n) 					\
	((type *)fi0Alloc(						\
		sizeof(type)+(n)*sizeof(itype)-fiNARY*sizeof(itype),	\
		CENSUS_TrailingArray))

/*****************************************************************************
 *
 * :: FOAM operations -- expression versions
 *
 ****************************************************************************/

extern double	*fiAlloc	(unsigned long);
extern double	*fi0Alloc	(unsigned long, int);
extern double	*fi0RecAlloc	(unsigned long, int);
extern void	fiFree		(Ptr);
extern FiChar	*fiArrNew_Char	(unsigned long);
extern FiBool	*fiArrNew_Bool	(unsigned long);
extern FiByte	*fiArrNew_Byte	(unsigned long);
extern FiHInt	*fiArrNew_HInt	(unsigned long);
extern FiSInt	*fiArrNew_SInt	(unsigned long);
extern FiSFlo	*fiArrNew_SFlo	(unsigned long);
extern FiDFlo	*fiArrNew_DFlo	(unsigned long);
extern FiWord	*fiArrNew_Word	(unsigned long);
extern FiPtr	*fiArrNew_Ptr	(unsigned long);
extern FiNil	*fiArrNew_Nil	(unsigned long);
extern FiRec	*fiArrNew_Rec	(unsigned long);
extern FiArr	*fiArrNew_Arr	(unsigned long);
extern FiArb	*fiArrNew_Arb	(unsigned long);
extern FiRRec	*fiArrNew_RRec	(unsigned long);
extern FiTR	*fiArrNew_TR	(unsigned long);
extern FiBInt	*fiArrNew_BInt	(unsigned long);
extern FiClos	*fiArrNew_Clos	(unsigned long);

#define		fiNil		((Ptr) 0)
#define		fiPtrIsNil(p)	((p) == 0)
#define		fiPtrMagicEQ(p,q) ((p) == (q))

extern FiChar	fiCharMin	(void);
extern FiChar	fiCharMax	(void);
extern FiBool	fiCharIsDigit	(FiChar);
extern FiBool	fiCharIsLetter	(FiChar);
extern FiChar	fiCharLower	(FiChar);
extern FiChar	fiCharUpper	(FiChar);


extern FiSFlo	fiSFloMin	(void);
extern FiSFlo	fiSFloMax	(void);
extern FiSFlo	fiSFloEpsilon	(void);
extern FiSFlo	fiSFloPrev	(FiSFlo);
extern FiSFlo	fiSFloNext	(FiSFlo);
#define		fiSFloTimesPlus(a,b,c)	((a)*(b)+(c))
extern FiSFlo	fiSFloRPlus	(FiSFlo, FiSFlo, FiSInt);
extern FiSFlo	fiSFloRMinus	(FiSFlo, FiSFlo, FiSInt);
extern FiSFlo	fiSFloRTimes	(FiSFlo, FiSFlo, FiSInt);
extern FiSFlo	fiSFloRTimesPlus(FiSFlo,FiSFlo, FiSFlo, FiSInt);
extern FiSFlo	fiSFloRDivide	(FiSFlo, FiSFlo, FiSInt);
extern FiBInt	fiSFloTruncate	(FiSFlo);
extern FiSFlo	fiSFloFraction	(FiSFlo);
extern FiBInt	fiSFloRound	(FiSFlo, FiSInt);
extern void	fiSFloDissemble	(FiSFlo, FiBool *, FiSInt *, FiWord *);
extern FiSFlo	fiSFloAssemble	(FiBool, FiSInt, FiWord);
extern FiBInt	fiSFloExponent	(FiSFlo);
extern FiBInt	fiSFloMantissa	(FiSFlo);

extern FiDFlo	fiDFloMin	(void);
extern FiDFlo	fiDFloMax	(void);
extern FiDFlo	fiDFloEpsilon	(void);
extern FiDFlo	fiDFloPrev	(FiDFlo);
extern FiDFlo	fiDFloNext	(FiDFlo);
#define		fiDFloTimesPlus(a,b,c)	((a)*(b)+(c))
extern FiDFlo	fiDFloRPlus	(FiDFlo, FiDFlo, FiSInt);
extern FiDFlo	fiDFloRMinus	(FiDFlo, FiDFlo, FiSInt);
extern FiDFlo	fiDFloRTimes	(FiDFlo, FiDFlo, FiSInt);
extern FiDFlo	fiDFloRTimesPlus(FiDFlo,FiDFlo, FiDFlo, FiSInt);
extern FiDFlo	fiDFloRDivide	(FiDFlo, FiDFlo, FiSInt);
extern FiBInt	fiDFloTruncate	(FiDFlo);
extern FiDFlo	fiDFloFraction	(FiDFlo);
extern FiBInt	fiDFloRound	(FiDFlo, FiSInt);
extern void	fiDFloDissemble	(FiDFlo,FiBool*,FiSInt*,FiWord*,FiWord*);
extern FiDFlo	fiDFloAssemble	(FiBool, FiSInt, FiWord, FiWord);
extern FiBInt	fiDFloExponent	(FiDFlo);
extern FiBInt	fiDFloMantissa	(FiDFlo);

extern FiByte	fiByteMin	(void);
extern FiByte	fiByteMax	(void);

extern FiHInt	fiHIntMin	(void);
extern FiHInt	fiHIntMax	(void);

extern FiSInt	fiSIntMin	(void);
extern FiSInt	fiSIntMax	(void);
#define		fiSIntTimesPlus(a,b,c) ((a)*(b)+(c))
#define		fiSIntQuo(a,b)  ((a)/(b))
#define		fiSIntRem(a,b)  ((a)%(b))
extern void	fiSIntDivide	(FiSInt, FiSInt, FiSInt *, FiSInt *);
extern FiSInt	fiSIntGcd	(FiSInt, FiSInt);
extern FiSInt	fiSIntTimesMod	(FiSInt, FiSInt, FiSInt);
extern FiSInt	fiSIntTimesModInv(FiSInt, FiSInt, FiSInt, FiDFlo);
extern FiSInt	fiSIntLength	(FiSInt);
#define		fiSIntBit(n,i)	(!!((n) & (1L << (i))))

#ifndef OPT_NoDoubleOps

extern void	fiWordTimesDouble	(FiWord, FiWord,
					 FiWord *, FiWord *);
extern void	fiWordDivideDouble	(FiWord, FiWord, FiWord,
					 FiWord *, FiWord *, FiWord *);
#endif


extern void	fiWordPlusStep	(FiWord, FiWord, FiWord,
				 FiWord *, FiWord *);
extern void	fiWordTimesStep	(FiWord, FiWord, FiWord, FiWord,
				 FiWord *, FiWord *);
extern FiBInt	fiBIntFrPlacev	(int, unsigned long, FiBIntS *);
extern FiBInt	fiBIntFrInt	(long);
extern FiBInt	fiBInt0		(void);
extern FiBInt	fiBInt1		(void);
extern FiBInt	fiBIntNew	(long);
extern FiBool	fiBIntIsSingle	(FiBInt);
extern FiBool	fiBIntIsZero	(FiBInt);
extern FiBool	fiBIntIsNeg	(FiBInt);
extern FiBool	fiBIntIsPos	(FiBInt);
extern FiBool	fiBIntIsEven	(FiBInt);
extern FiBool	fiBIntIsOdd	(FiBInt);
extern FiBool	fiBIntEQ	(FiBInt, FiBInt);
extern FiBool	fiBIntNE	(FiBInt, FiBInt);
extern FiBool	fiBIntLT	(FiBInt, FiBInt);
extern FiBool	fiBIntLE	(FiBInt, FiBInt);
extern FiBInt	fiBIntNegate	(FiBInt);
extern FiBInt	fiBIntPlus	(FiBInt, FiBInt);
extern FiBInt	fiBIntMinus	(FiBInt, FiBInt);
extern FiBInt	fiBIntTimes	(FiBInt, FiBInt);
extern FiBInt	fiBIntTimesPlus	(FiBInt, FiBInt, FiBInt);
extern FiSInt	fiBIntLength	(FiBInt);
extern FiBInt	fiBIntShiftUp	(FiBInt, FiSInt);
extern FiBInt	fiBIntShiftDn	(FiBInt, FiSInt);
extern FiBInt	fiBIntShiftRem	(FiBInt, FiSInt);
extern FiBool	fiBIntBit	(FiBInt, FiSInt);
extern FiBInt	fiBIntMod	(FiBInt, FiBInt);
extern FiBInt	fiBIntQuo	(FiBInt, FiBInt);
extern FiBInt	fiBIntRem	(FiBInt, FiBInt);
extern void	fiBIntDivide	(FiBInt, FiBInt, FiBInt *, FiBInt *);
extern FiBInt	fiBIntGcd	(FiBInt, FiBInt);
extern FiBInt	fiBIntSIPower	(FiBInt, FiSInt);
extern FiBInt	fiBIntBIPower	(FiBInt, FiBInt);
extern FiBInt	fiBIntPowerMod  (FiBInt, FiBInt, FiBInt);
extern void	fiBIntFree	(FiBInt);
extern char *	fiBIntToString	(FiBInt);
extern void	fiBIntInit	(void);

#define		fiSFloToDFlo(v)	((FiDFlo)(v))
#define		fiDFloToSFlo(v)	((FiSFlo)(v))
#define		fiByteToSInt(v)	((FiSInt)(v))
#define		fiSIntToByte(v)	((FiByte)(v))
#define		fiHIntToSInt(v)	((FiSInt)(v))
#define		fiSIntToHInt(v)	((FiHInt)(v))
extern FiBInt	fiSIntToBInt	(FiSInt);
extern FiSInt	fiBIntToSInt	(FiBInt);
#define		fiSIntToSFlo(v)	((FiSFlo)(v))
#define		fiSIntToDFlo(v)	((FiDFlo)(v))
extern FiSFlo	fiBIntToSFlo	(FiBInt);
extern FiDFlo	fiBIntToDFlo	(FiBInt);
#define		fiPtrToSInt(v)	((FiSInt)(v))
#define		fiSIntToPtr(v)	((FiPtr) (v))
#define		fiPtrNil	((FiPtr) 0)

extern FiSInt	fiFormatSFlo	(FiSFlo, FiArr, FiSInt);
extern FiSInt	fiFormatDFlo	(FiDFlo, FiArr, FiSInt);
extern FiSInt	fiFormatSInt	(FiSInt, FiArr, FiSInt);
extern FiSInt	fiFormatBInt	(FiBInt, FiArr, FiSInt);

extern void	fiScanSFlo	(FiArr, FiSInt, FiSFlo *, FiSInt *);
extern void	fiScanDFlo	(FiArr, FiSInt, FiDFlo *, FiSInt *);
extern void	fiScanSInt	(FiArr, FiSInt, FiSInt *, FiSInt *);
extern void	fiScanBInt	(FiArr, FiSInt, FiBInt *, FiSInt *);

extern FiSFlo	fiArrToSFlo	(FiArr);
extern FiDFlo	fiArrToDFlo	(FiArr);
extern FiSInt	fiArrToSInt	(FiArr);
extern FiBInt	fiArrToBInt	(FiArr);
extern FiWord	fiScanSmallIntFrString	(char *, FiWord, FiWord);

extern FiWord	fiPlatformRTE	(void);
extern FiWord	fiPlatformOS	(void);

extern FiWord	fiHalt		(FiSInt);	/* Does not return. */

extern void	fiSetDebugVar	(FiWord);
extern FiWord	fiGetDebugVar	(void);
extern void	fiSetDebugger	(FiDbgTag event, FiWord v);
extern FiWord	fiGetDebugger	(FiDbgTag event);

extern FiWord	fiStrHash	(FiWord);

extern FiPtr	fiListNil	(void);
extern FiBool	fiListEmptyP	(FiPtr);
extern FiWord	fiListHead	(FiPtr);
extern FiPtr	fiListTail	(FiPtr);
extern FiPtr	fiListCons	(FiWord, FiPtr);

/*****************************************************************************
 *
 * :: FOAM operations -- statement versions
 *
 ****************************************************************************/

#define fi_ALLOC(r,t,a)			((r) = (t) fiAlloc((unsigned long) (a)))
#define fiARRNEW_Char(r,t,i)		((r) = (t) fiArrNew_Char(i))
#define fiARRNEW_Bool(r,t,i)		((r) = (t) fiArrNew_Bool(i))
#define fiARRNEW_Byte(r,t,i)		((r) = (t) fiArrNew_Byte(i))
#define fiARRNEW_HInt(r,t,i)		((r) = (t) fiArrNew_HInt(i))
#define fiARRNEW_SInt(r,t,i)		((r) = (t) fiArrNew_SInt(i))
#define fiARRNEW_SFlo(r,t,i)		((r) = (t) fiArrNew_SFlo(i))
#define fiARRNEW_DFlo(r,t,i)		((r) = (t) fiArrNew_DFlo(i))
#define fiARRNEW_Word(r,t,i)		((r) = (t) fiArrNew_Word(i))
#define fiARRNEW_Ptr(r,t,i)		((r) = (t) fiArrNew_Ptr(i))
#define fiARRNEW_Nil(r,t,i)		((r) = (t) fiArrNew_Nil(i))
#define fiARRNEW_Rec(r,t,i)		((r) = (t) fiArrNew_Rec(i))
#define fiARRNEW_Arr(r,t,i)		((r) = (t) fiArrNew_Arr(i))
#define fiARRNEW_Arb(r,t,i)		((r) = (t) fiArrNew_Arb(i))
#define fiARRNEW_RRec(r,t,i)		((r) = (t) fiArrNew_RRec(i))
#define fiARRNEW_TR(r,t,i)		((r) = (t) fiArrNew_TR(i))
#define fiARRNEW_BInt(r,t,i)		((r) = (t) fiArrNew_BInt(i))
#define fiARRNEW_Clos(r,t,i)		((r) = (t) fiArrNew_Clos(i))

#define	fiPTR_IS_NIL(r,t,p)		((r) = (t)((FiPtr) (p) == (FiPtr) 0))
#define	fiPTR_MAGIC_EQ(r,t,p,q)		((r) = (t)((FiPtr) (p) == (FiPtr) (q)))

#define fiCHAR_MIN(r,t)			((r) = (t) fiCharMin())
#define fiCHAR_MAX(r,t)			((r) = (t) fiCharMax())
#define fiCHAR_IS_DIGIT(r,t,c)		((r) = (t) fiCharIsDigit(c))
#define fiCHAR_IS_LETTER(r,t,c)		((r) = (t) fiCharIsLetter(c))
#define fiCHAR_LOWER(r,t,c)		((r) = (t) fiCharLower(c))
#define fiCHAR_UPPER(r,t,c)		((r) = (t) fiCharUpper(c))

#define fiSFLO_MIN(r,t)			((r) = (t) fiSFloMin())
#define fiSFLO_MAX(r,t)			((r) = (t) fiSFloMax())
#define fiSFLO_EPSILON(r,t)		((r) = (t) fiSFloEpsilon())
#define fiSFLO_PREV(r,t,a)		((r) = (t) fiSFloPrev(a))
#define fiSFLO_NEXT(r,t,a)		((r) = (t) fiSFloNext(a))
#define fiSFLO_TIMES_PLUS(r,t,a,b,c)	((r) = (t) fiSFloTimesPlus(a,b,c))
#define fiSFLO_R_PLUS(r,t,a,b,d)	((r) = (t) fiSFloRPlus(a,b,d))
#define fiSFLO_R_MINUS(r,t,a,b,d)	((r) = (t) fiSFloRMinus(a,b,d))
#define fiSFLO_R_TIMES(r,t,a,b,d)	((r) = (t) fiSFloRTimes(a,b,d))
#define fiSFLO_R_TIMES_PLUS(r,t,a,b,c,d)((r) = (t) fiSFloRTimesPlus(a,b,c,d))
#define fiSFLO_R_DIVIDE(r,t,a,b,d)	((r) = (t) fiSFloRDivide(a,b,d))

#define fiWORD_FR_SFLO(r,a)		{ FiSFlo fi_tmp = (a); (r) = *(FiWord *)(&fi_tmp); }
#define fiSFLO_FR_WORD(r,a)		{ FiWord fi_tmp = (a); (r) = *(FiSFlo *)(&fi_tmp);}

#define fiDFLO_MIN(r,t)			((r) = (t) fiDFloMin())
#define fiDFLO_MAX(r,t)			((r) = (t) fiDFloMax())
#define fiDFLO_EPSILON(r,t)		((r) = (t) fiDFloEpsilon())
#define fiDFLO_PREV(r,t,a)		((r) = (t) fiDFloPrev(a))
#define fiDFLO_NEXT(r,t,a)		((r) = (t) fiDFloNext(a))
#define fiDFLO_TIMES_PLUS(r,t,a,b,c)	((r) = (t) fiDFloTimesPlus(a,b,c))
#define fiDFLO_R_PLUS(r,t,a,b,d)	((r) = (t) fiDFloRPlus(a,b,d))
#define fiDFLO_R_MINUS(r,t,a,b,d)	((r) = (t) fiDFloRMinus(a,b,d))
#define fiDFLO_R_TIMES(r,t,a,b,d)	((r) = (t) fiDFloRTimes(a,b,d))
#define fiDFLO_R_TIMES_PLUS(r,t,a,b,c,d)((r) = (t) fiDFloRTimesPlus(a,b,c,d))
#define fiDFLO_R_DIVIDE(r,t,a,b,d)	((r) = (t) fiDFloRDivide(a,b,d))

#define fiBYTE_MIN(r,t)			((r) = (t) fiByteMin())
#define fiBYTE_MAX(r,t)			((r) = (t) fiByteMax())

#define fiHINT_MIN(r,t)			((r) = (t) fiHIntMin())
#define fiHINT_MAX(r,t)			((r) = (t) fiHIntMax())

#define fiSINT_MIN(r,t)			((r) = (t) fiSIntMin())
#define fiSINT_MAX(r,t)			((r) = (t) fiSIntMax())
#define fiSINT_TIMES_PLUS(r,t,a,b,c)	((r) = (t) fiSIntTimesPlus(a,b,c))
#define fiSINT_QUO(r,t,a,b)		((r) = (t) fiSIntQuo(a,b))
#define fiSINT_REM(r,t,a,b)		((r) = (t) fiSIntRem(a,b))
#define fiSINT_GCD(r,t,a,b)		((r) = (t) fiSIntGcd(a,b))
#define fiSINT_LENGTH(r,t,si)		((r) = (t) fiSIntLength(si))
#define fiSINT_BIT(r,t,a,b)		((r) = (t) fiSIntBit(a,b))
#define fiSINT_TIMES_MOD(r,t,a,b,n)	((r) = (t) fiSIntTimesMod(a,b,n))
#define fiSINT_TIMES_MOD_INV(r,t,a,b,n,ni)\
					((r) = (t) fiSIntTimesModInv(a,b,n,ni))

#define fiBINT_FR_PLACEV(r,t,b,l,s)	((r) = (t) fiBIntFrPlacev(b,l,s))
#define fiBINT_FR_INT(r,t,i)		((r) = (t) fiBIntFrInt(i))
#define fiBINT_0(r,t)			((r) = (t) fiBInt0())
#define fiBINT_1(r,t)			((r) = (t) fiBInt1())
#define fiBINT_NEW(r,t,i)		((r) = (t) fiBIntNew(i))
#define fiBINT_IS_ZERO(r,t,b)		((r) = (t) fiBIntIsZero(b))
#define fiBINT_IS_NEG(r,t,b)		((r) = (t) fiBIntIsNeg(b))
#define fiBINT_IS_POS(r,t,b)		((r) = (t) fiBIntIsPos(b))
#define fiBINT_IS_EVEN(r,t,b)		((r) = (t) !fiBIntBit(b, (FiSInt) 0))
#define fiBINT_IS_ODD(r,t,b)		((r) = (t) fiBIntBit(b, (FiSInt) 0))
#define fiBINT_IS_SINGLE(r,t,b)		((r) = (t) fiBIntIsSingle(b))
#define fiBINT_LE(r,t,a,b)		((r) = (t) fiBIntLE(a,b))
#define fiBINT_EQ(r,t,a,b)		((r) = (t) fiBIntEQ(a,b))
#define fiBINT_LT(r,t,a,b)		((r) = (t) fiBIntLT(a,b))
#define fiBINT_NE(r,t,a,b)		((r) = (t) fiBIntNE(a,b))
#define fiBINT_NEGATE(r,t,b)		((r) = (t) fiBIntNegate(b))
#define fiBINT_PLUS1(r,t,a)		fiBINT_PLUS(r, t, a, fiBInt1())
#define fiBINT_MINUS1(r,t,a)		fiBINT_MINUS(r, t, a, fiBInt1())
#define fiBINT_PLUS(r,t,a,b)		((r) = (t) fiBIntPlus(a,b))
#define fiBINT_MINUS(r,t,a,b)		((r) = (t) fiBIntMinus(a,b))
#define fiBINT_TIMES(r,t,a,b)		((r) = (t) fiBIntTimes(a,b))
#define fiBINT_TIMES_PLUS(r,t,a,b,c)	((r) = (t) fiBIntTimesPlus(a,b,c))
#define fiBINT_LENGTH(r,t,b)		((r) = (t) fiBIntLength(b))
#define fiBINT_SHIFT_UP(r,t,a,i)	((r) = (t) fiBIntShiftUp(a,i))
#define fiBINT_SHIFT_DN(r,t,a,i)	((r) = (t) fiBIntShiftDn(a,i))
#define fiBINT_BIT(r,t,a,i)		((r) = (t) fiBIntBit(a,i))
#define fiBINT_MOD(r,t,a,b)		((r) = (t) fiBIntMod(a,b))
#define fiBINT_QUO(r,t,a,b)		((r) = (t) fiBIntQuo(a,b))
#define fiBINT_REM(r,t,a,b)		((r) = (t) fiBIntRem(a,b))
#define fiBINT_GCD(r,t,a,b)		((r) = (t) fiBIntGcd(a,b))
#define fiBINT_SI_POWER(r,t,a,b)	((r) = (t) fiBIntSIPower(a,b))
#define fiBINT_BI_POWER(r,t,a,b)	((r) = (t) fiBIntBIPower(a,b))
#define fiBINT_POWER_MOD(r,t,a,b,c)	((r) = (t) fiBIntPowerMod(a,b,c))

#define fiSFLO_TO_DFLO(r,t,l)		((r) = (t) fiSFloToDFlo(l))
#define fiDFLO_TO_SFLO(r,t,l)		((r) = (t) fiDFloToSFlo(l))
#define fiBYTE_TO_SINT(r,t,l)		((r) = (t) fiByteToSInt(l))
#define fiSINT_TO_BYTE(r,t,l)		((r) = (t) fiSIntToByte(l))
#define fiHINT_TO_SINT(r,t,l)		((r) = (t) fiHIntToSInt(l))
#define fiSINT_TO_HINT(r,t,l)		((r) = (t) fiSIntToHInt(l))
#define fiSINT_TO_BINT(r,t,l)		((r) = (t) fiSIntToBInt(l))
#define fiBINT_TO_SINT(r,t,l)		((r) = (t) fiBIntToSInt(l))
#define fiSINT_TO_SFLO(r,t,l)		((r) = (t) fiSIntToSFlo(l))
#define fiSINT_TO_DFLO(r,t,l)		((r) = (t) fiSIntToDFlo(l))
#define fiBINT_TO_SFLO(r,t,l)		((r) = (t) fiBIntToSFlo(l))
#define fiBINT_TO_DFLO(r,t,l)		((r) = (t) fiBIntToDFlo(l))
#define fiPTR_TO_SINT(r,t,l)		((r) = (t) fiPtrToSInt(l))
#define fiSINT_TO_PTR(r,t,l)		((r) = (t) fiSIntToPtr(l))

#define fiSINT_HASHCOMBINE(r, t, a, b)	((r) = (t) fiSIntHashCombine(a, b))

#define fiFORMAT_SFLO(r,t,s,w,si)	((r) = (t) fiFormatSFlo(s,(FiArr)w,si))
#define fiFORMAT_DFLO(r,t,d,w,si)	((r) = (t) fiFormatDFlo(d,(FiArr)w,si))
#define fiFORMAT_SINT(r,t,a,w,b)	((r) = (t) fiFormatSInt(a,(FiArr)w,b))
#define fiFORMAT_BINT(r,t,b,w,si)	((r) = (t) fiFormatBInt(b,(FiArr)w,si))

#define	fiARR_TO_SFLO(r,t,v)		((r) = (t) fiArrToSFlo((FiArr)v))
#define	fiARR_TO_DFLO(r,t,v)		((r) = (t) fiArrToDFlo((FiArr)v))
#define	fiARR_TO_SINT(r,t,v)		((r) = (t) fiArrToSInt((FiArr)v))
#define	fiARR_TO_BINT(r,t,v)		((r) = (t) fiArrToBInt((FiArr)v))

#define fiHALT(r,t,i)			((r) = (t) fiHalt(i))

#define fiLIST_NIL(r,t)			((r) = (t) ((FiPtr)0))
#define fiLIST_EMPTYP(r,t,l)		((r) = (t) (l))
#define fiLIST_HEAD(r,t,l)		((r) = (t) (((FiList *)l)->data))
#define fiLIST_TAIL(r,t,l)		((r) = (t) ((((FiList *)l)->next)))

extern FiSInt	fiSIntHashCombine	(FiSInt h1, FiSInt h2);

/******************************************************************************
 *
 * :: [S|D]Float Rounding Directions.
 *
 *****************************************************************************/

#define fiRoundZero()		0
#define fiRoundNearest()	1
#define fiRoundUp()		2
#define fiRoundDown()		3
#define fiRoundDontCare()	4

#define FI_ROUND_ZERO(x,t)	((x) = (t) fiRoundZero())
#define FI_ROUND_NEAREST(x,t)	((x) = (t) fiRoundNearest())
#define FI_ROUND_UP(x,t)	((x) = (t) fiRoundUp())
#define FI_ROUND_DOWN(x,t)	((x) = (t) fiRoundDown())
#define FI_ROUND_DONT_CARE(x,t)	((x) = (t) fiRoundDontCare())

#define fiSFLO_TRUNC(x)		fiSFloTruncate(x)
#define fiSFLO_ROUND_DOWN(x)	fiSFloTruncate((x) < 0 && fiSFloFraction(x) != 0 ? (x) - (FiSFlo)1.0 : (x))
#define fiSFLO_ROUND_UP(x)	fiSFloTruncate((x) > 0 && fiSFloFraction(x) != 0 ? (x) + (FiSFlo)1.0 : (x))
#define fiSFLO_ROUND_ZERO(x)	fiSFloTruncate(x)
#define fiSFLO_ROUND_NEAREST(x)	fiSFloNearest(x)
#define fiSFLO_ROUND(x)		fiSFloTruncate(x)

#define fiDFLO_TRUNC(x)		fiDFloTruncate(x)
#define fiDFLO_ROUND_DOWN(x)	fiDFloTruncate((x) < 0 && fiDFloFraction(x) != 0 ? (x) - 1.0 : (x))
#define fiDFLO_ROUND_UP(x)	fiDFloTruncate((x) > 0 && fiDFloFraction(x) != 0 ? (x) + 1.0 : (x))
#define fiDFLO_ROUND_ZERO(x)	fiDFloTruncate(x)
#define fiDFLO_ROUND_NEAREST(x)	fiDFloNearest(x)
#define fiDFLO_ROUND(x)		fiDFloTruncate(x)

/* Here lieth the ieee754 stuff */
extern void	fiInitialiseFpu			(void);
extern FiWord	fiIeeeGetRoundingMode		(void);
extern FiWord	fiIeeeSetRoundingMode		(FiWord s);
extern FiWord	fiIeeeGetEnabledExceptions	(void);
extern FiWord	fiIeeeSetEnabledExceptions	(FiWord s);
extern FiWord	fiIeeeGetExceptionStatus	(void);
extern FiWord	fiIeeeSetExceptionStatus	(FiWord s);
extern FiWord	fiDoubleHexPrintToString	(FiWord f);

/******************************************************************************
 *
 * :: Raw-record type tags.
 *
 *****************************************************************************/

#define fiTypeInt8()		(-1)
#define fiTypeInt16()		(-2)
#define fiTypeInt32()		(-3)
#define fiTypeInt64()		(-4)
#define fiTypeInt128()		(-5)

#define fiTypeNil()		( 0)
#define fiTypeChar()		( 1)
#define fiTypeBool()		( 2)
#define fiTypeByte()		( 3)
#define fiTypeHInt()		( 4)
#define fiTypeSInt()		( 5)
#define fiTypeBInt()		( 6)
#define fiTypeSFlo()		( 7)
#define fiTypeDFlo()		( 8)
#define fiTypeWord()		( 9)
#define fiTypeClos()		(10)
#define fiTypePtr()		(11)
#define fiTypeRec()		(12)
#define fiTypeArr()		(14)
#define fiTypeTR()		(15)

/* These 5 are waiting until we implement the data-types */
#define fiSizeOfInt8()		1
#define fiSizeOfInt16()		2
#define fiSizeOfInt32()		4
#define fiSizeOfInt64()		8
#define fiSizeOfInt128()	16

#define fiSizeOfNil()		sizeof(FiNil)
#define fiSizeOfChar()		sizeof(FiChar)
#define fiSizeOfBool()		sizeof(FiBool)
#define fiSizeOfByte()		sizeof(FiByte)
#define fiSizeOfHInt()		sizeof(FiHInt)
#define fiSizeOfSInt()		sizeof(FiSInt)
#define fiSizeOfBInt()		sizeof(FiBInt)
#define fiSizeOfSFlo()		sizeof(FiSFlo)
#define fiSizeOfDFlo()		sizeof(FiDFlo)
#define fiSizeOfWord()		sizeof(FiWord)
#define fiSizeOfClos()		sizeof(FiClos)
#define fiSizeOfPtr()		sizeof(FiPtr)
#define fiSizeOfRec()		sizeof(FiRec)
#define fiSizeOfArr()		sizeof(FiArr)
#define fiSizeOfTR()		sizeof(FiTR)

/******************************************************************************
 *
 * :: Fluids
 *
 *****************************************************************************/

typedef struct fiFluidStack {
	FiWord value;
	char *tag;	
	struct fiFluidStack *prev;
} *FiFluidStack;

typedef FiFluidStack FiFluid;

extern FiFluidStack fiGlobalFluidStack;

#define fiOpenFluidLevel() \
	FiFluidStack local = fiGlobalFluidStack;

#define fiCloseFluidLevel() \
	fiGlobalFluidStack = local;

extern FiFluid		fiAddFluid	(char *);
extern void		fiSetFluid	(FiFluid, FiWord);
extern void		fiKillFluid	(FiFluid);
extern FiWord		fiFluidValue	(FiFluid);

extern FiFluid		fiGetFluid	(char *);


/******************************************************************************
 *
 * :: Interface to the store management
 *
 *****************************************************************************/

/* Taken from store.h: move into axlobs.h or similar */
extern void	fiStoForceGC		(void);
extern FiBool	fiStoInHeap		(FiPtr);
extern FiSInt	fiStoIsWritable		(FiPtr);
extern FiSInt	fiStoMarkObject		(FiPtr);
extern FiWord	fiStoRecode		(FiPtr, FiSInt);
extern void	fiStoNewObject		(FiSInt, FiBool);
extern void	fiStoATracer		(FiSInt, FiClos);
extern void	fiStoCTracer		(FiSInt, FiWord);
extern void	fiStoShow		(FiSInt);
extern int	fiStoShowArgs		(FiPtr);


/******************************************************************************
 *
 * :: State Mangler...
 *
 *****************************************************************************/

#if 1

#include <setjmp.h>

typedef struct _FiState {
	FiWord 		tag;

	struct _FiState	*next;
	/* Temporary --- used while jumping */
	FiWord		target;
	FiWord		value;

	/* "State" */
	/* Could think about making this a list of foam closures.
	 *  This way, people could write their own dynamic scope
	 *  manipulators.  Rome wasn't burnt in a day...
	 */
	int		nStates;
	void		**states;
	jmp_buf		machineState;

} FiStateBox, *FiState, *FiStateChain;

extern FiStateChain	fiGlobStates;
extern void		fiJump			(FiWord tag);
extern void		fiRestoreState0		(FiState state);
extern void		fiSaveState0		(FiState state);
extern void		fiUnwind		(FiWord, FiWord);
extern void		fiUnhandledException	(FiWord);
extern void		fiRegisterStateFns	(void *(*)(), void (*)(void *));

#define fiRestoreState(x) fiRestoreState0(x)
#define fiSaveState(state) \
	(fiSaveState0(state), setjmp(state->machineState))

#define fiDeclareNewState(name) \
	FiStateBox frobnitz; \
	FiState name = &frobnitz;

#define fiProtect(res, expr, prot) \
{							\
	fiDeclareNewState(state);			\
/**/							\
	if (!fiSaveState(state)) {			\
		/* 1st time through */			\
		res = expr;				\
		fiRestoreState(state);			\
		prot;					\
	} 						\
	else { /* boing */				\
		fiRestoreState(state);			\
		prot;					\
		fiUnwind(state->target, state->value);	\
	}						\
}

#define fiBlock(ok, val, exn, expr)				\
{								\
	fiDeclareNewState(state);				\
								\
	if (!fiSaveState(state)) {				\
		/* 1st time through */				\
		val = expr;					\
		fiRestoreState(state);				\
		ok = 1;						\
	}							\
	else {	/* should look for additional protects	*/	\
		fiRestoreState(state);				\
		if (state->target != (FiWord) state) {		\
			fiUnwind(state->target, state->value);	\
		}						\
		exn = state->value;				\
		ok = 0;						\
	}							\
}

#define fiVoidBlock(ok, exn, expr)				\
{								\
	fiDeclareNewState(state);				\
								\
	if (!fiSaveState(state)) {				\
		/* 1st time through */				\
		expr;						\
		fiRestoreState(state);				\
		ok = 1;						\
	}							\
	else {	/* should look for additional protects	*/	\
		fiRestoreState(state);				\
		if (state->target != (FiWord) state) {		\
			fiUnwind(state->target, state->value);	\
		}						\
		exn = state->value;				\
		ok = 0;						\
	}							\
}

/* Use this for reporting errors - Word argument means we can import {<name>:String->()} */
extern void	fiRaiseException	(FiWord);

#endif

/******************************************************************************
 *
 * :: Callbacks to error routines, etc.
 *
 *****************************************************************************/

extern void	fiRuntimeError		(char *, char *);

/******************************************************************************
 *
 * :: Globals linking
 *
 *****************************************************************************/

extern void	fiExportGlobalFun	(char *, Ptr, int);
extern void	fiImportGlobalFun	(char *, Ptr *);

#define	fiExportGlobal(name, x)	fiExportGlobalFun(name, (Ptr) &x, sizeof(x))
#define fiImportGlobal(name, x) fiImportGlobalFun(name, (Ptr *) &x)

/******************************************************************************
 *
 * :: Dynamic linking and files initialization
 *
 *****************************************************************************/

#define		fiInitModulePrefix	"INIT_"

extern  void	(*fiFileInitializer)	(char *);

/******************************************************************************
 *
 * :: Miscellaneous utilities
 *
 *****************************************************************************/

extern FiArb	fiBoxDFlo	(FiDFlo);
extern FiDFlo	fiUnBoxDFlo	(FiArb);

extern FiSFlo	fiWordToSFlo	(FiWord);
extern FiWord	fiSFloToWord	(FiSFlo);

/******************************************************************************
 *
 * :: Raw record operations
 *
 *****************************************************************************/

#define fiRawRecordNew(n,f) \
	(FiRRec)fi0Alloc(((FiSInt *)f)[(FiSInt)n], CENSUS_RRec)

/* Assumes we can perform address arithmetic like this ... */
#define fiRawRecordElt(r,e,f) \
	(FiWord)(((FiSInt)(r)) + ((FiSInt *)f)[(FiSInt)e])

extern FiSInt	fiRawRepSize		(FiSInt);
extern FiPtr	fiRawRecordValues	(FiSInt, ...);

extern FiSInt	fiCounter		(void);

#endif
