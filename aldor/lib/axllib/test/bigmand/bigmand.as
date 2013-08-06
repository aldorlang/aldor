-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib
#pile

#include "axllib.as"

macro 
    SI       == SingleInteger
    F        == Float
    CF       == Complex F
    Infinity == 0


import from CF
inline from CF


drawMand(minR:F, maxR:F, numR:SI, minI:F, maxI:F, numI:SI): SI ==

  maxIters: SI == 100
  mandel(c: CF): SI ==
    z:  CF := 0
    nc: SI := 0

    for n in 1..maxIters while norm z < makeFloat 4 repeat
        z  := z*z + c
	nc := n
    if nc = maxIters then nc := Infinity
    nc

  for i in step(numI)(minI, maxI) for ic in 1..numI repeat
    for r in step(numR)(minR, maxR) for rc in 1..numR repeat
      drawPoint(rc, ic, mandel complex(r,i))
    endRow()
  numR * numI


drawPoint(x: SI, y: SI, n: SI): () ==
  import from String
  n = Infinity => print << "   "
  print<<(if n < 10 then "  " else " ")<<n

endRow(): () == print<<newline

doit():() ==
  half :F := 1/(1+1)
  import from SingleInteger
  drawMand(makeFloat(-2), -1, 25, -half, half, 25)
doit()
