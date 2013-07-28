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

/* modified by LDR  */
#if defined (__STDC__) || (_MSC_VER)
#define _of_(x)	  x 
#else
#define _of_(x)	  
#endif

typedef char	  *Ptr;

typedef Ptr	  FiNil;
typedef unsigned long int  FiWord;
typedef long int  FiArb;
typedef Ptr	  FiPtr;
typedef long	  FiBool; /* Fortran-77 compatability */
typedef unsigned char FiByte;
typedef short	  FiHInt;
typedef long	  FiSInt;
typedef unsigned char	  FiChar;
typedef Ptr	  FiArr;
typedef Ptr	  FiRec;
typedef Ptr	  FiRRec;
typedef Ptr	  FiTR;
typedef Ptr	  FiBInt;
#ifdef CC_SF_is_double
  typedef double  FiSFlo;
#else
  typedef float	  FiSFlo;
#endif
typedef double	  FiDFlo;
typedef Ptr	  (*FiFun)();

/* When adding FiInt8 etc, please implement fiArrNew_Int8 too! */

typedef unsigned short	  FiBIntS; /* For fiBIntFrPlacev */

/* If you change this then change StoFiProg as well */
typedef struct _FiProg {
	FiFun	fun;
	FiFun	fcall;
	FiWord	progInfo;
	FiWord	data;
} *FiProg;

typedef struct _FiEnv {
	Ptr   	level;
	struct _FiEnv	*next;
	FiWord	info;
} *FiEnv;

/* If you change this then change StoFiClos as well */
typedef struct _FiClos {
	FiEnv	env;
	FiProg 	prog;
} *FiClos;

#ifdef CC_SF_is_double
typedef struct _FiComplexSF {
        double real;
        double imag;
} FiComplexSF;
#else
typedef struct _FiComplexSF {
        float real;
        float imag;
} FiComplexSF;
#endif

typedef struct _FiComplexDF {
        double real;
        double imag;
} FiComplexDF;


typedef struct _FiList
{
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
#define         fiRawCProg(cl)		((cl)->prog->fun)
#define		fiCFun(t, fn)		(*((t (*)())(fn)->prog->fun))
#define		fiProtoCFun(t, at, fn)  (*((t (*) at)(fn)->prog->fun))
#define		fiCFcall(cl)		((cl)->prog->fcall)
#define		fiCProgInfo(cl)		((cl)->prog->progInfo)
#define		fiCData(cl)		((cl)->prog->data)

#define		fiCCall0(t,fn)          (fiCFun(t,fn)((fn)->env))
#define		fiCCall1(t,fn,a)        (fiCFun(t,fn)((fn)->env,a))
#define		fiCCall2(t,fn,a,b)      (fiCFun(t,fn)((fn)->env,a,b))
#define		fiCCall3(t,fn,a,b,c)    (fiCFun(t,fn)((fn)->env,a,b,c))
#define		fiCCall4(t,fn,a,b,c,d)  (fiCFun(t,fn)((fn)->env,a,b,c,d))
#define		fiCCall5(t,fn,a,b,c,d,e) (fiCFun(t,fn)((fn)->env,a,b,c,d,e))

#define		fiEnvLevel(e)		((FiEnv) (e)->level)
#define		fiEnvNext(e)		((FiEnv) (e)->next)
#define		fiEnvPush(a,e)		fiEnvPushFun((FiPtr) (a), e)
#define		fiEnvInfo(e)		(((FiEnv) (e))->info)
extern FiEnv	fiEnvPushFun		_of_((FiPtr, FiEnv));

#define 	fiEnvEnsure(e)		fiEnvEnsureFun(e)
extern void	fiEnvEnsureFun		_of_((FiEnv));

#define		fiEnvLevel0(e) 		(((FiEnv)(e))->level)
#define		fiEnvLevel1(e) 		(((FiEnv)(e))->next->level)
#define		fiEnvLevel2(e) 		(((FiEnv)(e))->next->next->level)
#define		fiEnvLevel3(e) 		(((FiEnv)(e))->next->next->next->level)
#define		fiEnvLevel4(e) 		(((FiEnv)(e))->next->next->next\
							 ->next->level)
#define		fiEnvLevel5(e) 		(((FiEnv)(e))->next->next->next\
						         ->next->next->level)

#define		fiNew(T)		((T *) fiAlloc(sizeof(T)))
#define		fi0New(T,O)		((T *) fi0Alloc(sizeof(T), O))
#define		fi0RecNew(T,O)		((T *) fi0RecAlloc(sizeof(T), O))

#define		fiClosMake(e,p)		fiClosMakeFun(e, (FiProg) (p))
extern FiClos	fiClosMakeFun		_of_((FiEnv, FiProg));

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

