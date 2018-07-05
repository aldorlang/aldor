#ifndef _FORG_H_
#define _FORG_H_
#include "foam.h"

/******************************************************************************
 *
 * :: Foreign origin
 *
 *****************************************************************************/

struct foreign_origin {
	FoamProtoTag	protocol;
	String		file;
};

extern ForeignOrigin	forgNew		(FoamProtoTag, String);
extern ForeignOrigin    forgFrAbSyn	(AbSyn);
extern Bool   		forgEqual	(ForeignOrigin, ForeignOrigin);
extern AInt   		forgHash	(ForeignOrigin);
extern void             forgFree        (ForeignOrigin);

extern ForeignOrigin    forgFrBuffer    (Buffer);
extern void             forgToBuffer    (Buffer, ForeignOrigin);

extern void             forgBufferSkip  (Buffer);

#endif
