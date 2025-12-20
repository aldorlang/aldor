#include "foamlib"
#pile

(A: Tuple Type) <- (B: Tuple Type): with == add
--PatMatch(A: Tuple Type, B: Tuple Type): with == add
#if 0
extend PPartial(T: Tuple Type): with
    success1: T -> %
    failed1: () -> %
== add
    Rep == Cross T
    import from Rep

    success1(t: T): % == t pretend %
    failed1(): % == nil$Pointer pretend %

#endif

import from PPartial()
success$PPartial()
--explode$Record()
