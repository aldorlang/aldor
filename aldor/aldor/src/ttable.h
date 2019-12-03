#ifndef _TTABLE_H_
#define _TTABLE_H_
#include "cport.h"
#include "list.h"
#include "ostream.h"
#include "table.h"

typedef struct tsetIter { TableIterator iter; } *ANY_TSetIter;

#define TSet(Type) Type##TSet
#define TSetIter(Type) Type##TSetIter
#define DECLARE_TSET(Type)			\
	typedef struct Type##_TSet {		\
		Table table;			\
	} *TSet(Type);				\
	typedef ANY_TSetIter Type##TSetIter;	\
	typedef Hash (*Type##TSetHashFn)(Type);  \
	typedef Bool (*Type##TSetEqFn)(Type, Type); \
	TSetOpsStruct(Type);			\
	extern struct TSetOpsStructName(Type)	\
                 const *TSetOps(Type)		\


#define CREATE_TSET(Type)					\
struct TSetOpsStructName(Type) const *TSetOps(Type) =		\
	(struct TSetOpsStructName(Type) const *) &ptrTSetOps

#if 0
	; /* for editor indentation */
#endif

#define tsetCreate(Type) (TSetOps(Type)->Create)
#define tsetCreateCustom(Type) (TSetOps(Type)->CreateCustom)
#define tsetEmpty(Type) (TSetOps(Type)->Create)
#define tsetFree(Type) (TSetOps(Type)->Free)
#define tsetSize(Type) (TSetOps(Type)->Size)
#define tsetAdd(Type) (TSetOps(Type)->Add)
#define tsetAddAll(Type) (TSetOps(Type)->AddAll)
#define tsetRemove(Type) (TSetOps(Type)->Remove)
#define tsetMember(Type) (TSetOps(Type)->Member)
#define tsetIsEmpty(Type) (TSetOps(Type)->IsEmpty)
#define tsetIter(Type) (TSetOps(Type)->Iter)
#define tsetIterNext(Type) (TSetOps(Type)->IterNext)
#define tsetIterElt(Type) (TSetOps(Type)->IterElt)
#define tsetIterHasNext(Type) (TSetOps(Type)->IterHasNext)
#define tsetIterDone(Type) (TSetOps(Type)->IterDone)

#define TSetOps(Type) Type##_tsetPointer
#define TSetOpsStructName(Type) Type##_tsetOpsStruct

#define TSetOpsStruct(Type)				\
struct TSetOpsStructName(Type) {			\
	TSet(Type) (*Create) (void);			\
	TSet(Type) (*CreateCustom) (Type##TSetHashFn, Type##TSetEqFn); \
	void 	   (*Free)   (TSet(Type));		\
	Length 	   (*Size)   (TSet(Type));		\
	void 	   (*Add)    (TSet(Type), Type);	\
	void 	   (*AddAll) (TSet(Type), List(Type));	\
	void 	   (*Remove) (TSet(Type), Type);	\
	Bool 	   (*Member) (TSet(Type), Type);	\
	Bool 	   (*IsEmpty)(TSet(Type));		\
	TSet(Type) (*Empty)(void);			\
	TSetIter(Type)	(*Iter)(TSet(Type));		\
	TSetIter(Type)	(*IterNext)(TSetIter(Type));		\
	Type		(*IterElt)(TSetIter(Type));		\
	Bool 		(*IterHasNext)(TSetIter(Type));		\
	void 		(*IterDone)(TSetIter(Type));		\
}
#if 0
	; /* for editor indentation */
#endif

DECLARE_TSET(Pointer);

extern const struct TSetOpsStructName(Pointer) ptrTSetOps;

#endif
