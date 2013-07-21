/* 
   TO DO:

   on ALPHA/OSF 
        we need to start with the IEEE default mode : nearest

   on HPUX we need to call fpsetfastmode(0) to get gradual underflow.

   on AIX we have a choice of trap modes 

   */
#if defined(_AIX32)
#undef  _POSIX_SOURCE
#define _ALL_SOURCE
#endif

#include "axlgen.h"
#include "cport.h"
#include "foam_c.h"
#include "foam_cfp.h"

static char double_hex_print_string[80];

#if defined(sparc) && defined(sun) && !defined(__SVR4)
/* SPARC SUNOS4 cc or gcc */
/* rely on assembler get_fsr set_fsr */
#include <sys/ieeefp.h>
/* a view of the fsr */
union fsr_reg {
        unsigned int fsr_word;
        struct {
                unsigned rd:2;
                unsigned rp:2;
                unsigned tem:5;
                unsigned ns:1;
                unsigned resv:2;
                unsigned vers:3;
                unsigned ftt:3;
                unsigned qne:1;
                unsigned pr:1;
                unsigned fcc:2;
                unsigned aexc:5;
                unsigned cexc:5;
        } fsr_struct;
} ;

#define IEEE_FP_NEAREST fp_nearest
#define IEEE_FP_UP      fp_positive
#define IEEE_FP_DOWN    fp_negative
#define IEEE_FP_ZERO    fp_tozero
#define IEEE_FP_INEXACT     (1 << fp_inexact)
#define IEEE_FP_DIVBYZERO   (1 << fp_division)
#define IEEE_FP_UNDERFLOW   (1 << fp_underflow)
#define IEEE_FP_OVERFLOW    (1 << fp_overflow)
#define IEEE_FP_INVALID     (1 << fp_invalid)

unsigned int
get_fsr()
{
        unsigned int res;

        asm volatile ("st %%fsr, %0":"=m" (res));
        return res;
}
void
set_fsr(unsigned int r)
{
        asm volatile ("ld %0, %%fsr":"=m" (r));
}

FiWord
fiIeeeGetRoundingMode()
{
  FiWord tmp;
  union fsr_reg tmp1;

  tmp1.fsr_word=get_fsr();
  switch(tmp1.fsr_struct.rd) {
  case IEEE_FP_UP :
    tmp = fiRoundUp();
    break;
  case IEEE_FP_DOWN:
    tmp = fiRoundDown();
    break;
  case IEEE_FP_ZERO:
    tmp = fiRoundZero();
    break;
  case IEEE_FP_NEAREST :
  default:
    tmp = fiRoundNearest();
    break;
  }
  return tmp;
}

FiWord
fiIeeeSetRoundingMode(FiWord s)
{
  union fsr_reg tmp,tmp1;
  FiWord result;

  tmp1.fsr_word = get_fsr();
  tmp.fsr_word = tmp1.fsr_word;
  switch(s) {
  case fiRoundUp():
    tmp.fsr_struct.rd = IEEE_FP_UP;
    break;
  case fiRoundDown():
    tmp.fsr_struct.rd = IEEE_FP_DOWN;
    break;
  case fiRoundZero():
    tmp.fsr_struct.rd = IEEE_FP_ZERO;
    break;
  case fiRoundNearest():
  default:
    tmp.fsr_struct.rd = IEEE_FP_NEAREST;
    break;
  }
  set_fsr(tmp.fsr_word);
  switch(tmp1.fsr_struct.rd) {
  case IEEE_FP_UP :
    result = fiRoundUp();
    break;
  case IEEE_FP_DOWN:
    result = fiRoundDown();
    break;
  case IEEE_FP_ZERO:
    result = fiRoundZero();
    break;
  case IEEE_FP_NEAREST :
  default:
    result = fiRoundNearest();
    break;
  }
  return result;
}

FiWord
fiIeeeGetEnabledExceptions()
{
  FiWord tmp;
  union fsr_reg tmp1;

  tmp1.fsr_word=get_fsr();
  tmp = (tmp1.fsr_struct.tem & IEEE_FP_INVALID) ? 16 :0;
  tmp |= (tmp1.fsr_struct.tem & IEEE_FP_OVERFLOW) ? 8 :0;
  tmp |= (tmp1.fsr_struct.tem & IEEE_FP_UNDERFLOW) ? 4 :0;
  tmp |= (tmp1.fsr_struct.tem & IEEE_FP_DIVBYZERO) ? 2 :0;
  tmp |= (tmp1.fsr_struct.tem & IEEE_FP_INEXACT) ? 1 :0;
  return tmp;
}

FiWord
fiIeeeSetEnabledExceptions(s)
     FiWord s;
{
  union fsr_reg tmp1;
  unsigned int set;
  FiWord result;

  set = (s & 16) ? IEEE_FP_INVALID   :0;
  set |= (s & 8) ? IEEE_FP_OVERFLOW  :0;
  set |= (s & 4) ? IEEE_FP_UNDERFLOW :0;
  set |= (s & 2) ? IEEE_FP_DIVBYZERO :0;
  set |= (s & 1) ? IEEE_FP_INEXACT   :0;
  tmp1.fsr_word = get_fsr();
  result = (tmp1.fsr_struct.tem & IEEE_FP_INVALID) ? 16 :0;
  result |= (tmp1.fsr_struct.tem & IEEE_FP_OVERFLOW) ? 8 :0;
  result |= (tmp1.fsr_struct.tem & IEEE_FP_UNDERFLOW) ? 4 :0;
  result |= (tmp1.fsr_struct.tem & IEEE_FP_DIVBYZERO) ? 2 :0;
  result |= (tmp1.fsr_struct.tem & IEEE_FP_INEXACT) ? 1 :0;
  tmp1.fsr_struct.tem= set;
  set_fsr(tmp1.fsr_word);
  return result;
}