extern double *fiAlloc		_of_((unsigned long));
extern double *fi0Alloc		_of_((unsigned long, int));
extern double *fi0RecAlloc	_of_((unsigned long, int));
extern void    fiFree           _of_((Ptr));
extern FiChar *fiArrNew_Char	_of_((unsigned long));
extern FiBool *fiArrNew_Bool	_of_((unsigned long));
extern FiByte *fiArrNew_Byte	_of_((unsigned long));
extern FiHInt *fiArrNew_HInt	_of_((unsigned long));
extern FiSInt *fiArrNew_SInt	_of_((unsigned long));
extern FiSFlo *fiArrNew_SFlo	_of_((unsigned long));
extern FiDFlo *fiArrNew_DFlo	_of_((unsigned long));
extern FiWord *fiArrNew_Word	_of_((unsigned long));
extern FiPtr  *fiArrNew_Ptr	_of_((unsigned long));
extern FiNil  *fiArrNew_Nil	_of_((unsigned long));
extern FiRec  *fiArrNew_Rec	_of_((unsigned long));
extern FiArr  *fiArrNew_Arr	_of_((unsigned long));
extern FiArb  *fiArrNew_Arb	_of_((unsigned long));
extern FiRRec *fiArrNew_RRec	_of_((unsigned long));
extern FiTR   *fiArrNew_TR	_of_((unsigned long));
extern FiBInt *fiArrNew_BInt	_of_((unsigned long));
extern FiClos *fiArrNew_Clos	_of_((unsigned long));

#define		fiNil        	((Ptr) 0)
#define		fiPtrIsNil(p)	((p) == 0)
#define		fiPtrMagicEQ(p,q) ((p) == (q))

extern FiChar	fiCharMin	_of_((void));
extern FiChar	fiCharMax	_of_((void));
extern FiBool	fiCharIsDigit	_of_((FiChar));
extern FiBool	fiCharIsLetter	_of_((FiChar));
extern FiChar	fiCharLower	_of_((FiChar));
extern FiChar	fiCharUpper	_of_((FiChar));


extern FiSFlo	fiSFloMin	_of_((void));
extern FiSFlo	fiSFloMax	_of_((void));
extern FiSFlo	fiSFloEpsilon	_of_((void));
extern FiSFlo	fiSFloPrev	_of_((FiSFlo));
extern FiSFlo	fiSFloNext	_of_((FiSFlo));
#define      	fiSFloTimesPlus(a,b,c)	((a)*(b)+(c))
extern FiSFlo	fiSFloRPlus	_of_((FiSFlo, FiSFlo, FiSInt));
extern FiSFlo	fiSFloRMinus	_of_((FiSFlo, FiSFlo, FiSInt));
extern FiSFlo	fiSFloRTimes	_of_((FiSFlo, FiSFlo, FiSInt));
extern FiSFlo	fiSFloRTimesPlus _of_((FiSFlo,FiSFlo, FiSFlo, FiSInt));
extern FiSFlo	fiSFloRDivide	_of_((FiSFlo, FiSFlo, FiSInt));
extern FiBInt	fiSFloTruncate  _of_((FiSFlo));
extern FiSFlo	fiSFloFraction  _of_((FiSFlo));
extern FiBInt	fiSFloRound 	_of_((FiSFlo, FiSInt));
extern void	fiSFloDissemble _of_((FiSFlo, FiBool *, FiSInt *, FiWord *));
extern FiSFlo	fiSFloAssemble  _of_((FiBool, FiSInt, FiWord));
extern FiBInt	fiSFloExponent 	_of_((FiSFlo));
extern FiBInt	fiSFloMantissa 	_of_((FiSFlo));

extern FiDFlo	fiDFloMin	_of_((void));
extern FiDFlo	fiDFloMax	_of_((void));
extern FiDFlo	fiDFloEpsilon	_of_((void));
extern FiDFlo	fiDFloPrev	_of_((FiDFlo));
extern FiDFlo	fiDFloNext	_of_((FiDFlo));
#define      	fiDFloTimesPlus(a,b,c)  ((a)*(b)+(c))
extern FiDFlo	fiDFloRPlus	_of_((FiDFlo, FiDFlo, FiSInt));
extern FiDFlo	fiDFloRMinus	_of_((FiDFlo, FiDFlo, FiSInt));
extern FiDFlo	fiDFloRTimes	_of_((FiDFlo, FiDFlo, FiSInt));
extern FiDFlo	fiDFloRTimesPlus _of_((FiDFlo,FiDFlo, FiDFlo, FiSInt));
extern FiDFlo	fiDFloRDivide	_of_((FiDFlo, FiDFlo, FiSInt));
extern FiBInt	fiDFloTruncate  _of_((FiDFlo));
extern FiDFlo	fiDFloFraction  _of_((FiDFlo));
extern FiBInt	fiDFloRound     _of_((FiDFlo, FiSInt));
extern void	fiDFloDissemble _of_((FiDFlo,FiBool*,FiSInt*,FiWord*,FiWord*));
extern FiDFlo	fiDFloAssemble  _of_((FiBool, FiSInt, FiWord, FiWord));
extern FiBInt	fiDFloExponent  _of_((FiDFlo));
extern FiBInt	fiDFloMantissa  _of_((FiDFlo));

