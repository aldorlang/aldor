#include "aldor"

F ==> DoubleFloat;

exp(f: F): F == {
  e: F := 1;
  m: MachineInteger := 1;
  x: F := e;
  for i in 2..12 repeat {
      x := x * f;
      m := m * i;
      e := e +  x/(m::F);
  }
  e;
}

floatSequence(): Generator F == generate {
                x: F := 0.0;
                repeat {
                        yield exp(-x*x);
                        x := x + 0.05;
                }
}

runningMean(g: Generator F): Generator F == {
        n: MachineInteger := 0;
        sum: F   := 0;
        generate {
                for x in g repeat {
                        sum := sum + x;
                        n   := next(n);
                        yield sum/(n::F);
                }
        }
}

step(n: MachineInteger)(a: F, b: F): Generator F == generate {
                m: MachineInteger := prev(n);
                del: F := (b - a)/m::F;
                for i in 1..n repeat {
                        yield a;
                        a := a + del;
                }
}


main(): () == {
        import from F, MachineInteger, TextWriter, Character, String;
        for i in runningMean(x for x in step(11)(0.0, 1.0)) repeat 
                stdout << i << newline;

        for i in 1..10 for x in runningMean(floatSequence()) repeat
                 stdout << "next: " << x << newline;
}

main();
