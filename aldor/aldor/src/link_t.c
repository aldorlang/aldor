/*****************************************************************************
 *
 * link_t.c: Globals explicit linking.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/
 
#if !defined(TEST_LINK) && !defined(TEST_ALL)

void	testLink(void) { }

#else

#include "axlgen.h"
#include "cport.h"
#include "foam_c.h"

int	glo1;
int	glo2;
double	glo3;

int	* pGlo1;
int	* pGlo2;
int	* pGlo21;
int	* pGlo22;
double	* pGlo3;

FiWord	* pGlo10a;
FiWord	* pGlo10b;
FiWord	* pGlo10c;
FiWord	* pGlo10d;
FiWord	* pGlo10e;
FiWord	* pGlo10f;
FiWord	* pGlo10g;
FiWord	* pGlo10h;
FiWord	* pGlo10i;
FiWord	* pGlo10l;

FiWord    Glo10;

void
testLink()
{
	fiExportGlobal("glo1", glo1);
	fiImportGlobal("glo1", pGlo1);

	glo1 = 10;
	printf("glo1 = %d, *pGlo1 = %d\n", glo1, *pGlo1);
	*pGlo1 = 30;
	printf("glo1 = %d, *pGlo1 = %d\n", glo1, *pGlo1);
	
	fiImportGlobal("glo2", pGlo2);
	fiImportGlobal("glo2", pGlo21);
	fiExportGlobal("glo2", glo2);
	fiImportGlobal("glo2", pGlo22);

	glo2 = 20;
	printf("glo2 = %d, *pGlo2 = %d, *pGlo21 = %d, *pGlo22 = %d\n", glo2, *pGlo2, *pGlo21, *pGlo22);
	*pGlo2 = 40;
	printf("glo2 = %d, *pGlo2 = %d\n", glo2, *pGlo2);

	fiImportGlobal("glo3", pGlo3);
	fiExportGlobal("glo3", glo3);

	glo3 = 1.3223;
	printf("glo3 = %f, *pGlo3 = %f\n", glo3, *pGlo3);
	*pGlo3 = 2.3343;
	printf("glo3 = %f, *pGlo3 = %f\n", glo3, *pGlo3);

	/* --------------------------------------- */

	fiImportGlobal("Glo10", pGlo10a);
	fiImportGlobal("Glo10", pGlo10b);
	fiImportGlobal("Glo10", pGlo10c);
	fiImportGlobal("Glo10", pGlo10d);
	fiImportGlobal("Glo10", pGlo10e);
	fiImportGlobal("Glo10", pGlo10f);
	fiImportGlobal("Glo10", pGlo10g);

	fiExportGlobal("Glo10", Glo10);

	Glo10 = 100793;

	fiImportGlobal("Glo10", pGlo10h);
	fiImportGlobal("Glo10", pGlo10i);

	printf("These values correspond to the same global and must be equal:");
	printf("\n (%ld) %ld %ld %ld %ld %ld %ld %ld %ld %ld\n",
		Glo10,
		*pGlo10a, *pGlo10b, *pGlo10c, *pGlo10d, *pGlo10e, *pGlo10f,
		*pGlo10g, *pGlo10h, *pGlo10i); 
}



#endif

