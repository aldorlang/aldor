#include "axlobs.h"
#include "wildimp.h"
#include "utype.h"
#include "syme.h"
#include "store.h"

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
	return utformNew(utformVars(wimpExporterTForm(wimp)),
			 symeType(wimpSyme(wimp)));
}

