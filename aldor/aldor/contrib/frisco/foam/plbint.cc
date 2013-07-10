/****************************************************************************
 *
 * plbint.cc: CAR Interface to PoSSo BigInt Package (GMP).
 *
 * Copyright Pietro Iglio 1996
 * email: iglio@posso._mp_dm.unipi.it, iglio@di.unipi.it
 *
 ****************************************************************************/


extern "C" {
#include "foam_c.h"
#include "string.h"
}

#define PL_CMM
#ifdef _MSC_VER
#include "strstream"
using namespace std;
#include "BigInt.hh"
#else
#include "strstream.h"
#endif

#include "BigInt.H"

#define PL_BI(b)	((PL_BigInt *) (b))
#define PL_GMP(b)	(&(PL_BI(b)->z))

#define PL_GMP_SIZE(b)	(PL_GMP(b)->_mp_size)
#define PL_GMP_LIMBS(b)	(PL_GMP(b)->_mp_d)

#define PL_BIPTR(bPtr)	((PL_BigInt **) (bPtr))
#define PL_GMPPTR(bPtr)	(&(*(PL_BIPTR(bPtr)))->z)

#define SET_ABS(lhs, i)	{ (lhs) = (i); if ((lhs) >= 0) ; else (lhs) = -(lhs); }

/******************************************************************************
 * NOTE:
 * - Il discorso di precalcolare la size per ora non e' stato usato perche'
 *    new (size) PL_BigInt non e' corretto.
 *
 *****************************************************************************/

#include "plbint.h"

void
fiBIntInit()
{

}


FiSInt
fiFormatBInt(FiBInt x, FiWord s, FiSInt i)
{
	char*	buf;
	FiWord	l;

	buf = (char*) fiBIntToString(x);
	l   = (FiWord) strlen(buf);
	if (l+i > (FiWord) strlen((char*) s)) {
		fprintf(stderr, "Error -- not enough space!\n");
		exit(1);
	}
	memmove((char*) s + i, buf, l+1);
	return (FiSInt) l + i;
}

FiBInt
fiSIntToBInt(FiSInt i)
{
	return (FiBInt) new PL_BigInt(i);
}

FiSInt
fiBIntToSInt(FiBInt b)
{
	return (FiSInt) mpz_get_si(PL_GMP(b));
}

FiSFlo
fiBIntToSFlo(FiBInt b)
{
	return (FiSFlo) atof(fiBIntToString(b));
}

FiDFlo
fiBIntToDFlo(FiBInt b)
{
	return (FiDFlo) atof(fiBIntToString(b));
}

FiBInt
fiBIntFrInt(long li)
{
	return (FiBInt) new PL_BigInt(li);
}

FiBInt
fiBInt0(void)
{
	static FiBInt bInt0 = NULL;
	if (bInt0)
		return bInt0;
	else
		return (bInt0 = (FiBInt) new PL_BigInt(0));
}

FiBInt
fiBInt1(void)
{
	static FiBInt bInt1 = NULL;
	if (bInt1)
		return bInt1;
	else
		return (bInt1 = (FiBInt) new PL_BigInt(1));
}

FiBInt
fiBIntNew(long li)
{
	return (FiBInt) new PL_BigInt(li);
}

FiBool
fiBIntIsZero(FiBInt b)
{
  return PL_BI(b)->z._mp_size == 0;
}
FiBool
fiBIntIsNeg(FiBInt b)
{
  return PL_BI(b)->z._mp_size < 0;
}
FiBool
fiBIntIsPos(FiBInt b)
{
  return PL_BI(b)->z._mp_size > 0;
}
FiBool
fiBIntIsEven (FiBInt b)
{
/*  From BigInt.inl:
 *  return PL_BI(b)->z._mp_size == 0 || (PL_BI(b)->z._mp_d[0] ^ ((mp_limb_t) 0));
 *  DOESN'T WORK (BUG?)
 */
  return PL_BI(b)->z._mp_size == 0 || !(PL_BI(b)->z._mp_d[0] & ((mp_limb_t) 1));
}

FiBool
fiBIntIsOdd (FiBInt b )
{
  return PL_BI(b)->z._mp_size != 0 && (PL_BI(b)->z._mp_d[0] & ((mp_limb_t) 1));
}

