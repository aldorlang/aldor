/******************************************************************************
 *
 * :: Double Word operations.
 *
 *****************************************************************************/

/* #ifdef FOAM_RTS */
#include "axlgen.h"
#include "bigint.h"
#include "optcfg.h"
typedef ULong	BIntD;		/* To contain two BInt digits. */

#define BINT_LG_RADIX		(bitsizeof(BIntS))
#define BINT_RADIX		(((unsigned long) 1) << BINT_LG_RADIX)

#define DivideDouble(q, r, nh, nl, d) {			\
	BIntD n_ = (BIntD)(nh)*(BIntD)BINT_RADIX + (nl);\
	BIntD d_ = (d);					\
	(q)      = n_ / d_;				\
	(r)	 = n_ % d_;				\
}

#ifdef BIGINT_TEST_BASE
# define BASE_BITS      6
# define __TEST_BASE	(1L << BASE_BITS)
# define BASE_MINUS_1	(__TEST_BASE - 1)
# define MODB(x)	((x) % __TEST_BASE)
#else
# define BASE_MINUS_1	ULONG_MAX
# define BASE_BITS	(CHAR_BIT * sizeof(unsigned long))
# define MODB(x)	(x)
#endif

#define BASE_BITS_2     (BASE_BITS/2)
#define BASE_ROOT       (1L << (BASE_BITS_2))
#define COMPL(x)	(BASE_MINUS_1 - (x))
#define LO_HALF_LO(x)	((x) & (BASE_ROOT-1))
#define HI_HALF_LO(x)	((x) >> BASE_BITS_2)
#define LO_HALF_HI(x)	(((x) & (BASE_ROOT-1)) << BASE_BITS_2)
#define HI_HALF_HI(x)	(((x) >> BASE_BITS) << BASE_BITS_2)
#define COMBINE(h, l)   (((h) << BASE_BITS_2) | (l))
#define HI_BIT(n)	(((n) >> (BASE_BITS-1)) & 1)


void
xxTestGtDouble(int *pb, ULong AH, ULong AL, ULong BH, ULong BL)
{
	*pb = AH > BH || (AH == BH && AL > BL);
}

#ifndef OPT_NoDoubleOps

void
xxTimesDouble(ULong *pH, ULong *pL, ULong A, ULong B)
{
	ULong Ah, Al, Bh, Bl;
	ULong H, M, N, L;
	ULong Mh, Mx, Nh, Nx;
	ULong T;

	Ah = HI_HALF_LO(A);
	Al = LO_HALF_LO(A);
	Bh = HI_HALF_LO(B);
	Bl = LO_HALF_LO(B);

	H  = MODB(Ah * Bh);
	M  = MODB(Al * Bh);
	N  = MODB(Ah * Bl);
	L  = MODB(Al * Bl);

	Mh = HI_HALF_LO(M);
	Mx = LO_HALF_HI(M);
	Nh = HI_HALF_LO(N);
	Nx = LO_HALF_HI(N);

	T  = L;
	L  = MODB(L + Mx);
	H  = MODB(H + (L < T));
	H  = MODB(H + Mh);

	T  = L;
	L  = MODB(L + Nx);
	H  = MODB(H + (L < T));
	H  = MODB(H + Nh);

	*pH= H;
	*pL= L;
}
#endif

void
xxPlusStep(ULong *pko, ULong *pr, ULong a, ULong b, ULong ki)
{
	ULong  ko, r;

	r  = MODB(a + b);
	ko = r < a;
	r  = MODB(r + ki);
	ko+= r < ki;

	*pko = ko;
	*pr  = r;
}

#define minusStep(pkp1out,pr,a,b,kp1in) plusStep(pkp1out,pr,a,COMPL(b),kp1in)

void
xxTimesStep(ULong *pko, ULong *pr, ULong a, ULong b, ULong c, ULong ki)
{
	ULong	h, l, k, r;

	xxTimesDouble(&h, &l, a, b);

	r    = MODB(l + c);
	k    = r < l;
	r    = MODB(r + ki);
	k   += r < ki;
	*pr  = r;
	r    = MODB(h + k);
	*pko = r;
}

#ifndef OPT_NoDoubleOps

void
xxDivideDouble(ULong *pqh, ULong *pql, ULong *pr,
	       ULong nh, ULong nl, ULong d)
{
	ULong	qh, ql, rh, rl;		/* results */
	ULong	qrh,rrh,qrl,rrl,qB,rB;	/* quo/rem rh, rl, base */
	ULong	Bd;
	ULong	th, tl;			/* temporaries */


	if (d == 1) {
		*pqh = nh; *pql = nl;
		*pr  = 0;
		return;
	}
	if (d < BASE_ROOT) {
		ULong r = 0, th,tl;
		DivideDouble(th, r, r, HI_HALF_LO(nh), d);
		DivideDouble(tl, r, r, LO_HALF_LO(nh), d);
		ql = COMBINE(th, tl);
		DivideDouble(th, r, r, HI_HALF_LO(nl), d);
		DivideDouble(tl, r, r, LO_HALF_LO(nl), d);
		ql = COMBINE(th, tl);

		*pqh = 0;
		*pql = ql;
		*pr  = r;
		return;
	}
	/*
	 * B-d = (B-1) - d + 1 = q' d + r' => B = (q' + 1) d + r'
	 */
	Bd = COMPL(d-1);
	qB = Bd / d + 1;
	rB = Bd % d;

	qh = 0;  ql = 0;
	rh = nh; rl = nl;

	while (rh != 0 || rl >= d) {
		
		qrh = rh / d; rrh = rh % d;
		qrl = rl / d; rrl = rl % d;

		/* [qh,ql] += qrh*B + rrh*qB + qrl */
		/* [rh,rl]  =         rrh*rB + rrl */

		/*
		 * xxTimesDouble(&h, &l,  rrh, qB);
		 * plusStep   (&k, &tl, l,   qrl, 0);
		 * plusStep   (&k, &th, h,   k,   0);
		 * plusStep   (&k, &ql, tl,  ql,  0);
		 * plusStep   (&k, &qh, th,  qh,  qrh+k);
		 */
		xxTimesDouble(&th, &tl,  rrh, qB);
		tl  = MODB(tl + qrl);
		th  = MODB(th + (tl < qrl));
		ql  = MODB(ql + tl);
		qh  = MODB(qh + (ql < tl));
		qh  = MODB(qh + th);
		qh  = MODB(qh + qrh);

		/*
		 * xxTimesStep(&rh, &rl, rrh, rB, rrl, 0);
		 */
		xxTimesDouble(&th, &tl, rrh, rB);
		rl  = MODB(tl + rrl);
		rh  = MODB(th + (rl < rrl));
	}

	*pqh = qh; *pql = ql;
	*pr  = rl;
	
}


