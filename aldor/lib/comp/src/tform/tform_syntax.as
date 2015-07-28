#include "comp"
#include "aldorio"

#pile

TFormTagId: TFormTagCat with
    id: Id -> TForm
== add
    import from List TForm, TFormAttrs, Id

    name: String == "id"
    tfEquals(t1: TForm, t2: TForm): Boolean == id attrs t1 = id attrs t2

    tfFreeVars(cx: TForm): List Id == []

    id(name: Id): TForm ==
        import from BindingSet, TFormAttrs
        new(TFormTagId, [], empty(), create(name))

    info(o: TextWriter, tid: TForm): () ==
        o << id attrs tid

TFormTagApply: TFormTagCat with
    apply: (Id, List TForm) -> TForm
== add
    import from List TForm, TFormAttrs, Id

    name: String == "apply"
    tfEquals(t1: TForm, t2: TForm): Boolean ==
        id(attrs t1) = id(attrs t2) and args t1 = args t2

    tfFreeVars(cx: TForm): List Id == []

    apply(id: Id, tfs: List TForm): TForm ==
        import from BindingSet, TFormAttrs
        new(TFormTagApply, tfs, empty(), create(id))

    info(o: TextWriter, tid: TForm): () ==
        o << id attrs tid
