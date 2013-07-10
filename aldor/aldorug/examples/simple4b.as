#include "aldor"

define MiniListType(S: OutputType): Category == with {
               empty: %;
               empty?: % -> Boolean;
               bracket: Tuple S -> %;
               bracket: Generator S -> %;
               generator: % -> Generator S;
               apply: (%, MachineInteger) -> S;
               <<: (TextWriter, %) -> TextWriter;
}

MiniList(S: OutputType):  MiniListType(S) == add {
        Rep == Union(nil: Pointer, rec: Record(first: S, rest: %));
	import from MachineInteger, Boolean, Rep;

        local cons (s:S,l:%):% == per(union [s, l]);
        local first(l: %): S   == rep(l).rec.first;
        local rest (l: %): %   == rep(l).rec.rest;

        empty: %               == per(union nil);
        empty?(l: %):Boolean   == rep(l) case nil;

        [t: Tuple S]: % == {
                l: % := empty;
                for i in length t..1 by -1 repeat
                        l := cons(element(t, i), l);
                l
        }
        [g: Generator S]: % == {
                r: %  := empty; for s in g repeat r := cons(s, r);
                l: %  := empty; for s in r repeat l := cons(s, l);
                l
        }
        generator(l: %): Generator S == generate {
                while not empty? l repeat {
                        yield first l; l := rest l
                }
        }
        apply(l: %, i: MachineInteger): S == {
                while not empty? l and i > 1 repeat
                        (l, i) := (rest l, i-1);
                empty? l or i ~= 1 => error "No such element";
                first l
        }
        (out: TextWriter) << (l: %): TextWriter == {
                empty? l => out << "[]";
                out << "[" << first l;
                for s in rest l repeat out << ", " << s;
                out << "]";
         }
}