FiWord
fiIeeeGetExceptionStatus()
{
  FiWord tmp;
  union fsr_reg tmp1;

  tmp1.fsr_word=get_fsr();
  tmp = (tmp1.fsr_struct.aexc & IEEE_FP_INVALID) ? 16 :0;
  tmp |= (tmp1.fsr_struct.aexc & IEEE_FP_OVERFLOW) ? 8 :0;
  tmp |= (tmp1.fsr_struct.aexc & IEEE_FP_UNDERFLOW) ? 4 :0;
  tmp |= (tmp1.fsr_struct.aexc & IEEE_FP_DIVBYZERO) ? 2 :0;
  tmp |= (tmp1.fsr_struct.aexc & IEEE_FP_INEXACT) ? 1 :0;
  return tmp;
}

FiWord
fiIeeeSetExceptionStatus(s)
     FiWord s;
{
  union fsr_reg tmp1;
  unsigned int set;
  FiWord result;

  set = (s & 16) ? IEEE_FP_INVALID :0;
  set |= (s & 8) ? IEEE_FP_OVERFLOW :0;
  set |= (s & 4) ? IEEE_FP_UNDERFLOW :0;
  set |= (s & 2) ? IEEE_FP_DIVBYZERO  :0;
  set |= (s & 1) ? IEEE_FP_INEXACT :0;
  tmp1.fsr_word = get_fsr();
  result = (tmp1.fsr_struct.aexc & IEEE_FP_INVALID) ? 16 :0;
  result |= (tmp1.fsr_struct.aexc & IEEE_FP_OVERFLOW) ? 8 :0;
  result |= (tmp1.fsr_struct.aexc & IEEE_FP_UNDERFLOW) ? 4 :0;
  result |= (tmp1.fsr_struct.aexc & IEEE_FP_DIVBYZERO) ? 2 :0;
  result |= (tmp1.fsr_struct.aexc & IEEE_FP_INEXACT) ? 1 :0;
  tmp1.fsr_struct.aexc= set;
  set_fsr(tmp1.fsr_word);
  return result;
  
}

void
fiInitialiseFpu()
{
}

FiWord
fiDoubleHexPrintToString(f)
     FiWord f;	/* it's a (double *) really */
		/* (hope that word alignment == double alignment) */
{
  sprintf(double_hex_print_string,"%0.8x %0.8x",*(int *)f,*((int *)f+1));
  return (FiWord) double_hex_print_string;
}
#elif defined(__SVR4) && defined (__sun) && defined(__sparc)
/* SPARC SOLARIS GNU */
#include <ieeefp.h>
#define IEEE_FP_NEAREST FP_RN
#define IEEE_FP_UP      FP_RP
#define IEEE_FP_DOWN    FP_RM
#define IEEE_FP_ZERO    FP_RZ


FiWord
fiDoubleHexPrintToString(FiWord f) 
{
#if EDIT_1_0_n2_01
  sprintf(double_hex_print_string,"%.8x %.8x",*(int *)f,*((int *)f+1));
#else
  sprintf(double_hex_print_string,"%0.8x %0.8x",*(int *)f,*((int *)f+1));
#endif
  return (FiWord) double_hex_print_string;
}

void 
fiInitialiseFpu()
{
}

FiWord
fiIeeeGetRoundingMode()
{
  FiWord tmp = fiRoundNearest();
  
  switch(fpgetround()) {
  case IEEE_FP_NEAREST :
    tmp = fiRoundNearest();
    break;
  case IEEE_FP_UP :
    tmp = fiRoundUp();
    break;
  case IEEE_FP_DOWN:
    tmp = fiRoundDown();
    break;
  case IEEE_FP_ZERO:
    tmp = fiRoundZero();
    break;
  }
  return tmp;
}

FiWord
fiIeeeSetRoundingMode(FiWord s)
{
  unsigned int tmp;
  switch(s) {
  case fiRoundUp():
    tmp = fpsetround(IEEE_FP_UP);
    break;
  case fiRoundDown():
    tmp = fpsetround(IEEE_FP_DOWN);
    break;
  case fiRoundZero():
    tmp = fpsetround(IEEE_FP_ZERO);
    break;
  case fiRoundNearest():
  default:
    tmp = fpsetround(IEEE_FP_NEAREST);
    break;
  }

  switch(tmp) {
  case IEEE_FP_UP :
    return fiRoundUp();
  case IEEE_FP_DOWN:
    return fiRoundDown();
  case IEEE_FP_ZERO:
    return fiRoundZero();
  case IEEE_FP_NEAREST :
  default:
    return fiRoundNearest();
  }
}

FiWord
fiIeeeGetEnabledExceptions()
{
  fp_except fm;
  FiWord result=0;

  fm = fpgetmask();
  result =  (fm & FP_X_INV) ? 16 :0;
  result |= (fm & FP_X_OFL) ? 8  :0;
  result |= (fm & FP_X_UFL) ? 4  :0;
  result |= (fm & FP_X_DZ)  ? 2  :0;
  result |= (fm & FP_X_IMP) ? 1  :0;
  return result;
 }

FiWord
fiIeeeSetEnabledExceptions(FiWord s)
{
  FiWord result = 0;
  fp_except set,fm;

  set = (s & 16) ? FP_X_INV :0;
  set |= (s & 8) ? FP_X_OFL :0;
  set |= (s & 4) ? FP_X_UFL :0;
  set |= (s & 2) ? FP_X_DZ  :0;
  set |= (s & 1) ? FP_X_IMP :0;
  fm = fpsetmask(set);
  result =  (fm & FP_X_INV) ? 16 :0;
  result |= (fm & FP_X_OFL) ? 8  :0;
  result |= (fm & FP_X_UFL) ? 4  :0;
  result |= (fm & FP_X_DZ)  ? 2  :0;
  result |= (fm & FP_X_IMP) ? 1  :0;
  return result;
}

FiWord
fiIeeeGetExceptionStatus()
{
  fp_except fm;
  FiWord result=0;

  fm = fpgetsticky();
  result =  (fm & FP_X_INV) ? 16 :0;
  result |= (fm & FP_X_OFL) ? 8  :0;
  result |= (fm & FP_X_UFL) ? 4  :0;
  result |= (fm & FP_X_DZ)  ? 2  :0;
  result |= (fm & FP_X_IMP) ? 1  :0;
  return result;
 }