FiBool
fiBIntIsSingle (FiBInt b )
{
  mp_size_t size;

  SET_ABS(size, PL_BI(b)->z._mp_size);

  return (FiBool) size <= 1;
}


FiBool
fiBIntLE(FiBInt a, FiBInt b)
{
	return (FiBool) (mpz_cmp(PL_GMP(a), PL_GMP(b)) <= 0);
}

FiBool
fiBIntEQ(FiBInt a, FiBInt b)
{
	return (FiBool) (mpz_cmp(PL_GMP(a), PL_GMP(b)) == 0);
}

FiBool
fiBIntLT(FiBInt a, FiBInt b)
{
	return (FiBool) (mpz_cmp(PL_GMP(a), PL_GMP(b)) < 0);
}

FiBool
fiBIntNE(FiBInt a, FiBInt b)
{
	return (FiBool) (mpz_cmp(PL_GMP(a), PL_GMP(b)) != 0);
}

FiBInt
fiBIntNegate(FiBInt b)
{
	PL_BigInt * r = new PL_BigInt(*PL_BI(b));
	r->negate();
	return (FiBInt) r;
}

FiBInt
fiBIntPlus1 (FiBInt b)
{
	PL_BigInt * r = new PL_BigInt;
        mpz_add_ui(PL_GMP(r), PL_GMP(b), 1);
	return (FiBInt) r;
}

FiBInt
fiBIntMinus1 (FiBInt b)
{
	PL_BigInt * r = new PL_BigInt;
        mpz_sub_ui(PL_GMP(r), PL_GMP(b), 1);
	return (FiBInt) r;
}

FiBInt
fiBIntPlus(FiBInt a1, FiBInt a2)
{
	//int 	r1;
	//int 	r2;
	//int	size;

	PL_BigInt * r;
	// SET_ABS(r1, PL_BI(a1)->z._mp_size);
	// SET_ABS(r2, PL_BI(a2)->z._mp_size);
	// size = (r1 > r2 ? r1 : r2) + 1;
	r = new PL_BigInt();
        mpz_add(PL_GMP(r), PL_GMP(a1), PL_GMP(a2));

	return (FiBInt) r;
}

FiBInt
fiBIntMinus(FiBInt a1, FiBInt a2)
{
	PL_BigInt * r = new PL_BigInt;
        mpz_sub(PL_GMP(r), PL_GMP(a1), PL_GMP(a2));
	return (FiBInt) r;
}

FiBInt
fiBIntTimes(FiBInt a1, FiBInt a2)
{
	PL_BigInt * r = new PL_BigInt;
        mpz_mul(PL_GMP(r), PL_GMP(a1), PL_GMP(a2));
	return (FiBInt) r;
}

FiBInt
fiBIntTimesPlus(FiBInt a1, FiBInt a2, FiBInt a3)
{
	PL_BigInt * r = new PL_BigInt;
        mpz_mul(PL_GMP(r), PL_GMP(a1), PL_GMP(a2));
	mpz_add(PL_GMP(r), PL_GMP(r), PL_GMP(a3));
	return (FiBInt) r;
}

FiBInt
fiBIntShiftUp(FiBInt a, int n)
{
  if (n < 0) {
	  printf("ShiftUp: Negative shift");
	  exit(0);
  }

  PL_BigInt * r = new PL_BigInt;

  mpz_mul_2exp(PL_GMP(r), PL_GMP(a), (unsigned int) n);
  return (FiBInt) r;
}

FiBInt
fiBIntShiftDn(FiBInt a, int n)
{
  if (n < 0) {
	  printf("ShiftDn: Negative shift");
	  exit(0);
  }

  PL_BigInt * r = new PL_BigInt;

  mpz_tdiv_q_2exp(PL_GMP(r), PL_GMP(a), (unsigned int) n);
  return (FiBInt) r;
}

FiBInt
fiBIntSIPower(FiBInt a, FiSInt exp)
{
  PL_BigInt * r = new PL_BigInt;

  assert(exp >= 0);
  mpz_pow_ui(PL_GMP(r), PL_GMP(a), exp);
  return (FiBInt) r;
}