extern FiByte	fiByteMin	_of_((void));
extern FiByte	fiByteMax	_of_((void));

extern FiHInt	fiHIntMin	_of_((void));
extern FiHInt	fiHIntMax	_of_((void));

extern FiSInt	fiSIntMin	_of_((void));
extern FiSInt	fiSIntMax	_of_((void));
#define         fiSIntTimesPlus(a,b,c) ((a)*(b)+(c))
#define      	fiSIntQuo(a,b)  ((a)/(b))
#define      	fiSIntRem(a,b)  ((a)%(b))
extern void	fiSIntDivide	_of_((FiSInt, FiSInt, FiSInt *, FiSInt *));
extern FiSInt	fiSIntGcd	_of_((FiSInt, FiSInt));
extern FiSInt	fiSIntTimesMod	_of_((FiSInt, FiSInt, FiSInt));
extern FiSInt	fiSIntTimesModInv _of_((FiSInt, FiSInt, FiSInt, FiDFlo));
extern FiSInt	fiSIntLength	_of_((FiSInt));
#define      	fiSIntBit(n,i)  (!!((n) & (1L << (i))))

#ifndef OPT_NoDoubleOps

extern void	fiWordTimesDouble  _of_((FiWord, FiWord,
					 FiWord *, FiWord *));
extern void	fiWordDivideDouble _of_((FiWord, FiWord, FiWord,
					 FiWord *, FiWord *, FiWord *));
#endif


extern void	fiWordPlusStep     _of_((FiWord, FiWord, FiWord,
					 FiWord *, FiWord *));
extern void	fiWordTimesStep    _of_((FiWord, FiWord, FiWord, FiWord,
					 FiWord *, FiWord *));
extern FiBInt	fiBIntFrPlacev	_of_((int, unsigned long, FiBIntS *));
extern FiBInt	fiBIntFrInt	_of_((long));
extern FiBInt	fiBInt0		_of_((void));
extern FiBInt	fiBInt1		_of_((void));
extern FiBInt	fiBIntNew	_of_((long));
extern FiBool 	fiBIntIsSingle	_of_((FiBInt));
extern FiBool	fiBIntIsZero    _of_((FiBInt));
extern FiBool	fiBIntIsNeg     _of_((FiBInt));
extern FiBool	fiBIntIsPos     _of_((FiBInt));
extern FiBool	fiBIntIsEven    _of_((FiBInt));
extern FiBool	fiBIntIsOdd     _of_((FiBInt));
extern FiBool 	fiBIntEQ	_of_((FiBInt, FiBInt));
extern FiBool 	fiBIntNE	_of_((FiBInt, FiBInt));
extern FiBool 	fiBIntLT	_of_((FiBInt, FiBInt));
extern FiBool 	fiBIntLE	_of_((FiBInt, FiBInt));
extern FiBInt 	fiBIntNegate	_of_((FiBInt));
extern FiBInt 	fiBIntPlus 	_of_((FiBInt, FiBInt));
extern FiBInt 	fiBIntMinus 	_of_((FiBInt, FiBInt));
extern FiBInt 	fiBIntTimes 	_of_((FiBInt, FiBInt));
extern FiBInt	fiBIntTimesPlus	_of_((FiBInt, FiBInt, FiBInt));
extern FiSInt 	fiBIntLength	_of_((FiBInt));
extern FiBInt 	fiBIntShiftUp 	_of_((FiBInt, FiSInt));
extern FiBInt 	fiBIntShiftDn 	_of_((FiBInt, FiSInt));
extern FiBInt 	fiBIntShiftRem 	_of_((FiBInt, FiSInt));
extern FiBool 	fiBIntBit 	_of_((FiBInt, FiSInt));
extern FiBInt	fiBIntMod	_of_((FiBInt, FiBInt));
extern FiBInt	fiBIntQuo	_of_((FiBInt, FiBInt));
extern FiBInt	fiBIntRem	_of_((FiBInt, FiBInt));
extern void 	fiBIntDivide	_of_((FiBInt, FiBInt, FiBInt *, FiBInt *));
extern FiBInt	fiBIntGcd	_of_((FiBInt, FiBInt));
extern FiBInt	fiBIntSIPower	_of_((FiBInt, FiSInt));
extern FiBInt	fiBIntBIPower	_of_((FiBInt, FiBInt));
extern FiBInt   fiBIntPowerMod  _of_((FiBInt, FiBInt, FiBInt));
extern void 	fiBIntFree	_of_((FiBInt));
extern char *   fiBIntToString  _of_((FiBInt));
extern void	fiBIntInit	_of_((void));