FiWord
fiIeeeSetExceptionStatus(FiWord s)
{
  FiWord result = 0;
  fp_except set,fm;

  set = (s & 16) ? FP_X_INV :0;
  set |= (s & 8) ? FP_X_OFL :0;
  set |= (s & 4) ? FP_X_UFL :0;
  set |= (s & 2) ? FP_X_DZ  :0;
  set |= (s & 1) ? FP_X_IMP :0;
  fm = fpsetsticky(set);
  result =  (fm & FP_X_INV) ? 16 :0;
  result |= (fm & FP_X_OFL) ? 8  :0;
  result |= (fm & FP_X_UFL) ? 4  :0;
  result |= (fm & FP_X_DZ)  ? 2  :0;
  result |= (fm & FP_X_IMP) ? 1  :0;
  return result;
}

/*
//  2006/Dec/20 -- &&!defined(__ia64__) added by BDS to prevent this case from
//                 being processed on ia64 architectures
*/
#elif defined (__GNUC__) && defined(__linux__) && !defined(__alpha__) && !defined(__ia64__) /* && defined(__i386) */
/* i386 LINUX GNU [or 486/586/686 if we comment out                         ^^^^^^^^ (mnd)] */
/* Added !defined(__alpha__) SMW. */
#define IEEE_FP_NEAREST 0x000
#define IEEE_FP_UP      0x800
#define IEEE_FP_DOWN    0x400
#define IEEE_FP_ZERO    0xC00
#define IEEE_FP_INEXACT       0x20
#define IEEE_FP_DIVBYZERO     0x04
#define IEEE_FP_UNDERFLOW     0x10
#define IEEE_FP_OVERFLOW      0x08
#define IEEE_FP_INVALID       0x01

FiWord
fiDoubleHexPrintToString (FiWord f)
{
  sprintf(double_hex_print_string,"%.8x %.8x",*((int *)f+1),*(int *)f);
  return (FiWord)double_hex_print_string;
}
	


struct __fenv {
  unsigned long control;
  unsigned long status;
  unsigned long tags;
  unsigned long ip_offset;
  unsigned long opcode;
  unsigned long data_offset;
  unsigned long op_sel;
};

typedef struct __fenv fenv_t;

void 
fiInitialiseFpu()
{
}

FiWord 
fiIeeeGetRoundingMode(void)
{
	unsigned short cw;

	asm volatile ("fnstcw %0":"=m" (cw));

	switch (cw & IEEE_FP_ZERO) {
	case IEEE_FP_NEAREST:	return fiRoundNearest();
	case IEEE_FP_UP:	return fiRoundUp();
	case IEEE_FP_DOWN:	return fiRoundDown();
	case IEEE_FP_ZERO:	return fiRoundZero();
	default:		return fiRoundNearest(); /* Silence GCC */
	}
}


FiWord
fiIeeeSetRoundingMode(FiWord round)
{
	unsigned short cw;
	FiWord previous = 0;

	asm volatile ("fnstcw %0":"=m" (cw));
	previous = cw & IEEE_FP_ZERO;

	switch (round) {
	case fiRoundNearest():
		cw = (cw & ~IEEE_FP_ZERO) | (IEEE_FP_NEAREST);
		break;
	case fiRoundUp():
		cw = (cw & ~IEEE_FP_ZERO) | (IEEE_FP_UP);
		break;
	case fiRoundDown():
		cw = (cw & ~IEEE_FP_ZERO) | (IEEE_FP_DOWN);
		break;
	case fiRoundZero():
		cw = (cw & ~IEEE_FP_ZERO) | (IEEE_FP_ZERO);
		break;
	}

	asm volatile ("fldcw %0"::"m" (cw));

	switch(previous) {
	case IEEE_FP_NEAREST:	return fiRoundNearest();
	case IEEE_FP_UP:	return fiRoundUp();
	case IEEE_FP_DOWN:	return fiRoundDown();
	case IEEE_FP_ZERO:	return fiRoundZero();
	default:		return fiRoundZero(); /* Silence GCC */
	}
}


FiWord
fiIeeeGetEnabledExceptions()
{
  unsigned short cw;
  FiWord result;

  asm volatile ("fnstcw %0":"=m" (cw));
  /* translate */
  result =  (cw & IEEE_FP_INVALID)   ? 0 :16;
  result |= (cw & IEEE_FP_OVERFLOW)  ? 0  :8;
  result |= (cw & IEEE_FP_UNDERFLOW) ? 0  :4;
  result |= (cw & IEEE_FP_DIVBYZERO) ? 0  :2;
  result |= (cw & IEEE_FP_INEXACT)   ? 0  :1;
  return result;
}

FiWord
fiIeeeSetEnabledExceptions(FiWord s)
{
  unsigned short cw,set=0;
  FiWord result;
  
  asm volatile ("fnstcw %0":"=m" (cw));
  /* translate result */
  result =  (cw & IEEE_FP_INVALID)   ? 0 :16;
  result |= (cw & IEEE_FP_OVERFLOW)  ? 0  :8;
  result |= (cw & IEEE_FP_UNDERFLOW) ? 0  :4;
  result |= (cw & IEEE_FP_DIVBYZERO) ? 0  :2;
  result |= (cw & IEEE_FP_INEXACT)   ? 0  :1;
  /* translate input */
  set =  (s & 16) ? 0: IEEE_FP_INVALID  ;
  set |= (s & 8)  ? 0: IEEE_FP_OVERFLOW ;
  set |= (s & 4)  ? 0: IEEE_FP_UNDERFLOW;
  set |= (s & 2)  ? 0: IEEE_FP_DIVBYZERO;
  set |= (s & 1)  ? 0: IEEE_FP_INEXACT  ;
  /* clear all relevant bits */
  cw &= ~ ( IEEE_FP_INVALID | IEEE_FP_OVERFLOW | 
	    IEEE_FP_UNDERFLOW | IEEE_FP_DIVBYZERO | IEEE_FP_INEXACT);
  /* set them according to the set */
  cw |= set;
  asm volatile ("fnclex");
  asm volatile ("fldcw %0"::"m" (cw));
  return result;
}

