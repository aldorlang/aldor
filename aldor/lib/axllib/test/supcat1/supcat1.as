-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#library LangLib "lang.ao"
import from LangLib;

#library SupLib "supcat0.ao"
import from SupLib;

ComplexCat (RCom: CommRng) : Category == with {
	MonAlg(RCom, SUP RCom);
}