FiBInt
fiBIntBIPower(FiBInt a, FiBInt exp)
{
  PL_BigInt * r = new PL_BigInt;

  assert(fiBIntIsSingle(exp));
  assert(PL_GMP_SIZE(exp) >= 0);

  int i = fiBIntToSInt(exp);

  mpz_pow_ui(PL_GMP(r), PL_GMP(a), i);
  return (FiBInt) r;
}

FiSInt
fiBIntLength(FiBInt a)
{
	return mpz_sizeinbase(PL_GMP(a), 2);
}

FiBInt
fiBIntMod(FiBInt a, FiBInt b)
{
	PL_BigInt * r = new PL_BigInt;

	mpz_mod(PL_GMP(r), PL_GMP(a), PL_GMP(b));

	return (FiBInt) r;
}

FiBInt
fiBIntQuo(FiBInt a, FiBInt b)
{
	PL_BigInt * r = new PL_BigInt;

	mpz_mdiv(PL_GMP(r), PL_GMP(a), PL_GMP(b));

	return (FiBInt) r;
}

FiBInt
fiBIntRem(FiBInt a, FiBInt b)
{
	PL_BigInt * r = new PL_BigInt;
	
	mpz_mmod(PL_GMP(r), PL_GMP(a), PL_GMP(b));

	return (FiBInt) r;
}

void
fiBIntDivide(FiBInt num, FiBInt den, FiBInt* quot , FiBInt* rem)
{
	*quot = (FiBInt) new PL_BigInt;
	*rem = (FiBInt) new PL_BigInt;

   	mpz_divmod(PL_GMPPTR(quot), PL_GMPPTR(rem), PL_GMP(num), PL_GMP(den));
}

FiBInt
fiBIntGcd(FiBInt a, FiBInt b)
{
	PL_BigInt * r = new PL_BigInt;

	mpz_gcd(PL_GMP(r), PL_GMP(a), PL_GMP(b));

	return (FiBInt) r;
}

char *
fiBIntToString(FiBInt a)
{
        std::ostrstream obuf;

        obuf << *PL_BI(a) << '\0';

        return obuf.str();
}

FiBInt
fiArrToBInt(FiWord s)
{
	return (FiBInt) new PL_BigInt((char *) s);
}

void
fiScanBInt(FiWord s, FiSInt i, FiBInt* r0, FiSInt* r1)
{
	*r0 = (FiBInt) new PL_BigInt((char*) s + i);
	/*!! The following is wrong.  We need bintFrString to tell us where
	  the integer ended. */
	*r1  = strlen((char*) s + i) + i;
	return;
}

// !!Easy-but-none-too-fast
FiBInt
fiBIntFrPlacev(int isNeg, unsigned long placec, FiBIntS *placev)
{
	FiBInt x = fiBInt0();
	int i;
	for (i=placec-1 ; i >= 0; i--) {
		/* 8 bits per byte... */
		x = fiBIntPlus(fiBIntShiftUp(x, (FiSInt)(8*sizeof(FiBIntS))),
			       fiBIntFrInt(placev[i]));
	}
	if (isNeg)
		x = fiBIntNegate(x);
	return x;
}

// NB: Not deleted code, but a great example of what goes wrong when
// you test on small-endian machines, and still expect this sort
// of bit-twiddling to work.