FiWord
fiIeeeGetExceptionStatus()
{
  FiWord result=0;
  unsigned short sw;
  fenv_t env;

  asm volatile ("fnstenv %0":"=m" (env));
  sw = env.status;
  result =  (sw & IEEE_FP_INVALID)   ? 16 :0;
  result |= (sw & IEEE_FP_OVERFLOW)  ? 8  :0;
  result |= (sw & IEEE_FP_UNDERFLOW) ? 4  :0;
  result |= (sw & IEEE_FP_DIVBYZERO) ? 2  :0;
  result |= (sw & IEEE_FP_INEXACT)   ? 1  :0;
  return result;
}

FiWord
fiIeeeSetExceptionStatus(FiWord s)
{  
  FiWord result = 0;
  unsigned short sw,set;
  fenv_t env;

  /* translate input */
  set =  (s & 16) ? IEEE_FP_INVALID   :0;
  set |= (s & 8)  ? IEEE_FP_OVERFLOW  :0;
  set |= (s & 4)  ? IEEE_FP_UNDERFLOW :0;
  set |= (s & 2)  ? IEEE_FP_DIVBYZERO :0;
  set |= (s & 1)  ? IEEE_FP_INEXACT   :0;
  asm volatile ("fnstenv %0":"=m" (env));
  sw = env.status;
  /* translate result */
  result =  (sw & IEEE_FP_INVALID)   ? 16 :0;
  result |= (sw & IEEE_FP_OVERFLOW)  ? 8  :0;
  result |= (sw & IEEE_FP_UNDERFLOW) ? 4  :0;
  result |= (sw & IEEE_FP_DIVBYZERO) ? 2  :0;
  result |= (sw & IEEE_FP_INEXACT)   ? 1  :0;
  /* clear all relevant bits */
  sw &= ~ ( IEEE_FP_INVALID | IEEE_FP_OVERFLOW | 
	    IEEE_FP_UNDERFLOW | IEEE_FP_DIVBYZERO | IEEE_FP_INEXACT);
  /* set them according to the set */
  sw |= set;
  env.status = sw;
  /* load the environment */
  asm volatile ("fldenv %0":"=m" (env));
  return result;
}


#elif defined(__osf__) && defined(__alpha)
/* ALPHA OSF CC */
#include <float.h>

#if EDIT_1_0_n1_07
/******************************************************************************
 * Here we do a certain amount of contortion to get a clean compile.
 * 1. We include the file as though we were in an assembler environment
 *    so we do not get all the bad struct definitions.
 * 2. We give by hand the prototypes of the two functions we really need.
 * SMW -- September 2001
 */
#ifdef __LANGUAGE_C__
# define __SAVE_LANGUAGE_C__ __LANGUAGE_C__
# define __LANGUAGE_ASSEMBLER__
# undef __LANGUAGE_C__
#endif
#endif /* EDIT_1_0_n1_07 */

#include <machine/fpu.h>

#if EDIT_1_0_n1_07
extern long   ieee_get_fp_control(void);
extern void   ieee_set_fp_control(long);

#ifdef __SAVE_LANGUAGE_C__
# define __LANGUAGE_C__ __SAVE_LANGUAGE_C__
# undef __LANGUAGE_ASSEMBLER__
# undef __SAVE_LANGUAGE_C__
#endif
/******************************************************************************/
#endif /* EDIT_1_0_n1_07 */



#define IEEE_FP_NEAREST FP_RND_RN
#define IEEE_FP_UP      FP_RND_RP
#define IEEE_FP_DOWN    FP_RND_RM
#define IEEE_FP_ZERO    FP_RND_RZ


FiWord
fiDoubleHexPrintToString(FiWord f)
{
  sprintf(double_hex_print_string,"%0.8x %0.8x",*((int *)f+1),*(int *)f);
  return (FiWord) double_hex_print_string;
}
	

#include <signal.h>
void
fiInitialiseFpu()
{
  signal(SIGFPE,SIG_IGN);
}

FiWord
fiIeeeGetRoundingMode()
{
  unsigned int tmp;
  
  tmp = read_rnd();
  switch(tmp) {
  case IEEE_FP_NEAREST :
    return fiRoundNearest();
  case IEEE_FP_UP :
    return fiRoundUp();
  case IEEE_FP_DOWN:
    return fiRoundDown();
  case IEEE_FP_ZERO:
    return fiRoundZero();
  }
}

FiWord
fiIeeeSetRoundingMode(FiWord s)
{
  unsigned int tmp;
  switch(s) {
  case fiRoundNearest():
    tmp = write_rnd(IEEE_FP_NEAREST);
    break;
  case fiRoundUp():
    tmp = write_rnd(IEEE_FP_UP);
    break;
  case fiRoundDown():
    tmp = write_rnd(IEEE_FP_DOWN);
    break;
  case fiRoundZero():
    tmp = write_rnd(IEEE_FP_ZERO);
    break;
  }
  switch(tmp) {
  case IEEE_FP_NEAREST :
    return fiRoundNearest();
  case IEEE_FP_UP :
    return fiRoundUp();
  case IEEE_FP_DOWN:
    return fiRoundDown();
  case IEEE_FP_ZERO:
    return fiRoundZero();
  }
}


FiWord
fiIeeeGetExceptionStatus()
{
  unsigned long fpc;
  FiWord result=0;

  fpc = ieee_get_fp_control();
  /* translate result */
  result =  (fpc & IEEE_STATUS_INV) ? 16 :0;
  result |= (fpc & IEEE_STATUS_OVF) ?  8 :0;
  result |= (fpc & IEEE_STATUS_UNF) ?  4 :0;
  result |= (fpc & IEEE_STATUS_DZE) ?  2 :0;
  result |= (fpc & IEEE_STATUS_INE) ?  1 :0;
  return result;
}

FiWord
fiIeeeSetExceptionStatus(FiWord s)
{
  unsigned long result=0,fpc,set=0;
  /* translate input */
  set =  (s & 16) ? IEEE_STATUS_INV :0;
  set |= (s & 8)  ? IEEE_STATUS_OVF :0;
  set |= (s & 4)  ? IEEE_STATUS_UNF :0;
  set |= (s & 2)  ? IEEE_STATUS_DZE :0;
  set |= (s & 1)  ? IEEE_STATUS_INE :0;
  fpc = ieee_get_fp_control();
  /* translate result */
  result =  (fpc & IEEE_STATUS_INV) ? 16 :0;
  result |= (fpc & IEEE_STATUS_OVF) ?  8 :0;
  result |= (fpc & IEEE_STATUS_UNF) ?  4 :0;
  result |= (fpc & IEEE_STATUS_DZE) ?  2 :0;
  result |= (fpc & IEEE_STATUS_INE) ?  1 :0;
  /* clear all status bits */
  fpc &= ~IEEE_STATUS_MASK;
  /* set them */
  fpc |= set; 
  ieee_set_fp_control(fpc);
  return result;
}

