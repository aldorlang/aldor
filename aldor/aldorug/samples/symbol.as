#include "aldor"
#include "aldorio"

define BasicType: Category == Join(OutputType, PrimitiveType);

Symbol: BasicType with {
        name:   % -> String;    ++ the name of the symbol
        coerce: String -> %;    ++ conversion operations
        coerce: % -> String;
} == add {
        Rep    == String;

        import from Rep, Pointer;

        local symTab: HashTable(String, %) := table();

        name(sym: %): String == sym::String;

        coerce(sym: %): String == rep(sym);

        coerce(s: String): % == {
                symb?: Partial(%) := find(s, symTab);
                import from Boolean;
                not failed? symb? => retract symb?;
                str := copy s;
                set!(symTab,str,per str);
                per str;
        }

        (s1: %) = (s2: %): Boolean == rep s1 = rep s2;
        (p: TextWriter) << (sym: %): TextWriter == {
                        p << "'" << sym::String << "'";
        }
}

Test(): () == {
        import from Symbol;

        stdout << "hello"::Symbol << newline;
}
Test()

