#ifndef _FBOX_H
#define _FBOX_H

#include "foam.h"

struct foamBox {
	FoamTag		tag;
	FoamList	l;
	Length		argc;
	Foam		initial;
};

/* Functions for Foam boxes. */
extern	FoamBox		fboxNewEmpty 	(FoamTag);
extern	FoamBox		fboxNew		(Foam);
extern	void		fboxFree	(FoamBox);
extern	int		fboxAdd		(FoamBox, Foam);
extern	Foam		fboxMake	(FoamBox);
extern	Foam		fboxNth		(FoamBox, int);


#endif