FiWord
fiIeeeGetEnabledExceptions()
{
  unsigned long fpc;
  FiWord result=0;

  fpc = ieee_get_fp_control();
  /* translate result */
  result =  (fpc & IEEE_TRAP_ENABLE_INV) ? 16 :0;
  result |= (fpc & IEEE_TRAP_ENABLE_OVF) ?  8 :0;
  result |= (fpc & IEEE_TRAP_ENABLE_UNF) ?  4 :0;
  result |= (fpc & IEEE_TRAP_ENABLE_DZE) ?  2 :0;
  result |= (fpc & IEEE_TRAP_ENABLE_INE) ?  1 :0;
  return result;

}

FiWord
fiIeeeSetEnabledExceptions(FiWord s)
{
  unsigned long result=0,fpc,set=0;
  /* translate input */
  set =  (s & 16) ? IEEE_TRAP_ENABLE_INV :0;
  set |= (s & 8)  ? IEEE_TRAP_ENABLE_OVF :0;
  set |= (s & 4)  ? IEEE_TRAP_ENABLE_UNF :0;
  set |= (s & 2)  ? IEEE_TRAP_ENABLE_DZE :0;
  set |= (s & 1)  ? IEEE_TRAP_ENABLE_INE :0;
  fpc = ieee_get_fp_control();
  /* translate result */
  result =  (fpc & IEEE_TRAP_ENABLE_INV) ? 16 :0;
  result |= (fpc & IEEE_TRAP_ENABLE_OVF) ?  8 :0;
  result |= (fpc & IEEE_TRAP_ENABLE_UNF) ?  4 :0;
  result |= (fpc & IEEE_TRAP_ENABLE_DZE) ?  2 :0;
  result |= (fpc & IEEE_TRAP_ENABLE_INE) ?  1 :0;
  /* clear all status bits */
  fpc &= ~IEEE_TRAP_ENABLE_MASK;
  /* set them */
  fpc |= set; 
  ieee_set_fp_control(fpc);
  return result;
}

#elif defined(__hpux) && defined(__hppa)
/* HPPA HP/UX CC */

#if defined(_PA_RISC2_0)
#include <fenv.h>


FiWord 
fiDoubleHexPrintToString(FiWord f) 
{
  sprintf(double_hex_print_string,"%0.8x %0.8x",*(int *)f,*((int *)f+1));
  return (FiWord) double_hex_print_string;
}


void
fiInitialiseFpu()
{
}

FiWord
fiIeeeGetRoundingMode()
{
  unsigned int tmp;
  
  tmp = fegetround();
  switch(tmp) {
  case FE_TONEAREST :
    return fiRoundNearest();
  case FE_UPWARD :
    return fiRoundUp();
  case FE_DOWNWARD:
    return fiRoundDown();
  case FE_TOWARDZERO:
    return fiRoundZero();
  }
}

FiWord
fiIeeeSetRoundingMode(FiWord s)
{
  unsigned int tmp;
  switch(s) {
  case fiRoundNearest():
    tmp = fesetround(FE_TONEAREST);
    break;
  case fiRoundUp():
    tmp = fesetround(FE_UPWARD);
    break;
  case fiRoundDown():
    tmp = fesetround(FE_DOWNWARD);
    break;
  case fiRoundZero():
    tmp = fesetround(FE_TOWARDZERO);
    break;
  }
  switch (tmp) {
  case FE_TONEAREST:
    return fiRoundNearest();
  case FE_UPWARD:
    return fiRoundUp();
  case FE_DOWNWARD:
    return fiRoundDown();
  case FE_TOWARDZERO:
    return fiRoundZero();
  }
}

/*
      The fegettrapenable() function returns the bitwise OR of the exception
      macros corresponding to the currently set exception trap enable bits.
      The macros are FE_INEXACT, FE_DIVBYZERO, FE_UNDERFLOW, FE_OVERFLOW,
      and FE_INVALID.  FE_ALL_EXCEPT represents all the exceptions.
*/

FiWord
fiIeeeGetEnabledExceptions()
{
  int fm;
  FiWord result=0;

  fm = fegettrapenable();
  result =  (fm & FE_INVALID) ? 16 :0;
  result |= (fm & FE_OVERFLOW) ? 8  :0;
  result |= (fm & FE_UNDERFLOW) ? 4  :0;
  result |= (fm & FE_DIVBYZERO)  ? 2  :0;
  result |= (fm & FE_INEXACT) ? 1  :0;
  return result;
 }

/*
      The fesettrapenable() function sets the exception trap enable bits
      indicated by the argument excepts.  The argument can be constructed as
      a bitwise OR of the exception macros: FE_INEXACT, FE_DIVBYZERO,
      FE_UNDERFLOW, FE_OVERFLOW, and FE_INVALID.  FE_ALL_EXCEPT represents
      all the exceptions.

      The function also clears the trap enable bits for any exceptions not
      indicated by the argument excepts.
*/

FiWord
fiIeeeSetEnabledExceptions(FiWord s)
{
  FiWord result = 0;
  int set,fm;

  result = fiIeeeGetEnabledExceptions();
  set = (s & 16) ? FE_INVALID :0;
  set |= (s & 8) ? FE_OVERFLOW :0;
  set |= (s & 4) ? FE_UNDERFLOW :0;
  set |= (s & 2) ? FE_DIVBYZERO  :0;
  set |= (s & 1) ? FE_INEXACT :0;
  fesettrapenable(set);
  return result;
}

