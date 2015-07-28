#include "comp"
#include "aldorio"

#pile

TFormTagComma: TFormTagCat with
    comma: Tuple TForm -> TForm
== add
    name: String == "comma"
    import from List TForm
    import from BooleanFold
    import from MachineInteger

    tfEquals(cx1: TForm, cx2: TForm): Boolean ==
        # args cx1 ~= # args cx2 => false
        (_and)/( a = b for a in args cx1 for b in args cx2)

    tfFreeVars(cx: TForm): List Id == []

    info(o: TextWriter, cx: TForm): () ==
        import from BindingSet
        o << bindings cx

    comma(t: Tuple TForm): TForm ==
        import from TFormTagDeclare
        import from Integer, TFormAttrs, List Id
        id(tf: TForm): Id == id attrs(tf)
        undeclare(tf: TForm): TForm == if declare? tf then args(tf).1 else tf
	bs: BindingSet := [([keys bindings tf].0, 1) for tf in [t] | declare? tf]
	stdout << "BS: " << bs << newline
        new(TFormTagComma, [undeclare tf for tf in [t]], bs)

TFormTagDeclare: TFormTagCat with
    declare: (Id, TForm) -> TForm
    declare?: TForm -> Boolean
== add
    import from List TForm, BindingSet
    name: String == "declare"
    tfEquals(def1: TForm, def2: TForm): Boolean ==
        bindings def1 = bindings def2 and args def1 = args def2

    tfFreeVars(decl: TForm): List Id == []

    declare?(tf: TForm): Boolean == kind tf = name
    declare(s: Id, tf: TForm): TForm == new(TFormTagDeclare, [tf], one(s))

    info(o: TextWriter, decl: TForm): () ==
        import from BindingSet
        o << bindings decl


TFormTagType: TFormTagCat with
    type: () -> TForm
== add
    import from List TForm, TForm
    import from MachineInteger

    name: String == "type"
    tfEquals(ttf1: TForm, ttf2: TForm): Boolean == true

    tfFreeVars(cx: TForm): List Id == []

    type(): TForm == new(TFormTagType, [])

TFormTagTuple: TFormTagCat with
    create: TForm -> TForm
== add
    import from List TForm
    import from BooleanFold
    import from MachineInteger

    name: String == "tuple"
    tfEquals(t1: TForm, t2: TForm): Boolean == first args t1 = first args t2

    tfFreeVars(cx: TForm): List Id == []

    create(tf: TForm): TForm == new(TFormTagType, [tf])

#if ALDORTEST
#include "aldor"
#include "aldorio"
#include "comp"

#pile
string(l: Literal): Id == id string l

test(): () ==
    import from Assert TForm
    import from TForm
    Type := type()$TFormTagType
    c1 := comma(Type)$TFormTagComma
    c2 := comma(c1, Type)$TFormTagComma
    stdout << c2 << newline
    assertNotEquals(Type, c1)
    assertNotEquals(c1, Type)
    equalityAxioms(c1, c2, Type)

test2(): () ==
    import from Assert TForm
    import from TForm, List TForm, TFormTagApply, TFormTagId, TFormTagDeclare, TFormTagComma
    import from List TForm
    Type := type()$TFormTagType
    stdout << "XXX" << newline
    Int := id("Int")
    decl := declare("x", Int)
    stdout << decl << newline
    fx := apply("f", [id("x")])
    c := comma(decl, fx)
    stdout << c << newline

test()
test2()
#endif
