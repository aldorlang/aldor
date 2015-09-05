#include "comp"
#include "aldorio"

#pile

TFormTagComma: TFormTagCat with
    comma: Tuple TForm -> TForm
    comma: Generator TForm -> TForm
    comma?: TForm -> Boolean
== add
    import from List TForm
    import from BooleanFold
    import from MachineInteger

    name: String == "comma"
    type?(): Boolean == false
    comma?(tf: TForm): Boolean == kind tf = name

    tfEquals(cx1: TForm, cx2: TForm): Boolean ==
        # args cx1 ~= # args cx2 => false
        (_and)/( a = b for a in args cx1 for b in args cx2)

    tfFreeVars(cx: TForm): List Id ==
        import from Fold2(List Id, List Id), BindingSet
        theBindings := bindings cx
        innerFv := (append!, [])/(freeVars arg for arg in args cx)
        [v for v in innerFv | not member?(v, theBindings)]

    info(o: TextWriter, cx: TForm): () ==
        import from BindingSet
        o << bindings cx

    comma(t: Tuple TForm): TForm == comma(tf for tf in [t])
    comma(g: Generator TForm): TForm ==
        import from TFormTagDeclare
        import from Integer, TFormAttrs, List Id

        id(tf: TForm): Id == id attrs(tf)
        undeclare(tf: TForm): TForm == if declare? tf then args(tf).1 else tf

        l := [g]
        bs: BindingSet := [(declareId tf, n) for tf in l for n in 1@Integer.. | declare? tf]
        new(TFormTagComma, [undeclare tf for tf in l], bs)

    tfSubst(tf: TForm, sigma: Subst): TForm ==
        comma(subst(arg, sigma) for arg in args tf)

TFormTagDeclare: TFormTagCat with
    declare: (Id, TForm) -> TForm
    declare?: TForm -> Boolean
    declareId: TForm -> Id
    declareTForm: TForm -> TForm
== add
    import from List TForm, TFormAttrs, Id, BindingSet
    name: String == "declare"
    tfEquals(def1: TForm, def2: TForm): Boolean ==
        id attrs def1 = id attrs def2 and args def1 = args def2

    tfFreeVars(decl: TForm): List Id == freeVars first args decl

    type?(): Boolean == true
    declare?(tf: TForm): Boolean == kind tf = name
    declare(id: Id, tf: TForm): TForm == new(TFormTagDeclare, [tf], empty(), create(id))

    info(o: TextWriter, decl: TForm): () == o << declareId decl

    tfSubst(tf: TForm, sigma: Subst): TForm == declare(declareId tf, subst(first args tf, sigma))

    declareId(tf: TForm): Id == id attrs tf
    declareTForm(tf: TForm): TForm == first args tf

TFormTagTuple: TFormTagCat with
    tuple: TForm -> TForm
    tuple?: TForm -> Boolean
    arg: TForm -> TForm
== add
    import from List TForm
    import from BooleanFold
    import from MachineInteger

    name: String == "tuple"
    type?(): Boolean == true
    tuple?(tf: TForm): Boolean == kind tf = name
    arg(tf: TForm): TForm == first args tf

    tfEquals(t1: TForm, t2: TForm): Boolean == first args t1 = first args t2

    tfFreeVars(cx: TForm): List Id == []

    tuple(tf: TForm): TForm == new(TFormTagTuple, [tf])
    tfSubst(tf: TForm, sigma: Subst): TForm == tuple(subst(first args tf, sigma))

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
    assertNotEquals(Type, c1)
    assertNotEquals(c1, Type)
    equalityAxioms(c1, c2, Type)

test2(): () ==
    import from Assert TForm
    import from TForm, List TForm, TFormTagApply, TFormTagId, TFormTagDeclare, TFormTagComma
    import from List TForm
    Type := type()$TFormTagType
    Int := id("Int")
    decl := declare("x", Int)
    stdout << decl << newline
    fx := apply(id("f"), [id("x")])
    c := comma(decl, fx)
    assertEquals(fx, fx)
    assertNotEquals(fx, c)

testFreeVariables(): () ==
    import from Assert List Id
    import from TForm, TFormTagApply, TFormTagId, TFormTagDeclare, TFormTagComma
    import from List TForm, List Id
    Type := type()$TFormTagType
    Int := id("Int")
    decl := declare("x", Int)
    stdout << decl << newline
    fx := apply(id("f"), [id("x")])
    c := comma(decl, fx)
    stdout << "FV " << freeVars c << newline
    stdout << "Comma " << c << newline
    assertEquals(["x"], freeVars id("x"))
    assertEquals(["f", "x"], freeVars fx)
    assertEquals(["f", "Int"], freeVars c)

testSubstitution(): () ==
    import from Assert TForm, Subst, TForm, List TForm
    import from TFormTagApply, TFormTagId, TFormTagComma
    import from TFormTagTuple, TFormTagDeclare
    Type := type()$TFormTagType
    Int := id("Int")

    f := id("f")
    x := id("x")
    y := id("y")
    fx := apply(f, [x])
    fy := apply(f, [y])

    theSubst := create("x", id("y"))
    assertEquals(Type, subst(Type, theSubst))
    assertEquals(fy, subst(fx, theSubst))
    assertEquals(fy, subst(fy, theSubst))

    assertEquals(comma(fy, fy), subst(comma(fx, fy), theSubst))
    assertEquals(tuple(y), subst(tuple(x), theSubst))

    assertEquals(declare("k", y), subst(declare("k", x), theSubst))

test()
test2()
testFreeVariables()
testSubstitution()
#endif