/*
      The fetestexcept() function determines which of a specified subset of
      the exception flags are currently set.  The excepts argument specifies
      the exception flags to be queried.  The argument can be constructed as
      a bitwise OR of the exception macros: FE_INEXACT, FE_DIVBYZERO,
      FE_UNDERFLOW, FE_OVERFLOW, and FE_INVALID.  FE_ALL_EXCEPT represents
      all the exceptions.
*/
FiWord
fiIeeeGetExceptionStatus()
{
  fexcept_t fm;
  FiWord result=0;

  fegetexceptflag(&fm,FE_ALL_EXCEPT);
  result =  (fm & FE_INVALID) ? 16 :0;
  result |= (fm & FE_OVERFLOW) ? 8  :0;
  result |= (fm & FE_UNDERFLOW) ? 4  :0;
  result |= (fm & FE_DIVBYZERO)  ? 2  :0;
  result |= (fm & FE_INEXACT) ? 1  :0;
  return result;
 }

FiWord
fiIeeeSetExceptionStatus(FiWord s)
{
  FiWord result = 0;
  fexcept_t set;

  result = fiIeeeGetExceptionStatus();
  set = (s & 16) ? FE_INVALID :0;
  set |= (s & 8) ? FE_OVERFLOW:0;
  set |= (s & 4) ? FE_UNDERFLOW:0;
  set |= (s & 2) ? FE_DIVBYZERO:0;
  set |= (s & 1) ? FE_INEXACT:0;
  fesetexceptflag(&set,FE_ALL_EXCEPT);
  return result;
}


#else
#include <math.h>
#define IEEE_FP_NEAREST FP_RN
#define IEEE_FP_UP      FP_RP
#define IEEE_FP_DOWN    FP_RM
#define IEEE_FP_ZERO    FP_RZ


FiWord 
fiDoubleHexPrintToString(FiWord f) 
{
  sprintf(double_hex_print_string,"%0.8x %0.8x",*(int *)f,*((int *)f+1));
  return (FiWord) double_hex_print_string;
}


void
fiInitialiseFpu()
{
}

FiWord
fiIeeeGetRoundingMode()
{
  unsigned int tmp;
  
  tmp = fpgetround();
  switch(tmp) {
  case IEEE_FP_NEAREST :
    return fiRoundNearest();
  case IEEE_FP_UP :
    return fiRoundUp();
  case IEEE_FP_DOWN:
    return fiRoundDown();
  case IEEE_FP_ZERO:
    return fiRoundZero();
  }
}

FiWord
fiIeeeSetRoundingMode(FiWord s)
{
  unsigned int tmp;
  switch(s) {
  case fiRoundNearest():
    tmp = fpsetround(IEEE_FP_NEAREST);
    break;
  case fiRoundUp():
    tmp = fpsetround(IEEE_FP_UP);
    break;
  case fiRoundDown():
    tmp = fpsetround(IEEE_FP_DOWN);
    break;
  case fiRoundZero():
    tmp = fpsetround(IEEE_FP_ZERO);
    break;
  }
  switch (tmp) {
  case IEEE_FP_NEAREST :
    return fiRoundNearest();
  case IEEE_FP_UP :
    return fiRoundUp();
  case IEEE_FP_DOWN:
    return fiRoundDown();
  case IEEE_FP_ZERO:
    return fiRoundZero();
  }
}

FiWord
fiIeeeGetEnabledExceptions()
{
  fp_except fm;
  FiWord result=0;

  fm = fpgetmask();
  result =  (fm & FP_X_INV) ? 16 :0;
  result |= (fm & FP_X_OFL) ? 8  :0;
  result |= (fm & FP_X_UFL) ? 4  :0;
  result |= (fm & FP_X_DZ)  ? 2  :0;
  result |= (fm & FP_X_IMP) ? 1  :0;
  return result;
 }

FiWord
fiIeeeSetEnabledExceptions(FiWord s)
{
  FiWord result = 0;
  fp_except set,fm;

  set = (s & 16) ? FP_X_INV :0;
  set |= (s & 8) ? FP_X_OFL :0;
  set |= (s & 4) ? FP_X_UFL :0;
  set |= (s & 2) ? FP_X_DZ  :0;
  set |= (s & 1) ? FP_X_IMP :0;
  fm = fpsetmask(set);
  result =  (fm & FP_X_INV) ? 16 :0;
  result |= (fm & FP_X_OFL) ? 8  :0;
  result |= (fm & FP_X_UFL) ? 4  :0;
  result |= (fm & FP_X_DZ)  ? 2  :0;
  result |= (fm & FP_X_IMP) ? 1  :0;
  return result;
}

FiWord
fiIeeeGetExceptionStatus()
{
  fp_except fm;
  FiWord result=0;

  fm = fpgetsticky();
  result =  (fm & FP_X_INV) ? 16 :0;
  result |= (fm & FP_X_OFL) ? 8  :0;
  result |= (fm & FP_X_UFL) ? 4  :0;
  result |= (fm & FP_X_DZ)  ? 2  :0;
  result |= (fm & FP_X_IMP) ? 1  :0;
  return result;
 }

FiWord
fiIeeeSetExceptionStatus(FiWord s)
{
  FiWord result = 0;
  fp_except set,fm;

  set = (s & 16) ? FP_X_INV :0;
  set |= (s & 8) ? FP_X_OFL :0;
  set |= (s & 4) ? FP_X_UFL :0;
  set |= (s & 2) ? FP_X_DZ  :0;
  set |= (s & 1) ? FP_X_IMP :0;
  fm = fpsetsticky(set);
  result =  (fm & FP_X_INV) ? 16 :0;
  result |= (fm & FP_X_OFL) ? 8  :0;
  result |= (fm & FP_X_UFL) ? 4  :0;
  result |= (fm & FP_X_DZ)  ? 2  :0;
  result |= (fm & FP_X_IMP) ? 1  :0;
  return result;
}
#endif 


#elif defined(__sgi) && defined(__host_mips) && defined(__mips) 
/* MIPS1 IRIX5 CC */
/* MIPS3 IRIX6 CC */
#include <sys/fpu.h>
#define IEEE_FP_NEAREST ROUND_TO_NEAREST
#define IEEE_FP_UP      ROUND_TO_PLUS_INFINITY
#define IEEE_FP_DOWN    ROUND_TO_MINUS_INFINITY
#define IEEE_FP_ZERO    ROUND_TO_ZERO

