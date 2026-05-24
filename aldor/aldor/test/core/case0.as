#include "foamlib"
#pile

Int ==> MachineInteger
import from Int

foo(): () ==
    () case () => true
--    (1, 2) case (?, ?) => true
    never

#if 0
Stuff: with
    nil: () <- Int
== add
    Rep == Union(n: MachineInteger, s: String)
--    nil(u: %): PPartial() == if rep(u) case n then success() else fail()
#endif
#if 0
Stuff: with
    nil: () <- Int
    pair: (Int, Int) <- Int
    cross: (Int, Int) <- (Int, Int)
== add
    (-> nil)(n: Int): () == if n = 0 then return
    (<- pair)(n: Int): (Int, Int) == if n > 0 then return (n+1, n-1) else failed
    (<- cross)(n: Int, m: Int): (Int, Int) ==
        if n+m > 0 then return (n+1, n-1)

bar(): () ==
    import from Stuff
    12 case nil()
    12 case pair(a := ?, ?)

#endif