#include "aldor"
#include "aldorio"

#include "debuglib"
start!()$NewDebugPackage;

define VectorType(T: ArithmeticType): Category == AdditiveType with {
    *: (T, %) -> %;
}

Vector(T:  ArithmeticType): VectorType(T) with {
	  bracket: Tuple T -> %;
}  == add {
	  Rep == Record(le: MachineInteger, pa: PrimitiveArray(T));
	  import from T, PrimitiveArray(T), Rep, MachineInteger;
	  import from String, TextWriter;
	  bracket(t: Tuple T): % == {
		     local s: MachineInteger := length(t);
		     local p: PrimitiveArray(T) := new(s,0$T);
		     for i in 0..(s-1) repeat p.i := element(t,i);
		     per [s, p];
          }
	  0: % == per [1,new(1,0)];
	  zero?(x: %): Boolean == {
		   for i in 0..(rep(x).le) repeat {
		       not zero? ((rep(x).pa).i) => return false;
		   }
		   true;
	  }
 	  (x:%) + (y:%):% == {	
	      local s: MachineInteger := (rep(x).le);
	      s ~= (rep(y).le) => error "incompatible vectors";
	      local p: PrimitiveArray(T) := new((rep(x).le),0$T);
	      for i in 0..(s-1) repeat p.i := (rep(x).pa).i + (rep(y).pa).i;
	      per [s, p];
          }
	  - (x: %): % == {
	      local s: MachineInteger := (rep(x).le);
	      local p: PrimitiveArray(T) := new((rep(x).le),0$T);
	      for i in 0..(s-1) repeat p.i := - (rep(x).pa).i;
	      per [s, p];
          }
	  (t:T) * (x:%): % == {	
	      local s: MachineInteger := (rep(x).le);
	      local p: PrimitiveArray(T) := new(s,0$T);
	      for i in 0..(s-1) repeat {
		  p.i := t * (rep(x).pa).i;
	      }
	      per [s, p];
          }
	  (x: %) = (y:%): Boolean == {
	      local s: MachineInteger := (rep(x).le);
	      s ~= (rep(y).le) => false;
	      for i in 0..(s-1) repeat {
		  (rep(y).pa).i ~= (rep(x).pa).i => return false;
	      }
	      true;
	  }
}


define UnivariatePolynomialType(T: Join(ArithmeticType, OutputType)): Category == VectorType(T)  with {
    ArithmeticType;
    OutputType;
    variable: %;
}

UnivariatePolynomial(T: Join(ArithmeticType, OutputType)): UnivariatePolynomialType(T) == Vector(T) add {
    	  Rep == Record(le: MachineInteger, pa: PrimitiveArray(T));
	  import from T, PrimitiveArray(T), Rep, MachineInteger;

	  variable: % == {
	      local p: PrimitiveArray (T) := new (2,0$T);
	      p.1 := 1;
	      per [2,p];
	  }
	  1: % == per [1,new(1,1$T)];
 	  (x:%) + (y:%):% == {	
	      local s__x: MachineInteger := (rep(x).le);
	      local s__y: MachineInteger := (rep(y).le);
	      local d1: MachineInteger := min(s__x - 1, s__y - 1);
	      local d2: MachineInteger := max(s__x - 1, s__y - 1);
	      local p: PrimitiveArray(T) := new(d2+1,0$T);
	      for i in 0..d1 repeat p.i := (rep(x).pa).i + (rep(y).pa).i;
	      if (s__x < s__y) then {
		  for i in (d1+1)..d2 repeat p.i := (rep(y).pa).i;
	      } else {
		  for i in (d1+1)..d2 repeat p.i := (rep(x).pa).i;
	      }
	      per [d2+1, p];
          }
	  (x:%) * (y:%): % == {	
	      local s__x: MachineInteger := (rep(x).le);
	      local s__y: MachineInteger := (rep(y).le);
	      local d: MachineInteger := s__x + s__y - 2;
	      local p__x: PrimitiveArray(T) := (rep(x).pa);
	      local p__y: PrimitiveArray(T) := (rep(y).pa);
	      local p: PrimitiveArray (T) := new (d+1,0$T);
	      for k in 0..d repeat {
		  for i in 0..k repeat {
		      p.i := p.i + p__x.i * p__y.(k-i);
		  }
	      }
	      per [d+1, p];
	  }
	  (p: %) ^ (n: MachineInteger): % == {
	      n < 0 => error "Negative exponent";
	      n = 0 => 1;
	      n = 1 => p;
	      n = 2 => p * p;
	      odd? n => (p ^ (n quo 2)) * p;
	      p ^ (n quo 2);
	  }
	  commutative?: Boolean == true;
	  (p:TextWriter) << (x:%):TextWriter == {
	      p << "[";
	      local s: MachineInteger := (rep(x).le);
	      for i in 0..(s-1) repeat {
		  p << (rep(x).pa).i;
		  if i < s-1 then p << ", " else p << "]";
	      }
	      p;
	  }
	      
}

T == MachineInteger;
V == Vector(T);
U == UnivariatePolynomial(T);
import from T, V, U;
      
main(): () == {

    v: U := variable;
    s: String := "v ? ";
    stdout << s;
    stdout <<  v << newline;

    p1: U := 2 * v;
    stdout << "2 * v ?";
    stdout <<  p1 << newline;

    p2: U := 1$U;
    stdout << "1$U ? ";
    stdout <<  p2 << newline;

    p3: U := p1 + p2;
    stdout << "2*v + 1$U ?";
    stdout <<  p3 << newline;

    p4: U := 3* v - 1$U;
    stdout << "3* v - 1$U ? ";
    stdout <<  p4 << newline;

    p5: U := p1 * p2;
    stdout << "(2*v + 1$U) * (3* v - 1$U) " ;
    stdout <<  p5 << newline;

}

main();