FiWord 
fiDoubleHexPrintToString(FiWord f) 
{
  sprintf(double_hex_print_string,"%0.8x %0.8x",*(int *)f,*((int *)f+1));
  return (FiWord) double_hex_print_string;
}


void
fiInitialiseFpu()
{
}

FiWord
fiIeeeGetEnabledExceptions()
{
 
  union fpc_csr fm;
  FiWord result=0;

  fm.fc_word = get_fpc_csr();
  result =  (fm.fc_struct.en_invalid)  ? 16 :0;
  result |= (fm.fc_struct.en_overflow) ? 8  :0;
  result |= (fm.fc_struct.en_underflow)? 4  :0;
  result |= (fm.fc_struct.en_divide0)  ? 2  :0;
  result |= (fm.fc_struct.en_inexact)  ? 1  :0;
  return result;
 }

FiWord
fiIeeeSetEnabledExceptions(FiWord s)
{
  FiWord result = 0;
  union fpc_csr set,fm;

  set.fc_word = get_fpc_csr();
  fm = set;
  set.fc_struct.en_invalid  = (s & 16) ? 1 :0;
  set.fc_struct.en_overflow = (s & 8) ?  1 :0;
  set.fc_struct.en_underflow= (s & 4) ?  1 :0;
  set.fc_struct.en_divide0  = (s & 2) ?  1 :0;
  set.fc_struct.en_inexact  = (s & 1) ?  1 :0;
  result =  (fm.fc_struct.en_invalid)  ? 16 :0;
  result |= (fm.fc_struct.en_overflow) ? 8  :0;
  result |= (fm.fc_struct.en_underflow)? 4  :0;
  result |= (fm.fc_struct.en_divide0)  ? 2  :0;
  result |= (fm.fc_struct.en_inexact)  ? 1  :0;
  set_fpc_csr(set.fc_word);
  return result;
}

FiWord
fiIeeeGetExceptionStatus()
{
  union fpc_csr fm;
  FiWord result=0;

  fm.fc_word = get_fpc_csr();

  result =  (fm.fc_struct.se_invalid)  ? 16 :0;
  result |= (fm.fc_struct.se_overflow) ? 8  :0;
  result |= (fm.fc_struct.se_underflow)? 4  :0;
  result |= (fm.fc_struct.se_divide0)  ? 2  :0;
  result |= (fm.fc_struct.se_inexact)  ? 1  :0;
  return result;

 }

FiWord
fiIeeeSetExceptionStatus(FiWord s)
{
  FiWord result = 0;
  union fpc_csr set,fm;

  set.fc_word = get_fpc_csr();
  fm = set;
  set.fc_struct.se_invalid  = (s & 16) ? 1 :0;
  set.fc_struct.se_overflow = (s & 8) ?  1 :0;
  set.fc_struct.se_underflow= (s & 4) ?  1 :0;
  set.fc_struct.se_divide0  = (s & 2) ?  1 :0;
  set.fc_struct.se_inexact  = (s & 1) ?  1 :0;
  result =  (fm.fc_struct.se_invalid)  ? 16 :0;
  result |= (fm.fc_struct.se_overflow) ? 8  :0;
  result |= (fm.fc_struct.se_underflow)? 4  :0;
  result |= (fm.fc_struct.se_divide0)  ? 2  :0;
  result |= (fm.fc_struct.se_inexact)  ? 1  :0;
  set_fpc_csr(set.fc_word);
  return result;
}

FiWord
fiIeeeGetRoundingMode()
{
  union fpc_csr fm;
  FiWord result=0;

  fm.fc_word = get_fpc_csr();
  result = fm.fc_struct.rounding_mode;
  switch(result) {
  case IEEE_FP_NEAREST :
    return fiRoundNearest();
  case IEEE_FP_UP :
    return fiRoundUp();
  case IEEE_FP_DOWN:
    return fiRoundDown();
  case IEEE_FP_ZERO:
    return fiRoundZero();
  }
}

FiWord
fiIeeeSetRoundingMode(FiWord x)
{
  FiWord result = 0;
  union fpc_csr set,fm;

  set.fc_word = get_fpc_csr();
  result = fm.fc_struct.rounding_mode;
  switch(x) {
  case fiRoundNearest():
    set.fc_struct.rounding_mode  = IEEE_FP_NEAREST;
    break;
  case fiRoundUp():
    set.fc_struct.rounding_mode  = IEEE_FP_UP;
    break;
  case fiRoundDown():
    set.fc_struct.rounding_mode  = IEEE_FP_DOWN;
    break;
  case fiRoundZero():
    set.fc_struct.rounding_mode  = IEEE_FP_ZERO;
    break;
  }
  set_fpc_csr(set.fc_word);
  switch(result) {
  case IEEE_FP_NEAREST :
    return fiRoundNearest(); break;
  case IEEE_FP_UP :
    return fiRoundUp(); break;
  case IEEE_FP_DOWN:
    return fiRoundDown(); break;
  case IEEE_FP_ZERO:
    return fiRoundZero(); break;
  }
  /* stupid compiler... */
  return -1;
}



#elif defined(_AIX41) || defined(_AIX32)
/* RS/6000 AIX4.1 xlc */
/* RS/6000 AIX3.2 xlc */
#define _ALL_SOURCE 
#include <float.h>
#include <fpxcp.h>
#include <fptrap.h>
#define IEEE_FP_NEAREST FP_RND_RN
#define IEEE_FP_UP      FP_RND_RP
#define IEEE_FP_DOWN    FP_RND_RM
#define IEEE_FP_ZERO    FP_RND_RZ

#define DBL_HEX_PRINT_TO_STRING(f)  (FiWord) DBL_HEX_PRINT_TO_STRING_FUN_BIG((double *)f)


FiWord
fiDoubleHexPrintToString (FiWord f) 
{
  sprintf(double_hex_print_string,"%0.8x %0.8x",*(int *)f,*((int *)f+1));
  return (FiWord) double_hex_print_string;
}


void
fiInitialiseFpu()
{
}