#define		fiSFloToDFlo(v)	((FiDFlo)(v))
#define		fiDFloToSFlo(v)	((FiSFlo)(v))
#define		fiByteToSInt(v)	((FiSInt)(v))
#define		fiSIntToByte(v)	((FiByte)(v))
#define		fiHIntToSInt(v)	((FiSInt)(v))
#define		fiSIntToHInt(v)	((FiHInt)(v))
extern FiBInt	fiSIntToBInt	_of_((FiSInt));
extern FiSInt	fiBIntToSInt	_of_((FiBInt));
#define         fiSIntToSFlo(v) ((FiSFlo)(v))
#define		fiSIntToDFlo(v)	((FiDFlo)(v))
extern FiSFlo	fiBIntToSFlo	_of_((FiBInt));
extern FiDFlo	fiBIntToDFlo	_of_((FiBInt));
#define 	fiPtrToSInt(v)	((FiSInt)(v))
#define		fiSIntToPtr(v)	((FiPtr) (v))
#define 	fiPtrNil	((FiPtr) 0)

extern FiSInt	fiFormatSFlo	_of_((FiSFlo, FiArr, FiSInt));
extern FiSInt	fiFormatDFlo	_of_((FiDFlo, FiArr, FiSInt));
extern FiSInt	fiFormatSInt	_of_((FiSInt, FiArr, FiSInt));
extern FiSInt	fiFormatBInt	_of_((FiBInt, FiArr, FiSInt));

extern void	fiScanSFlo	_of_((FiArr, FiSInt, FiSFlo *, FiSInt *));
extern void	fiScanDFlo	_of_((FiArr, FiSInt, FiDFlo *, FiSInt *));
extern void	fiScanSInt	_of_((FiArr, FiSInt, FiSInt *, FiSInt *));
extern void	fiScanBInt	_of_((FiArr, FiSInt, FiBInt *, FiSInt *));

extern FiSFlo	fiArrToSFlo	_of_((FiArr));
extern FiDFlo	fiArrToDFlo	_of_((FiArr));
extern FiSInt	fiArrToSInt	_of_((FiArr));
extern FiBInt	fiArrToBInt	_of_((FiArr));
extern FiWord	fiScanSmallIntFrString	_of_((char *, FiWord, FiWord));

extern FiWord	fiPlatformRTE	_of_((void));
extern FiWord	fiPlatformOS	_of_((void));

extern FiWord	fiHalt		_of_((FiSInt));	/* Does not return. */

extern void   	fiSetDebugVar 	_of_((FiWord));
extern FiWord   fiGetDebugVar 	_of_((void));
extern void	fiSetDebugger	_of_((FiDbgTag event, FiWord v));
extern FiWord	fiGetDebugger	_of_((FiDbgTag event));

extern FiWord   fiStrHash 	_of_((FiWord));

extern FiPtr	fiListNil	_of_((void));
extern FiBool	fiListEmptyP	_of_((FiPtr));
extern FiWord	fiListHead	_of_((FiPtr));
extern FiPtr	fiListTail	_of_((FiPtr));
extern FiPtr	fiListCons	_of_((FiWord, FiPtr));

/*****************************************************************************
 *
 * :: FOAM operations -- statement versions
 *
 ****************************************************************************/

#define fi_ALLOC(r,t,a)		     ((r) = (t) fiAlloc((unsigned long) (a)))
#define fiARRNEW_Char(r,t,i)	     ((r) = (t) fiArrNew_Char(i))
#define fiARRNEW_Bool(r,t,i)	     ((r) = (t) fiArrNew_Bool(i))
#define fiARRNEW_Byte(r,t,i)	     ((r) = (t) fiArrNew_Byte(i))
#define fiARRNEW_HInt(r,t,i)	     ((r) = (t) fiArrNew_HInt(i))
#define fiARRNEW_SInt(r,t,i)	     ((r) = (t) fiArrNew_SInt(i))
#define fiARRNEW_SFlo(r,t,i)	     ((r) = (t) fiArrNew_SFlo(i))
#define fiARRNEW_DFlo(r,t,i)	     ((r) = (t) fiArrNew_DFlo(i))
#define fiARRNEW_Word(r,t,i)	     ((r) = (t) fiArrNew_Word(i))
#define fiARRNEW_Ptr(r,t,i)	     ((r) = (t) fiArrNew_Ptr(i))
#define fiARRNEW_Nil(r,t,i)	     ((r) = (t) fiArrNew_Nil(i))
#define fiARRNEW_Rec(r,t,i)	     ((r) = (t) fiArrNew_Rec(i))
#define fiARRNEW_Arr(r,t,i)	     ((r) = (t) fiArrNew_Arr(i))
#define fiARRNEW_Arb(r,t,i)	     ((r) = (t) fiArrNew_Arb(i))
#define fiARRNEW_RRec(r,t,i)	     ((r) = (t) fiArrNew_RRec(i))
#define fiARRNEW_TR(r,t,i)	     ((r) = (t) fiArrNew_TR(i))
#define fiARRNEW_BInt(r,t,i)	     ((r) = (t) fiArrNew_BInt(i))
#define fiARRNEW_Clos(r,t,i)	     ((r) = (t) fiArrNew_Clos(i))

