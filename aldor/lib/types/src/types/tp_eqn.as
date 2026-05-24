#include "types"
#include "aldorio"
#pile

Constraint: SExpressionOutputType with
    hasType: (Expression, TypeTerm) -> %
    

TermBind: with
    bind: (Unit Expression, e: Env) -> ()
    Annotations: Unit Expression -> Annotated(Expression, List Constraint)
== add