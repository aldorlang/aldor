#include "comp.as"
#include "aldorio.as"
#pile

AbSynMeaning: with
    meaning: (AbSyn) -> TForm
== add
    import from String
    import from List AbSyn, List TForm
    default ab: AbSyn

    meaning(ab): TForm ==
	import from String
	stdout << "(meaning " << ab << newline
	xx := meaning0 ab
	stdout << " --> " << xx << ")" << newline
        xx
    meaning0(ab): TForm ==
        -- assumption is that ab is in a reasonable state, and
	-- conversion is simply syntactic
	tag ab = apply => meaningApply(ab)
	tag ab = _add => meaningAdd(ab)
	tag ab = comma => meaningComma(ab)
	tag ab = declare => meaningDeclare(asDeclare ab)
	tag ab = id => meaningId(ab)
	tag ab = _with => meaningWith(ab)
	error("unknown tag " + name tag ab)

    meaningApply(ab): TForm ==
        import from AbSynApply, TFormTagApply
	op := applyOp ab
	args := applyArgs ab
	apply(meaning(op), [meaning(abn) for abn in args])

    meaningComma(ab): TForm ==
        import from TFormTagComma
        comma(meaning(abn) for abn in children ab)

    meaningDeclare(decl: AbSynDeclare): TForm ==
        import from TFormTagDeclare, AbSynDeclare
	id  := declareId decl
	declare(abFrSyme(syme toAbSyn decl), meaning(declareType(decl)))

    abFrSyme(syme: Syme): AbSyn == never

    meaningId(ab): TForm ==
        import from TFormTagId
        id(id ab)

    meaningAdd(ab): TForm == error "Add not implemented"
    meaningWith(ab): TForm == error " not implemented"

#if ALDORTEST
#include "comp"
#pile
import from String
string(l: Literal): Id == id string l

test(): () ==
    import from AbSynMeaning
    import from Assert TForm
    import from TFormTagId
    ab: AbSyn := var "fred"
    tf := meaning ab
    assertEquals(id "fred", tf)

test()

#endif
