/*****************************************************************************
 *
 * list.h: Parameterized list type.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _LIST_H_
#define _LIST_H_

#include "cport.h"
#include "ostream.h"

/*****************************************************************************
 *
 * Public part.
 *
 ****************************************************************************/

# define List(Type)		  Type##List

# define DECLARE_LIST(Type)					\
	typedef struct Type##ListCons {				\
		Type first;					\
		struct Type##ListCons *rest;			\
	} *List(Type);						\
	ListOpsStruct(Type);					\
	extern struct ListOpsStructName(Type) const *ListOps(Type)

# define CREATE_LIST(Type)					\
	struct ListOpsStructName(Type) const *ListOps(Type) =	\
		(struct ListOpsStructName(Type) const *) &ptrlistOps


/*
 * Various list operations.
 * listList(XXX)(n, ...): create a list containing the following n elements
 * listListv(XXX)(argp): Create a list containing all following arguments, up to a 'NULL'.
 * listPrint, GPrint: print & more generic printing
 * listFormat: Format via ostream.
 */
#  define listSingleton(Type)		(ListOps(Type)->Singleton)
#  define listList(Type)		(ListOps(Type)->List)
#  define listListv(Type)		(ListOps(Type)->Listv)
#  define listListNull(Type)		(ListOps(Type)->ListNull)
#  define listCons(Type)		(ListOps(Type)->Cons)
#  define listEqual(Type)		(ListOps(Type)->Equal)
#  define listFind(Type)		(ListOps(Type)->Find)
#  define listFreeCons(Type)		(ListOps(Type)->FreeCons)
#  define listFree(Type)		(ListOps(Type)->Free)
#  define listFreeTo(Type)		(ListOps(Type)->FreeTo)
#  define listFreeDeeply(Type)		(ListOps(Type)->FreeDeeply)
#  define listFreeDeeplyTo(Type)	(ListOps(Type)->FreeDeeplyTo)
#  define listFreeIfSat(Type)		(ListOps(Type)->FreeIfSat)
#  define listElt(Type)			(ListOps(Type)->Elt)
#  define listDrop(Type)		(ListOps(Type)->Drop)
#  define listLastCons(Type)		(ListOps(Type)->LastCons)
#  define listLength(Type)		(ListOps(Type)->_Length)
#  define listIsLength(Type)		(ListOps(Type)->IsLength)
#  define listIsLonger(Type)		(ListOps(Type)->IsLonger)
#  define listIsShorter(Type)		(ListOps(Type)->IsShorter)
#  define listCopy(Type)		(ListOps(Type)->Copy)
#  define listCopyTo(Type)		(ListOps(Type)->CopyTo)
#  define listCopyDeeply(Type)		(ListOps(Type)->CopyDeeply)
#  define listCopyDeeplyTo(Type)	(ListOps(Type)->CopyDeeplyTo)
#  define listMap(Type)			(ListOps(Type)->Map)
#  define listNMap(Type)		(ListOps(Type)->NMap)
#  define listReverse(Type)		(ListOps(Type)->Reverse)
#  define listNReverse(Type)		(ListOps(Type)->NReverse)
#  define listConcat(Type)		(ListOps(Type)->Concat)
#  define listNConcat(Type)		(ListOps(Type)->NConcat)
#  define listMemq(Type)		(ListOps(Type)->Memq)
#  define listMember(Type)		(ListOps(Type)->Member)
#  define listPosq(Type)		(ListOps(Type)->Posq)
#  define listPosition(Type)		(ListOps(Type)->Position)
#  define listNRemove(Type)		(ListOps(Type)->NRemove)
#  define listFillVector(Type)		(ListOps(Type)->FillVector)
#  define listPrint(Type)		(ListOps(Type)->Print)
#  define listGPrint(Type)		(ListOps(Type)->GPrint)
#  define listFormat(Type)		(ListOps(Type)->Format)

#  define listNil(Type)			((Type##List) 0)

#  define car(l)			((l)->first)
#  define cdr(l)			((l)->rest)
#  define setcar(l,a)			((l)->first = (a))
#  define setcdr(l,d)			((l)->rest = (d))

