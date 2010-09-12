#ifndef _INTSET_H
#define _INTSET_H
/**
 * Used to represent a set of (assumed small) integers.
 * Internally represented as a bit vector.
 */
typedef struct intSet *IntSet;

extern IntSet intSetNew(int sz);
extern void   intSetFree(IntSet s);
extern void   intSetAdd(IntSet s, int i);
extern void   intSetRemove(IntSet s, int i);
extern Bool   intSetMember(IntSet s, int i);

#endif