#define	fiPTR_IS_NIL(r,t,p)	     ((r) = (t)((FiPtr) (p) == (FiPtr) 0))
#define	fiPTR_MAGIC_EQ(r,t,p,q)	     ((r) = (t)((FiPtr) (p) == (FiPtr) (q)))

#define fiCHAR_MIN(r,t)		     ((r) = (t) fiCharMin())
#define fiCHAR_MAX(r,t)		     ((r) = (t) fiCharMax())
#define fiCHAR_IS_DIGIT(r,t,c)	     ((r) = (t) fiCharIsDigit(c))
#define fiCHAR_IS_LETTER(r,t,c)	     ((r) = (t) fiCharIsLetter(c))
#define fiCHAR_LOWER(r,t,c)	     ((r) = (t) fiCharLower(c))
#define fiCHAR_UPPER(r,t,c)	     ((r) = (t) fiCharUpper(c))

#define fiSFLO_MIN(r,t)		     ((r) = (t) fiSFloMin())
#define fiSFLO_MAX(r,t)		     ((r) = (t) fiSFloMax())
#define fiSFLO_EPSILON(r,t)	     ((r) = (t) fiSFloEpsilon())
#define fiSFLO_PREV(r,t,a)	     ((r) = (t) fiSFloPrev(a))
#define fiSFLO_NEXT(r,t,a)	     ((r) = (t) fiSFloNext(a))
#define fiSFLO_TIMES_PLUS(r,t,a,b,c) ((r) = (t) fiSFloTimesPlus(a,b,c))
#define fiSFLO_R_PLUS(r,t,a,b,d)     ((r) = (t) fiSFloRPlus(a,b,d))
#define fiSFLO_R_MINUS(r,t,a,b,d)    ((r) = (t) fiSFloRMinus(a,b,d))
#define fiSFLO_R_TIMES(r,t,a,b,d)    ((r) = (t) fiSFloRTimes(a,b,d))
#define fiSFLO_R_TIMES_PLUS(r,t,a,b,c,d)((r) = (t) fiSFloRTimesPlus(a,b,c,d))
#define fiSFLO_R_DIVIDE(r,t,a,b,d)   ((r) = (t) fiSFloRDivide(a,b,d))

#define fiWORD_FR_SFLO(r,a)   {FiSFlo fi_tmp = (a); (r) = *(FiWord *)(&fi_tmp); }
#define fiSFLO_FR_WORD(r,a)   {FiWord fi_tmp = (a); (r) = *(FiSFlo *)(&fi_tmp);}

#define fiDFLO_MIN(r,t)		     ((r) = (t) fiDFloMin())
#define fiDFLO_MAX(r,t)		     ((r) = (t) fiDFloMax())
#define fiDFLO_EPSILON(r,t)	     ((r) = (t) fiDFloEpsilon())
#define fiDFLO_PREV(r,t,a)	     ((r) = (t) fiDFloPrev(a))
#define fiDFLO_NEXT(r,t,a)	     ((r) = (t) fiDFloNext(a))
#define fiDFLO_TIMES_PLUS(r,t,a,b,c) ((r) = (t) fiDFloTimesPlus(a,b,c))
#define fiDFLO_R_PLUS(r,t,a,b,d)     ((r) = (t) fiDFloRPlus(a,b,d))
#define fiDFLO_R_MINUS(r,t,a,b,d)    ((r) = (t) fiDFloRMinus(a,b,d))
#define fiDFLO_R_TIMES(r,t,a,b,d)    ((r) = (t) fiDFloRTimes(a,b,d))
#define fiDFLO_R_TIMES_PLUS(r,t,a,b,c,d)((r) = (t) fiDFloRTimesPlus(a,b,c,d))
#define fiDFLO_R_DIVIDE(r,t,a,b,d)   ((r) = (t) fiDFloRDivide(a,b,d))

#define fiBYTE_MIN(r,t)		     ((r) = (t) fiByteMin())
#define fiBYTE_MAX(r,t)		     ((r) = (t) fiByteMax())

