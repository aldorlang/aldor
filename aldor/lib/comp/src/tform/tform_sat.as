#include "comp"
#include "aldorio"
#pile

SatOptions: with
    std: () -> %
== add
    Rep == Integer
    import from Rep
    std(): % == per 0

SatEmbed == 'crossToTuple,tupleToCross,none'

SatResult: with
    succeed: () -> %
    failed: () -> %
    success?: % -> Boolean
    failure?: % -> Boolean
    embedding: % -> SatEmbed
    _and: (%, %) -> %
    _or: (%, %) -> %
== add
    Rep == Boolean

    succeed(): % == per true
    failed(): % == per false

    embedding(r: %): SatEmbed == none

    success?(r: %): Boolean == rep r
    failure?(r: %): Boolean == not rep r

    _and(r1: %, r2: %): % ==
        failure? r1 => r1
        failure? r2 => r2
        succeed()

    _or(r1: %, r2: %): % ==
        success? r1 => r1
        success? r2 => r2
        failed()

TFormSatisfaction: with
    satisfies: (SatOptions, TForm, TForm) -> SatResult
    export from SatOptions, SatResult
== add
    import from TFormTagApply, TFormTagId, TFormTagComma, TFormTagExit
    import from TFormTagTuple, TFormTagDeclare, TFormTagType
    import from List TForm
    import from Integer

    satisfies(opts: SatOptions, S: TForm, T: TForm): SatResult ==
        import from Fold SatResult
        import from SatResult

        satisfiesArgs(S: TForm, T: TForm): SatResult ==
            argCount S ~= argCount T => failed()
            (_and)/(satisfies(opts, Sn, Tn) for Sn in args S for Tn in args T)
        -- exit matched by anything
        -- anything labelled a type matches type (cross, tuple, map)
        -- comma matches comma
        -- tuple matches tuple
        -- apply matches apply
        -- id equality (actually more complex)
        -- declare matches if type does
        -- declare matched by equality
        innerSatisfies(S: TForm, T: TForm): SatResult ==
            rS == if declare? S then declareTForm S else S
            exit? rS => succeed()
            type? T and type?()$(tag rS) => succeed()
            comma? T and comma? rS => satisfiesArgs(rS, T)
            tuple? T and tuple? rS => satisfies(opts, arg rS, arg T)
            declare? T and declare? S => failed() -- We shouldn't see declares?
            -- syntax things - probably wrong
            id? T and id? rS => if S = T then succeed() else failed()
            apply? T and apply? rS => satisfiesArgs(S, T)
            failed()
        stdout << "(Sat: " << S << " --> " << T << newline
        r := innerSatisfies(S, T)
        stdout << " Sat: " << success? r << ")" << newline
        r

#if ALDORTEST
#include "comp.as"
#include "aldorio.as"
#pile
string(l: Literal): Id == id string l

test(): () ==
    import from TFormTagApply, TFormTagId, TFormTagComma, TFormTagExit
    import from TFormTagTuple, TFormTagDeclare, TFormTagType
    import from TFormSatisfaction
    import from List TForm
    import from Assert TForm
    Type: TForm := type()
    TupleType := tuple(Type)
    F := id("F")
    X := id("X")
    Y := id("Y")
    x := id("x")
    y := id("y")

    assertTrue(success? satisfies(std(), TupleType, Type))
    assertFalse(success? satisfies(std(), Type, TupleType))

    assertTrue(success? satisfies(std(), comma(X, Y), comma(X, Y)))
    assertTrue(failure? satisfies(std(), comma(X, Y), comma(Y, X)))
    assertTrue(failure? satisfies(std(), Type, comma(X, Y)))
--    assertTrue(success? satisfies(std(), comma(y, apply(F, [y])),
--                                       comma(declare("x", X), apply(F, [x]))))
test()

#endif
