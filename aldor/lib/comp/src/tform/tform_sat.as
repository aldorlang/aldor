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

SatResult: OutputType with
    succeed: () -> %
    failed: () -> %
    test: % -> Boolean
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

    test(r: %): Boolean == rep r
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

    (o: TextWriter) << (r: %): TextWriter ==
        import from String
        o << (if r then "True" else "False")

TFormSatisfaction: with
    satisfies: (SatOptions, TForm, TForm) -> SatResult
    satisfiesMapArgs: (SatOptions, Subst, List AbSyn, TForm) -> SatResult

    export from SatOptions, SatResult
== add
    import from TFormTagApply, TFormTagId, TFormTagComma, TFormTagExit
    import from TFormTagTuple, TFormTagDeclare, TFormTagType, TFormTagMap
    import from TFormTagJoin, TFormTagNamedCat, TFormTagWith
    import from BindingSet
    import from List TForm
    import from Id
    import from Integer
    import from TFormSubst
    import from Fold SatResult
    import from SatResult

    satisfies(opts: SatOptions, S: TForm, T: TForm): SatResult ==

        satisfiesArgs(S: TForm, T: TForm): SatResult ==
            argCount S ~= argCount T => failed()
            (_and)/(satisfies(opts, Sn, Tn) for Sn in args S for Tn in args T)

        satisfiesComma(S: TForm, T: TForm): SatResult ==
            empty? bindings T => satisfiesArgs(S, T)
            sigma: Subst := create [(key, absyn lookup(S, path(bindings T, key))) for key in keys bindings T]
	    stdout << "Sigma: " << sigma << newline
            T0 := comma( (if declare? t then id declareId t else t) for t in args T)
	    stdout << "T0: " << sigma << newline
            substT := subst(T0, sigma)
            satisfies(S, substT)

        satisfiesMap(S: TForm, T: TForm): SatResult ==
            -- A -> B sat X -> Y iff X sat A and B sat Y
            -- eg. R -> Z sat N -> R
	    stdout << "SatMap "<< S << " " << T << newline
            argS := mapArgs S
            argT := mapArgs T
            argResult := satisfies(argT, argS)
            failure? argResult => argResult
            empty? bindings T => satisfies(mapRets S, mapRets T)
            sigma: Subst := create [(key, absyn lookup(S, path(bindings T, key))) for key in keys bindings T]
            substRetT := subst(mapRets T, sigma)
            satisfies(mapRets S, substRetT)

        satisfiesCategory(S: TForm, T: TForm): SatResult ==
            join? T =>
                empty? joinArgs T => succeed()
                (_and)/(innerSatisfies(S, Tn) for Tn in joinArgs T)
            namedCategory? T =>
                namedCategory? S =>
                    namedCategoryId S = namedCategoryId T => succeed()
                    innerSatisfies(namedCategoryTForm S, T)
                join? S =>
                    empty? joinArgs S => failed()
                    (_or)/(innerSatisfies(Sn, T) for Sn in joinArgs S)
                with? S => failed()
                stdout << "CatN failed: " << S << " " << T << newline
                never
            with? T =>
                with? S =>
                    withId S = withId T => innerSatisfies(withType S, withType T)
                    failed()
                namedCategory? S => innerSatisfies(namedCategoryTForm S, T)
                join? S =>
                    empty? joinArgs S => failed()
                    (_or)/(innerSatisfies(Sn, T) for Sn in joinArgs S)
                stdout << "CatW failed: " << S << " " << T << newline
                never
            stdout << "Cat failed: " << S << " " << T << newline
            never
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
            comma? T and comma? rS => satisfiesComma(rS, T)
            tuple? T and tuple? rS => innerSatisfies(arg rS, arg T)
            declare? T and declare? S => failed() -- We shouldn't see declares?
            -- syntax things - probably wrong
            id? T and id? rS => if S = T then succeed() else failed()
            apply? T and apply? rS => satisfiesArgs(S, T)
            map? T and map? rS => satisfiesMap(rS, T)
            category?()$(tag rS) and category?()$(tag T) => satisfiesCategory(rS, T)
            failed()
        satisfies(S: TForm, T: TForm): SatResult ==
            stdout << "(Sat: " << S << " --> " << T << newline
            r := innerSatisfies(S, T)
            stdout << " Sat: " << success? r << ")" << newline
            r
        satisfies(S, T)

    -- way too simple (default args, dependent types, tuple/cross embedding)
    satisfiesMapArgs(opts: SatOptions, sigma: Subst, Sargs: List AbSyn, T: TForm): SatResult ==
        import from MachineInteger, AbSyn
        abMapArg(path: List MachineInteger): AbSyn ==
	    # path > 1 => error "odd path"
	    Sargs.(first path)
        not comma? T => failed()
	# Sargs ~= # args T => failed()
	if empty? bindings T then
	    substT := T
	else
	    add!(sigma, [(key, abMapArg(path(bindings T, key))) for key in keys bindings T])
	    T0 := comma((if declare? t then declareTForm t else t) for t in args T)
	    substT := subst(T0, sigma)
	i := 1$MachineInteger
	while i < # Sargs repeat
	    if failure? satisfiesArg(opts, Sargs.i, args(substT).i) then return failed()
	    i := i + 1
	succeed()

    satisfiesArg(opts: SatOptions, ab: AbSyn, T: TForm): SatResult ==
        import from AbState
	import from TPoss
        (_or)/(satisfies(opts, tf, T) for tf in tposs(state ab))

