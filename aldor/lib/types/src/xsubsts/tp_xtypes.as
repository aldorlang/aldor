#include "types"
#include "aldorio"
#pile

SymbolMeaning: with
    HashType
    WrappedCategory
    SExpressionOutputType

    id: % -> Symbol
    new: Symbol -> %
== add
    Rep == Record(val: Symbol, tbl: HashTable(String, Pointer))
    import from Rep, HashTable(String, Pointer)
    import from Partial Pointer
    import from Pointer

    (=)(a: %, b: %): Boolean == (a pretend Pointer) = (b pretend Pointer)
    hash(a: %): MachineInteger == hash id a

    new(sym: Symbol): % == per [sym, []]

    isSet?(syme: %, s: String): Boolean ==
        not failed? get(syme, s)
    get(syme: %, s: String): Partial Pointer == find(s, rep(syme).tbl)
    set!(syme: %, s: String, p: Pointer): () ==
        rep(syme).tbl.s := p

    id(syme: %): Symbol == rep(syme).val
    sexpression(s: %): SExpression == sexpr rep(s).val

SymbolMeaningFields: with
    type: Item TypeTerm
    isVar?: ItemAndDefault Boolean
    export from WLookup(SymbolMeaning, TypeTerm)
    export from WLookup(SymbolMeaning, Boolean)
== add
    type: Item TypeTerm == new "type"
    isVar?: ItemAndDefault Boolean == new("isVar?", false)

TermFields: with
    syme: Item SymbolMeaning
    export from Lookup(Symbol, SymbolMeaning)
== add
    syme: Item SymbolMeaning == new "syme"


XTypes: with
    type?:  TypeTerm -> Boolean
    exit?:  TypeTerm -> Boolean
    tuple?: TypeTerm -> Boolean
    multi?: TypeTerm -> Boolean
    cross?: TypeTerm -> Boolean
    bool?:  TypeTerm -> Boolean

    map?:   TypeTerm -> Boolean
    mapArgC: TypeTerm -> MachineInteger
    mapArgN: (TypeTerm, MachineInteger) -> TypeTerm
    mapRet: TypeTerm -> TypeTerm
    mapArgs: TypeTerm -> List TypeTerm

    boolType: () -> TypeTerm
    intType: () -> TypeTerm
    exitType: () -> TypeTerm
    newVar: () -> TypeTerm

    map: (TypeTerm, TypeTerm) -> TypeTerm
    syme?: TypeTerm -> Boolean
    isVar?: TypeTerm -> Boolean
    syme: TypeTerm -> SymbolMeaning

    syme?: Expression -> Boolean
    isVar?: Expression -> Boolean
    syme: Expression -> SymbolMeaning
== add
    import from Symbol, TypeTerm, Expression
    import from List Expression
    import from SymbolMeaningFields
    import from TermFields
    
    type?(t: TypeTerm): Boolean == false
    exit?(t: TypeTerm): Boolean == term? expr t and term expr t = -"exit"
    tuple?(t: TypeTerm): Boolean == false
    multi?(t: TypeTerm): Boolean == false
    cross?(t: TypeTerm): Boolean == false

    boolType(): TypeTerm == constant expr(-"bool")
    intType(): TypeTerm == constant expr(-"int")
    exitType(): TypeTerm == constant expr(-"exit")
    bool?(t: TypeTerm): Boolean == constant? t and term? expr t and term expr t = -"bool"

    map?(t: TypeTerm): Boolean == applyOf?(-"map", expr t)
    mapArgN(t: TypeTerm, n: MachineInteger): TypeTerm == 
        forall(vars t, apply(commaParts(first applicationArgs expr t), n))

    mapArgs(t: TypeTerm): List TypeTerm == 
        [forall(vars t, x) for x in commaParts first applicationArgs expr t]

    mapRet(t: TypeTerm): TypeTerm == forall(vars t, first rest applicationArgs expr t)
    mapArgC(t: TypeTerm): MachineInteger ==
        stdout << "argc " << t << newline
        # commaParts first applicationArgs expr t

    local applyOf?(sym: Symbol, e: Expression): Boolean ==
        apply? e and term? applicationOp e and term applicationOp e = sym


    newVar(): TypeTerm ==
        newVar := new()$Symbol
        varType: TypeTerm := constant expr newVar
	meaning: SymbolMeaning := new newVar
	isVar? meaning := true
	syme termProperties expr varType := meaning
	varType
    
    map(a: TypeTerm, b: TypeTerm): TypeTerm ==
        not constant? a => never
        not constant? b => never
        constant apply(expr(-"map"), [comma [expr a], expr b])

    syme?(a: TypeTerm): Boolean == syme? expr a

    isVar?(a: TypeTerm): Boolean == isVar? expr a

    syme(t: TypeTerm): SymbolMeaning == syme expr t
    syme(e: Expression): SymbolMeaning == syme termProperties e

    syme?(e: Expression): Boolean ==
        term? e and isSet?(syme, termProperties e)

    isVar?(e: Expression): Boolean == syme? e and isVar? syme termProperties e

#if 0
-- More type utils; comment in as needed
XTypeTermUtils: with
    map?: TypeTerm -> Boolean
    mapRet: TypeTerm -> TypeTerm

    map?: ParamTypeTerm -> Boolean
    mapRet: ParamTypeTerm -> ParamTypeTerm
    mapArgC: ParamTypeTerm -> MachineInteger
    mapArgN: (ParamTypeTerm, MachineInteger) -> ParamTypeTerm
== add
    import from Symbol
    import from Expression
    import from TypeTerm
    import from List Expression

    map?(t: TypeTerm): Boolean == applyOf?(-"map", expr t)
    mapArgN(t: TypeTerm, n: MachineInteger): TypeTerm == 
        forall(vars t, apply(commaParts(first applicationArgs expr t), n))

    mapRet(t: TypeTerm): TypeTerm == forall(vars t, first rest applicationArgs expr t)
    mapArgC(t: TypeTerm): MachineInteger == # commaParts first applicationArgs expr t

    map?(t: ParamTypeTerm): Boolean == applyOf?(-"map", expr typeTerm t)
    mapRet(t: ParamTypeTerm): ParamTypeTerm == withParams(asParam mapRet(typeTerm t), paramSet t)
    mapArgC(t: ParamTypeTerm): MachineInteger == # commaParts first applicationArgs expr typeTerm t
    mapArgN(t: ParamTypeTerm, n: MachineInteger): ParamTypeTerm == withParams(asParam mapArgN(typeTerm t, n), paramSet t)

    local applyOf?(sym: Symbol, e: Expression): Boolean ==
        apply? e and term? applicationOp e and term applicationOp e = sym

#endif
