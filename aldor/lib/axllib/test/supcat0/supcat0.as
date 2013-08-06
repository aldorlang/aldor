-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#library LangLib "lang.ao"
import from LangLib;

-- Needed for the conditionals
Boolean:with == add;
import from Boolean;

CommRng : Category == with {
	*:	(%, %) -> %;
	comm:	Type -> Type;
}

UPC (RUPC: Type) : Category == with {
	*:	(%, %) -> %;
	if RUPC has CommRng then CommRng;
}

export SUP: (RSUP: Type) -> with {
	UPC RSUP;
	outputForm:	Type;
}

MonAlg (RMon: Type, C: UPC RMon) : Category
== with {
}
