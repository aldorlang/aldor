#include "comp"
#include "aldorio"

#pile

TFormTagId: TFormTagCat with
    id: Id -> TForm
    id?: TForm -> Boolean
    idName: TForm -> Id
== add
    import from List TForm, TFormAttrs, Id

    name: String == "id"
    id?(tf: TForm): Boolean == kind tf = name
    type?(): Boolean == false
    tfEquals(t1: TForm, t2: TForm): Boolean == id attrs t1 = id attrs t2

    tfFreeVars(tf: TForm): List Id == [id attrs tf]

    id(name: Id): TForm ==
        import from BindingSet, TFormAttrs
        new(TFormTagId, [], empty(), create(name))

    idName(tf: TForm): Id == id attrs tf

    info(o: TextWriter, tid: TForm): () ==
        o << id attrs tid

    tfSubst(tf: TForm, sigma: Subst): TForm ==
        lookup(sigma, id attrs tf, tf)

TFormTagApply: TFormTagCat with
    apply: (TForm, List TForm) -> TForm
    apply?: TForm -> Boolean
== add
    import from List TForm

    name: String == "apply"
    type?(): Boolean == false
    apply?(tf: TForm): Boolean == kind tf = name
    tfEquals(t1: TForm, t2: TForm): Boolean ==
        args t1 = args t2

    tfFreeVars(tf: TForm): List Id ==
        import from Fold2(List Id, List Id), List Id
        reverse!((append!, [])/(freeVars arg for arg in args tf))

    apply(op: TForm, tfs: List TForm): TForm ==
        new(TFormTagApply, cons(op, tfs))

    tfSubst(tf: TForm, sigma: Subst): TForm ==
        l := [subst(elt, sigma) for elt in args tf]
        apply(first l, rest l)