#define fiHINT_MIN(r,t)		     ((r) = (t) fiHIntMin())
#define fiHINT_MAX(r,t)		     ((r) = (t) fiHIntMax())

#define fiSINT_MIN(r,t)		     ((r) = (t) fiSIntMin())
#define fiSINT_MAX(r,t)		     ((r) = (t) fiSIntMax())
#define fiSINT_TIMES_PLUS(r,t,a,b,c) ((r) = (t) fiSIntTimesPlus(a,b,c))
#define fiSINT_QUO(r,t,a,b)	     ((r) = (t) fiSIntQuo(a,b))
#define fiSINT_REM(r,t,a,b)	     ((r) = (t) fiSIntRem(a,b))
#define fiSINT_GCD(r,t,a,b)	     ((r) = (t) fiSIntGcd(a,b))
#define fiSINT_LENGTH(r,t,si)	     ((r) = (t) fiSIntLength(si))
#define fiSINT_BIT(r,t,a,b)	     ((r) = (t) fiSIntBit(a,b))
#define fiSINT_TIMES_MOD(r,t,a,b,n)  ((r) = (t) fiSIntTimesMod(a,b,n))
#define fiSINT_TIMES_MOD_INV(r,t,a,b,n,ni)\
				     ((r) = (t) fiSIntTimesModInv(a,b,n,ni))

#define fiBINT_FR_PLACEV(r,t,b,l,s)  ((r) = (t) fiBIntFrPlacev(b,l,s))
#define fiBINT_FR_INT(r,t,i)	     ((r) = (t) fiBIntFrInt(i))
#define fiBINT_0(r,t)		     ((r) = (t) fiBInt0())
#define fiBINT_1(r,t)		     ((r) = (t) fiBInt1())
#define fiBINT_NEW(r,t,i)	     ((r) = (t) fiBIntNew(i))
#define fiBINT_IS_ZERO(r,t,b)	     ((r) = (t) fiBIntIsZero(b))
#define fiBINT_IS_NEG(r,t,b)	     ((r) = (t) fiBIntIsNeg(b))
#define fiBINT_IS_POS(r,t,b)	     ((r) = (t) fiBIntIsPos(b))
#define fiBINT_IS_EVEN(r,t,b)	     ((r) = (t) !fiBIntBit(b, (FiSInt) 0))
#define fiBINT_IS_ODD(r,t,b)	     ((r) = (t) fiBIntBit(b, (FiSInt) 0))
#define fiBINT_IS_SINGLE(r,t,b)	     ((r) = (t) fiBIntIsSingle(b))
#define fiBINT_LE(r,t,a,b)	     ((r) = (t) fiBIntLE(a,b))
#define fiBINT_EQ(r,t,a,b)	     ((r) = (t) fiBIntEQ(a,b))
#define fiBINT_LT(r,t,a,b)	     ((r) = (t) fiBIntLT(a,b))
#define fiBINT_NE(r,t,a,b)	     ((r) = (t) fiBIntNE(a,b))
#define fiBINT_NEGATE(r,t,b)	     ((r) = (t) fiBIntNegate(b))
#define fiBINT_PLUS1(r,t,a)	     fiBINT_PLUS(r, t, a, fiBInt1())
#define fiBINT_MINUS1(r,t,a)	     fiBINT_MINUS(r, t, a, fiBInt1())
#define fiBINT_PLUS(r,t,a,b)	     ((r) = (t) fiBIntPlus(a,b))
#define fiBINT_MINUS(r,t,a,b)	     ((r) = (t) fiBIntMinus(a,b))
#define fiBINT_TIMES(r,t,a,b)	     ((r) = (t) fiBIntTimes(a,b))
#define fiBINT_TIMES_PLUS(r,t,a,b,c) ((r) = (t) fiBIntTimesPlus(a,b,c))
#define fiBINT_LENGTH(r,t,b)	     ((r) = (t) fiBIntLength(b))
#define fiBINT_SHIFT_UP(r,t,a,i)     ((r) = (t) fiBIntShiftUp(a,i))
#define fiBINT_SHIFT_DN(r,t,a,i)     ((r) = (t) fiBIntShiftDn(a,i))
#define fiBINT_BIT(r,t,a,i)	     ((r) = (t) fiBIntBit(a,i))
#define fiBINT_MOD(r,t,a,b)	     ((r) = (t) fiBIntMod(a,b))
#define fiBINT_QUO(r,t,a,b)	     ((r) = (t) fiBIntQuo(a,b))
#define fiBINT_REM(r,t,a,b)	     ((r) = (t) fiBIntRem(a,b))
#define fiBINT_GCD(r,t,a,b)	     ((r) = (t) fiBIntGcd(a,b))
#define fiBINT_SI_POWER(r,t,a,b)     ((r) = (t) fiBIntSIPower(a,b))
#define fiBINT_BI_POWER(r,t,a,b)     ((r) = (t) fiBIntBIPower(a,b))
#define fiBINT_POWER_MOD(r,t,a,b,c)  ((r) = (t) fiBIntPowerMod(a,b,c))

