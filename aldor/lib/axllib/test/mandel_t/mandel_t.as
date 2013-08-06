-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp -O
--> testgen l -O
--> testgen c -O
--> testrun -O -l axllib

#pile
#include "axllib.as"

macro 
    SI       == SingleInteger
    F        == DoubleFloat
    CF       == Complex F
    Infinity == 0

import from CF
inline from CF

maxIters: SI == 100

drawMand(minR:F, maxR:F, numR:SI, minI:F, maxI:F, numI:SI): SI ==

  mandel(c: CF): SI ==
    z:  CF := 0
    nc: SI := 0

    for n in 1..maxIters while norm z < 4.0 repeat
        z  := z*z + c
	nc := n
    if nc = maxIters then nc := Infinity
    nc

  sum : SI := 0

  for i in step(numI)(minI, maxI) repeat
    for r in step(numR)(minR, maxR) repeat
	sum := sum + mandel complex(r,i)
      -- drawPoint(rc, ic, mandel complex(r,i))
    -- endRow()

  print<<"The sum is "<<sum<<newline
  numR * numI

(
  drawPoint(x: SI, y: SI, n: SI): () ==
    import from String
    n = Infinity => print << "   "
    print<<(if n < 10 then "  " else " ")<<n

  endRow(): () == print<<newline

  import from F

  print<<drawMand(-2.0, -1.0, 600, -0.5, 0.5, 600) << newline
)
