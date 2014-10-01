/*****************************************************************************
 *
 * tposs.h: Type possibility sets.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _TPOSS_H_
#define _TPOSS_H_

#include "axlobs.h"
#include "tform.h"
#include "utype.h"

struct tposs {
	UTFormList	possl;
	int		possc;
	int		refc;
};

typedef TPoss	(*TPossGetter)		(Pointer, Length);

extern TPoss	tpossEmpty		(void);
extern TPoss	tpossSingleton		(TForm);
extern TPoss	tpossFrSymes		(SymeList);
extern TPoss	tpossDeclare		(Syme, TPoss);
extern TPoss	tpossMulti		(Length, Pointer, TPossGetter);
extern TPoss	tpossAdd1		(TPoss, TForm);
extern TPoss    tpossFrTheUTFormList	(UTFormList);
extern UTForm   tpossUniqueUTForm	(TPoss);


extern TPoss	tpossRefer		(TPoss);
extern TPoss	tpossCopy		(TPoss);
extern int	tpossCount 		(TPoss);
extern void	tpossFree		(TPoss);
extern int	tpossPrint		(FILE *, TPoss);
extern int	tpossPrintDb		(TPoss);
extern int      tpossOStreamWrite       (OStream, TPoss);

extern Bool	tpossIsUnique		(TPoss);
extern TPoss	tpossIntersect   	(TPoss,  TPoss);
extern TPoss	tpossUnion		(TPoss,  TPoss);

extern TForm	tpossUnique		(TPoss);

extern Bool	tpossHas		(TPoss tp, TForm t);
		/*
		 * Is there at least one member of the type possibility set tp
		 * which is equal to the target type t?
		 */

extern Bool	tpossHasSatisfier	(TPoss tp, TForm t);
		/*
		 * Is there at least one member of the type possibility set tp
		 * which satisfies the target type t?
		 */

extern TForm	tpossSelectSatisfier	(TPoss tp, TForm t);
		/*
		 * Select the unique member of the type possibility set tp
		 * which satisfies the target type t, or return zero if there
		 * is no such unique member.
		 */

extern TPoss	tpossSatisfies   	(TPoss S, TPoss T);
		/*
		 * Return the set of all types t in T which are satisfied by
		 * at least one type s in S.
		 */

extern TPoss	tpossSatisfiesType   	(TPoss S, TForm T);
		/*
		 * Return the set of all types s in S which satisfy T.
		 */

extern Bool	tpossHasMapType		(TPoss);
		/*
		 * Does the type possibility set contain a mapping type?
		 */

extern TPoss	tpossGeneratorArg	(TPoss tp);
		/*
		 * Return the type possibilities X such that tp is Generator(X).
		 */

extern Bool	tpossIsHaving		(TPoss tp, TFormPredicate pred);
		/*
		 * Is there any type form in tp which satisfies the predicate?
		 */

/*
 * Abstract iteration over type possibility sets:
 *
 * TPoss	 p;
 * TPossIterator ip;
 * for (tpossITER(ip, p); tpossMORE(ip); tpossSTEP(ip)) {
 *	TForm t = tpossELT(ip);
 * }
 */
typedef struct {
	UTFormList	possl;
} TPossIterator;

#define tpossITER(ip,p)	((ip).possl = (p ? (p)->possl : NULL))
#define tpossMORE(ip)   ((ip).possl)
#define tpossSTEP(ip)	((ip).possl = cdr((ip).possl))
#define tpossELT(ip)	utformConstOrFail(car((ip).possl))
#define tpossUELT(ip)	(car((ip).possl))

#endif /* !_TPOSS_H_ */
