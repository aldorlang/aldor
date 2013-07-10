--> testcomp
--> testrun -l axllib
--> testrun -O  -l axllib
--> testint

#include "axllib.as"

define ZeroDivide: Category == ArithmeticException with;
ZeroDivide: ZeroDivide@Category == add;
 
g(s: String): () == print << s << newline;

h(i: Float): Float == {
  i = 0.0 => throw ZeroDivide;
  5.0 * 4.0 / i
}

f():() == {

 local i : Float := 0.0;
 local c : Float; 

 try c:=h(i) catch E in {
        E has ZeroDivide => g("Berk");
        E has ArithmeticException => g("Oh no !");
        g("ok");
 }

g("Oh yeah");

}

f();
