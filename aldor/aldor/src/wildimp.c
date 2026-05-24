#include "axlobs.h"
#include "store.h"
#include "syme.h"
#include "utform.h"
#include "wildimp.h"

CREATE_LIST(WildImport);

WildImport
wimpNew(Syme impSyme, UTForm exporter)
{
	WildImport wimp = (WildImport) stoAlloc(OB_Other, sizeof(*wimp));
	wimp->syme = impSyme;
	wimp->type = exporter;

	return wimp;
}

void
wimpFree(WildImport wimp)
{
	stoFree(wimp);
}

UTForm
wimpExporterTForm(WildImport wimp)
{
	return wimp->type;
}

Syme
wimpSyme(WildImport wimp)
{
	return wimp->syme;
}

UTForm
wimpType(WildImport wimp)
{
	return utformNewWithCond(utformVars(wimpExporterTForm(wimp)),
				 symeType(wimpSyme(wimp)),
				 listCopy(Sefo)(symeCondition(wimpSyme(wimp))));
					  
}