#  define listIsSingleton(l)		((l) && !cdr(l))

#  define listPush(T, X, L)		(L = listCons(T)(X, L))
#  define listPop(T, X, L, F)		(L = listNRemove(T)(L, X, F))

# define listIter(T, arg, list, action)				\
Statement({							\
	T##List	_l0;						\
	T arg;							\
	for (_l0 = (list); _l0; _l0 = cdr(_l0)) {		\
		arg = car(_l0);					\
		Statement(action);				\
	}							\
})
/*****************************************************************************
 *
 * Private part.
 *
 ****************************************************************************/

# define ListOps(Type)		 Type##_listPointer
# define ListOpsStructName(Type) Type##_listOpsStruct

# define ListOpsStruct(Type)							\
	struct ListOpsStructName(Type) {					\
		List(Type)	(*Cons)		(Type, List(Type));		\
		List(Type)	(*Singleton)	(Type);				\
		List(Type)	(*List)		(int n, ...);			\
		List(Type)	(*Listv)	(va_list argp);			\
		List(Type)	(*ListNull)	(Type, ...);			\
		Bool		(*Equal)	(List(Type), List(Type),	\
						 Bool (*f) (Type, Type));	\
		Type		(*Find)		(List(Type), Type,		\
						 Bool(*eq)(Type,Type) , int *);	\
		List(Type)	(*FreeCons)	(List(Type));			\
		void		(*Free)		(List(Type));			\
		List(Type)	(*FreeTo)	(List(Type), List(Type));	\
		void		(*FreeDeeply)	(List(Type), void (*f)(Type));	\
		List(Type)	(*FreeDeeplyTo) (List(Type), List(Type),	\
						 void (*f) (Type) );		\
		List(Type)	(*FreeIfSat)	(List(Type), void (*f)(Type),	\
						 Bool (*s)(Type));		\
		Type		(*Elt)		(List(Type), Length);		\
		List(Type)	(*Drop)		(List(Type), Length);		\
		List(Type)	(*LastCons)	(List(Type));			\
		Length		(*_Length)	(List(Type));			\
		Bool		(*IsLength)	(List(Type), Length);		\
		Bool		(*IsShorter)	(List(Type), Length);		\
		Bool		(*IsLonger)	(List(Type), Length);		\
		List(Type)	(*Copy)		(List(Type));			\
		List(Type)	(*CopyTo)	(List(Type), List(Type));	\
		List(Type)	(*CopyDeeply)	(List(Type), Type (*f)(Type));	\
		List(Type)	(*CopyDeeplyTo) (List(Type), List(Type),	\
						 Type (*f) (Type) );		\
		List(Type)	(*Map)		(Type (*f)(Type), List(Type));	\
		List(Type)	(*NMap)		(Type (*f)(Type), List(Type));	\
		List(Type)	(*Reverse)	(List(Type));			\
		List(Type)	(*NReverse)	(List(Type));			\
		List(Type)	(*Concat)	(List(Type), List(Type));	\
		List(Type)	(*NConcat)	(List(Type), List(Type));	\
		Bool		(*Memq)		(List(Type), Type);		\
		Bool		(*Member)	(List(Type), Type,		\
						 Bool(*eq)(Type,Type) );	\
		int		(*Posq)		(List(Type), Type);		\
		int		(*Position)	(List(Type), Type,		\
						 Bool(*eq)(Type,Type) );	\
		List(Type)	(*NRemove)	(List(Type), Type,		\
						 Bool(*eq)(Type,Type) );	\
		void		(*FillVector)	(Type *, List(Type));		\
		int		(*Print)	(FILE *, List(Type),		\
						 int (*pr)(FILE *, Type) );	\
		int		(*GPrint)	(FILE *, List(Type),		\
						 int (*pr)(FILE *, Type),	\
						 char *l,char *m,char *r);	\
		int		(*Format)	(OStream, CString, List(Type));	\
	}

DECLARE_LIST(Pointer);
extern const struct ListOpsStructName(Pointer) ptrlistOps;

#endif /* !_LIST_H_ */
