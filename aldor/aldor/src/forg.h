#ifndef _FORG_H_
#define _FORG_H_
/******************************************************************************
 *
 * :: Foreign origin
 *
 *****************************************************************************/

struct foreign_origin {
	FoamProtoTag	protocol;
	String		file;
};

typedef struct foreign_origin * ForeignOrigin;

extern ForeignOrigin    forgFrAbSyn	(AbSyn);
extern Bool   		forgEqual	(ForeignOrigin, ForeignOrigin);

#endif
