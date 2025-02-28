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

    annotation(tbl: %, o: Base): A ==
        failed?(find(o pretend Pointer, rep tbl)) and Base has OutputType =>
	    stdout << "Missing " << o << "("
	    for (k, v) in rep tbl repeat
	        stdout << (k pretend Base) << " "
 	    stdout << ")" << newline
	    error "Missing!"
        rep(tbl).(o pretend Pointer)

    annotate!(tbl: %, o: Base, a: A) ==
        rep(tbl).(o pretend Pointer) := a

    if Base has OutputType and A has OutputType then
      (w: TextWriter) << (tbl: %): TextWriter ==
        w << "("
	for (k, v) in rep tbl repeat
	    w << "(" << (k pretend Base) << " " << v << ") "
	w << ")"

NamedAnnotation(Base: with, A: with): with
    name: % -> String
    new: String -> %
    instance: % -> Annotated(Base, A)
== add
    Rep == String

    new(name: String): % == per name
    name(a: %): String == rep a
    instance(a: %): Annotated(Base, A) == new()
    
Unit(Base: with): with
    new: Base -> %
    annotations: (%, A: with, NamedAnnotation(Base, A)) -> Annotated(Base, A)
    register!: (%, A: with, NamedAnnotation(Base, A), Annotated(Base, A)) -> ()
    base: % -> Base
== add
    Rep == Record(base: Base, tbl: HashTable(String, Pointer))
    import from Rep

    new(base: Base): % == per [base, []]

    base(u: %): Base == rep(u).base

    register!(mm: %, A: with, type: NamedAnnotation(Base, A), aa: Annotated(Base, A)): () ==
        rep(mm).tbl.(name type) := aa pretend Pointer

    annotations(mm: %, A: with, type: NamedAnnotation(Base, A)): Annotated(Base, A) ==
        rep(mm).tbl.(name type) pretend Annotated(Base, A)
