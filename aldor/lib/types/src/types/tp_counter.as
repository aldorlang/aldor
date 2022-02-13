#include "aldor"
#include "aldorio"
#pile

CallCount: OutputType with
    new: CallCounter -> %
    <<: (TextWriter, %) -> TextWriter
    writer: % -> TextWriter
    close: % -> ()
== add
    Rep == Record(n: MachineInteger, d: MachineInteger, cc: CallCounter)
    import from Rep, MachineInteger

    new(cc: CallCounter): % == per [count(cc), depth(cc), cc]
    close(c: %): () == close(rep(c).cc)
    (o: TextWriter) << (c: %): TextWriter == o << rep(c).d << "," << rep(c).n

    writer(c: %): TextWriter == writer rep(c).cc

CallCounter: with
    counter: String -> %
    counter: (String, Boolean) -> %
    open: % -> CallCount
    depth: % -> MachineInteger
    count: % -> MachineInteger
    close: % -> ()

    writer: % -> TextWriter

    export from CallCount
== add
    Rep == Record(name: String, write: Boolean, c: MachineInteger, d: MachineInteger)
    import from Rep, MachineInteger

    counter(name: String, wr: Boolean): % == per [name, wr, 0, 0]
    counter(name: String): % == counter(name, true)

    depth(cc: %): MachineInteger == rep(cc).d
    count(cc: %): MachineInteger == rep(cc).c

    open(cc: %): CallCount ==
        count := new(cc)$CallCount
        rep(cc).d := rep(cc).d + 1
        rep(cc).c := rep(cc).c + 1
        return count

    local drop(c: Character): () == return

    writer(c: %): TextWriter == if rep(c).write then stdout else textWriter(drop)

    close(cc: %): () ==
        rep(cc).d := rep(cc).d - 1;