#if ALDORTEST
#include "comp.as"
#include "aldorio.as"
#pile
string(l: Literal): Id == id string l

test(): () ==
    import from TFormTagApply, TFormTagId, TFormTagComma, TFormTagExit
    import from TFormTagTuple, TFormTagDeclare, TFormTagType, TFormTagMap
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

    assertTrue(success? satisfies(std(), apply(F, [y]), apply(F, [y])))

    assertTrue(success? satisfies(std(), comma(X, Y), comma(X, Y)))
    assertTrue(failure? satisfies(std(), comma(X, Y), comma(Y, X)))
    assertTrue(failure? satisfies(std(), Type, comma(X, Y)))
    assertTrue(success? satisfies(std(), comma(y, apply(F, [y])),
                                         comma(declare("x", X), apply(F, [x]))))

    assertTrue(success? satisfies(std(), map(comma(Type), comma(TupleType)),
                                         map(comma(Type), comma(Type))))
    assertTrue(success? satisfies(std(), map(comma(Type), comma(TupleType)),
                                         map(comma(TupleType), comma(Type))))

    -- (x: Int) -> F x satisfies (y: Int) -> F y
    --
test()


testCategories(): () ==
    import from TFormTagJoin, TFormTagWith, TFormTagNamedCat
    import from TFormTagId, TFormTagComma, TFormTagMap
    import from TFormSatisfaction
    import from Assert TForm
    import from TForm
    allCat := join()
    self := id "%"
    star := _with("*", map(comma(self, self), comma(self)))
    inverse := _with("inverse", map(comma(self), comma(self)))
    Monoid := namedCategory("Monoid", star)
    Group := join(Monoid, inverse)

    assertTrue(success? satisfies(std(), allCat, allCat))
    assertTrue(success? satisfies(std(), star, allCat))
    assertTrue(success? satisfies(std(), inverse, allCat))
    assertTrue(success? satisfies(std(), Monoid, allCat))
    assertTrue(success? satisfies(std(), Group, allCat))

    assertTrue(failure? satisfies(std(), allCat, star))
    assertTrue(failure? satisfies(std(), allCat, inverse))
    assertTrue(failure? satisfies(std(), allCat, Monoid))
    assertTrue(failure? satisfies(std(), allCat, Group))

    assertTrue(success? satisfies(std(), star, star))
    assertTrue(failure? satisfies(std(), inverse, star))
    assertTrue(success? satisfies(std(), Monoid, star))
    assertTrue(success? satisfies(std(), Group, star))

    assertTrue(failure? satisfies(std(), star, inverse))
    assertTrue(success? satisfies(std(), inverse, inverse))
    assertTrue(failure? satisfies(std(), Monoid, inverse))
    assertTrue(success? satisfies(std(), Group, inverse))

    assertTrue(failure? satisfies(std(), star, Monoid))
    assertTrue(failure? satisfies(std(), inverse, Monoid))
    assertTrue(success? satisfies(std(), Monoid, Monoid))
    assertTrue(success? satisfies(std(), Group, Monoid))

    assertTrue(failure? satisfies(std(), star, Group))
    assertTrue(failure? satisfies(std(), inverse, Group))
    assertTrue(failure? satisfies(std(), Monoid, Group))
    assertTrue(success? satisfies(std(), Group, Group))

testCategories()

#endif
