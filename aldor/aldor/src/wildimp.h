/*
 */

#ifndef WILDIMP_H
#define WILDIMP_H

#include "axlobs.h"

struct wildImp {
	Syme syme;
	UTForm type;
};

DECLARE_LIST(WildImport);

extern WildImport	wimpNew(Syme impSyme, UTForm exporter);
extern UTForm 		wimpType(WildImport wimp);
extern void 		wimpFree(WildImport wimp);
extern UTForm 		wimpExporterTForm(WildImport wimp);
extern Syme 		wimpSyme(WildImport wimp);
extern UTForm 		wimpType(WildImport wimp);


#endif
