#include "comp"
#include "aldorio"

#pile

TFormTagId: TFormTagCat with
    id: String -> TForm
== add
    import from List TForm, TFormAttrs
    
    name: String == "id"
    tfEquals(t1: TForm, t2: TForm): Boolean == string(attrs t1) = string(attrs t2)

    tfFreeVars(cx: TForm): List String == []

    id(name: String): TForm == 
        import from BindingSet, TFormAttrs
        new(TFormTagId, [], empty(), create(name))

    info(o: TextWriter, id: TForm): () ==
        o << string attrs id

TFormTagApply: TFormTagCat with
    apply: (String, List TForm) -> TForm
== add
    import from List TForm, TFormAttrs
    
    name: String == "apply"
    tfEquals(t1: TForm, t2: TForm): Boolean ==
        string(attrs t1) = string(attrs t2) and args t1 = args t2

    tfFreeVars(cx: TForm): List String == []

    apply(id: String, tfs: List TForm): TForm == 
        import from BindingSet, TFormAttrs
        new(TFormTagApply, tfs, empty(), create(id))

    info(o: TextWriter, id: TForm): () ==
        o << string attrs id