FiWord
fiIeeeGetRoundingMode()
{
  unsigned int tmp;
  
  tmp = fp_read_rnd();
  switch(tmp) {
  case IEEE_FP_NEAREST :
    return fiRoundNearest();
  case IEEE_FP_UP :
    return fiRoundUp();
  case IEEE_FP_DOWN:
    return fiRoundDown();
  case IEEE_FP_ZERO:
    return fiRoundZero();
  }
}

FiWord
fiIeeeSetRoundingMode(FiWord s)
{
  unsigned int tmp;
  switch(s) {
  case fiRoundNearest():
    tmp = fp_swap_rnd(IEEE_FP_NEAREST);
    break;
  case fiRoundUp():
    tmp = fp_swap_rnd(IEEE_FP_UP);
    break;
  case fiRoundDown():
    tmp = fp_swap_rnd(IEEE_FP_DOWN);
    break;
  case fiRoundZero():
    tmp = fp_swap_rnd(IEEE_FP_ZERO);
    break;
  }
  switch(tmp) {
  case IEEE_FP_NEAREST :
    return fiRoundNearest();
  case IEEE_FP_UP :
    return fiRoundUp();
  case IEEE_FP_DOWN:
    return fiRoundDown();
  case IEEE_FP_ZERO:
    return fiRoundZero();
  }
}




FiWord
fiIeeeGetEnabledExceptions()
{
 
  FiWord result=0;

  result =  fp_is_enabled( TRP_INVALID)    ? 16 :0;
  result |= fp_is_enabled( TRP_OVERFLOW)   ? 8  :0;
  result |= fp_is_enabled( TRP_UNDERFLOW)  ? 4  :0;
  result |= fp_is_enabled( TRP_DIV_BY_ZERO)? 2  :0;
  result |= fp_is_enabled( TRP_INEXACT)    ? 1  :0;
  return result;
 }

FiWord
fiIeeeSetEnabledExceptions(FiWord s)
{
  FiWord result = 0;
  fptrap_t set =0;

  result =  fp_is_enabled( TRP_INVALID)    ? 16 :0;
  result |= fp_is_enabled( TRP_OVERFLOW)   ? 8  :0;
  result |= fp_is_enabled( TRP_UNDERFLOW)  ? 4  :0;
  result |= fp_is_enabled( TRP_DIV_BY_ZERO)? 2  :0;
  result |= fp_is_enabled( TRP_INEXACT)    ? 1  :0;

  set  = (s & 16) ? TRP_INVALID     :0;
  set |= (s & 8)  ? TRP_OVERFLOW    :0;
  set |= (s & 4)  ? TRP_UNDERFLOW   :0;
  set |= (s & 2)  ? TRP_DIV_BY_ZERO :0;
  set |= (s & 1)  ? TRP_INEXACT     :0;

  fp_disable_all();
  fp_enable(set);
  /* we choose the precise trapping mode */
  if (fp_any_enable()) fp_trap(FP_TRAP_SYNC);
  return result;
}

FiWord
fiIeeeGetExceptionStatus()
{
  FiWord result=0;

  result =  fp_invalid_op() ? 16 :0;
  result |= fp_overflow()   ? 8  :0;
  result |= fp_underflow()  ? 4  :0;
  result |= fp_divbyzero()  ? 2  :0;
  result |= fp_inexact()    ? 1  :0;
  return result;
 }

FiWord
fiIeeeSetExceptionStatus(FiWord s)
{
  FiWord result = 0;
  fptrap_t set,fm =0;

  
  set  = (s & 16) ? FP_INVALID     :0;
  set |= (s & 8)  ? FP_OVERFLOW    :0;
  set |= (s & 4)  ? FP_UNDERFLOW   :0;
  set |= (s & 2)  ? FP_DIV_BY_ZERO :0;
  set |= (s & 1)  ? FP_INEXACT     :0;
  fm = fp_swap_flag(set);
  result =  (fm & FP_INVALID)     ? 16 :0;
  result |= (fm & FP_OVERFLOW)    ? 8  :0;
  result |= (fm & FP_UNDERFLOW)   ? 4  :0;
  result |= (fm & FP_DIV_BY_ZERO) ? 2  :0;
  result |= (fm & FP_INEXACT)     ? 1  :0;
  return result;
}



#elif defined(OS_WIN32) && (defined(CC_MICROSOFT) || defined(CC_GNU))
/* MSVC */
FiWord
fiDoubleHexPrintToString(FiWord  f)
{
#if 0
	sprintf(double_hex_print_string,"%0.8x %0.8x",*((int *)f+1),*(int *)f);
#endif
  sprintf(double_hex_print_string,"%.8x %.8x",*((int *)f+1),*(int *)f);
  return (FiWord) double_hex_print_string;
}
	
void
fiInitialiseFpu()
{
}

FiWord
fiIeeeGetRoundingMode()
{
        return 0;
}

FiWord
fiIeeeSetRoundingMode(FiWord s)
{
        return 0;
}


FiWord
fiIeeeGetExceptionStatus()
{
        return 0;              
}

FiWord
fiIeeeSetExceptionStatus(FiWord s)
{
        return 0;
}

FiWord
fiIeeeGetEnabledExceptions()
{
        return 0;
}

FiWord
fiIeeeSetEnabledExceptions(FiWord s)
{
        return 0;
}
#else

#if EDIT_1_0_n1_07
/* Unknown platform. */
FiWord
fiDoubleHexPrintToString(FiWord  f)
{
  sprintf(double_hex_print_string,"%.8x %.8x",*((int *)f+1),*(int *)f);
  return (FiWord) double_hex_print_string;
}
	
void
fiInitialiseFpu()
{
}

FiWord
fiIeeeGetRoundingMode()
{
        return 0;
}

FiWord
fiIeeeSetRoundingMode(FiWord s)
{
        return 0;
}


FiWord
fiIeeeGetExceptionStatus()
{
        return 0;              
}

FiWord
fiIeeeSetExceptionStatus(FiWord s)
{
        return 0;
}

FiWord
fiIeeeGetEnabledExceptions()
{
        return 0;
}

FiWord
fiIeeeSetEnabledExceptions(FiWord s)
{
        return 0;
}
#else  /* EDIT_1_0_n1_07 */
#error  UNKNOWN platform in foam_cfp.c (see ieeecontrol.h?)
#endif /* EDIT_1_0_n1_07 */

#endif

