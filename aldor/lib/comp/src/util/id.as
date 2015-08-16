#include "aldor"
#include "aldorio"

#pile

+++ Id - no real semantics
--- Q: Add depMap?
Id: Join(HashType, OutputType) with
    id: String -> %
    string: % -> String
== add
    Rep ==> String

    id(s: String): % == per s
    string(id: %): String == rep id

    (o: TextWriter) << (id: %): TextWriter == o << rep(id)

    hash(id: %): MachineInteger == hash rep id
    (id1: %) = (id2: %): Boolean == rep(id1) = rep(id2)
