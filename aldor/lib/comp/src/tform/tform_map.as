#include "comp"
#include "aldorio"

#pile

TFormTagMap: TFormTagCat with
    map: (TForm, TForm) -> TForm
    map?: TForm -> Boolean
    mapArgs: TForm -> TForm
    mapRets: TForm -> TForm
== add
    import from BooleanFold
    import from MachineInteger
    import from List TForm
    default tf: TForm
    
    name: String == "map"
    type?(): Boolean == true
    
    map? tf: Boolean == kind tf = name
    mapArgs tf: TForm == (args tf).1
    mapRets tf: TForm == (args tf).2

    tfEquals(map1: TForm, map2: TForm): Boolean ==
        import from List TForm
        (_and)/( a = b for a in args map1 for b in args map2)

    map(argTf: TForm, retTf: TForm): TForm ==
        import from MachineInteger, List MachineInteger
        argBs := bindings argTf
	bs: BindingSet := [ (id, cons(1, path(argBs, id))) for id in keys argBs]

	new(TFormTagMap, [argTf, retTf], bs)

    tfFreeVars(tf: TForm): List Id ==
        import from BindingSet
        fvArgs := freeVars mapArgs tf
	fvRets := freeVars mapRets tf
	theBindings := bindings tf
	[v for v in append!(copy fvArgs, fvRets) | not member?(v, theBindings)]
	
    tfSubst(tf: TForm, sigma: Subst): TForm ==
        map(subst(mapArgs tf, sigma), subst(mapRets tf, sigma))
	
#if ALDORTEST
#include "comp.as"
#include "aldorio.as"
#pile

string(l: Literal): Id == id string l

test(): () ==
    import from Assert List Id, List Id
    import from Assert List TForm, Assert TForm
    import from TForm, List TForm
    import from TFormTagMap, TFormTagId, TFormTagComma
    import from TFormTagDeclare, TFormTagApply
    import from Id, Subst
    -- (x: Int, M x) -> (y: Int, K(x, y))
    theMap := map(comma(declare(id("x"), id("Int")), apply(id("M"), [id("x")])),
    	       comma(declare(id "y", id "Int"), apply(id("K"), [id("y"), id("x")])))

    assertEquals(["M", "Int", "K", "Int"], freeVars theMap)

    assertEquals(theMap, subst(theMap, create []))
test()
#endif
