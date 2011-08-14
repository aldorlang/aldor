-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testint
--> testcomp -Q3 -Qinline-limit=10
--> testgen l -Q3 -Qinline-limit=10
--> testgen c -Q3 -Qinline-limit=10
--> testrun -Q3 -Qinline-limit=10 -l axllib
--> testrun -l axllib
#pile

#include "axllib.as"

SI ==> SingleInteger
F  ==> DoubleFloat
CF ==> Complex F

import from CF
inline from CF

default minR, maxR, minI, maxI: F
default numR, numI, maxIters:   SI
default drawPt: (r: SI, i: SI, n: SI) -> ()

drawMand(minR, maxR, numR, minI, maxI, numI, drawPt, maxIters): () ==

  mandel(c: CF): SI ==
    z: CF := 0;
    n: SI := 0;
    while norm z < 4.0 for free n in 1..maxIters repeat
        z  := z*z + c
    n

  for i in step(numI)(minI, maxI) for ic in 1..numI repeat
    for r in step(numR)(minR, maxR) for rc in 1..numR repeat
      drawPt(rc, ic, mandel complex(r,i))

import from F
maxN: SI == 100
maxX: SI == 25
maxY: SI == 25

drawPoint(x: SI, y: SI, n: SI): () ==
  import from TextWriter
  if      n = maxN then print << "   "
  else if n < 10   then print << "  " << n
  else                  print << " "  << n
  if x = maxX then print << newline

drawMand(-2.0, -1.0, maxX, -0.5, 0.5, maxY, drawPoint, maxN)
