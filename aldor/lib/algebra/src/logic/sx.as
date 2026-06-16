#include "aldor"
#include "aldorio"
#pile

SExpressionOutputType: Category == OutputType with
    sexpression: % -> SExpression
    default
        import from SExpression
        (o: TextWriter) << (obj: %): TextWriter == o << sexpression obj


SExpressionInputType: Category == InputType with
    parseSExpression: SExpression -> %
    default
        import from SExpressionReader, Partial SExpression
        << (ins: TextReader): % == parseSExpression retract readCased(ins)

SExpressionType: Category == Join(SExpressionInputType, SExpressionOutputType)

SExpressionBuilder: SExpressionOutputType with
    sxbuilder: () -> %
    <<: (%, Symbol) -> ()
    <<: (%, String) -> ()
    <<: (%, Integer) -> ()
    <<: (%, SExpression) -> ()

    build: % -> SExpression
== add
    Rep == Record(h: List SExpression, tl: List SExpression)
    import from Rep, SExpression
    import from List SExpression

    sxbuilder(): % == per [[], []]
    build(b: %): SExpression == [sx for sx in rep(b).h]

    (b: %) << (sym: Symbol): () == add!(b, sexpr sym)
    (b: %) << (n: Integer): () == add!(b, sexpr n)
    (b: %) << (str: String): () == add!(b, sexpr str)
    (b: %) << (sx: SExpression): () == add!(b, sx)

    local add!(b: %, sx: SExpression): () ==
        if not empty? rep(b).h then
	    cell: List SExpression := [sx]
	    setRest!(rep(b).tl, cell)
	    rep(b).tl := cell
	else
	    cell: List SExpression := [sx]
	    rep(b).h := cell
	    rep(b).tl := cell

    sexpression(b: %): SExpression == [sx for sx in rep(b).h]
