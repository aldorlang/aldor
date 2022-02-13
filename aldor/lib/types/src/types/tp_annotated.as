#include "aldor"
#include "aldorio"
#pile

Annotated(Base: with, A: with): with
    new: () -> %
    annotation: (%, Base) -> A
    annotate!: (%, Base, A) -> ()
    annotation?: (%, Base) -> Boolean
    if Base has OutputType and A has OutputType then OutputType
    
== add
    Rep == HashTable(Pointer, A)
    import from Rep, Base, A
    import from Partial A
    
    new(): % == per []
    annotation?(tbl: %, o: Base): Boolean == not failed?(find(o pretend Pointer, rep tbl))
    annotation(tbl: %, o: Base): A == rep(tbl).(o pretend Pointer)
    annotate!(tbl: %, o: Base, a: A) ==
        rep(tbl).(o pretend Pointer) := a

    if Base has OutputType and A has OutputType then
      (w: TextWriter) << (tbl: %): TextWriter ==
        w << "("
	for (k, v) in rep tbl repeat
	    w << "(" << (k pretend Base) << " " << v << ") "
	w << ")"

NamedAnnotation(A: with): with
    name: % -> String
    new: String -> %
== add
    Rep == String

    new(name: String): % == per name
    name(a: %): String == rep a

Unit(Base: with): with
    new: Base -> %
    annotations: (%, A: with, NamedAnnotation(A)) -> Annotated(Base, A)
    register: (%, A: with, NamedAnnotation(A), Annotated(Base, A)) -> ()
    base: % -> Base
== add
    Rep == Record(base: Base, tbl: HashTable(String, Pointer))
    import from Rep

    new(base: Base): % == per [base, []]

    base(u: %): Base == rep(u).base

    register(mm: %, A: with, type: NamedAnnotation(A), aa: Annotated(Base, A)): () ==
        rep(mm).tbl.(name type) := aa pretend Pointer

    annotations(mm: %, A: with, type: NamedAnnotation(A)): Annotated(Base, A) ==
        rep(mm).tbl.(name type) pretend Annotated(Base, A)
