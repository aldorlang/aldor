#include "aldor"
#include "aldorio"

MI ==> MachineInteger;
F  ==> DoubleFloat;

step(n: MachineInteger)(a: F, b: F): Generator F == generate {
    m: MachineInteger := prev(n);
    del: F := (b - a)/m::F;
    for i in 1..n repeat {
        yield a;
        a := a + del;
    }
}

default minR, maxR, minI, maxI: F;
default numR, numI, maxIters:   MI;
default drawPt: (r: MI, i: MI, n: MI) -> ();

drawMand(minR, maxR, numR, minI, maxI, numI, drawPt, maxIters): () == {

    mandel(cr: F, ci: F): MI == {
       zr: F := 0;
       zi: F := 0;
       n: MI := 0;
       while (zr*zr + zi*zi) < 4.0 for free n in 1..maxIters repeat {
          zr := zr*zr -zi*zi + cr;
          zi := 2.0*zi*zr + ci;
       }
       return n;
    }

    for i in step(numI)(minI, maxI) for ic in 1..numI repeat
      for r in step(numR)(minR, maxR) for rc in 1..numR repeat
        drawPt(rc, ic, mandel(r,i));
}

import from F;
maxN: MI == 100;
maxX: MI == 25;
maxY: MI == 25;

drawPoint(x: MI, y: MI, n: MI): () =={
  if      n = maxN then stdout << "   ";
  else if n < 10   then stdout << "  " << n;
  else                  stdout << " "  << n;
  if x = maxX then stdout << newline;
}

drawMand(-2.0, -1.0, maxX, -0.5, 0.5, maxY, drawPoint, maxN);

