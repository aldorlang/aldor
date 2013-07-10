#include "aldor"
#include "aldorio"

-- Arithmetic objects ------------------------------------------------------
--
-- The objects have arithmetic because each belongs to ArithmeticType.
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
    import from MachineInteger, Integer;
    robfun(rob: Object IntegerType): () == f avail rob  where {
        f(T: IntegerType, r: T): () == {

            -- Object-specific arithmetic:
            s := (r + 1)^3;
            t := (r - 1)^4;
            u := s * t;

            -- Object-specific output:
            stdout << "r = " << r << newline;
            stdout << "    s = (r + 1) ^ 3 = " << s << newline;
            stdout << "    t = (r - 1) ^ 2 = " << t << newline;
            stdout << "    s * t = "         << u << newline;

            -- Can check for additional properties and use if there.
            if T has TotallyOrderedType  then {
                stdout << "The result is "; 
                if u < 0 then stdout << "negative";
                if u > 0 then stdout << "positive";
                if u = 0 then stdout << "zero";
                stdout << newline;
            }
            else
                stdout << "No order for this object." << newline;

            stdout << newline;
        }
    }
    import from DoubleFloat, Integer;
    import from Object IntegerType;
    roblist: List Object IntegerType := [
        object (     Integer, -42),
        object (      MachineInteger, -42)
    ];
    for rob in roblist repeat robfun rob
}

main();