/* As above, but no quotient */
ULong
xxModDouble(ULong nh, ULong nl, ULong d)
{
	ULong	qh, ql, rh, rl;		/* results */
	ULong	rrh,rrl,qB,rB;	/* quo/rem rh, rl, base */
	ULong	Bd;
	ULong	th, tl;			/* temporaries */

	if (d == 1) {
		return 0;
	}

	if (d < BASE_ROOT) {
		ULong r = 0, tmp;
		DivideDouble(tmp, r, r, HI_HALF_LO(nh), d);
		DivideDouble(tmp, r, r, LO_HALF_LO(nh), d);
		DivideDouble(tmp, r, r, HI_HALF_LO(nl), d);
		DivideDouble(tmp, r, r, LO_HALF_LO(nl), d);

		return r;
	}

	/*
	 * B-d = (B-1) - d + 1 = q' d + r' => B = (q' + 1) d + r'
	 */
	Bd = COMPL(d-1);
	qB = Bd / d + 1;
	rB = Bd % d;

	qh = 0;  ql = 0;
	rh = nh; rl = nl;

	while (rh != 0 || rl >= d) {

		rrh = rh % d;
		rrl = rl % d;

		/*
		 * xxTimesStep(&rh, &rl, rrh, rB, rrl, 0);
		 */
		xxTimesDouble(&th, &tl, rrh, rB);
		rl  = MODB(tl + rrl);
		rh  = MODB(th + (rl < rrl));
	}

	return rl;
}

#endif

#if defined(BIGINT_TEST_BASE)

#define BASE __TEST_BASE
main()
{
	BIntS	a, b, c, d, h, l, k;
	BIntS	r1, r2;
	int	t;

  for (a = 0; a < BASE; a++) {
    for (b = 0; b < BASE; b++) {
			printf(".");

			timesDouble(&h, &l, a, b);
			r1 = h * BASE + l;
			r2 = a * b;
			if (h >= BASE || l >= BASE || r1 != r2)
				printf("%d * %d fails\n", a, b);

			plusStep(&h, &l, a, b, int0);
			r1 = h * BASE + l;
			r2 = a + b + 0;
			if (h >= BASE || l >= BASE || r1 != r2)
				printf("%d + %d + 0 fails\n", a, b);

			plusStep(&h, &l, a, b, 1);
			r1 = h * BASE + l;
			r2 = a + b + 1;
			if (h >= BASE || l >= BASE || r1 != r2)
				printf("%d + %d + 1 fails\n", a, b);
			
			minusStep(&h, &l, a, b, 1);
			r1 = (h-1) * BASE + l;
			r2 = a - b - 0;
			if (h >= BASE || l >= BASE || r1 != r2)
				printf("%d - %d - 0 fails\n", a, b);
			
			minusStep(&h, &l, a, b, int0);
			r1 = (h-1) * BASE + l;
			r2 = a - b - 1;
			if (h >= BASE || l >= BASE || r1 != r2)
				printf("%d - %d - 1 fails\n", a, b);

      for (c = 0; c < BASE; c++) {
	for (d = 0; d < BASE; d++) {

			timesStep(&h, &l, a, b, c, d);
			r1 = h * BASE + l;
			r2 = a * b + c + d;
			if (h >= BASE || l >= BASE || r1 != r2)
				printf("%d * %d + %d + %d fails\n", a, b, c, d);
			
			testGtDouble(&t, a, b, c, d);
			r1 = t;
			r2 = (a * BASE + b) > (c * BASE + d);
			if (r1 != r2)
				printf("%d,%d > %d,%d fails\n", a, b, c, d);
        }
      }
      for (c = 1; c < BASE; c++) {
			divideDouble(&h, &l, &k, a, b, c);

			r1 = h * BASE + l;
			r2 =(a * BASE + b) / c;
			if (h >= BASE || l >= BASE || r1 != r2) {
				printf("(%d,%d) / %d fails\n", a, b, c);
				divideDouble(&h, &l, &k, a, b, c);
			}

			r1 = k;
			r2 =(a * BASE + b) % c;
			if (k >= BASE || r1 != r2) {
				printf("(%d,%d) %% %d fails\n", a, b, c);
				divideDouble(&h, &l, &k, a, b, c);
			}
      }
	
    }
    printf("\n");
  }
}

#endif /* (BIGINT_TEST_WHOLE_WORDS) */
