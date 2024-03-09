#ifndef SLOTUSAGE_H
#define SLOTUSAGE_H

#include "cport.h"
#include "list.h"
/**
 * Indicates usage and format number of a foam format in
 * the format Usage stack.  We need to track where a slot
 * is used independently of the format id.
 * (outside of here, 4 is not used, whereas any other value
 * represents used. In code gen we want to decide late
 * which one to use).
 * Representation is that the lowest bit indicates usage.
 */
typedef AInt SlotUsage;

DECLARE_LIST(SlotUsage);

#define suSetUse(x) ( (x) | 1)
#define suClrUse(x) ( (x) & (~1))
#define suVal(x)    ( (x & 2) == 2 ? ((x) >> 2): *(int*) 0)
#define suIsUsed(x) ( (x) & 1)

#define suFrFormat(x) (((x) << 2) | 2)
#endif
