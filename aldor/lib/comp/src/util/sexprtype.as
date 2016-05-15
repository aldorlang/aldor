#include "aldor.as"
#pile

SExpressionType: Category == with
    OutputType
    sexpression: % -> SExpression

    default
        import from SExpression
        (o: TextWriter) << (sxt: %): TextWriter == o << sexpression sxt
