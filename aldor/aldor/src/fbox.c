#include "axlobs.h"

FoamBox
fboxNewEmpty(FoamTag tag)
{
        FoamBox newFB;
 
        newFB = (FoamBox) stoAlloc(OB_Other, sizeof(struct foamBox));
        newFB->tag     = tag;
        newFB->l       = listNil(Foam);
        newFB->argc    = 0;
        newFB->initial = 0;
 
        return newFB;
}
 
FoamBox
fboxNew(Foam foam)
{
	FoamBox newFB;

        if (foam == NULL) return NULL;

	newFB = fboxNewEmpty(foamTag(foam));
	newFB->initial = foam;
	newFB->argc    = foamArgc(foam) - foamNaryStart(newFB->tag);

	return newFB;
}
 
void
fboxFree(FoamBox fbox)
{
        listFree(Foam)(fbox->l);
        stoFree(fbox);
}
 
int
fboxAdd(FoamBox fbox, Foam foam)
{
        fbox->l = listCons(Foam)(foam, fbox->l);
        return fbox->argc++;
}
 
Foam
fboxMake(FoamBox fbox)
{
        Foam            newFoam;
        int             i=0;
        FoamList        l;
 
        if (fbox->l == 0) {
                if (fbox->initial)
                        return fbox->initial;
                else
                        return foamNewEmpty(fbox->tag, int0);
        }
 
        newFoam = foamNewEmpty(fbox->tag, fbox->argc + foamNaryStart(fbox->tag));
 
        if (fbox->initial)
                for(i=0; i<foamArgc(fbox->initial); i++)
                        foamArgv(newFoam)[i].code =
                                foamArgv(fbox->initial)[i].code;
        fbox->l = listNReverse(Foam)(fbox->l);
        for (l = fbox->l; l; l = cdr(l), i++)
                foamArgv(newFoam)[i].code = car(l);
 
        if (fbox->initial) foamFreeNode(fbox->initial);
        listFree(Foam)(fbox->l);
        stoFree(fbox);
        return newFoam;
}
 
Foam
fboxNth(FoamBox fbox, int n)
{
        int     initArgc;
        if (!fbox->initial)
                initArgc = 0;
        else
                initArgc = foamArgc(fbox->initial) - foamNaryStart(fbox->tag);
        if (n < initArgc)
                return foamArgv(fbox->initial)[n + foamNaryStart(fbox->tag)].code;
        else {
                int     i = fbox->argc - n - 1;
                return listElt(Foam)(fbox->l, i);
        }
}
 
 