#define	fiSFLO_TO_DFLO(r,t,l)	     ((r) = (t) fiSFloToDFlo(l))
#define	fiDFLO_TO_SFLO(r,t,l)	     ((r) = (t) fiDFloToSFlo(l))
#define	fiBYTE_TO_SINT(r,t,l)	     ((r) = (t) fiByteToSInt(l))
#define	fiSINT_TO_BYTE(r,t,l)	     ((r) = (t) fiSIntToByte(l))
#define	fiHINT_TO_SINT(r,t,l)	     ((r) = (t) fiHIntToSInt(l))
#define	fiSINT_TO_HINT(r,t,l)	     ((r) = (t) fiSIntToHInt(l))
#define	fiSINT_TO_BINT(r,t,l)	     ((r) = (t) fiSIntToBInt(l))
#define	fiBINT_TO_SINT(r,t,l)	     ((r) = (t) fiBIntToSInt(l))
#define	fiSINT_TO_SFLO(r,t,l)	     ((r) = (t) fiSIntToSFlo(l))
#define	fiSINT_TO_DFLO(r,t,l)	     ((r) = (t) fiSIntToDFlo(l))
#define	fiBINT_TO_SFLO(r,t,l)	     ((r) = (t) fiBIntToSFlo(l))
#define	fiBINT_TO_DFLO(r,t,l)	     ((r) = (t) fiBIntToDFlo(l))
#define	fiPTR_TO_SINT(r,t,l)	     ((r) = (t) fiPtrToSInt(l))
#define	fiSINT_TO_PTR(r,t,l)	     ((r) = (t) fiSIntToPtr(l))

#define fiFORMAT_SFLO(r,t,s,w,si)    ((r) = (t) fiFormatSFlo(s,(FiArr)w,si))
#define fiFORMAT_DFLO(r,t,d,w,si)    ((r) = (t) fiFormatDFlo(d,(FiArr)w,si))
#define fiFORMAT_SINT(r,t,a,w,b)     ((r) = (t) fiFormatSInt(a,(FiArr)w,b))
#define fiFORMAT_BINT(r,t,b,w,si)    ((r) = (t) fiFormatBInt(b,(FiArr)w,si))

#define	fiARR_TO_SFLO(r,t,v)	     ((r) = (t) fiArrToSFlo((FiArr)v))
#define	fiARR_TO_DFLO(r,t,v)	     ((r) = (t) fiArrToDFlo((FiArr)v))
#define	fiARR_TO_SINT(r,t,v)	     ((r) = (t) fiArrToSInt((FiArr)v))
#define	fiARR_TO_BINT(r,t,v)	     ((r) = (t) fiArrToBInt((FiArr)v))

#define fiHALT(r,t,i)		     ((r) = (t) fiHalt(i))

#define fiLIST_NIL(r,t)		     ((r) = (t) ((FiPtr)0))
#define fiLIST_EMPTYP(r,t,l)	     ((r) = (t) (l))
#define fiLIST_HEAD(r,t,l)	     ((r) = (t) (((FiList *)l)->data))
#define fiLIST_TAIL(r,t,l)	     ((r) = (t) ((((FiList *)l)->next)))

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

#define fiSFLO_TRUNC(x)		(fiSFloTruncate(x))
#define fiSFLO_ROUND_DOWN(x)	\
	fiSFloTruncate((x) < 0 && fiSFloFraction(x) != 0 ? (x) - (FiSFlo)1.0 : (x))
#define fiSFLO_ROUND_UP(x)	\
	fiSFloTruncate((x) > 0 && fiSFloFraction(x) != 0 ? (x) + (FiSFlo)1.0 : (x))
#define fiSFLO_ROUND_ZERO(x)	(fiSFloTruncate(x))
#define fiSFLO_ROUND_NEAREST(x)	(fiSFloNearest(x))
#define fiSFLO_ROUND(x)		(fiSFloTruncate(x))

#define fiDFLO_TRUNC(x)		(fiDFloTruncate(x))
#define fiDFLO_ROUND_DOWN(x)	\
	fiDFloTruncate((x) < 0 && fiDFloFraction(x) != 0 ? (x) - 1.0 : (x))
#define fiDFLO_ROUND_UP(x)	\
	fiDFloTruncate((x) > 0 && fiDFloFraction(x) != 0 ? (x) + 1.0 : (x))
#define fiDFLO_ROUND_ZERO(x)	(fiDFloTruncate(x))
#define fiDFLO_ROUND_NEAREST(x)	(fiDFloNearest(x))
#define fiDFLO_ROUND(x)		(fiDFloTruncate(x))

