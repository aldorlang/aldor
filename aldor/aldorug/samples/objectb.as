#include "aldor"
#include "aldorio"

-- OutputType objects -------------------------------------------------
--
-- These objects can be printed because each belongs to some OutputType.
--

Object(C: Category): with {
        object:         (T: C, T) -> %;
        avail:          % -> (T: C, T);
}
== add {
        Rep == Record(T: C, val: T);
        import from Rep;

        object  (T: C, t: T) : %        == per [T, t];
        avail   (ob: %) : (T: C, T)     == explode rep ob;
}

main():() == {
    import from Integer, List Integer;
    bobfun(bob: Object OutputType): () == {
        f avail bob where
        f(T: OutputType, t: T) : () == {
                stdout << "This prints itself as: " << t << newline;
        }
    }
    import from Object OutputType;
    boblist: List Object OutputType := [
        object (String,       "Ahem!"),
        object (Integer,      42),
        object (List Integer, [1,2,3,4])
    ];
    for bob in boblist repeat bobfun bob;
}

main();