//#define BYTES_PER_MP_LIMB  sizeof(mp_limb_t)
//extern void *  (*_mp_allocate_func) (size_t);
//
//#if 0
//FiBInt
//fiBIntFrPlacev(int isNeg, unsigned long placec , FiBIntS * placev)
//{	
//        PL_BigInt* x = new PL_BigInt;
//	int        bytes = placec * sizeof(FiBIntS) /* + 2 */;
//        mp_size_t  limbs = (sizeof(mp_limb_t) - 1 + bytes) / sizeof(mp_limb_t);
//	int        i, pad;
//
//	///////////////////////////////////////////////////
//	XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//
//        PL_GMP(x)->_mp_alloc = limbs;
//	PL_GMP(x)->_mp_d = (mp_ptr) (*_mp_allocate_func) (limbs * BYTES_PER_MP_LIMB);
//	PL_GMP(x)->_mp_size = (isNeg ? -limbs : limbs);
//
//	char* limbv = (char*) PL_GMP(x)->_mp_d;
//	char* datav = (char*) placev;
//
//	if ((pad = (limbs * sizeof(mp_limb_t) - bytes)) > 0)
//	        for (i = 0; i < pad; i++)
//	                *limbv++ = 0;
//
//	for (i = 0; i < bytes /* - 2 */; i++)
//	        *limbv++ = *datav++;
//
////	*limbv++ = 0;
////	*limbv++ = 0;
//
//	return (FiBInt) fiBIntShiftDn((FiBInt)x, 16);
//}
//#endif
//
//FiBInt
//fiBIntFrPlacev(int isNeg, unsigned long placec , FiBIntS * placev)
//{	
//        PL_BigInt* x = new PL_BigInt;
//	int        bytes = placec * sizeof(FiBIntS);
//        mp_size_t  limbs = (sizeof(mp_limb_t) - 1 + bytes) / sizeof(mp_limb_t);
//	unsigned long   i;
//	int         pad;
//
//        PL_GMP(x)->_mp_alloc = limbs;
//	PL_GMP(x)->_mp_d = (mp_ptr) (*_mp_allocate_func) (limbs * BYTES_PER_MP_LIMB);
//	PL_GMP(x)->_mp_size = (isNeg ? -limbs : limbs);
//
//	FiBIntS* limbv = (FiBIntS*) PL_GMP(x)->_mp_d;
//	FiBIntS* datav = placev;
//
//	if ((pad = (limbs * sizeof(mp_limb_t) - bytes)) > 0) {
//	        pad = pad / sizeof(FiBIntS);
//	        for (int j = 0; j < pad; j++)
//	                *limbv++ = 0;
//        }
//
//	for (i = 0; i < placec; i++)
//	        *limbv++ = *datav++;
//
//	limbv = (FiBIntS*) PL_GMP(x)->_mp_d;
//
//	if (limbs * sizeof(mp_limb_t) != (unsigned) bytes)
//	      for (i = 0; i < (unsigned long) limbs; i++) {
//	            FiBIntS  place = limbv[0];
//	            limbv[0] = limbv[1];
//	            limbv[1] = place;
//	            limbv +=2;
//	      }
//
//	return (FiBInt) x;
//}
//
FiBool
fiBIntBit(FiBInt a, FiSInt bit)
{
	PL_BigInt * r = new PL_BigInt(*PL_BI(a));
	FiBool      res;

	mpz_setbit(PL_GMP(r), bit);
	res = (mpz_cmp(PL_GMP(r), PL_GMP(a)) == 0);
	
	delete r;
	return res;
}

/******************************************************************************
 * :: Missing in FOAM
 *****************************************************************************/

void
fiBIntFree(FiBInt a)
{
	delete PL_BI(a);
}

/******************************************************************************
 * :: Auxiliary Functions
 *****************************************************************************/

#if 0
/* Initializes a bigint allocating "size" bytes;
 * Useful when we can estimate the size of the result (eg. before a sum);
 */
static void
plbSmartInit(mpz_ptr x, mp_size_t size)
{
         x->_mp_alloc = size;
         x->_mp_d = (mp_ptr) (*_mp_allocate_func) (size * BYTES_PER_MP_LIMB);
	 x->_mp_size = 0;
}
#endif

/******************************************************************************
 * :: DEBUG STUFF
 *****************************************************************************/

#ifdef PLBINT_DEBUG

void
fiBIntPrint(FiBInt b)
{
  cout << *PL_BI(b) << endl;
}

short int
fiGMPSize(FiBInt b)
{
  return PL_GMP_SIZE(b);
}
short int
fiGMPAlloc(FiBInt b)
{
  return PL_GMP(b)->_mp_alloc;
}

mp_limb_t *
fiGMPLimb(FiBInt b)
{
  return PL_GMP_LIMBS(b);
}
mpz_srcptr
fiGMPZ(FiBInt b)
{
  return PL_GMP(b);
}

#define ubPrintBits(a)  ubPrintBits0(sizeof(a), (unsigned char *)&(a))

extern "C" void    ubPrintBits0    (int, unsigned char *);

void
fiBIntExplode(FiBInt b)
{
        int placec = PL_GMP(b)->_mp_size;
        mp_limb_t* placev = PL_GMP(b)->_mp_d;
        int i;

        for (i = 0; i < placec; i++) {
            ubPrintBits(placev[i]);
            printf("-");
        }

        printf("\n");
}

#endif  /* PLBINT_DEBUG */