/* Here lieth the ieee754 stuff */
void fiInitialiseFpu			_of_((void));
FiWord fiIeeeGetRoundingMode		_of_((void));
FiWord fiIeeeSetRoundingMode		_of_((FiWord s));
FiWord fiIeeeGetEnabledExceptions	_of_((void));
FiWord fiIeeeSetEnabledExceptions	_of_((FiWord s));
FiWord fiIeeeGetExceptionStatus		_of_((void));
FiWord fiIeeeSetExceptionStatus		_of_((FiWord s));
FiWord fiDoubleHexPrintToString	        _of_((FiWord f));

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

extern 	FiFluid fiAddFluid _of_((char *));
extern  void	fiSetFluid	_of_((FiFluid, FiWord));
extern  void 	fiKillFluid	_of_((FiFluid));
FiWord	fiFluidValue	_of_((FiFluid));

FiFluid	fiGetFluid	_of_((char *));


/******************************************************************************
 *
 * :: Interface to the store management
 *
 *****************************************************************************/

/* Taken from store.h: move into axlobs.h or similar */
extern	void	fiStoForceGC		_of_((void));
extern	FiBool	fiStoInHeap		_of_((FiPtr));
extern	FiSInt	fiStoIsWritable		_of_((FiPtr));
extern	FiSInt	fiStoMarkObject		_of_((FiPtr));
extern	FiWord	fiStoRecode		_of_((FiPtr, FiSInt));
extern	void	fiStoNewObject		_of_((FiSInt, FiBool));
extern	void	fiStoATracer		_of_((FiSInt, FiClos));
extern	void	fiStoCTracer		_of_((FiSInt, FiWord));
extern	void	fiStoShow		_of_((FiSInt));
extern  int	fiStoShowArgs		_of_((FiPtr));


/******************************************************************************
 *
 * :: State Mangler...
 *
 *****************************************************************************/

#if 1

#include <setjmp.h>

typedef struct _FiState {
	FiWord 		tag;

	struct _FiState *next;
	/* Temporary --- used while jumping */
	FiWord 		target;
	FiWord 		value;
	
  	/* "State" */
	/* Could think about making this a list of foam closures.
	 *  This way, people could write their own dynamic scope
	 *  manipulators.  Rome wasn't burnt in a day...
	 */
	int		nStates;
	void 	      **states;
	jmp_buf 	machineState;

} FiStateBox, *FiState, *FiStateChain;

extern FiStateChain fiGlobStates;
extern void 	    fiJump _of_((FiWord tag));
extern void 	    fiRestoreState0 _of_((FiState state));
extern void 	    fiSaveState0 _of_((FiState state));
extern void 	    fiUnwind _of_((FiWord, FiWord));
extern void	    fiUnhandledException _of_ ((FiWord));
extern void	    fiRegisterStateFns _of_ ((void *(*)(), void (*)(void *))); 

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
		if (!state->target == (FiWord) state) {		\
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
		if (!state->target == (FiWord) state) {		\
			fiUnwind(state->target, state->value);	\
		}						\
	        exn = state->value;				\
		ok = 0;						\
	}							\
}

/* Use this for reporting errors - Word argument means we can import {<name>:String->()} */
extern void fiRaiseException _of_((FiWord));

#endif

/******************************************************************************
 *
 * :: Callbacks to error routines, etc.
 *
 *****************************************************************************/

void fiRuntimeError  _of_((char *, char *));

/******************************************************************************
 *
 * :: Globals linking
 *
 *****************************************************************************/

void	fiExportGlobalFun	_of_((char *, Ptr, int));
void	fiImportGlobalFun	_of_((char *, Ptr *));

#define	fiExportGlobal(name, x)	fiExportGlobalFun(name, (Ptr) &x, sizeof(x))
#define fiImportGlobal(name, x) fiImportGlobalFun(name, (Ptr *) &x)

/******************************************************************************
 *
 * :: Dynamic linking and files initialization
 *
 *****************************************************************************/

#define		fiInitModulePrefix	"INIT_"

extern  void	(* fiFileInitializer) _of_((char *));

/******************************************************************************
 *
 * :: Miscellaneous utilities
 *
 *****************************************************************************/

extern FiArb	fiBoxDFlo	_of_((FiDFlo));
extern FiDFlo	fiUnBoxDFlo	_of_((FiArb));

extern FiSFlo	fiWordToSFlo    _of_((FiWord));
extern FiWord	fiSFloToWord    _of_((FiSFlo));

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

extern FiSInt	fiRawRepSize		_of_((FiSInt));
extern FiPtr	fiRawRecordValues	_of_((FiSInt, ...));

extern FiSInt fiCounter();

#endif
