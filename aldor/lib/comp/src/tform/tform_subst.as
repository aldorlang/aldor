#include "comp"
#include "aldorio"
#pile

TFormSubst: with
    subst: (tf: TForm, sigma: Subst) -> TForm
== add
    PtrTForm == TypedPointer TForm
    import from TFormTagApply, TFormTagId, TFormTagComma
    import from TFormTagTuple, TFormTagDeclare, TFormTagType
    import from TFormTagMap
    import from Field(HashTable(PtrTForm, PtrTForm))
    import from HashTable(PtrTForm, PtrTForm)
    import from AbSynMeaning
    import from String
    import from Subst
    import from List TForm
    
    substTf: Field(HashTable(PtrTForm, PtrTForm)) == field("substTf")

    subst(tf: TForm, sigma: Subst): TForm ==
        id? tf => substId(tf, sigma)
	comma? tf => comma(subst(arg, sigma) for arg in args tf)
	declare? tf => declare(declareId tf, subst(first args tf, sigma))
	tuple? tf => tuple(subst(first args tf, sigma))
	apply? tf => (apply(first l, rest l) where l := [subst(elt, sigma) for elt in args tf])
	map? tf => map(subst(mapArgs(tf), sigma), subst(mapRets(tf), sigma))
	type? tf => tf
	stdout << ("Unknown tag " + kind tf) << newline
	import from String
	error("Unknown tag " + kind tf)


    local substId(tf: TForm, sigma: Subst): TForm ==
        import from Partial AbSyn
 	id := idName tf
        absynMaybe := find(sigma, id)
	failed? absynMaybe => tf
	meaning(retract absynMaybe)
